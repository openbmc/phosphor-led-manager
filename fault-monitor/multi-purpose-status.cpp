#include "multi-purpose-status.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Chassis/Buttons/HostSelector/server.hpp>
#include <xyz/openbmc_project/Led/Physical/server.hpp>

#include <algorithm>
#include <fstream>
#include <string>

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

using namespace sdbusplus::xyz::openbmc_project::Led::server;
using namespace sdbusplus::xyz::openbmc_project::Chassis::Buttons::server;

static constexpr size_t bmcPosition = 0;

template <typename T>
T Status::getPropertyValue(const std::string& objectPath,
                           const std::string& interface,
                           const std::string& propertyName)
{
    PropertyValue propertyValue{};
    try
    {
        propertyValue =
            dBusHandler.getProperty(objectPath, interface, propertyName);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Failed to get property", entry("ERROR=%s", e.what()),
                        entry("PATH=%s", objectPath.c_str()));
        throw std::runtime_error("Failed to get property value");
    }

    return std::get<T>(propertyValue);
}

PowerState Status::powerStatus(const std::string& hostSelection)
{
    PowerState powerStatus;

    auto power = getPropertyValue<std::string>(
        power::POWER_STATE_OBJPATH + hostSelection, power::POWER_STATE_IFACE,
        power::POWER_STATE_PROPERTY);

    if (power == "xyz.openbmc_project.State.Chassis.PowerState.On")
    {
        powerStatus = PowerState::On;
    }
    else if (power == "xyz.openbmc_project.State.Chassis.PowerState.Off")
    {
        powerStatus = PowerState::Off;
    }
    else
    {
        log<level::ERR>("Failed to get property of powerstate",
                        entry("PATH=%s", power::POWER_STATE_OBJPATH));
    }
    return powerStatus;
}

std::string Status::healthStatus(const std::string& hostSelection)
{
    std::string healthStatus = "Good";
    auto sensorPath = dBusHandler.getSubTreePaths(sensor::SENSOR_OBJPATH,
                                                  sensor::SENSOR_THRES_IFACE);

    if (sensorPath.empty())
    {
        log<level::INFO>("Failed to get sensor paths");
        return "Bad";
    }
    std::string ser = "/" + hostSelection + "_";

    for (auto& sensor : sensorPath)
    {
        if (sensor.find(ser) != std::string::npos)
        {
            auto low = getPropertyValue<bool>(
                sensor, sensor::SENSOR_THRES_IFACE, sensor::SENSOR_CRI_LOW);
            auto high = getPropertyValue<bool>(
                sensor, sensor::SENSOR_THRES_IFACE, sensor::SENSOR_CRI_HIGH);
            if (low || high)
            {
                healthStatus = "Bad";
            }
        }
    }
    return healthStatus;
}

void Status::setPhysicalLed(const std::string& objPath, Action action,
                            DutyOn dutyOn = 0, Period period = 0)
{
    try
    {
        if (action == Action::Blink)
        {
            PropertyValue dutyOnValue{dutyOn};
            PropertyValue periodValue{period};

            dBusHandler.setProperty(objPath, led::PHY_LED_IFACE, "DutyOn",
                                    dutyOnValue);
            dBusHandler.setProperty(objPath, led::PHY_LED_IFACE, "Period",
                                    periodValue);
        }

        std::string ledAction;

        if (action == Action::On)
        {
            ledAction = convertForMessage(Physical::Action::On);
        }
        else if (action == Action::Blink)
        {
            ledAction = convertForMessage(Physical::Action::Blink);
        }
        else
        {
            ledAction = convertForMessage(Physical::Action::Off);
        }

        PropertyValue actionValue{ledAction};
        dBusHandler.setProperty(objPath, led::PHY_LED_IFACE, "State",
                                actionValue);
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Error setting property for physical LED",
                        entry("ERROR=%s", e.what()),
                        entry("OBJECT_PATH=%s", objPath.c_str()));
    }
}

void Status::selectPhysicalLed(const std::string& host, PowerState powerStatus,
                               const std::string& healthStatus)
{
    switch (powerStatus)
    {
        case PowerState::On:
            if (healthStatus == "Good")
            {
                setPhysicalLed(led::SYSTEM_LED + host, Action::Off);
                setPhysicalLed(led::POWER_LED + host, Action::Blink, 90, 900);
            }
            else
            {
                setPhysicalLed(led::POWER_LED + host, Action::Off);
                setPhysicalLed(led::SYSTEM_LED + host, Action::Blink, 90, 900);
            }
            break;

        case PowerState::Off:
            if (healthStatus == "Good")
            {
                setPhysicalLed(led::SYSTEM_LED + host, Action::Off);
                setPhysicalLed(led::POWER_LED + host, Action::Blink, 10, 100);
            }
            else
            {
                setPhysicalLed(led::POWER_LED + host, Action::Off);
                setPhysicalLed(led::SYSTEM_LED + host, Action::Blink, 10, 100);
            }
            break;

        default:
            log<level::ERR>("Error in selecting physical LED");
            return;
    }
}

