#include "multi-purpose-status.hpp"

#include <nlohmann/json.hpp>
#include <phosphor-logging/elog.hpp>

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

using namespace phosphor::logging;

std::map<std::string, std::string> sledDbusNames = {{"SledObjpath", ""},
                                                    {"PowerLedOff", ""}};

std::map<std::string, std::string> debugCardDbusNames = {
    {"KnobSelectorObjpath", ""},  {"KnobSelectorInterface", ""},
    {"KnobSelectorProperty", ""}, {"PowerStatusObjpath", ""},
    {"PowerStatusInterface", ""}, {"PowerStatusProperty", ""},
    {"SensorObjpath", ""},        {"SensorInterface", ""},
    {"SensorCriticalLow", ""},    {"SensorCriticalHigh", ""},
    {"SledObjpath", ""},          {"LedBmcObjpath", ""},
    {"PowerLedOff", ""},          {"LedPowerOn", ""},
    {"LedPowerOff", ""},          {"LedSystemOn", ""},
    {"LedSystemOff", ""}};

void Status::selectPurpose(const std::string& purpose)
{
    if (purpose == "SLED")
    {
        setLedGroup(sledDbusNames["PowerLedOff"], "true");
        setLedGroup(sledDbusNames["SledObjPath"], "true");
    }
    else if (purpose == "DebugCard")
    {
        /* Get Position property */

        auto pos = getPropertyValue(debugCardDbusNames["KnobSelectorObjpath"],
                                    debugCardDbusNames["KnobSelectorInterface"],
                                    debugCardDbusNames["KnobSelectorProperty"]);
        uint16_t position = std::get<uint16_t>(pos);

        if (position == 5)
        {
            setLedGroup(debugCardDbusNames["SledObjpath"], "false");
            setLedGroup(debugCardDbusNames["LedBmcObjpath"], "true");
        }
        else
        {
            /* Get Power Status */

            auto power =
                getPropertyValue(debugCardDbusNames["PowerStatusObjpath"] +
                                     std::to_string(position),
                                 debugCardDbusNames["PowerStatusInterface"],
                                 debugCardDbusNames["PowerStatusProperty"]);
            std::string powerState = std::get<std::string>(power);
            std::string powerStatus = powerState.substr(45);

            std::string healthStatus = "Good";

            /* Get Sensor Status */

            auto sensorPath = dBusHandler.getSubTreePaths(
                debugCardDbusNames["SensorObjpath"],
                debugCardDbusNames["SensorInterface"]);
            std::string ser = std::to_string(position) + "_";
            for (auto& sensor : sensorPath)
            {
                if (sensor.find(ser) != std::string::npos)
                {
                    auto low = getPropertyValue(
                        sensor, debugCardDbusNames["SensorInterface"],
                        debugCardDbusNames["SensorCriticalLow"]);
                    bool sensorPropLow = std::get<bool>(low);

                    auto high = getPropertyValue(
                        sensor, debugCardDbusNames["SensorInterface"],
                        debugCardDbusNames["SensorCriticalHigh"]);
                    bool sensorPropHigh = std::get<bool>(high);

                    if (sensorPropLow || sensorPropHigh)
                    {
                        healthStatus = "Bad";
                    }
                }
            }
            selectLedGroup(position, powerStatus, healthStatus);
        }
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
        dBusHandler.setProperty(objectPath, "xyz.openbmc_project.Led.Group",
                                "Asserted", value);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Failed to set Asserted property",
                        entry("ERROR=%s", e.what()),
                        entry("PATH=%s", objectPath.c_str()));
    }
}

void Status::selectLedGroup(uint16_t position, const std::string& powerStatus,
                            const std::string& healthStatus)
{

    if ((powerStatus == "On") && (healthStatus == "Good"))
    {
        setLedGroup(debugCardDbusNames["LedPowerOn"] + std::to_string(position),
                    "true");
    }
    else if ((powerStatus == "On") && (healthStatus == "Bad"))
    {
        setLedGroup(debugCardDbusNames["LedSystemOn"] +
                        std::to_string(position),
                    "true");
    }
    else if ((powerStatus == "Off") && (healthStatus == "Good"))
    {
        setLedGroup(debugCardDbusNames["LedPowerOff"] +
                        std::to_string(position),
                    "true");
    }
    else if ((powerStatus == "Off") && (healthStatus == "Bad"))
    {
        setLedGroup(debugCardDbusNames["LedSystemrOff"] +
                        std::to_string(position),
                    "true");
    }
}

void Status::loadConfigValues()
{
    const std::string configFilePath =
        "/usr/share/phosphor-led-manager/config.json";

    std::ifstream configFile(configFilePath.c_str());

    if (!configFile.is_open())
    {
        log<level::ERR>("loadConfigValues : Cannot open config path");
        return;
    }

    auto data = nlohmann::json::parse(configFile);

    auto ledPurpose = data["purpose"];

    for (auto& led : ledPurpose)
    {
        if (led == "SLED")
        {
            auto sledDbus = data["sled_dbus"];
            for (auto& name : sledDbusNames)
            {
                if (sledDbus.contains(name.first.c_str()))
                {
                    name.second = sledDbus[name.first];
                }
            }
        }
        else if (led == "DebugCard")
        {
            auto debugCardDbus = data["debugcard_dbus"];
            for (auto& name : debugCardDbusNames)
            {
                if (debugCardDbus.contains(name.first.c_str()))
                {
                    name.second = debugCardDbus[name.first];
                }
            }
        }
        else
        {
            throw std::runtime_error("Failed to select LED name");
        }
        selectPurpose(led);
    }
}

} // namespace status
} // namespace purpose
} // namespace multi
} // namespace led
} // namespace phosphor
