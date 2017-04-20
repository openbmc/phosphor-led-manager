#include "fru-fault-monitor.hpp"

namespace phosphor
{
namespace led
{
namespace inventory
{
namespace fault
{
namespace monitor
{

constexpr auto MAPPER_BUSNAME   = "xyz.openbmc_project.ObjectMapper";
constexpr auto MAPPER_OBJ_PATH  = "/xyz/openbmc_project/object_mapper";
constexpr auto MAPPER_IFACE     = "xyz.openbmc_project.ObjectMapper";
constexpr auto OBJMGR_IFACE     = "org.freedesktop.DBus.ObjectManager";
constexpr auto LED_GROUPS       = "/xyz/openbmc_project/led/groups/";
constexpr auto LOG_PATH         = "/xyz/openbmc_project/logging";

using AssociationList = std::vector<std::tuple<
                        std::string, std::string, std::string>>;

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
        throw std::runtime_error("Error in GetObject");
    }

    std::map<std::string, std::vector<std::string>> mapperResponse;
    mapperResponseMsg.read(mapperResponse);
    if (mapperResponse.empty())
    {
        throw std::runtime_error("Error in Get Mapper response");
    }

    return mapperResponse.cbegin()->first;
}

void action(sdbusplus::bus::bus& bus,
            const std::string& path,
            const bool assert)
{
    const auto& service = getService(bus, LED_GROUPS);

    auto pos = path.rfind("/");
    if (pos == std::string::npos)
    {
        return;
    }
    auto unit = path.substr(pos + 1);

    std::string ledPath = LED_GROUPS +
                          unit + "_" + LED_FAULT;

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

int Add::created(sd_bus_message* msg,
                 void* data,
                 sd_bus_error* retError)
{
    auto m = sdbusplus::message::message(msg);
    auto bus = m.get_bus();

    sdbusplus::message::object_path obpath;
    m.read(obpath);
    std::string objectpath(obpath);

    std::size_t found = objectpath.find(ELOG_ENTRY);
    if (found == std::string::npos)
    {
        //Not a new error entry skip
        return 0;
    }

    auto service = getService(bus, LOG_PATH);
    auto method =  bus.new_method_call(service.c_str(), objectpath.c_str(),
                                       "org.freedesktop.DBus.Properties",
                                       "Get");

    method.append("org.openbmc.Associations");
    method.append("associations");
    auto reply = bus.call(method);
    if (reply.is_method_error())
    {
        throw std::runtime_error("Error in getting associations");
    }

    sdbusplus::message::variant<AssociationList> assoc;
    reply.read(assoc);

    AssociationList assocs =
        sdbusplus::message::variant_ns::get<AssociationList>(assoc);
    if (assocs.empty())
    {
        //No associations skip
        return 0;
    }

    for (const auto& item : assocs)
    {
        if (std::get<1>(item).compare(CALLOUT_REV_ASSOCIATION) == 0)
        {
            action(bus, std::get<2>(item), true);
            static_cast<Add*>(data)->removeWatch.emplace_back(
                std::make_unique<Remove>(bus, std::get<2>(item)));
        }
    }
    return 0;
}

int Remove::removed(sd_bus_message* msg,
                    void* data,
                    sd_bus_error* retError)
{
    auto m = sdbusplus::message::message(msg);
    auto bus = m.get_bus();
    std::string assoc;
    m.read(assoc);

    if (assoc.compare("org.openbmc.Association"))
    {
        //Skip if not about association
        return 0;
    }

    std::map<std::string, std::vector<std::string>> endpoints;
    m.read(endpoints);
    std::map<std::string, std::vector<std::string>>::iterator it =
                endpoints.find("endpoints");

    if (it == endpoints.end())
    {
        //No end points,skip
        return 0;
    }

    if (!((*it).second.empty()))
    {
        //Skip, end points are not empty
        return 0;
    }

    action(bus, static_cast<Remove*>(data)->inventoryPath, false);
    return 0;
}

}//namespace monitor
}//namespace fault
}//namespace inventory
}//namespace led
}//namespace phosphor
