#pragma once
#include <sdbusplus/server.hpp>

#include <unordered_map>
#include <vector>
namespace phosphor
{
namespace led
{
namespace utils
{
constexpr auto MAPPER_BUSNAME = "xyz.openbmc_project.ObjectMapper";
constexpr auto MAPPER_OBJ_PATH = "/xyz/openbmc_project/object_mapper";
constexpr auto MAPPER_IFACE = "xyz.openbmc_project.ObjectMapper";
constexpr auto DBUS_PROPERTY_IFACE = "org.freedesktop.DBus.Properties";

// The value of the property(type: variant, contains some basic types)
// Eg: uint8_t : dutyOn, uint16_t : Period, std::string : Name,
// std::vector<std::string> : endpoints, bool : Functional
using PropertyValue = std::variant<uint8_t, uint16_t, std::string,
                                   std::vector<std::string>, bool>;

// The name of the property
using DbusProperty = std::string;

// The Map to constructs all properties values of the interface
using PropertyMap = std::unordered_map<DbusProperty, PropertyValue>;

/**
 *  @class DBusHandler
 *
 *  Wrapper class to handle the D-Bus calls
 *
 *  This class contains the APIs to handle the D-Bus calls.
 */
class DBusHandler
{
  public:
    /** @brief Get the bus connection. */
    static auto& getBus()
    {
        static auto bus = sdbusplus::bus::new_default();
        return bus;
    }

    /**
     *  @brief Get service name by the path and interface of the DBus.
     *
     *  @param[in] path      -  D-Bus object path
     *  @param[in] interface -  D-Bus Interface
     *
     *  @return std::string  -  the D-Bus service name
     *
     */
    const std::string getService(const std::string& path,
                                 const std::string& interface) const;

    /** @brief Get All properties
     *
     *  @param[in] objectPath       -   D-Bus object path
     *  @param[in] interface        -   D-Bus interface
     *
     *  @return The Map to constructs all properties values
     *
     *  @throw sdbusplus::exception::exception when it fails
     */
    const PropertyMap getAllProperties(const std::string& objectPath,
                                       const std::string& interface) const;

    /** @brief Get property(type: variant)
     *
     *  @param[in] objectPath       -   D-Bus object path
     *  @param[in] interface        -   D-Bus interface
     *  @param[in] propertyName     -   D-Bus property name
     *
     *  @return The value of the property(type: variant)
     *
     *  @throw sdbusplus::exception::exception when it fails
     */
    const PropertyValue getProperty(const std::string& objectPath,
                                    const std::string& interface,
                                    const std::string& propertyName) const;

    /** @brief Set D-Bus property
     *
     *  @param[in] objectPath       -   D-Bus object path
     *  @param[in] interface        -   D-Bus interface
     *  @param[in] propertyName     -   D-Bus property name
     *  @param[in] value            -   The value to be set
     *
     *  @throw sdbusplus::exception::exception when it fails
     */
    void setProperty(const std::string& objectPath,
                     const std::string& interface,
                     const std::string& propertyName,
                     const PropertyValue& value) const;

    /** @brief Get sub tree paths by the path and interface of the DBus.
     *
     *  @param[in]  objectPath   -  D-Bus object path
     *  @param[in]  interface    -  D-Bus object interface
     *
     *  @return std::vector<std::string> vector of subtree paths
     */
    const std::vector<std::string>
        getSubTreePaths(const std::string& objectPath,
                        const std::string& interface);
};

} // namespace utils
} // namespace led
} // namespace phosphor
