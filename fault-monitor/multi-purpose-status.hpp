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
 *  @brief Implementation of LED handling SLED and during the change of the Position
 *         property of the Host Selector interface
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
        if (loadConfigValues() == "DebugCard")
        {
            matchSignal = std::make_unique<sdbusplus::bus::match_t>(
                bus,
                sdbusplus::bus::match::rules::type::signal() +
                    sdbusplus::bus::match::rules::member("PropertiesChanged") +
                    sdbusplus::bus::match::rules::path(
                        "/xyz/openbmc_project/Chassis/Buttons/Selector0") +
                    sdbusplus::bus::match::rules::interface(
                        "org.freedesktop.DBus.Properties"),
                [this](sdbusplus::message::message& msg) {
                    std::string objectName;
                    std::map<std::string, std::variant<uint16_t>> msgData;
                    msg.read(objectName, msgData);

                    auto valPropMap = msgData.find("Position");
                    {
                        if (valPropMap != msgData.end())
                        {
                            DebugCard();
                        }
                    }
                });
        }
        else
        {
            sled();
        }
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
    void DebugCard();

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
    void selectLedGroup(uint16_t host, const std::string& powerStatus,
                        const std::string& healthStatus);

    /** @brief Load JSON config and return led selection.
     *
     *  @return Led selection will be returned(type: string)
     */
    std::string loadConfigValues();
};

} // namespace status
} // namespace purpose
} // namespace multi
} // namespace led
} // namespace phosphor
