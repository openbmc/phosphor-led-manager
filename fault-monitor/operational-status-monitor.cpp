#include "operational-status-monitor.hpp"

#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/lg2.hpp>

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
            lg2::error(
                "Faild to get the Functional property, INVENTORY_PATH = {PATH}",
                "PATH", invObjectPath);
            return;
        }

        // See if the Inventory D-Bus object has an association with LED groups
        // D-Bus object.
        auto ledGroupPath = getLedGroupPaths(invObjectPath);
        if (ledGroupPath.empty())
        {
            lg2::info(
                "The inventory D-Bus object is not associated with the LED "
                "group D-Bus object. INVENTORY_PATH = {PATH}",
                "PATH", invObjectPath);
            return;
        }

        // Update the Asserted property by the Functional property value.
        updateAssertedProperty(ledGroupPath, *value);
    }
}

const std::vector<std::string>
    Monitor::getLedGroupPaths(const std::string& inventoryPath) const
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
    catch (const sdbusplus::exception::exception& e)
    {
        lg2::error(
            "Failed to get endpoints property, ERROR = {ERROR}, PATH = {PATH}",
            "ERROR", e, "PATH", faultLedAssociation);

        return {};
    }

    auto& endpoints = std::get<std::vector<std::string>>(endpoint);

    return endpoints;
}

void Monitor::updateAssertedProperty(
    const std::vector<std::string>& ledGroupPaths, bool value)
{
    for (const auto& path : ledGroupPaths)
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
        catch (const sdbusplus::exception::exception& e)
        {
            lg2::error(
                "Failed to set Asserted property, ERROR = {ERROR}, PATH = {PATH}",
                "ERROR", e, "PATH", path);
        }
    }
}
} // namespace monitor
} // namespace status
} // namespace Operational
} // namespace led
} // namespace phosphor
