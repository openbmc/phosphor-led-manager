#pragma once

#include "physical.hpp"

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/interface.hpp>
#include <sdbusplus/vtable.hpp>

#include <unordered_map>

static constexpr auto busName = "xyz.openbmc_project.LED.Controller";
static constexpr auto ledPath = "/xyz/openbmc_project/led";
static constexpr auto physParent = "/xyz/openbmc_project/led/physical";
static constexpr auto internalInterface =
    "xyz.openbmc_project.Led.Sysfs.Internal";
static constexpr auto ledAddMethod = "AddLED";

namespace phosphor
{
namespace led
{
namespace sysfs
{
namespace interface
{

class InternalInterface
{
  public:
    InternalInterface() = delete;
    InternalInterface(const InternalInterface&) = delete;
    InternalInterface& operator=(const InternalInterface&) = delete;
    InternalInterface(InternalInterface&&) = delete;
    InternalInterface& operator=(InternalInterface&&) = delete;
    virtual ~InternalInterface() = default;

    /**
     *  @brief Construct a class to put object onto bus at a dbus path.
     *
     *  @param[in] bus  - D-Bus object.
     *  @param[in] path - D-Bus Path.
     */

    InternalInterface(sdbusplus::bus_t& bus, const char* path);

    /**
     *  @brief Implementation for the AddLed method to add
     *  the LED name to dbus path.
     *
     *  @param[in] name - LED name to add.
     */

    void addLED(const std::string& name);

    /**
     *  @brief Implementation for the RemoveLed method to remove
     *  the LED name to dbus path.
     *
     *  @param[in] name - LED name to remove.
     */

    void removeLED(const std::string& name);

    /** @brief Generates LED DBus name from LED description
     *
     *  @param[in] name      - LED description
     *  @return              - DBus LED name
     */

    static std::string getDbusName(const LedDescr& ledDescr);

  private:
    /**
     *  @brief  Unordered map to declare the sysfs LEDs
     */

    std::unordered_map<std::string, std::unique_ptr<phosphor::led::Physical>>
        leds;

    /**
     *  @brief sdbusplus D-Bus connection.
     */

    sdbusplus::bus_t& bus;

    /**
     *  @brief Systemd bus callback for the AddLed method.
     */

    static int addLedConfigure(sd_bus_message* msg, void* context,
                               sd_bus_error* error);

    /**
     *  @brief Systemd bus callback for the RemoveLed method.
     */

    static int removeLedConfigure(sd_bus_message* msg, void* context,
                                  sd_bus_error* error);

    /**
     *  @brief Systemd vtable structure that contains all the
     *  methods, signals, and properties of this interface with their
     *  respective systemd attributes
     */

    static const std::array<sdbusplus::vtable::vtable_t, 4> vtable;

    /**
     *  @brief Support for the dbus based instance of this interface.
     */

    sdbusplus::server::interface_t serverInterface;

    /**
     *   @brief Implementation to create a dbus path for LED.
     *
     *   @param[in] name - LED name.
     */

    void createLEDPath(const std::string& ledName);
};

} // namespace interface
} // namespace sysfs
} // namespace led
} // namespace phosphor
