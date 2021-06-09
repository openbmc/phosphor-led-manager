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

std::map<std::string, std::string> dbusNames = {{"HostSelectorObjpath", ""},
                                                {"HostSelectorInterface", ""},
                                                {"HostSelectorProperty", ""},
                                                {"PowerStatusObjpath", ""},
                                                {"PowerStatusInterface", ""},
                                                {"PowerStatusProperty", ""},
                                                {"SensorObjpath", ""},
                                                {"SensorInterface", ""},
                                                {"SensorCriticalLow", ""},
                                                {"SensorCriticalHigh", ""},
                                                {"SledObjpath", ""},
                                                {"BmcObjpath", ""},
                                                {"PowerLed", ""},
                                                {"SystemLed", ""}};

void Status::sled()
{
    setLedGroup(dbusNames["BmcObjpath"], true);
    setLedGroup(dbusNames["SledObjpath"], true);
}

void Status::DebugCard()
{
    /* Get Position property */

    auto position = getPropertyValue(dbusNames["HostSelectorObjpath"],
                                dbusNames["HostSelectorInterface"],
                                dbusNames["HostSelectorProperty"]);
    uint16_t hostSelection = std::get<uint16_t>(position);

    if (hostSelection == BMC)
    {
        for (int pos = 1; pos < BMC; pos++)
        {
            setPhysicalLed(dbusNames["SystemLed"] + std::to_string(pos), "Off",
                           0, 0);
            setPhysicalLed(dbusNames["PowerLed"] + std::to_string(pos), "Blink",
                           50, 500);
        }
    }
    else
    {
        /* Get Power Status */

        auto power = getPropertyValue(dbusNames["PowerStatusObjpath"] +
                                          std::to_string(hostSelection),
                                      dbusNames["PowerStatusInterface"],
                                      dbusNames["PowerStatusProperty"]);

        std::string powerState = std::get<std::string>(power);
        std::string powerStatus = powerState.substr(45);

        std::string healthStatus = "Good";

        /* Get Sensor Status */

        auto sensorPath = dBusHandler.getSubTreePaths(
            dbusNames["SensorObjpath"], dbusNames["SensorInterface"]);
        std::string ser = "/" + std::to_string(hostSelection) + "_";

        for (auto& sensor : sensorPath)
        {
            if (sensor.find(ser) != std::string::npos)
            {
                auto low =
                    getPropertyValue(sensor, dbusNames["SensorInterface"],
                                     dbusNames["SensorCriticalLow"]);
                bool sensorPropLow = std::get<bool>(low);

                auto high =
                    getPropertyValue(sensor, dbusNames["SensorInterface"],
                                     dbusNames["SensorCriticalHigh"]);
                bool sensorPropHigh = std::get<bool>(high);

                if (sensorPropLow || sensorPropHigh)
                {
                    healthStatus = "Bad";
                }
            }
        }
        selectLedGroup(hostSelection, powerStatus, healthStatus);
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

void Status::selectLedGroup(uint16_t host, const std::string& powerStatus,
                            const std::string& healthStatus)
{
    if ((powerStatus == "On") && (healthStatus == "Good"))
    {
        setPhysicalLed(dbusNames["SystemLed"] + std::to_string(host), "Off",
                       0, 0);
        setPhysicalLed(dbusNames["PowerLed"] + std::to_string(host),
                       "Blink", 90, 900);
    }
    else if ((powerStatus == "On") && (healthStatus == "Bad"))
    {
        setPhysicalLed(dbusNames["PowerLed"] + std::to_string(host), "On",
                       0, 0);

        setPhysicalLed(dbusNames["SystemLed"] + std::to_string(host),
                       "Blink", 90, 900);
    }
    else if ((powerStatus == "Off") && (healthStatus == "Good"))
    {
        setPhysicalLed(dbusNames["SystemLed"] + std::to_string(host), "Off",
                       0, 0);

        setPhysicalLed(dbusNames["PowerLed"] + std::to_string(host),
                       "Blink", 10, 100);
    }
    else if ((powerStatus == "Off") && (healthStatus == "Bad"))
    {
        setPhysicalLed(dbusNames["PowerLed"] + std::to_string(host), "On",
                       0, 0);

        setPhysicalLed(dbusNames["SystemLed"] + std::to_string(host),
                       "Blink", 10, 100);
    }
}

std::string Status::loadConfigValues()
{
    const std::string configFilePath =
        "/usr/share/phosphor-led-manager/config.json";

    std::ifstream configFile(configFilePath.c_str());

    if (!configFile.is_open())
    {
        log<level::ERR>(" LoadConfigValues : Not able to open config file path");
        return 0;
    }

    auto data = nlohmann::json::parse(configFile);

    auto ledPurpose = data["purpose"];
    std::string opt;

    for (auto& led : ledPurpose)
    {
        opt = led;
        if (led == "DebugCard" || led == "SLED")
        {
            auto dbus = data["dbus_names"];
            for (auto& name : dbusNames)
            {
                if (dbus.contains(name.first.c_str()))
                {
                    name.second = dbus[name.first];
                }
            }
        }
        else
        {
            throw std::runtime_error("Failed to select LED name");
        }
    }
    return opt;
}

} // namespace status
} // namespace purpose
} // namespace multi
} // namespace led
} // namespace phosphor
