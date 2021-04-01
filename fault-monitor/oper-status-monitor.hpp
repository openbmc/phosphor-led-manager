#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server.hpp>

namespace phosphor
{
namespace led
{
namespace oper
{
namespace status
{
namespace monitor
{
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

    /**
     *  @brief Get service name by the path and interface of the DBus.
     *
     *  @param[in] path      -  D-Bus object path
     *  @param[in] interface -  D-Bus Interface
     *
     *  @return std::string  -  the D-Bus service name
     *
     */
    const std::string getService(const std::string& path,
                                 const std::string& interface) const;

    /**
     * @brief The matchHandler callback function that looks for the
     *        OperationalStatus interface. If it finds it, See if the Inventory
     *        D-Bus object has an association with LED groups D-Bus object, and
     *        then Set the Asserted property.
     *
     * @param[in] msg - The D-Bus message contents
     */
    void matchHandler(sdbusplus::message::message& msg);

    /**
     * @brief Get the Inventory D-Bus object has an association(forward type of
     *        Associationss attribute: fault_led_group) with LED groups D-Bus
     *        object.
     *
     * @param[in] inventoryPath   - The inventory path of the Inventory D-Bus
     *                              object
     *
     * @return std::string        - The object path of the LED Group Manager
     */
    const std::string getLedGroupPath(const std::string& inventoryPath) const;

    /**
     * @brief Update the Asserted property of the LED Group Manager.
     *
     * @param[in] ledGroupPath    - The led group path of the LED Group Manager
     * @param[in] value           - the Asserted value, True / False
     */
    void updateAssertedProperty(const std::string& ledGroupPath, bool value);
};
} // namespace monitor
} // namespace status
} // namespace oper
} // namespace led
} // namespace phosphor
