#pragma once

#include "../utils.hpp"

#include <phosphor-logging/elog.hpp>

#include <algorithm>
#include <fstream>

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
    Status()
    {
        loadConfigValues();
    }

  private:
    /* @brief sdbusplus signal matches for Monitor */
    std::unique_ptr<sdbusplus::bus::match_t> matchSignal;

    /* @brief DBusHandler class handles the D-Bus operations */
    DBusHandler dBusHandler;

    /* @brief This method gets called when the led selection is SLED */
    void sled();

    /* @brief This method gets called when the led selection is DebugCard */
    void DebugCard(uint16_t position);

    /** @brief Get property(type: variant)
     *
     *  @param[in] objectPath       -   D-Bus object path
     *  @param[in] interface        -   D-Bus interface
     *  @param[in] propertyName     -   D-Bus property name
     *
     *  @return The value of the property(type: variant)
     */
    const PropertyValue getPropertyValue(const std::string& objectPath,
                                         const std::string& interface,
                                         const std::string& propertyName);

    /** @brief Set the Asserted property of the LED Group Manager.
     *
     *  @param[in] ledGroupPaths   - LED Group D-Bus object Paths
     *  @param[in] value           - The Asserted property value, True / False
     *
     */
    void setLedGroup(const std::string& objectPath, bool value);

    /** @brief Set the appropriate action to be updated on physical LED.
     *
     *  @param[in]  objectPath   -  D-Bus object path
     *  @param[in]  action       -  Action to be updated
     *  @param[in]  dutyOn       -  Duty Cycle ON percentage
     *  @param[in]  period       -  Time taken for one blink cycle
     */
    void setPhysicalLed(const std::string& objectPath,
                        const std::string& action, uint8_t dutyOn,
                        uint16_t period);

    /** @brief Select the appropriate action to be updated on physical LED.
     *
     *  @param[in]  position      -  Host selector
     *  @param[in]  powerStatus   -  Power status of the host
     *  @param[in]  healthStatus  -  Health status of the host
     */
    void selectPhysicalLed(uint16_t host, const std::string& powerStatus,
                           const std::string& healthStatus);

    /* @brief Load JSON config and return led selection.*/
    void loadConfigValues();

    /* @brief sdbusplus signal monitoring for hostSelection */
    void matchHandler(const std::string& config);
};

} // namespace status
} // namespace purpose
} // namespace multi
} // namespace led
} // namespace phosphor
