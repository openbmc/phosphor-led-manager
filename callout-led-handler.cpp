#include "callout-led-handler.hpp"
#include <phosphor-logging/log.hpp>
namespace phosphor
{
namespace led
{
    using namespace phosphor::logging;
    int LedCalloutCreateHandler::LedAction(std::string, bool assert)
    {
        return 0;
    }
    int LedCalloutCreateHandler::HandleCalloutCreation(sd_bus_message* msg,
                                  void *data,
                                  sd_bus_error* retError)
    {
        return 0;
    }

    int LedCalloutCreateHandler::HandleCalloutRemoved(sd_bus_message* msg,
                                  void *data,
                                  sd_bus_error* retError)
    {
        return 0;
    }
     
}
}
