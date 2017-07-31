#include <phosphor-logging/elog.hpp>
#include "xyz/openbmc_project/Led/Fru/Monitor/error.hpp"
#include "xyz/openbmc_project/Led/Mapper/error.hpp"
#include "elog-errors.hpp"
#include "fru-fault-monitor.hpp"
#include <phosphor-logging/elog-errors.hpp>
#include "xyz/openbmc_project/Common/error.hpp"

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

constexpr auto MAPPER_BUSNAME   = "xyz.openbmc_project.ObjectMapper";
constexpr auto MAPPER_OBJ_PATH  = "/xyz/openbmc_project/object_mapper";
constexpr auto MAPPER_IFACE     = "xyz.openbmc_project.ObjectMapper";
constexpr auto OBJMGR_IFACE     = "org.freedesktop.DBus.ObjectManager";
constexpr auto LED_GROUPS       = "/xyz/openbmc_project/led/groups/";
constexpr auto LOG_PATH         = "/xyz/openbmc_project/logging";
constexpr auto LOG_IFACE        = "xyz.openbmc_project.Logging.Entry";

using AssociationList = std::vector<std::tuple<
                        std::string, std::string, std::string>>;
using Attributes = sdbusplus::message::variant<bool,AssociationList>;
using AttributeName = std::string;
using AttributeMap = std::map<AttributeName, Attributes>;
using PropertyName = std::string;
using PropertyMap = std::map<PropertyName, AttributeMap>;
using LogEntryMsg = std::pair<sdbusplus::message::object_path, PropertyMap>;

using Service = std::string;
using Path = std::string;
using Interface = std::string;
using Interfaces = std::vector<Interface>;
using MapperResponseType = std::map<Path, std::map<Service, Interfaces>>;

using InternalFailure =
    sdbusplus::xyz::openbmc_project::Common::Error::InternalFailure;

using MethodErr  =
    sdbusplus::xyz::openbmc_project::Led::Mapper::Error::MethodError;
using ObjectNotFoundErr =
    sdbusplus::xyz::openbmc_project::Led::Mapper::Error::ObjectNotFoundError;
using InventoryPathErr =
    sdbusplus::xyz::openbmc_project::
    Led::Fru::Monitor::Error::InventoryPathError;

std::string getService(sdbusplus::bus::bus& bus,
                       const std::string& path)
{
    auto mapper = bus.new_method_call(MAPPER_BUSNAME,
                                      MAPPER_OBJ_PATH,
                                      MAPPER_IFACE, "GetObject");
    mapper.append(path.c_str(), std::vector<std::string>({OBJMGR_IFACE}));
    auto mapperResponseMsg = bus.call(mapper);
    if (mapperResponseMsg.is_method_error())
    {
        using namespace xyz::openbmc_project::Led::Mapper;
        elog<MethodErr>(
            MethodError::METHOD_NAME("GetObject"),
            MethodError::PATH(path.c_str()),
            MethodError::INTERFACE(
                OBJMGR_IFACE));
    }

    std::map<std::string, std::vector<std::string>> mapperResponse;
    mapperResponseMsg.read(mapperResponse);
    if (mapperResponse.empty())
    {
        using namespace xyz::openbmc_project::Led::Mapper;
        elog<ObjectNotFoundErr>(
            ObjectNotFoundError::METHOD_NAME("GetObject"),
            ObjectNotFoundError::PATH(path.c_str()),
            ObjectNotFoundError::INTERFACE(
                OBJMGR_IFACE));
    }

    return mapperResponse.cbegin()->first;
}

void action(sdbusplus::bus::bus& bus,
            const std::string& path,
            bool assert)
{
    std::string service;
    try
    {
        service = getService(bus, LED_GROUPS);
    }
    catch (MethodErr& e)
    {
        commit<MethodErr>();
        return;
    }
    catch (ObjectNotFoundErr& e)
    {
        commit<ObjectNotFoundErr>();
        return;
    }

    auto pos = path.rfind("/");
    if (pos == std::string::npos)
    {
        using namespace xyz::openbmc_project::Led::Fru::Monitor;
        report<InventoryPathErr>(
            InventoryPathError::PATH(
                path.c_str()));
        return;
    }
    auto unit = path.substr(pos + 1);

    std::string ledPath = LED_GROUPS +
                          unit + '_' + LED_FAULT;

    auto method =  bus.new_method_call(service.c_str(),
                                       ledPath.c_str(),
                                       "org.freedesktop.DBus.Properties",
                                       "Set");
    method.append("xyz.openbmc_project.Led.Group");
    method.append("Asserted");

    method.append(sdbusplus::message::variant<bool>(assert));
    bus.call_noreply(method);

    return;
}

