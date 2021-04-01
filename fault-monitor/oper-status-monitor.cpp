#include "oper-status-monitor.hpp"

#include <phosphor-logging/elog.hpp>

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
using namespace phosphor::logging;

static constexpr auto MAPPER_BUSNAME = "xyz.openbmc_project.ObjectMapper";
static constexpr auto MAPPER_OBJ_PATH = "/xyz/openbmc_project/object_mapper";
static constexpr auto MAPPER_IFACE = "xyz.openbmc_project.ObjectMapper";

// Get service name
const std::string Monitor::getService(const std::string& path,
                                      const std::string& interface) const
{
    using InterfaceList = std::vector<std::string>;
    std::map<std::string, std::vector<std::string>> mapperResponse;

    auto mapper = bus.new_method_call(MAPPER_BUSNAME, MAPPER_OBJ_PATH,
                                      MAPPER_IFACE, "GetObject");
    mapper.append(path, InterfaceList({interface}));

    auto mapperResponseMsg = bus.call(mapper);
    if (mapperResponseMsg.is_method_error())
    {
        log<level::ERR>("Failed to invoke ObjectMapper method",
                        entry("OBJECT_PATH=%s", path.c_str()),
                        entry("INTERFACE=%s", interface.c_str()));
        return "";
    }

    mapperResponseMsg.read(mapperResponse);
    if (mapperResponse.empty())
    {
        log<level::ERR>("Failed to read getService mapper response",
                        entry("OBJECT_PATH=%s", path.c_str()),
                        entry("INTERFACE=%s", interface.c_str()));
        return "";
    }

    // the value here will be the service name
    return mapperResponse.cbegin()->first;
}

void Monitor::matchHandler(sdbusplus::message::message& msg)
{
    // Get the ObjectPath of the `xyz.openbmc_project.Inventory.Manager`
    // service
    std::string invObjectPath = msg.get_path();

    // Get all properties of the `xyz.openbmc_project.State.Decorator` interface
    std::string interfaceName{};
    std::map<std::string, std::variant<bool>> properties;
    msg.read(interfaceName, properties);

    const auto it = properties.find("Functional");
    if (it != properties.end())
    {
        const bool* value = std::get_if<bool>(&it->second);
        if (!value)
        {
            log<level::ERR>("Faild to get the Functional property",
                            entry("INVENTORY_PATH=%s", invObjectPath.c_str()));
            return;
        }

        // See if the Inventory D-Bus object has an association with LED groups
        // D-Bus object.
        std::string ledGroupPath = getLedGroupPath(invObjectPath);
        if (ledGroupPath.empty())
        {
            log<level::INFO>("The inventory D-Bus object is not associated "
                             "with the LED group D-Bus object.",
                             entry("INVENTORY_PATH=%s", invObjectPath.c_str()));
            return;
        }

        // Update the Asserted proerpty by the Functional property value.
        updateAssertedProperty(ledGroupPath, *value);
    }
}

const std::string
    Monitor::getLedGroupPath(const std::string& inventoryPath) const
{
    constexpr auto serviceName = "xyz.openbmc_project.ObjectMapper";
    constexpr auto propertyIface = "org.freedesktop.DBus.Properties";
    constexpr auto interfaceName = "xyz.openbmc_project.Association";
    constexpr auto propertyName = "endpoints";

    // Return led group path
    std::string ledGroupPath{};
    std::variant<std::vector<std::string>> endpoint{};
    std::string faultLedGroupPath = inventoryPath + "/fault_led_group";
    try
    {
        auto method = bus.new_method_call(
            serviceName, faultLedGroupPath.c_str(), propertyIface, "Get");
        method.append(interfaceName, propertyName);

        auto reply = bus.call(method);
        reply.read(endpoint);
    }
    catch (const std::exception& e)
    {
        log<level::ERR>(
            "Failed to get the Associations property",
            entry("FAULT_LED_GROUP_PATH=%s", faultLedGroupPath.c_str()),
            entry("ERROR=%s", e.what()));
        return ledGroupPath;
    }
    auto& endpoints = std::get<std::vector<std::string>>(endpoint);
    if (endpoints.empty())
    {
        return ledGroupPath;
    }

    for (const auto& fruInstancePath : endpoints)
    {
        ledGroupPath = fruInstancePath;
        break;
    }

    return ledGroupPath;
}

void Monitor::updateAssertedProperty(const std::string& ledGroupPath,
                                     bool value)
{
    constexpr auto propertyIface = "org.freedesktop.DBus.Properties";
    constexpr auto interfaceName = "xyz.openbmc_project.Led.Group";
    constexpr auto propertyName = "Asserted";

    auto serviceName = getService(ledGroupPath, interfaceName);
    if (serviceName.empty())
    {
        log<level::ERR>("Failed to get service",
                        entry("LED_GROUP_PATH=%s", ledGroupPath.c_str()));

        return;
    }

    // Call "Group Asserted --> true" if the value of Functional is false
    // Call "Group Asserted --> false" if the value ofFunctional is true
    std::variant<bool> asserted{!value};
    try
    {
        auto method = bus.new_method_call(
            serviceName.c_str(), ledGroupPath.c_str(), propertyIface, "Set");
        method.append(interfaceName, propertyName, asserted);

        bus.call_noreply(method);
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Failed to set the Asserted property",
                        entry("ERROR=%s", e.what()));
    }
}
} // namespace monitor
} // namespace status
} // namespace oper
} // namespace led
} // namespace phosphor
