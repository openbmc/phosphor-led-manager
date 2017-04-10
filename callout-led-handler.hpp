#pragma once
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include <sdbusplus/server.hpp>
#include <phosphor-logging/log.hpp>

namespace phosphor
{
namespace led
{
using namespace phosphor::logging;
class LedCalloutHandler
{
    public:
        LedCalloutHandler() = delete;
        ~LedCalloutHandler() = default;
        LedCalloutHandler(const LedCalloutHandler&) = delete;
        LedCalloutHandler& operator=(const LedCalloutHandler&) = delete;
        LedCalloutHandler(LedCalloutHandler&&) = default;
        LedCalloutHandler& operator=(LedCalloutHandler&&) = default;

        /** @brief
         */
        LedCalloutHandler(sdbusplus::bus::bus& bus):
                          busItem(bus), 
                          calloutCreated(
                              bus,
                              "type ='signal',"
                              "member='InterfacesAdded',"
                              "path='/xyz/openbmc_project/logging',"
                              "interface='org.freedesktop.DBus.ObjectManager'",
                              AssertFaultLed,
                              this)
        {
            log<level::ERR>("started led handle");
        }
    private:

        sdbusplus::bus::bus& busItem;

        sdbusplus::server::match::match calloutCreated;

        static int AssertFaultLed(sd_bus_message* msg,
                                  void *data,
                                  sd_bus_error* retError);
};
}//namespace led
}//namespace phosphor
