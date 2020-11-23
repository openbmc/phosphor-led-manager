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
 *  @brief Manager LampTest feature
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
     * method is called back to stop timer and also end the lamp test.
     *
     * @param[in] event   - sd event handler
     * @param[in] manager - manage group of LEDs
     */
    LampTest(const sdeventplus::Event& event, Manager& manager) :
        timer(event, std::bind(std::mem_fn(&LampTest::timeOutHandler), this)),
        manager(manager), groupObj(NULL)
    {}

    /** @brief the lamp test request handler
     *
     *  @param[in]  group    -  Point to Group object
     *  @param[in]  value    -  true: start lamptest
     *                          false: stop lamptest
     *  @return
     */
    void requestHandler(Group* group, bool value);

  private:
    /** @brief Timer used for LEDs lamp test period */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> timer;

    /** @brief Reference to Manager object */
    Manager& manager;

    /** DBusHandler class handles the D-Bus operations */
    DBusHandler dBusHandler;

    /** Is the lamp test on */
    bool isRunning;

    /** @brief Point to Group object */
    Group* groupObj;

    /** @brief  Sart lamp test. */
    void reStartLampTest();

    /** @brief  Stop lamp test. */
    void stopLampTest();

    /** @brief  The lamp test timeout. */
    void timeOutHandler();
};

} // namespace led
} // namespace phosphor
