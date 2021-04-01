#include "operational-status-monitor.hpp"

#include <phosphor-logging/elog.hpp>

namespace phosphor
{
namespace led
{
namespace Operational
{
namespace status
{
namespace monitor
{
using namespace phosphor::logging;

void Monitor::matchHandler(sdbusplus::message::message& msg)
{
    // Get the ObjectPath of the `xyz.openbmc_project.Inventory.Manager`
    // service
    std::string invObjectPath = msg.get_path();

    // Get all the properties of
    // "xyz.openbmc_project.State.Decorator.OperationalStatus" interface
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
        std::vector<std::string> ledGroupPath = getLedGroupPath(invObjectPath);
        if (ledGroupPath.empty())
        {
            log<level::INFO>("The inventory D-Bus object is not associated "
                             "with the LED group D-Bus object.",
                             entry("INVENTORY_PATH=%s", invObjectPath.c_str()));
            return;
        }

        // Update the Asserted property by the Functional property value.
        updateAssertedProperty(ledGroupPath, *value);
    }
}

const std::vector<std::string>
    Monitor::getLedGroupPath(const std::string& inventoryPath) const
{
    // Get endpoints from the rType
    std::string faultLedAssociation = inventoryPath + "/fault_led_group";

    // endpoint contains the vector of strings, where each string is a Inventory
    // D-Bus object that this, associated with this LED Group D-Bus object
    // pointed to by fru_fault
    PropertyValue endpoint{};

    try
    {
        endpoint = dBusHandler.getProperty(faultLedAssociation,
                                           "xyz.openbmc_project.Association",
                                           "endpoints");
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Failed to get endpoints property",
                        entry("ERROR=%s", e.what()),
                        entry("PATH=%s", faultLedAssociation.c_str()));

        return {};
    }

    auto& endpoints = std::get<std::vector<std::string>>(endpoint);

    return endpoints;
}

void Monitor::updateAssertedProperty(
    const std::vector<std::string>& ledGroupPath, bool value)
{
    for (const auto& path : ledGroupPath)
    {
        try
        {
            // Call "Group Asserted --> true" if the value of Functional is
            // false Call "Group Asserted --> false" if the value of Functional
            // is true
            PropertyValue assertedValue{!value};
            dBusHandler.setProperty(path, "xyz.openbmc_project.Led.Group",
                                    "Asserted", assertedValue);
        }
        catch (const sdbusplus::exception::SdBusError& e)
        {
            log<level::ERR>("Failed to set Asserted property",
                            entry("ERROR=%s", e.what()),
                            entry("PATH=%s", path.c_str()));
        }
    }
}
} // namespace monitor
} // namespace status
} // namespace Operational
} // namespace led
} // namespace phosphor
