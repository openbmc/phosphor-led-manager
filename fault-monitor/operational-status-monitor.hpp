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
    Monitor& operator=(Monitor&&) = delete;

    /** @brief Add a watch for OperationalStatus.
     *
     *  @param[in] bus -  D-Bus object
     */
    explicit Monitor(sdbusplus::bus_t& bus) :
        matchSignal(bus,
                    "type='signal',member='PropertiesChanged', "
                    "interface='org.freedesktop.DBus.Properties', "
                    "sender='xyz.openbmc_project.Inventory.Manager', "
                    "arg0namespace='xyz.openbmc_project.State.Decorator."
                    "OperationalStatus'",
                    [](sdbusplus::message_t& m) { matchHandler(m); })

    {}

  private:
    /** @brief sdbusplus signal matches for Monitor */
    sdbusplus::bus::match_t matchSignal;

    /**
     * @brief Callback handler that gets invoked when the PropertiesChanged
     *        signal is caught by this app. Message is scanned for Inventory
     *        D-Bus object path and if OperationalStatus::Functional is changed,
     *        then corresponding LED Group D-Bus object is called to assert.
     *
     * @param[in] msg - The D-Bus message contents
     */
    static void matchHandler(sdbusplus::message_t& msg);

    /**
     * @brief From the Inventory D-Bus object, obtains the associated LED group
     *        D-Bus object, where the association name is "fault_led_group"
     *
     * @param[in] inventoryPath         - Inventory D-Bus object path
     *
     * @return std::vector<std::string> - Vector of LED Group D-Bus object paths
     */
    static std::vector<std::string> getLedGroupPaths(
        const std::string& inventoryPath);

    /**
     * @brief Update the Asserted property of the LED Group Manager.
     *
     * @param[in] ledGroupPaths   - LED Group D-Bus object Paths
     * @param[in] value           - The Asserted property value, True / False
     */
    static void updateAssertedProperty(
        const std::vector<std::string>& ledGroupPaths, bool value);
};
} // namespace monitor
} // namespace status
} // namespace Operational
} // namespace led
} // namespace phosphor
