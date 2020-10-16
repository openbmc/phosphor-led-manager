#pragma once
#include <sdbusplus/server.hpp>

#include <map>
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
using PropertyValue =
    std::variant<bool, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t,
                 uint64_t, double, std::string>;
using Paths = std::vector<std::string>;
using ObjectTree =
    std::map<std::string, std::map<std::string, std::vector<std::string>>>;

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
     *  @param[in] path - D-BUS object path
     *  @param[in] interface - D-BUS Interface
     *
     *  @return std::string - the D-Bus service name
     *
     *  @throw sdbusplus::exception::SdBusError when it fails
     */
    std::string getService(const char* path, const char* interface);

    /** @brief Get property(type: variant)
     *
     *  @param[in] objPath - The D-Bus object path
     *  @param[in] dbusProp - The property name to get
     *  @param[in] dbusInterface - The D-Bus interface
     *
     *  @return The value of the property(type: variant)
     *
     *  @throw sdbusplus::exception::SdBusError when it fails
     */
    PropertyValue getProperty(const char* objPath, const char* dbusProp,
                              const char* dbusInterface);

    /** @brief Get sub tree paths by the path and interface of the DBus.
     *
     *  @param[in]  objPath   -  D-BUS object path
     *  @param[in]  intf      -  D-BUS object interface
     *  @param[in]  dep       -  depth, default is 0 to search all
     *
     *  @return Paths vector of subtree paths
     *
     *  @throw sdbusplus::exception::SdBusError when it fails
     */
    Paths getSubTreePaths(const std::string& objPath,
                          const std::vector<std::string>& intfs, int depth = 0);

    /** @brief Get sub tree by the path and interface of the DBus.
     *
     *  @param[in]  objPath   -  D-BUS object path
     *  @param[in]  intf      -  D-BUS object interface
     *  @param[in]  depth     -  Default is 0 to search all
     *
     *  @return ObjectTree map of subtree
     *
     *  @throw sdbusplus::exception::SdBusError when it fails
     */
    ObjectTree getSubTree(const std::string& objPath,
                          const std::vector<std::string>& intfs, int depth = 0);

    /** @brief Set D-BUS property
     *
     *  @param[in] objectPath - D-Bus object path
     *  @param[in] interface - D-Bus interface
     *  @param[in] propertyName - D-Bus property name
     *  @param[in] value - The value to be set
     *
     *  @throw sdbusplus::exception::SdBusError when it fails
     */
    void setProperty(const std::string& objectPath,
                     const std::string& interface,
                     const std::string& propertyName,
                     const PropertyValue& value);
};

} // namespace utils
} // namespace led
} // namespace phosphor