void Status::ledSlot(const std::string& host, PowerState power,
                     const std::string& health)
{
    if (power == PowerState::On)
    {
        if (health == "Good")
        {
            setPhysicalLed(led::POWER_LED + host, Action::On);
            setPhysicalLed(led::SYSTEM_LED + host, Action::Off);
        }
        else
        {
            setPhysicalLed(led::POWER_LED + host, Action::Off);
            setPhysicalLed(led::SYSTEM_LED + host, Action::On);
        }
    }
    else
    {
        setPhysicalLed(led::POWER_LED + host, Action::Off);
        setPhysicalLed(led::SYSTEM_LED + host, Action::Off);
    }
}

void Status::ledHandler()
{
    size_t maxPos = getPropertyValue<size_t>(hostselector::SELECTOR_OBJPATH,
                                             hostselector::SELECTOR_INTERFACE,
                                             hostselector::SELECTOR_MAXPOS);
    while (true)
    {

        /** When sled is identified by BMC, it will trigger all the system
         *  LEDs to blink at 20% DutyCycle with 200 ms On/Off, while keeping
         *  all the power LEDs in Off state irrespective of power and health
         *  status.
         */

        bool sledIdentify = getPropertyValue<bool>(
            led::SLED_DBUS_PATH, led::GROUP_LED_IFACE, led::SLED_PROP);

        if (sledIdentify)
        {
            for (size_t slotPos = 1; slotPos <= maxPos; slotPos++)
            {
                setPhysicalLed(led::POWER_LED + std::to_string(slotPos),
                               Action::Off);
                setPhysicalLed(led::SYSTEM_LED + std::to_string(slotPos),
                               Action::Blink, 20, 200);
            }
            continue;
        }

        /** Based on the DebugCard HostSelector position property, LEDs
         *  will be triggered. DebugCard position property varies from
         *  position 0 to 4.
         *
         *      Position 0 - BMC
         *      Position 1 - Host 1
         *      Position 2 - Host 2
         *      Position 3 - Host 3
         *      Position 4 - Host 4
         *
         *  For the BMC position, all the power LEDs will be blinking at
         *  50% DutyCycle (500 ms On/Off), while keeping all the system
         *  LEDs in Off state irrespective of power and health status.
         *
         *  For HostSelector position from 1 to 4, the specified host's
         *  power status will be checked and also health status of the
         *  host will be  checked using sensor threshold status.
         *
         *  Based on the power and health status of each host, the selected
         *  host's LED be On/Off/Blink at different DutyCycle and period.
         */

        size_t pos = getPropertyValue<size_t>(hostselector::SELECTOR_OBJPATH,
                                              hostselector::SELECTOR_INTERFACE,
                                              hostselector::SELECTOR_POSITION);
        if (pos == bmcPosition)
        {
            for (size_t slotPos = 1; slotPos <= maxPos; slotPos++)
            {
                setPhysicalLed(led::SYSTEM_LED + std::to_string(slotPos),
                               Action::Off);
                setPhysicalLed(led::POWER_LED + std::to_string(slotPos),
                               Action::Blink, 50, 500);
            }
            continue;
        }

        PowerState powerState;
        std::string healthState;

        for (size_t slot = 1; slot <= maxPos; slot++)
        {
            std::string slotNum = std::to_string(slot);
            std::string str = "_" + std::to_string(slot - 1);

            auto fruPath =
                dBusHandler.getSubTreePaths(fru::FRU_OBJPATH, fru::FRU_IFACE);

            if (fruPath.empty())
            {
                log<level::INFO>("Failed to get FRU paths");
                continue;
            }
            for (auto& fru : fruPath)
            {
                if (fru.find(str) != std::string::npos)
                {
                    auto prop = getPropertyValue<std::string>(
                        fru, fru::FRU_IFACE, fru::FRU_PROP);

                    if (prop.empty())
                    {
                        powerState = PowerState::Off;
                        healthState = "Good";
                    }
                    else
                    {
                        powerState = powerStatus(slotNum);
                        healthState = healthStatus(slotNum);
                    }
                }
            }
            if (pos == slot)
            {
                selectPhysicalLed(slotNum, powerState, healthState);
            }
            else
            {
                ledSlot(slotNum, powerState, healthState);
            }
        }
    }
}

} // namespace status
} // namespace purpose
} // namespace multi
} // namespace led
} // namespace phosphor
