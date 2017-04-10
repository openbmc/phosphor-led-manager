#include "callout-led-handler.hpp"
#include <phosphor-logging/log.hpp>
namespace phosphor
{
namespace led
{
    using namespace phosphor::logging;
    int LedCalloutHandler::AssertFaultLed(sd_bus_message* msg,
                                  void *data,
                                  sd_bus_error* retError)
    {
        log<level::ERR>("in callback");
        sdbusplus::message::message sdpMsg{msg};
        std::map<std::string,
            std::map<std::string,
                sdbusplus::message::variant<std::string,int>>> entry;
        sdpMsg.read(entry);
        for(auto& item:entry)
        {
            if(item.first == "org.openbmc.Associations")
            {
               log<level::ERR>("found"); 
            }
        }
        return 0;
    }
     
}
}
