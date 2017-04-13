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
  *  @param[in] unit      -  FRU name
  *  @param[in] assert    -  Assert if true deassert if false
  */
void action(sdbusplus::bus::bus& bus,
            const std::string& unit,
            const bool assert);

class RemoveMonitor;
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
                this)
        {
            //Do nothing
        }
    private:

        /** @brief sdbusplus signal match for fault created */
        sdbusplus::server::match::match matchCreated;

        std::unique_ptr<phosphor::led::inventory::fault::RemoveMonitor> rmon;

        /** @brief Callback function for fru fault created
         *  @param[in] msg       - Data associated with subscribed signal
         *  @param[in] data      - Pointer to this object instance
         *  @param[out] retError - Error returned
         *  @return zero on success and error code upon failure
         */
        static int created(sd_bus_message* msg,
                           void* data,
                           sd_bus_error* retError);

        /** @brief add a watch for fault removal
         *  @param[in] bus  - The Dbus bus object
         *  @param[in] path - Path to the entry
         */
        void addFaultRemovedWatch(sdbusplus::bus::bus& bus,
                                  const std::string& path);
};

class RemoveMonitor
{
    public:
        RemoveMonitor() = delete;
        ~RemoveMonitor() = default;
        RemoveMonitor(const RemoveMonitor&) = delete;
        RemoveMonitor& operator=(const RemoveMonitor&) = delete;
        RemoveMonitor(RemoveMonitor&&) = default;
        RemoveMonitor& operator=(RemoveMonitor&&) = default;

        /** @brief constructs RemoveMonitor
         *  @param[in] bus -  The Dbus bus object
         */
        RemoveMonitor(sdbusplus::bus::bus& bus, const std::string& unit):
            frunit(unit),
            matchRemoved(
                bus,
                match(unit).c_str(),
                removed,
                this)
        {
            //Do nothing
        }

        /** @brief returns the unit with fault
         *  @return the unit name
         */
        std::string& getUnit()
        {
            return frunit;
        }
    private:

        /** @brief unit name */

        std::string frunit;
        /** @brief sdbusplus signal matches for fault removed */
        sdbusplus::server::match::match matchRemoved;

        /** @brief Callback function for fru fault created
         *  @param[in] msg       - Data associated with subscribed signal
         *  @param[in] data      - Pointer to this object instance
         *  @param[out] retError - Error returned
         *  @return zero on success and error code upon failure
         */
        static int removed(sd_bus_message* msg,
                           void* data,
                           sd_bus_error* retError);

        /** @brief function to create fault remove match for a fru
         *  @param[in] unit  - Name of the faulty unit
         */
        std::string match(const std::string& unit)
        {
            std::string matchStmt =
                "type='signal',"
                "interface='org.freedesktop.DBus.Properties',"
                "member='PropertiesChanged',"
                "path='/xyz/openbmc_project/inventory/system/chassis/motherboard/" + unit +
                "/fault'";
            return matchStmt;
        }
};
}//namespace fault
}//namespace inventory
}//namespace led
}//namespace phosphor
