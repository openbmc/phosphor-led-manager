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

/** @class Status
 *  @brief Implementation of LED handling SLED and during the change of the
 * Position property of the Host Selector interface
 *
 *  @details This implements methods for watching Host Selector interface of
 *           Buttons D-Bus object and then update the corresponding LED
 *           D-Bus objects
 */
class Status
{
  public:
    /** @brief Constructs a watch for HostSelector
     *
     *  @param[in] bus -  D-Bus object
     */
    Status(sdbusplus::bus::bus& bus) : bus(bus)
    {
        loadConfigValues(bus);
    }

  private:
    /* @brief sdbusplus D-Bus connection */
    sdbusplus::bus::bus& bus;

    /* @brief sdbusplus signal matches for Monitor */
    std::unique_ptr<sdbusplus::bus::match_t> matchSignal;

    /* @brief DBusHandler class handles the D-Bus operations */
    DBusHandler dBusHandler;

    /* @brief This method gets called when the led selection is SLED */
    void sled();

    /* @brief This method gets called when the led selection is DebugCard */
    void DebugCard(size_t position);

    /** @brief Get property(type: variant)
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

    /** @brief Set the appropriate action to be updated on physical LED.
     *
     *  @param[in]  objectPath   -  D-Bus object path
     *  @param[in]  action       -  Action to be updated
     *  @param[in]  dutyOn       -  Duty Cycle ON percentage
     *  @param[in]  period       -  Time taken for one blink cycle (ms)
     */
    void setPhysicalLed(const std::string& objectPath, Action action,
                        DutyOn dutyOn, Period period);

    /** @brief Select the appropriate action to be updated on physical LED.
     *
     *  @param[in]  position      -  Host selector
     *  @param[in]  powerStatus   -  Power status of the host
     *  @param[in]  healthStatus  -  Health status of the host
     */

    void selectPhysicalLed(const std::string& host, PowerState powerStatus,
                           const std::string& healthStatus);

    /** @brief Load JSON config values and select the led config.
     *
     *  @param[in]  bus   - sdbusplus D-Bus connection
     */
    void loadConfigValues(sdbusplus::bus::bus& bus);

    /** @brief sdbusplus signal monitoring for hostSelection
     *
     *  @param[in]  bus      - sdbusplus D-Buds connection
     *  @param[in]  config   - led config from json file
     */
    void matchHandler(sdbusplus::bus::bus& bus, const std::string& config);
};

} // namespace status
} // namespace purpose
} // namespace multi
} // namespace led
} // namespace phosphor