void Add::created(sdbusplus::message::message& msg)
{
    auto bus = msg.get_bus();

    LogEntryMsg logEntry;
    msg.read(logEntry);
    std::string objectPath(std::move(logEntry.first));

    std::size_t found = objectPath.find(ELOG_ENTRY);
    if (found == std::string::npos)
    {
        //Not a new error entry skip
        return;
    }
    log<level::ERR>(objectPath.c_str());

    auto iter = logEntry.second.find("org.openbmc.Associations");
    if (iter == logEntry.second.end())
    {
        return;
    }

    auto attr = iter->second.find("associations");
    if (attr == iter->second.end())
    {
        return;
    }

    auto& assocs =
        sdbusplus::message::variant_ns::get<AssociationList>(attr->second);
    if (assocs.empty())
    {
        //No associations skip
        return;
    }

    for (const auto& item : assocs)
    {
        if (std::get<1>(item).compare(CALLOUT_REV_ASSOCIATION) == 0)
        {
            removeWatches.emplace_back(
                std::make_unique<Remove>(bus, std::get<2>(item)));
            action(bus, std::get<2>(item), true);
        }
    }

    return;
}

void Add::processExistingCallouts(sdbusplus::bus::bus& bus)
{
    auto depth = 0;
    auto mapperCall = bus.new_method_call(MAPPER_BUSNAME,
                                          MAPPER_OBJ_PATH,
                                          MAPPER_IFACE,
                                          "GetSubTree");
    mapperCall.append("/");
    mapperCall.append(depth);
    mapperCall.append(std::vector<Interface>({LOG_IFACE}));

    auto mapperResponseMsg = bus.call(mapperCall);
    if (mapperResponseMsg.is_method_error())
    {
        using namespace xyz::openbmc_project::Led::Mapper;
        report<MethodErr>(
            MethodError::METHOD_NAME("GetSubTree"),
            MethodError::PATH(MAPPER_OBJ_PATH),
            MethodError::INTERFACE(
                OBJMGR_IFACE));
        return;
    }

    MapperResponseType mapperResponse;
    mapperResponseMsg.read(mapperResponse);
    if (mapperResponse.empty())
    {
        using namespace xyz::openbmc_project::Led::Mapper;
        report<ObjectNotFoundErr>(
            ObjectNotFoundError::METHOD_NAME("GetSubTree"),
            ObjectNotFoundError::PATH(MAPPER_OBJ_PATH),
            ObjectNotFoundError::INTERFACE(
                OBJMGR_IFACE));
        return;
    }

    for (const auto& elem : mapperResponse)
    {
        auto method =  bus.new_method_call(elem.second.begin()->first.c_str(),
                                           elem.first.c_str(),
                                           "org.freedesktop.DBus.Properties",
                                           "Get");
        method.append("org.openbmc.Associations");
        method.append("associations");
        auto reply = bus.call(method);
        if (reply.is_method_error())
        {
            //do not stop, continue with next elog
            log<level::ERR>("Error in getting associations");
            continue;
        }

        sdbusplus::message::variant<AssociationList> assoc;
        reply.read(assoc);
        auto& assocs = assoc.get<AssociationList>();
        if (assocs.empty())
        {
            //no associations, skip
            continue;
        }

        for (const auto& item : assocs)
        {
            if (std::get<1>(item).compare(CALLOUT_REV_ASSOCIATION) == 0)
            {
                removeWatches.emplace_back(
                    std::make_unique<Remove>(bus, std::get<2>(item)));
                action(bus, std::get<2>(item), true);
            }
        }
    }
}

void Remove::removed(sdbusplus::message::message& msg)
{
    auto bus = msg.get_bus();
    std::string assoc;
    msg.read(assoc);

    if (assoc.compare("org.openbmc.Association"))
    {
        //Skip if not about association
        return;
    }

    std::map<std::string, std::vector<std::string>> endPoints;
    msg.read(endPoints);
    auto it = endPoints.find("endpoints");

    if (it == endPoints.end())
    {
        //No end points,skip
        return;
    }

    if (!((*it).second.empty()))
    {
        //Skip, end points are not empty
        return;
    }

    action(bus, inventoryPath, false);
    return;
}

}//namespace monitor
}//namespace fault
}//namespace fru
}//namespace led
}//namespace phosphor
