#pragma once

#include "group.hpp"
#include "manager.hpp"

#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>

#include <vector>

namespace phosphor
{
namespace led
{

using namespace std::chrono;

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
     * @param[in] event   - sd event handler
     * @param[in] manager - manage group of LEDs
     */
    LampTest(const sdeventplus::Event& event, Manager& manager) :
        timer(event, std::bind(std::mem_fn(&LampTest::stopLampTest), this)),
        manager(manager)
    {}

    /** @brief the lamp test handler
     *
     *  @param[in]  value    -  true: start lamptest, false: stop lamptest
     *  @return
     */
    void lampTestHandler(bool value);

  private:
    /** @brief Timer used for host transition with seconds */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> timer;

    /** @brief Reference to Manager object */
    Manager& manager;

    /** DBusHandler class handles the D-Bus operations */
    DBusHandler dBusHandler;

    /** @brief  Sart lamp test. */
    void startLampTest();

    /** @brief  Stop lamp test and timer. */
    void stopLampTest();

    /** @brief Set all the physical action to On for lamp test
     *
     *  @param[in]  action    -  Intended action to be triggered
     *  @return               -  Success or Failed
     */
    bool updatePhysicalAction(Layout::Action action);
};

} // namespace led
} // namespace phosphor
