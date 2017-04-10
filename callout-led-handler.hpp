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
class LedCalloutCreateHandler
{
    public:
        LedCalloutCreateHandler() = delete;
        ~LedCalloutCreateHandler() = default;
        LedCalloutCreateHandler(const LedCalloutCreateHandler&) = delete;
        LedCalloutCreateHandler& operator=(const LedCalloutCreateHandler&) = delete;
        LedCalloutCreateHandler(LedCalloutCreateHandler&&) = default;
        LedCalloutCreateHandler& operator=(LedCalloutCreateHandler&&) = default;

        /** @brief
         */
        LedCalloutCreateHandler(sdbusplus::bus::bus& bus):
                          busItem(bus), 
                          calloutCreated(
                              busItem,
                              "type='signal',"
                              "interface='org.freedesktop.DBus.ObjectManager',"
                              "member='InterfacesAdded',"
                              "path='/xyz/openbmc_project/logging'",
                              HandleCalloutCreation,
                              this)
        {
            log<level::ERR>("started led handle");
        }
    private:

        sdbusplus::bus::bus& busItem;

        sdbusplus::server::match::match calloutCreated;

        static int HandleCalloutCreation(sd_bus_message* msg,
                                  void *data,
                                  sd_bus_error* retError);

        int LedAction(std::string path, bool assert);
};
}//namespace led
}//namespace phosphor
