#include "callout-led-handler.hpp"
#include <phosphor-logging/log.hpp>
namespace phosphor
{
namespace led
{
    using namespace phosphor::logging;
    int LedCalloutCreateHandler::LedAction(std::string path, bool assert)
    {
        // auto bus = sdbusplus::bus::new_default();
         constexpr auto MAPPER_BUSNAME   = "xyz.openbmc_project.ObjectMapper";
        constexpr auto MAPPER_OBJ_PATH  = "/xyz/openbmc_project/object_mapper";
        constexpr auto MAPPER_IFACE     = "xyz.openbmc_project.ObjectMapper";
        //need to fill
        log<level::ERR>("Asserting LED");
        auto pos = path.rfind("/");
        if(pos == std::string::npos)
        {
            //error
        }
        auto unit = path.substr(pos + 1);
        log<level::ERR>(unit.c_str());
        std::string ledPath = "/xyz/openbmc_project/led/groups/"+unit+"_fault";
        constexpr auto IFACE_INTERNAL("org.freedesktop.DBus.ObjectManager");
        auto mapper = busItem.new_method_call(MAPPER_BUSNAME,MAPPER_OBJ_PATH,MAPPER_IFACE,"GetObject");
          mapper.append("/xyz/openbmc_project/led/groups", std::vector<std::string>({IFACE_INTERNAL}));
        auto mapperResponseMsg = busItem.call(mapper);
       if (mapperResponseMsg.is_method_error())
        {
            log<level::ERR>("error in method call1");
        //    throw std::runtime_error("Error in property Get");
            return 0;
        }
        std::map<std::string, std::vector<std::string>> mapperResponse;
        mapperResponseMsg.read(mapperResponse);
        if (mapperResponse.empty())
        {
            log<level::ERR>("Error reading mapper response");
            return 0;
        }
         const auto& host = mapperResponse.cbegin()->first;
       log<level::ERR>(host.c_str());
       log<level::ERR>(ledPath.c_str());
       auto method =  busItem.new_method_call(host.c_str(),ledPath.c_str(),
                                           "org.freedesktop.DBus.Properties",
                                            "Set");
        method.append("xyz.openbmc_project.Led.Group");
        method.append("Asserted");
        
	method.append(sdbusplus::message::variant<bool>(assert));
        busItem.call_noreply(method);
        /* if (reply.is_method_error())
        {
            log<level::ERR>("error in method call XXX");
            //throw std::runtime_error("Error in property Get");
                        return 0;
         }
        sdbusplus::message::variant<bool> val;
        reply.read(val);
        char xb[100];
        sprintf(xb,"led status: %d",sdbusplus::message::variant_ns::get<bool>(val)?1:0);
        log<level::ERR>(xb);
        */
        return 0;
    }
    int LedCalloutCreateHandler::HandleCalloutCreation(sd_bus_message* msg,
                                  void *data,
                                  sd_bus_error* retError)
    {
        log<level::ERR>("in callback");
        auto sdbpMsg = sdbusplus::message::message(msg);
        auto bus = sdbusplus::bus::new_default();
        LedCalloutCreateHandler* cah = static_cast<LedCalloutCreateHandler*>(data); 
        std::string name;
        sdbusplus::message::object_path obpath;
         constexpr auto MAPPER_BUSNAME   = "xyz.openbmc_project.ObjectMapper";
        constexpr auto MAPPER_OBJ_PATH  = "/xyz/openbmc_project/object_mapper";
        constexpr auto MAPPER_IFACE     = "xyz.openbmc_project.ObjectMapper";

        using Fault = std::tuple<std::string, std::string, std::string>;
        using AssociationList = std::vector<Fault>;
        using Association = std::map<std::string,AssociationList>;
        using AdditionalData = std::map<std::string,sdbusplus::message::variant<int,uint64_t,std::string,std::vector<std::string>,bool>>;
        std::map<std::string,
            sdbusplus::message::variant<std::map<std::string,sdbusplus::message::variant<int>>,
                Association,
                    AdditionalData>> entry;
        //std::map<sdbusplus::message::object_path,Entry> entry;
        
        log<level::ERR>("reading");
        sdbpMsg.read(obpath);
        std::string xpath(obpath);
        log<level::ERR>("comparing");
        log<level::ERR>(xpath.c_str());

        std::size_t found = xpath.find("entry");
        if(found == std::string::npos)
        {
            log<level::ERR>("not new ntry skipping");
            return 0;
        }
 
        constexpr auto IFACE_INTERNAL("org.freedesktop.DBus.ObjectManager");
        sdbusplus::message::variant<std::vector<std::tuple<std::string, std::string, std::string>>> assoc;
        auto mapper = bus.new_method_call(MAPPER_BUSNAME,MAPPER_OBJ_PATH,MAPPER_IFACE,"GetObject");
   
        mapper.append("/xyz/openbmc_project/logging", std::vector<std::string>({IFACE_INTERNAL}));
       // mapper.append("/xyz/openbmc_project/logging",1,IFACE_INTERNAL);
       
        auto mapperResponseMsg = bus.call(mapper);

        if (mapperResponseMsg.is_method_error())
        {
            log<level::ERR>("error in method call1");
            throw std::runtime_error("Error in property Get");
        }
        std::map<std::string, std::vector<std::string>> mapperResponse;
        mapperResponseMsg.read(mapperResponse);
        if (mapperResponse.empty())
        {
            log<level::ERR>("Error reading mapper response");
            return 0;
        }
         const auto& host = mapperResponse.cbegin()->first;
         log<level::ERR>(host.c_str());
         log<level::ERR>(xpath.c_str());
        auto method =  bus.new_method_call(host.c_str(),xpath.c_str(),
                                           "org.freedesktop.DBus.Properties",
                                            "Get");
        method.append("org.openbmc.Associations"); 
        method.append("associations");
        auto reply = bus.call(method);

        if (reply.is_method_error())
        {
            log<level::ERR>("error in method call");
            //throw std::runtime_error("Error in property Get");
            return 0;
        }
        log<level::ERR>("reading associations");
        reply.read(assoc);
        log<level::ERR>("read");
        AssociationList assocs =  sdbusplus::message::variant_ns::get<AssociationList>(assoc);
        if(assocs.empty())
        {
             log<level::ERR>("no associations skipping");
             return 0;

        }

        log<level::ERR>("Have associations");
 
        for(auto& item:assocs)
        {
            if(std::get<0>(item).compare("callout")==0)
            {
                 log<level::ERR>("Callouts prtesent");
                 cah->LedAction(std::get<2>(item),true);
                 //log<level::ERR>(std::get<2>(item));
                 break;
             }
        }

/*
        const auto& item = entry.find("org.openbmc.Associations");
        if(item != entry.end())
        {
            Association Assoc = sdbusplus::message::variant_ns::get<Association>(item->second);
            const auto& assocs = Assoc.find("association");
            if(assocs != Assoc.end())
            {
            for(const auto& assoc:assocs->second)
            {
                if(std::get<0>(assoc).compare("callout")==0)
                {
                    static_cast<LedCalloutCreateHandler*>(data)->LedAction(std::get<2>(assoc),true);
                    break;
                }
            }
            }
        }
*/
        return 0;
    }
     
}
}
