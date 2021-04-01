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

void OperStatusSingal::processHandle(sdbusplus::message::message& msg)
{
    // Get the ObjectPath of the `xyz.openbmc_project.Inventory.Manager`
    // service
    std::string objectPath = msg.get_path();

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
            log<level::ERR>("Faild to get the Functional property");
            return;
        }

        // See if the Inventory D-Bus object has an association with LED groups
        // D-Bus object.
        std::string ledGroupPath = getLedGroupPath(objectPath);
        if (ledGroupPath.empty())
        {
            log<level::INFO>("The inventory D-Bus object is not associated "
                             "with the LED group D-Bus object.\n");
            return;
        }

        // Update the Asserted proerpty by the Functional property value.
        updateAsserted(ledGroupPath, *value);
    }
}

const std::string OperStatusSingal::getLedGroupPath(const std::string& path)
{
    using AssociationList =
        std::vector<std::tuple<std::string, std::string, std::string>>;

    constexpr auto propertyIface = "org.freedesktop.DBus.Properties";
    constexpr auto serviceName = "xyz.openbmc_project.Inventory.Manager";
    constexpr auto interfaceName =
        "xyz.openbmc_project.Association.Definitions";
    constexpr auto propertyName = "Associations";
    std::string objectPath{};

    std::variant<AssociationList> associations;
    try
    {
        auto method = bus.new_method_call(serviceName, path.c_str(),
                                          propertyIface, "Get");
        method.append(interfaceName, propertyName);

        auto reply = bus.call(method);
        reply.read(associations);
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Failed to get the Associations property",
                        entry("ERROR=%s", e.what()));
        return objectPath;
    }

    auto& assocs = std::get<AssociationList>(associations);
    if (assocs.empty())
    {
        return objectPath;
    }

    for (const auto& assoc : assocs)
    {
        const auto& [forward, reverse, endpoint] = assoc;
        if (forward == "fault_led_group")
        {
            objectPath = endpoint;
            break;
        }
    }

    return objectPath;
}

void OperStatusSingal::updateAsserted(const std::string& path, bool value)
{
    constexpr auto propertyIface = "org.freedesktop.DBus.Properties";
    constexpr auto serviceName = "xyz.openbmc_project.LED.GroupManager";
    constexpr auto interfaceName = "xyz.openbmc_project.Led.Group";
    constexpr auto propertyName = "Asserted";

    std::variant<bool> asserted{!value};
    try
    {
        auto method = bus.new_method_call(serviceName, path.c_str(),
                                          propertyIface, "Set");
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
