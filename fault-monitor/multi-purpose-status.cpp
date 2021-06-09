#include "multi-purpose-status.hpp"

#include <nlohmann/json.hpp>
#include <xyz/openbmc_project/Chassis/Buttons/Selector/server.hpp>
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

static constexpr auto PHY_LED_IFACE = "xyz.openbmc_project.Led.Physical";
static constexpr auto GROUP_LED_IFACE = "xyz.openbmc_project.Led.Group";

static constexpr auto POWER_STATE_OBJPATH =
    "/xyz/openbmc_project/state/chassis";
static constexpr auto POWER_STATE_IFACE = "xyz.openbmc_project.State.Chassis";
static constexpr auto POWER_STATE_PROPERTY = "CurrentPowerState";
static constexpr auto SENSOR_OBJPATH = "/xyz/openbmc_project/sensors";
static constexpr auto SENSOR_THRES_IFACE =
    "xyz.openbmc_project.Sensor.Threshold.Critical";
static constexpr auto SENSOR_CRI_LOW = "CriticalAlarmLow";
static constexpr auto SENSOR_CRI_HIGH = "CriticalAlarmHigh";
static constexpr auto SLED_OBJPATH = "/xyz/openbmc_project/led/groups/sled";
static constexpr auto BMC_OBJPATH = "/xyz/openbmc_project/led/groups/bmc";
static constexpr auto POWER_LED = "/xyz/openbmc_project/led/physical/power";
static constexpr auto SYSTEM_LED = "/xyz/openbmc_project/led/physical/system";

/** This function will be called when sled is identified, it will trigger all
    the system LEDs to blink at 20% DutyCycle with 200 ms On/Off, while
    keeping all the power LEDs in Off state.
 */
void Status::sled()
{
    for (int pos = 1; pos <= HOST_INSTANCES; pos++)
    {
        setPhysicalLed(POWER_LED + std::to_string(pos), Action::On, 0, 0);
        setPhysicalLed(SYSTEM_LED + std::to_string(pos), Action::Blink, 20,
                       200);
    }
}

/** This function will be called based on the DebugCard host position property.
    For every signal changes, the host position will be passed as argument.

    DebugCard position property will be from 1 to 5,
    Position 1 - Host 1
    Position 2 - Host 2
    Position 3 - Host 3
    Position 4 - Host 4
    Position 5 - BMC

    For BMC position, all the power LEDs will be blinking at 50% DutyCycle
    (500 ms On/Off), while keeping all the system LEDs in Off state.

    For Host position, the specified host's power status will be checked and
    also health status of the host will be checked using sensor threshold
    status.

    Based on the power and health status of each host, the specified host's
    LED be triggered at different DutyCycle and period.
 */
void Status::DebugCard(size_t position)
{
    std::cerr << " Host : " << HOST_INSTANCES << "\n";

    std::string hostSelection = std::to_string(position);

    if ((position - 1) == HOST_INSTANCES)
    {
        for (int pos = 1; pos <= HOST_INSTANCES; pos++)
        {
            setPhysicalLed(SYSTEM_LED + std::to_string(pos), Action::Off, 0, 0);
            setPhysicalLed(POWER_LED + std::to_string(pos), Action::Blink, 50,
                           500);
        }
        return;
    }

    /* Get Power Status */

    auto power =
        getPropertyValue<std::string>(POWER_STATE_OBJPATH + hostSelection,
                                      POWER_STATE_IFACE, POWER_STATE_PROPERTY);

    PowerState powerStatus;
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
                        entry("PATH=%s", POWER_STATE_OBJPATH));
        return;
    }

    /* Get Sensor Status */

    std::string healthStatus = "Good";

    auto sensorPath =
        dBusHandler.getSubTreePaths(SENSOR_OBJPATH, SENSOR_THRES_IFACE);

    std::string ser = "/" + hostSelection + "_";

    for (auto& sensor : sensorPath)
    {
        if (sensor.find(ser) != std::string::npos)
        {
            auto low = getPropertyValue<bool>(sensor, SENSOR_THRES_IFACE,
                                              SENSOR_CRI_LOW);

            auto high = getPropertyValue<bool>(sensor, SENSOR_THRES_IFACE,
                                               SENSOR_CRI_HIGH);
            if (low || high)
            {
                healthStatus = "Bad";
            }
        }
    }
    selectPhysicalLed(hostSelection, powerStatus, healthStatus);
}

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

void Status::setPhysicalLed(const std::string& objPath, Action action,
                            DutyOn dutyOn, Period period)
{
    try
    {
        if (action == Action::Blink)
        {
            PropertyValue dutyOnValue{dutyOn};
            PropertyValue periodValue{period};

            dBusHandler.setProperty(objPath, PHY_LED_IFACE, "DutyOn",
                                    dutyOnValue);
            dBusHandler.setProperty(objPath, PHY_LED_IFACE, "Period",
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
        dBusHandler.setProperty(objPath, PHY_LED_IFACE, "State", actionValue);
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
                setPhysicalLed(SYSTEM_LED + host, Action::Off, 0, 0);

                setPhysicalLed(POWER_LED + host, Action::Blink, 90, 900);
            }
            else
            {
                setPhysicalLed(POWER_LED + host, Action::On, 0, 0);

                setPhysicalLed(SYSTEM_LED + host, Action::Blink, 90, 900);
            }
            break;
        case PowerState::Off:
            if (healthStatus == "Good")
            {
                setPhysicalLed(SYSTEM_LED + host, Action::Off, 0, 0);

                setPhysicalLed(POWER_LED + host, Action::Blink, 10, 100);
            }
            else
            {
                setPhysicalLed(POWER_LED + host, Action::On, 0, 0);

                setPhysicalLed(SYSTEM_LED + host, Action::Blink, 10, 100);
            }
            break;

        default:
            log<level::ERR>("Error in selecting physical LED");
            return;
    }
}

void Status::loadConfigValues(sdbusplus::bus::bus& bus)
{
    static const auto configFilePath =
        "/usr/share/phosphor-led-manager/config.json";

    std::ifstream configFile(configFilePath);

    if (!configFile.is_open())
    {
        log<level::INFO>(
            " LoadConfigValues : Not able to open config file path");
        return;
    }

    auto data = nlohmann::json::parse(configFile);

    auto ledPurpose = data["purpose"];

    for (auto& led : ledPurpose)
    {
        if (led == "DebugCard" || led == "Sled")
        {
            matchHandler(bus, led);
        }
        else
        {
            throw std::runtime_error("Failed to select LED name");
        }
    }
}

void Status::matchHandler(sdbusplus::bus::bus& bus, const std::string& config)
{

    if (config == "DebugCard")
    {
        matchSignal = std::make_unique<sdbusplus::bus::match_t>(
            bus,
            sdbusplus::bus::match::rules::propertiesChanged(
                "/xyz/openbmc_project/Chassis/Buttons/Selector0",
                "xyz.openbmc_project.Chassis.Buttons.Selector"),
            [this](sdbusplus::message::message& msg) {
                std::string objectName;
                std::map<std::string, Selector::PropertiesVariant> msgData;
                msg.read(objectName, msgData);

                auto valPropMap = msgData.find("Position");
                {
                    if (valPropMap != msgData.end())
                    {
                        size_t value = std::get<size_t>(valPropMap->second);
                        DebugCard(value);
                    }
                }
            });
    }
    else
    {
        sled();
    }
}
} // namespace status
} // namespace purpose
} // namespace multi
} // namespace led
} // namespace phosphor
