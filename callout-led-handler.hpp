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

/** @class LedCalloutHandler
 *  @brief Implementation of LED handling during FRU callout
 *  @details Handles LED assertion and deassertion based on the FRU callout
 */
class LedCalloutHandler
{
    public:
        LedCalloutHandler() = delete;
        ~LedCalloutHandler() = default;
        LedCalloutHandler(const LedCalloutHandler&) = delete;
        LedCalloutHandler& operator=(const LedCalloutHandler&) = delete;
        LedCalloutHandler(LedCalloutHandler&&) = default;
        LedCalloutHandler& operator=(LedCalloutHandler&&) = default;

        /** @brief constructs LedCalloutHandler
         *  @param[in] bus The Dbus bus object
         */
        LedCalloutHandler(sdbusplus::bus::bus& bus):
                          busItem(bus), 
                          calloutCreated(
                              busItem,
                              "type='signal',"
                              "interface='org.freedesktop.DBus.ObjectManager',"
                              "member='InterfacesAdded',"
                              "path='/xyz/openbmc_project/logging'",
                              HandleCalloutCreation,
                              this),
                          calloutRemoved(
                              busItem,
                              "type='signal',"
                              "interface='org.freedesktop.DBus.ObjectManager',"
                              "member='InterfacesRemoved',"
                              "path='/xyz/openbmc_project/logging'",
                              HandleCalloutRemoved,
                              this)
        {
            //Do nothing
        }
    private:

        /** @brief Handle to DBus bus object */
        sdbusplus::bus::bus& busItem;

        /** @brief sdbusplus signal match for callout created */
        sdbusplus::server::match::match calloutCreated;

        /** @brief sdbusplus signal match for callout removed */
        sdbusplus::server::match::match calloutRemoved;

        /** @brief Callback function for Callout created
         *  @param[in] msg       - Data associated with subscribed signal
         *  @param[in] data      - Pointer to this object instance
         *  @param[out] retError - Error returned
         */
        static int HandleCalloutCreation(sd_bus_message* msg,
                                  void *data,
                                  sd_bus_error* retError);

        /** @brief Callback function for Callout removed
         *  @param[in] msg       - Data associated with subscribed signal
         *  @param[in] data      - Pointer to this object instance
         *  @param[out] retError - Error returned
         */
        static int HandleCalloutRemoved(sd_bus_message* msg,
                                  void *data,
                                  sd_bus_error* retError);

        /** @brief Assert or deassert an LED based on the input FRU
         *  @param[in] path      - Path of the FRU
         *  @param[in] assert    - Assert if true deassert if false 
         */
        int LedAction(std::string path, bool assert);
};
}//namespace led
}//namespace phosphor
