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
// Eg: uint8_t : dutyOn, uint16_t : Period, std::string : Name
using PropertyValue = std::variant<uint8_t, uint16_t, std::string>;

/**
 * @brief The interface for DBusHandler
 */
class DBusHandlerInterface
{
  public:
    virtual ~DBusHandlerInterface() = default;

    virtual std::string getService(const char* path,
                                   const char* interface) const = 0;

    virtual PropertyValue getProperty(const char* objPath, const char* property,
                                      const char* interface) const = 0;

    virtual void setProperty(const std::string& objectPath,
                             const std::string& interface,
                             const std::string& propertyName,
                             const PropertyValue& value) const = 0;
};

/**
 *  @class DBusHandler
 *
 *  Wrapper class to handle the D-Bus calls
 *
 *  This class contains the APIs to handle the D-Bus calls.
 */
class DBusHandler : public DBusHandlerInterface
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
     *  @param[in] path      -  D-BUS object path
     *  @param[in] interface -  D-BUS Interface
     *
     *  @return std::string  -  the D-Bus service name
     *
     */
    std::string getService(const char* path,
                           const char* interface) const override;

    /** @brief Get property(type: variant)
     *
     *  @param[in] objectPath       -   D-Bus object path
     *  @param[in] propertyName     -   D-Bus property name
     *  @param[in] interface        -   D-Bus interface
     *
     *  @return The value of the property(type: variant)
     *
     *  @throw sdbusplus::exception::SdBusError when it fails
     */
    PropertyValue getProperty(const char* objectPath, const char* propertyName,
                              const char* interface) const override;

    /** @brief Set D-BUS property
     *
     *  @param[in] objectPath       -   D-Bus object path
     *  @param[in] interface        -   D-Bus interface
     *  @param[in] propertyName     -   D-Bus property name
     *  @param[in] value            -   The value to be set
     *
     *  @throw sdbusplus::exception::SdBusError when it fails
     */
    void setProperty(const std::string& objectPath,
                     const std::string& interface,
                     const std::string& propertyName,
                     const PropertyValue& value) const override;
};

} // namespace utils
} // namespace led
} // namespace phosphor
