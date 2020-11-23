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
     * Constructs timer and when the timeout occurs, the stopLampTest
     * method is called back to stop timer and lamp test.
     *
     * @param[in] event   - sd event handler
     * @param[in] manager - manage group of LEDs
     */
    LampTest(const sdeventplus::Event& event, Manager& manager) :
        timer(event, std::bind(std::mem_fn(&LampTest::lampTestTimeOut), this)),
        manager(manager)
    {}

    /** @brief the lamp test handler
     *
     *  @param[in]  value    -  true: start lamptest
     *                          false: stop lamptest
     *  @return
     */
    void lampTestHandler(bool value);

  private:
    /** @brief Timer used for LEDs lamp test with seconds */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> timer;

    /** @brief Reference to Manager object */
    Manager& manager;

    /** @brief  Sart lamp test. */
    void startLampTest();

    /** @brief  Stop lamp test. */
    void stopLampTest();

    /** @brief  The lamp test timeout. */
    void lampTestTimeOut();
};

} // namespace led
} // namespace phosphor
