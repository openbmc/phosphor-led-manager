#pragma once

#include "config.h"

#include "utils.hpp"

#include <phosphor-logging/elog.hpp>

namespace phosphor
{
namespace led
{
namespace multi
{
namespace purpose
{
namespace status
{

using namespace phosphor::led::utils;
using namespace phosphor::logging;

using DutyOn = uint8_t;
using Period = uint16_t;

enum class Action
{
    On,
    Off,
    Blink
};

enum class PowerState
{
    On,
    Off
};

namespace led
{
static constexpr auto SLED_DBUS_PATH =
    "/xyz/openbmc_project/led/groups/enclosure_identify";
static constexpr auto GROUP_LED_IFACE = "xyz.openbmc_project.Led.Group";
static constexpr auto SLED_PROP = "Asserted";
static constexpr auto POWER_LED = "/xyz/openbmc_project/led/physical/power";
static constexpr auto SYSTEM_LED = "/xyz/openbmc_project/led/physical/system";
static constexpr auto PHY_LED_IFACE = "xyz.openbmc_project.Led.Physical";
} // namespace led

namespace hostselector
{
static constexpr auto SELECTOR_OBJPATH =
    "/xyz/openbmc_project/Chassis/Buttons/HostSelector";
static constexpr auto SELECTOR_INTERFACE =
    "xyz.openbmc_project.Chassis.Buttons.HostSelector";
static constexpr auto SELECTOR_POSITION = "Position";
static constexpr auto SELECTOR_MAXPOS = "MaxPosition";
} // namespace hostselector

namespace power
{
static constexpr auto POWER_STATE_OBJPATH =
    "/xyz/openbmc_project/state/chassis";
static constexpr auto POWER_STATE_IFACE = "xyz.openbmc_project.State.Chassis";
static constexpr auto POWER_STATE_PROPERTY = "CurrentPowerState";
} // namespace power

namespace sensor
{
static constexpr auto SENSOR_OBJPATH = "/xyz/openbmc_project/sensors";
static constexpr auto SENSOR_THRES_IFACE =
    "xyz.openbmc_project.Sensor.Threshold.Critical";
static constexpr auto SENSOR_CRI_LOW = "CriticalAlarmLow";
static constexpr auto SENSOR_CRI_HIGH = "CriticalAlarmHigh";
} // namespace sensor

namespace fru
{
static constexpr auto FRU_OBJPATH = "/xyz/openbmc_project/";
static constexpr auto FRU_IFACE = "xyz.openbmc_project.Ipmb.FruDevice";
static constexpr auto FRU_PROP = "PRODUCT_PRODUCT_NAME";
} // namespace fru

/** @class Status
 *  @brief Implementation of LED handling during the change of the
 *         Position property of the Host Selector interface
 *
 *  @details This implements methods for monitoring Host Selector
 *           interface of Buttons D-Bus object and then update
 *           the corresponding LED D-Bus objects
 */
class Status
{
  public:
    /** @brief Constructs a class for HostSelector
     *
     *  @param[in] bus -  D-Bus object
     */

    Status()
    {
        ledHandler();
    }

  private:
    /* @brief DBusHandler class handles the D-Bus operations */
    DBusHandler dBusHandler;

    /** @brief Get property value(type: variant)
     *
     *  @param[in] objectPath       -   D-Bus object path
     *  @param[in] interface        -   D-Bus interface
     *  @param[in] propertyName     -   D-Bus property name
     *
     *  @return The value of the property(type: variant)
     */
    template <typename T>
    T getPropertyValue(const std::string& objectPath,
                       const std::string& interface,
                       const std::string& propertyName);

    /** @brief Get the power status of the given host.
     *
     *  @param[in]  position      -  Host selector position
     */
    PowerState powerStatus(const std::string& pos);

    /** @brief Get the health status of the given host.
     *
     *  @param[in]  position      -  Host selector position
     */
    std::string healthStatus(const std::string& pos);

    /** @brief Set the appropriate action to be updated on physical LED.
     *
     *  @param[in]  objectPath   -  D-Bus object path
     *  @param[in]  action       -  Action to be updated
     *  @param[in]  dutyOn       -  Duty Cycle ON percentage
     *  @param[in]  period       -  Time taken for one blink cycle (ms)
     */
    void setPhysicalLed(const std::string& objectPath, Action action,
                        DutyOn dutyOn, Period period);

    /** @brief Select the appropriate dutyOn and period to be updated
     *         on physical LED Blink.
     *
     *  @param[in]  position      -  Host selector
     *  @param[in]  powerStatus   -  Power status of the host
     *  @param[in]  healthStatus  -  Health status of the host
     */
    void selectPhysicalLed(const std::string& host, PowerState powerStatus,
                           const std::string& healthStatus);

    /** @brief Select the appropriate action to be updated on physical
     *         LED for all slots.
     *
     *  @param[in]  position      -  Host selector position
     *  @param[in]  powerStatus   -  Power status of the host
     *  @param[in]  healthStatus  -  Health status of the host
     */
    void ledSlot(const std::string& pos, PowerState power,
                 const std::string& health);

    /* @brief This function will monitor the debugcard host selection
     *        and based on the host, it will set the physical led.
     */
    void ledHandler();
};

} // namespace status
} // namespace purpose
} // namespace multi
} // namespace led
} // namespace phosphor
