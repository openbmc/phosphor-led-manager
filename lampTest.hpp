#pragma once

#include "group.hpp"
#include "manager.hpp"
#include "serialize.hpp"

#include <sdbusplus/server.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>

#include <vector>

namespace phosphor
{
namespace led
{
namespace sdbusRule = sdbusplus::bus::match::rules;

using ObjectPaths = std::vector<std::string>;

/** @class LampTest
 *  @brief Initiate a Lamp Test request
 */
class LampTest
{
  public:
    LampTest() = delete;
    ~LampTest() = default;
    LampTest(const LampTest&) = delete;
    LampTest& operator=(const LampTest&) = delete;
    LampTest(LampTest&&) = default;
    LampTest& operator=(LampTest&&) = default;

    /** @brief Constructs LED LampTest
     *
     * @param[in] bus     - Handle to system dbus
     * @param[in] manager - Reference to Manager
     * @param[in] serialize - Serialize object for lamp test
     * @param[in] groups - map of led groups
     */
    LampTest(
        sdbusplus::bus::bus& bus, const sdeventplus::Event& event,
        Manager& manager, Serialize& serialize,
        std::map<std::string, std::unique_ptr<phosphor::led::Group>> groups) :
        bus(bus),
        event(event), manager(manager), serialize(serialize),
        timer(event, std::bind(&LampTest::lampTestTimeout, this)),
        systemdSignals(
            bus,
            sdbusRule::type::signal() + sdbusRule::member("LampTestInitiated") +
                sdbusRule::path("/xyz/openbmc_project/led/xxx") +
                sdbusRule::interface("xyz.openbmc_project.Led.LampTest"),
            std::bind(&LampTest::lampTestHandler, this, std::placeholders::_1)),
        groups(std::move(groups))
    {}

  private:
    /** @brief sdbusplus handler */
    sdbusplus::bus::bus& bus;

    /** @brief sdbusplus event */
    const sdeventplus::Event& event;

    /** @brief Reference to Manager object */
    Manager& manager;

    /** @brief The serialize class for storing and restoring groups of LEDs */
    Serialize& serialize;

    /** @brief Timer used for host transition with seconds */
    sdeventplus::utility::Timer<sdeventplus::ClockId::RealTime> timer;

    /** @brief Used to subscribe to dbus systemd signals */
    sdbusplus::bus::match_t systemdSignals;

    /** @brief map of led groups */
    std::map<std::string, std::unique_ptr<phosphor::led::Group>> groups;

    /** @brief  Call back the restore function when timeout. */
    void lampTestTimeout();

    /** @brief The handler method of lamp test
     *
     *  @param[in] msg - bus message
     */
    void lampTestHandler(sdbusplus::message::message& msg);

    /** @brief Set all the physical action to On for lamp test */
    void updatePhysicalAction();

    /** @brief Get all the LED physical paths */
    ObjectPaths getLedPhysicalPahts();

    /** @brief Get all the LED group paths */
    ObjectPaths getLedGroupPahts();

    /** @brief Get Asserted property from group path
     *
     *  @param[in] path   - dbus path of group
     *  @param[in] status - true(default) or false
     */
    void setAssertedStatus(const std::string& path, bool status = true);
};

} // namespace led
} // namespace phosphor
