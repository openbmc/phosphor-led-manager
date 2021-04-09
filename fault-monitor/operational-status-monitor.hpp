#pragma once

#include "../utils.hpp"

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server.hpp>

namespace phosphor
{
namespace led
{
namespace Operational
{
namespace status
{
namespace monitor
{
using namespace phosphor::led::utils;

/** @class Monitor
 *  @brief Implementation of LED handling during the change of the Functional
 *         property of the OperationalStatus interface
 *
 *  @details This implements methods for watching OperationalStatus interface of
 *           Inventory D-Bus object and then assert corresponding LED Group
 *           D-Bus objects.
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

    /** @brief Add a watch for OperationalStatus.
     *
     *  @param[in] bus -  D-Bus object
     */
    Monitor(sdbusplus::bus::bus& bus) :
        bus(bus),
        matchSignal(bus,
                    "type='signal',member='PropertiesChanged', "
                    "interface='org.freedesktop.DBus.Properties', "
                    "sender='xyz.openbmc_project.Inventory.Manager', "
                    "arg0namespace='xyz.openbmc_project.State.Decorator."
                    "OperationalStatus'",
                    std::bind(std::mem_fn(&Monitor::matchHandler), this,
                              std::placeholders::_1))

    {}

  private:
    /** @brief sdbusplus D-Bus connection. */
    sdbusplus::bus::bus& bus;

    /** @brief sdbusplus signal matches for Monitor */
    sdbusplus::bus::match_t matchSignal;

    /** DBusHandler class handles the D-Bus operations */
    DBusHandler dBusHandler;

    /**
     * @brief Callback handler that gets invoked when the PropertiesChanged
     *        signal is caught by this app. Message is scanned for Inventory
     *        D-Bus object path and if OperationalStatus::Functional is changed,
     *        then corresponding LED Group D-Bus object is called to assert.
     *
     * @param[in] msg - The D-Bus message contents
     */
    void matchHandler(sdbusplus::message::message& msg);

    /**
     * @brief From the Inventory D-Bus object, obtains the associated LED group
     *        D-Bus object, where the association name is "fault_led_group"
     *
     * @param[in] inventoryPath         - Inventory D-Bus object path
     *
     * @return std::vector<std::string> - Vector of LED Group D-Bus object paths
     */
    const std::vector<std::string>
        getLedGroupPaths(const std::string& inventoryPath) const;

    /**
     * @brief Update the Asserted property of the LED Group Manager.
     *
     * @param[in] ledGroupPaths   - LED Group D-Bus object Paths
     * @param[in] value           - The Asserted property value, True / False
     */
    void updateAssertedProperty(const std::vector<std::string>& ledGroupPaths,
                                bool value);
};
} // namespace monitor
} // namespace status
} // namespace Operational
} // namespace led
} // namespace phosphor
