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
constexpr auto IFACE_INTERNAL("org.freedesktop.DBus.ObjectManager");

using AssociationList = std::vector<std::tuple<
                        std::string, std::string, std::string>>;

std::string getService(sdbusplus::bus::bus& bus,
                const std::string& path)
{
    auto mapper = bus.new_method_call(MAPPER_BUSNAME,
                                      MAPPER_OBJ_PATH,
                                      MAPPER_IFACE,"GetObject");
    mapper.append(path.c_str(), std::vector<std::string>({IFACE_INTERNAL}));
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
    const auto& host = getService(bus,"/xyz/openbmc_project/led/groups");

    std::string ledPath = "/xyz/openbmc_project/led/groups/"+unit+"_fault";

    auto method =  bus.new_method_call(host.c_str(),
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
    auto sdbpMsg = sdbusplus::message::message(msg);
    auto bus = sdbpMsg.get_bus();

    sdbusplus::message::object_path obpath;
    sdbpMsg.read(obpath);
    std::string objectpath(obpath);

    std::size_t found = objectpath.find("entry");
    if(found == std::string::npos)
    {
        //Not a new error entry skip
        return 0;
    } 

    const auto& host = getService(bus, "/xyz/openbmc_project/logging");
    auto method =  bus.new_method_call(host.c_str(),objectpath.c_str(),
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

    AssociationList assocs = sdbusplus::message::variant_ns::get<AssociationList>(assoc);
    if(assocs.empty())
    {
        //No associations skip
        return 0;
    }

    for(auto& item:assocs)
    {
        if(std::get<0>(item).compare("callout")==0)
        {
            auto pos = std::get<2>(item).rfind("/");
            if(pos == std::string::npos)
            {
                return 0;
            }
            auto unit = std::get<2>(item).substr(pos + 1);
            action(bus,unit,true);
            static_cast<Monitor*>(data)->
                addFaultRemovedWatch(bus,unit); 
        }
    }
    return 0;
}

int Remove::removed(sd_bus_message* msg,
                    void* data,
                    sd_bus_error* retError)
{
    auto sdbpMsg = sdbusplus::message::message(msg);
    auto bus = sdbpMsg.get_bus();
    std::string assoc;
    sdbpMsg.read(assoc);

    action(bus,static_cast<RemoveMonitor*>(data)->getUnit(),false);
    return 0;
}

}//namespace monitor
}//namespace fault
}//namespace inventory
}//namespace led
}//namespace phosphor
