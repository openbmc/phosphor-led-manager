#pragma once

#include "group.hpp"
#include "manager.hpp"

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
    LampTest(sdbusplus::bus::bus& bus, const sdeventplus::Event& event,
             Manager& manager) :
        bus(bus),
        event(event), manager(manager),
        timer(event, std::bind(&LampTest::lampTestTimeout, this)),
        systemdSignals(
            bus,
            sdbusRule::type::signal() + sdbusRule::member("LampTestInitiated") +
                sdbusRule::path("/xyz/openbmc_project/led/groups/bmc_booted") +
                sdbusRule::interface("xyz.openbmc_project.Led.Group"),
            std::bind(&LampTest::lampTestHandler, this, std::placeholders::_1))
    {}

  private:
    /** @brief sdbusplus handler */
    sdbusplus::bus::bus& bus;

    /** @brief sdbusplus event */
    const sdeventplus::Event& event;

    /** @brief Reference to Manager object */
    Manager& manager;

    /** @brief Timer used for host transition with seconds */
    sdeventplus::utility::Timer<sdeventplus::ClockId::RealTime> timer;

    /** @brief Used to subscribe to dbus systemd signals */
    sdbusplus::bus::match_t systemdSignals;

    /** @brief The handler method of lamp test
     *
     *  @param[in] msg - bus message
     */
    void lampTestHandler(sdbusplus::message::message& msg);

    /** @brief  Call back the restore function when timeout. */
    void lampTestTimeout();

    /** @brief Set all the physical action to On for lamp test */
    void updatePhysicalAction();
};

} // namespace led
} // namespace phosphor