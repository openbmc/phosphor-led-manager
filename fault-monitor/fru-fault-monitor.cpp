#include "fru-fault-monitor.hpp"

#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/exception.hpp>
#include <xyz/openbmc_project/Common/error.hpp>

namespace phosphor
{
namespace led
{
namespace fru
{
namespace fault
{
namespace monitor
{

using namespace phosphor::logging;

static constexpr auto mapperBusName = "xyz.openbmc_project.ObjectMapper";
static constexpr auto mapperObjPath = "/xyz/openbmc_project/object_mapper";
static constexpr auto mapperIntf = "xyz.openbmc_project.ObjectMapper";
static constexpr auto objMgrIntf = "org.freedesktop.DBus.ObjectManager";
static constexpr auto ledGroups = "/xyz/openbmc_project/led/groups/";
static constexpr auto logIntf = "xyz.openbmc_project.Logging.Entry";

using AssociationList =
    std::vector<std::tuple<std::string, std::string, std::string>>;
using Attributes = std::variant<bool, AssociationList>;
using PropertyName = std::string;
using PropertyMap = std::unordered_map<PropertyName, Attributes>;
using InterfaceName = std::string;
using InterfaceMap = std::unordered_map<InterfaceName, PropertyMap>;

using Service = std::string;
using Path = std::string;
using Interface = std::string;
using Interfaces = std::vector<Interface>;
using MapperResponseType =
    std::unordered_map<Path, std::unordered_map<Service, Interfaces>>;

using ResourceNotFoundErr =
    sdbusplus::xyz::openbmc_project::Common::Error::ResourceNotFound;
using InvalidArgumentErr =
    sdbusplus::xyz::openbmc_project::Common::Error::InvalidArgument;

std::string getService(sdbusplus::bus_t& bus, const std::string& path)
{
    auto mapper = bus.new_method_call(mapperBusName, mapperObjPath, mapperIntf,
                                      "GetObject");
    mapper.append(path.c_str(), std::vector<std::string>({objMgrIntf}));

    std::unordered_map<std::string, std::vector<std::string>> mapperResponse;
    try
    {
        auto mapperResponseMsg = bus.call(mapper);
        mapperResponseMsg.read(mapperResponse);
    }
    catch (const sdbusplus::exception_t& e)
    {
        lg2::error(
            "Failed to parse getService mapper response, ERROR = {ERROR}",
            "ERROR", e);
        using namespace xyz::openbmc_project::Common;
        elog<ResourceNotFoundErr>(ResourceNotFound::RESOURCE(path.c_str()));
    }
    if (mapperResponse.empty())
    {
        using namespace xyz::openbmc_project::Common;
        elog<ResourceNotFoundErr>(ResourceNotFound::RESOURCE(path.c_str()));
        return {};
    }

    return mapperResponse.cbegin()->first;
}

void action(sdbusplus::bus_t& bus, const std::string& path, bool assert)
{
    std::string service;
    try
    {
        std::string groups{ledGroups};
        groups.pop_back();
        service = getService(bus, groups);
    }
    catch (const ResourceNotFoundErr& e)
    {
        commit<ResourceNotFoundErr>();
        return;
    }

    auto pos = path.rfind('/');
    if (pos == std::string::npos)
    {
        using namespace xyz::openbmc_project::Common;
        report<InvalidArgumentErr>(
            InvalidArgument::ARGUMENT_NAME("path"),
            InvalidArgument::ARGUMENT_VALUE(path.c_str()));
        return;
    }
    auto unit = path.substr(pos + 1);

    std::string ledPath = ledGroups + unit + '_' + LED_FAULT;

    auto method = bus.new_method_call(service.c_str(), ledPath.c_str(),
                                      "org.freedesktop.DBus.Properties", "Set");
    method.append("xyz.openbmc_project.Led.Group");
    method.append("Asserted");

    method.append(std::variant<bool>(assert));

    try
    {
        bus.call_noreply(method);
    }
    catch (const sdbusplus::exception_t& e)
    {
        // Log an info message, system may not have all the LED Groups defined
        lg2::info("Failed to Assert LED Group, ERROR = {ERROR}", "ERROR", e);
    }

    return;
}

void Add::created(sdbusplus::message_t& msg)
{
    auto bus = msg.get_bus();

    sdbusplus::message::object_path objectPath;
    InterfaceMap interfaces;
    try
    {
        msg.read(objectPath, interfaces);
    }
    catch (const sdbusplus::exception_t& e)
    {
        lg2::error("Failed to parse created message, ERROR = {ERROR}", "ERROR",
                   e);
        return;
    }

    std::size_t found = objectPath.str.find(ELOG_ENTRY);
    if (found == std::string::npos)
    {
        // Not a new error entry skip
        return;
    }
    auto iter = interfaces.find("xyz.openbmc_project.Association.Definitions");
    if (iter == interfaces.end())
    {
        return;
    }

    // Nothing else shows when a specific error log
    // has been created. Do it here.
    lg2::debug("{PATH} created", "PATH", objectPath);

    auto attr = iter->second.find("Associations");
    if (attr == iter->second.end())
    {
        return;
    }

    auto& assocs = std::get<AssociationList>(attr->second);

    for (const auto& item : assocs)
    {
        if (std::get<1>(item) == CALLOUT_REV_ASSOCIATION)
        {
            removeWatches.emplace_back(
                std::make_unique<Remove>(bus, std::get<2>(item)));
            action(bus, std::get<2>(item), true);
        }
    }

    return;
}

void getLoggingSubTree(sdbusplus::bus_t& bus, MapperResponseType& subtree)
{
    auto depth = 0;
    auto mapperCall = bus.new_method_call(mapperBusName, mapperObjPath,
                                          mapperIntf, "GetSubTree");
    mapperCall.append("/");
    mapperCall.append(depth);
    mapperCall.append(std::vector<Interface>({logIntf}));

    try
    {
        auto mapperResponseMsg = bus.call(mapperCall);
        mapperResponseMsg.read(subtree);
    }
    catch (const sdbusplus::exception_t& e)
    {
        lg2::error(
            "Failed to parse existing callouts subtree message, ERROR = {ERROR}",
            "ERROR", e);
    }
}

void Add::processExistingCallouts(sdbusplus::bus_t& bus)
{
    MapperResponseType mapperResponse;

    getLoggingSubTree(bus, mapperResponse);
    if (mapperResponse.empty())
    {
        // No errors to process.
        return;
    }

    for (const auto& elem : mapperResponse)
    {
        auto method = bus.new_method_call(
            elem.second.begin()->first.c_str(), elem.first.c_str(),
            "org.freedesktop.DBus.Properties", "Get");
        method.append("xyz.openbmc_project.Association.Definitions");
        method.append("Associations");
        auto reply = bus.call(method);
        if (reply.is_method_error())
        {
            // do not stop, continue with next elog
            lg2::error("Error in getting associations");
            continue;
        }

        std::variant<AssociationList> assoc;
        try
        {
            reply.read(assoc);
        }
        catch (const sdbusplus::exception_t& e)
        {
            lg2::error(
                "Failed to parse existing callouts associations message, ERROR = {ERROR}",
                "ERROR", e);
            continue;
        }
        auto& assocs = std::get<AssociationList>(assoc);

        for (const auto& item : assocs)
        {
            if (std::get<1>(item) == CALLOUT_REV_ASSOCIATION)
            {
                removeWatches.emplace_back(
                    std::make_unique<Remove>(bus, std::get<2>(item)));
                action(bus, std::get<2>(item), true);
            }
        }
    }
}

void Remove::removed(sdbusplus::message_t& msg)
{
    auto bus = msg.get_bus();

    action(bus, inventoryPath, false);
    return;
}

} // namespace monitor
} // namespace fault
} // namespace fru
} // namespace led
} // namespace phosphor
