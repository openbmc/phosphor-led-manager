#include "multi-purpose-status.hpp"

#include <nlohmann/json.hpp>
#include <phosphor-logging/elog.hpp>

#include <fstream>
#include <string>

#define BMC 5

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

using namespace phosphor::logging;

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

void Status::sled()
{
    setLedGroup(BMC_OBJPATH, true);
    setLedGroup(SLED_OBJPATH, true);
}

void Status::DebugCard(uint16_t hostSelection)
{
    if (hostSelection == BMC)
    {
        for (int pos = 1; pos < BMC; pos++)
        {
            setPhysicalLed(SYSTEM_LED + std::to_string(pos), "Off", 0, 0);
            setPhysicalLed(POWER_LED + std::to_string(pos), "Blink", 50, 500);
        }
    }
    else
    {
        /* Get Power Status */

        auto power = getPropertyValue(POWER_STATE_OBJPATH +
                                          std::to_string(hostSelection),
                                      POWER_STATE_IFACE, POWER_STATE_PROPERTY);
        std::string* powerState = std::get_if<std::string>(&power);

        if (powerState == nullptr)
        {
            log<level::ERR>("Failed to get property of powerstate",
                            entry("PATH=%s", POWER_STATE_OBJPATH));
            return;
        }
        std::string powerStatus = powerState->substr(45);

        std::string healthStatus = "Good";

        /* Get Sensor Status */

        auto sensorPath =
            dBusHandler.getSubTreePaths(SENSOR_OBJPATH, SENSOR_THRES_IFACE);

        std::string ser = "/" + std::to_string(hostSelection) + "_";

        for (auto& sensor : sensorPath)
        {
            if (sensor.find(ser) != std::string::npos)
            {
                auto low = getPropertyValue(sensor, SENSOR_THRES_IFACE,
                                            SENSOR_CRI_LOW);
                bool sensorPropLow = std::get<bool>(low);

                auto high = getPropertyValue(sensor, SENSOR_THRES_IFACE,
                                             SENSOR_CRI_HIGH);
                bool sensorPropHigh = std::get<bool>(high);

                if (sensorPropLow || sensorPropHigh)
                {
                    healthStatus = "Bad";
                }
            }
        }
        selectPhysicalLed(hostSelection, powerStatus, healthStatus);
    }
}

const PropertyValue Status::getPropertyValue(const std::string& objectPath,
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
        return {};
    }

    return propertyValue;
}

void Status::setLedGroup(const std::string& objectPath, bool value)
{
    try
    {
        PropertyValue assertedValue{value};
        dBusHandler.setProperty(objectPath, GROUP_LED_IFACE, "Asserted",
                                assertedValue);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Failed to set Asserted property",
                        entry("ERROR=%s", e.what()),
                        entry("PATH=%s", objectPath.c_str()));
    }
}

void Status::setPhysicalLed(const std::string& objPath,
                            const std::string& action, uint8_t dutyOn,
                            uint16_t period)
{
    try
    {
        if (action == "Blink")
        {
            PropertyValue dutyOnValue{dutyOn};
            PropertyValue periodValue{period};

            dBusHandler.setProperty(objPath, PHY_LED_IFACE, "DutyOn",
                                    dutyOnValue);
            dBusHandler.setProperty(objPath, PHY_LED_IFACE, "Period",
                                    periodValue);
        }

        std::string ledAction =
            "xyz.openbmc_project.Led.Physical.Action." + action;

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

void Status::selectPhysicalLed(uint16_t host, const std::string& powerStatus,
                               const std::string& healthStatus)
{
    if ((powerStatus == "On") && (healthStatus == "Good"))
    {
        setPhysicalLed(SYSTEM_LED + std::to_string(host), "Off", 0, 0);

        setPhysicalLed(POWER_LED + std::to_string(host), "Blink", 90, 900);
    }
    else if ((powerStatus == "On") && (healthStatus == "Bad"))
    {
        setPhysicalLed(POWER_LED + std::to_string(host), "On", 0, 0);

        setPhysicalLed(SYSTEM_LED + std::to_string(host), "Blink", 90, 900);
    }
    else if ((powerStatus == "Off") && (healthStatus == "Good"))
    {
        setPhysicalLed(SYSTEM_LED + std::to_string(host), "Off", 0, 0);

        setPhysicalLed(POWER_LED + std::to_string(host), "Blink", 10, 100);
    }
    else if ((powerStatus == "Off") && (healthStatus == "Bad"))
    {
        setPhysicalLed(POWER_LED + std::to_string(host), "On", 0, 0);

        setPhysicalLed(SYSTEM_LED + std::to_string(host), "Blink", 10, 100);
    }
}

void Status::loadConfigValues()
{
    const std::string configFilePath =
        "/usr/share/phosphor-led-manager/config.json";

    std::ifstream configFile(configFilePath.c_str());

    if (!configFile.is_open())
    {
        log<level::ERR>(
            " LoadConfigValues : Not able to open config file path");
        return;
    }

    auto data = nlohmann::json::parse(configFile);

    auto ledPurpose = data["purpose"];

    for (auto& led : ledPurpose)
    {
        if (led == "DebugCard" || led == "Sled")
        {
            matchHandler(led);
        }
        else
        {
            throw std::runtime_error("Failed to select LED name");
        }
    }
}

void Status::matchHandler(const std::string& config)
{
    sdbusplus::bus::bus bus = sdbusplus::bus::new_default();

    if (config == "DebugCard")
    {
        matchSignal = std::make_unique<sdbusplus::bus::match_t>(
            bus,
            sdbusplus::bus::match::rules::propertiesChanged(
                "/xyz/openbmc_project/Chassis/Buttons/Selector0",
                "xyz.openbmc_project.Chassis.Buttons.Selector"),
            [this](sdbusplus::message::message& msg) {
                std::string objectName;
                std::map<std::string, std::variant<uint16_t>> msgData;
                msg.read(objectName, msgData);

                auto valPropMap = msgData.find("Position");
                {
                    if (valPropMap != msgData.end())
                    {
                        uint16_t value = std::get<uint16_t>(valPropMap->second);
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
