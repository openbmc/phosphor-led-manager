#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server.hpp>

#include <iostream>

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
class OperStatusSingal
{
  public:
    OperStatusSingal() = delete;
    ~OperStatusSingal() = default;
    OperStatusSingal(const OperStatusSingal&) = delete;
    OperStatusSingal& operator=(const OperStatusSingal&) = delete;
    OperStatusSingal(OperStatusSingal&&) = default;
    OperStatusSingal& operator=(OperStatusSingal&&) = default;

    /** @brief Add a watch for OperationalStatus.
     *  @param[in] bus -  D-Bus object
     */
    OperStatusSingal(sdbusplus::bus::bus& bus) :
        bus(bus),
        matchSignal(bus,
                    "type='signal',member='PropertiesChanged', "
                    "interface='org.freedesktop.DBus.Properties', "
                    "sender='xyz.openbmc_project.Inventory.Manager', "
                    "arg0namespace='xyz.openbmc_project.State.Decorator."
                    "OperationalStatus'",
                    std::bind(std::mem_fn(&OperStatusSingal::processHandle),
                              this, std::placeholders::_1))

    {}

  private:
    sdbusplus::bus::bus& bus;

    /** @brief sdbusplus signal matches for OperStatusSingal */
    sdbusplus::bus::match_t matchSignal;

    /**
     * @brief The processHandle callback function that looks for the
     *        OperationalStatus interface.  If it finds it, See if the Inventory
     *        D-Bus object has an association with LED groups D-Bus object, and
     *        then Set the Asserted property.
     *
     * @param[in] msg - The D-Bus message contents
     */
    void processHandle(sdbusplus::message::message& msg);

    /**
     * @brief Get the Inventory D-Bus object has an association with LED groups
     *        D-Bus object.
     *
     * @param[in] path     - The object path of the Inventory D-Bus object
     *
     * @return std::string - The object path of the LED Group Manager
     */
    const std::string getLedGroupPath(const std::string& path);

    /**
     * @brief Update the Asserted property of the LED Group Manager.
     *
     * @param[in] path     - The object path of the LED Group Manager
     * @param[in] value    - the Asserted value, True / False
     */
    void updateAsserted(const std::string& path, bool value);
};
} // namespace monitor
} // namespace status
} // namespace oper
} // namespace led
} // namespace phosphor
