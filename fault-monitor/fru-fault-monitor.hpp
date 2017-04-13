#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server.hpp>

namespace phosphor
{
namespace led
{
namespace inventory
{
namespace fault
{
        
/** @brief Assert or deassert an LED based on the input FRU
  *  @param[in] bus       -  The Dbus bus object
  *  @param[in] path      -  Path of the FRU
  *  @param[in] assert    -  Assert if true deassert if false
  */
void action(sdbusplus::bus::bus& bus,
           const std::string& path,
           const bool assert);

/** @class Monitor
 *  @brief Implementation of LED handling during FRU fault
 *  @details Handles LED assertion and deassertion based on the FRU fault
 */
class Monitor
{
    public:
        Monitor() = delete;
        ~Monitor() = default;
        Monitor(const Monitor&) = delete;
        Monitor& operator=(const Monitor&) = delete;
        Monitor(Monitor&&) = default;
        Monitor& operator=(Monitor&&) = default;

        /** @brief constructs Monitor
         *  @param[in] bus -  The Dbus bus object
         */
        Monitor(sdbusplus::bus::bus& bus):
                          matchCreated(
                              bus,
                              "type='signal',"
                              "interface='org.freedesktop.DBus.ObjectManager',"
                              "member='InterfacesAdded',"
                              "path='/xyz/openbmc_project/logging'",
                              created,
                              this),
                          matchRemoved(
                              bus,
                              "type='signal',"
                              "interface='org.freedesktop.DBus.ObjectManager',"
                              "member='InterfacesRemoved',"
                              "path='/xyz/openbmc_project/logging'",
                              removed,
                              this)
                              
        {
            //Do nothing
        }
    private:

        /** @brief sdbusplus signal match for fault created */
        sdbusplus::server::match::match matchCreated;

        /** @brief sdbusplus signal match for fault removed */
        sdbusplus::server::match::match matchRemoved;

        /** @brief Callback function for fru fault created
         *  @param[in] msg       - Data associated with subscribed signal
         *  @param[in] data      - Pointer to this object instance
         *  @param[out] retError - Error returned
         *  @return zero on success and error code upon failure
         */
        static int created(sd_bus_message* msg,
                           void *data,
                           sd_bus_error* retError);

        /** @brief Callback function for fru fault removed
         *  @param[in] msg       - Data associated with subscribed signal
         *  @param[in] data      - Pointer to this object instance
         *  @param[out] retError - Error returned
         *  @return zero on success and error code upon failure
         */
        static int removed(sd_bus_message* msg,
                           void *data,
                           sd_bus_error* retError);
};
}//namespace fault
}//namespace inventory
}//namespace led
}//namespace phosphor
