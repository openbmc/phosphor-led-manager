#pragma once
#include <sdbusplus/server.hpp>

#include <map>
#include <vector>
namespace phosphor
{
namespace utils
{
constexpr auto MAPPER_BUSNAME = "xyz.openbmc_project.ObjectMapper";
constexpr auto MAPPER_OBJ_PATH = "/xyz/openbmc_project/object_mapper";
constexpr auto MAPPER_IFACE = "xyz.openbmc_project.ObjectMapper";
constexpr auto DBUS_PROPERTY_IFACE = "org.freedesktop.DBus.Properties";

struct DBusMapping
{
    std::string objectPath;   //!< D-Bus object path
    std::string interface;    //!< D-Bus interface
    std::string propertyName; //!< D-Bus property name
};

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
     *  @brief Get the D-BUS Service name for the input D-Bus path
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
    PropertyValue getDbusPropertyVariant(const char* objPath,
                                         const char* dbusProp,
                                         const char* dbusInterface);

    /** @brief Get sub tree paths by the path and interface of the DBus.
     *
     *  @param[in]  objPath   -  D-BUS object path
     *  @param[in]  intf      -  D-BUS object interface
     *  @param[in]  dep       -  depth, default is 0 to search all
     *
     *  @return Paths vector of subtree paths
     */
    Paths getSubTreePaths(const std::string& objPath,
                          const std::vector<std::string>& intfs, int depth = 0);

    /** @brief Get sub tree by the path and interface of the DBus.
     *
     *  @param[in]  objPath   -  D-BUS object path
     *  @param[in]  intf      -  D-BUS object interface
     *  @param[in]  dep       -  depth, default is 0 to search all
     *
     *  @return ObjectTree map of subtree
     */
    ObjectTree getSubTree(const std::string& objPath,
                          const std::vector<std::string>& intfs, int depth = 0);

    /** @brief Set D-BUS property
     *
     *  @param[in] dBusMap - Object path, property name, interface and property
     *                       type for the D-Bus object
     *  @param[in] value - The value to be set
     *
     *  @throw sdbusplus::exception::SdBusError when it fails
     */
    void setDbusProperty(const DBusMapping& dBusMap,
                         const PropertyValue& value);
};

} // namespace utils
} // namespace phosphor
