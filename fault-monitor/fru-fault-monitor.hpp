#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server.hpp>
#include "config.h"

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

/** @brief Assert or deassert an LED based on the input FRU
  *  @param[in] bus       -  The Dbus bus object
  *  @param[in] path      -  Inventory path of the FRU
  *  @param[in] assert    -  Assert if true deassert if false
  */
void action(sdbusplus::bus::bus& bus,
            const std::string& path,
            const bool assert);

class Remove;

/** @class Add
 *  @brief Implementation of LED handling during FRU fault
 *  @details This implements methods for watching for a FRU fault
 *  being logged to assert the corresponding LED
 */
class Add
{
    public:
        Add() = delete;
        ~Add() = default;
        Add(const Add&) = delete;
        Add& operator=(const Add&) = delete;
        Add(Add&&) = default;
        Add& operator=(Add&&) = default;

        /** @brief constructs Add a watch for FRU faults.
         *  @param[in] bus -  The Dbus bus object
         */
        Add(sdbusplus::bus::bus& bus):
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

        std::vector<std::unique_ptr<Remove>> removeWatches;

        /** @brief Callback function for fru fault created
         *  @param[in] msg       - Data associated with subscribed signal
         *  @param[in] data      - Pointer to this object instance
         *  @param[out] retError - Error returned
         *  @return zero on success and error code upon failure
         */
        static int created(sd_bus_message* msg,
                           void* data,
                           sd_bus_error* retError);
};

/** @class Remove
 *  @brief Implementation of LED handling after resolution of FRU fault
 *  @details Implement methods for watching the resolution of FRU faults
 *  and deasserting corresponding LED.
 */
class Remove
{
    public:
        Remove() = delete;
        ~Remove() = default;
        Remove(const Remove&) = delete;
        Remove& operator=(const Remove&) = delete;
        Remove(Remove&&) = default;
        Remove& operator=(Remove&&) = default;

        /** @brief constructs Remove
         *  @param[in] bus  -  The Dbus bus object
         *  @param[in] path -  Inventory path to fru
         */
        Remove(sdbusplus::bus::bus& bus, const std::string& path):
            inventoryPath(path),
            matchRemoved(
                bus,
                match(path).c_str(),
                removed,
                this)
        {
            //Do nothing
        }

    private:

        /** @brief inventory path of the FRU */
        std::string inventoryPath;

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
         *  @param[in] path  - Inventory path of the faulty unit.
         */
        std::string match(const std::string& path)
        {
            std::string matchStmt =
                "type='signal',"
                "interface='org.freedesktop.DBus.Properties',"
                "member='PropertiesChanged',"
                "path='" + path +
                "/" + CALLOUT_REV_ASSOCIATION + "'";
            return matchStmt;
        }
};
}//namespace monitor
}//namespace fault
}//namespace inventory
}//namespace led
}//namespace phosphor
