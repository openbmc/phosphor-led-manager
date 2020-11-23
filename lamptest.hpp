#pragma once

#include "manager.hpp"

#include <sdeventplus/utility/timer.hpp>

#include <vector>

namespace phosphor
{
namespace led
{

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
     * Constructs timer and when the timeout occurs, the stop method is called
     * back to stop timer and also end the lamp test.
     *
     * @param[in] event   - sd event handler
     * @param[in] manager - reference to manager instance
     */
    LampTest(const sdeventplus::Event& event, Manager& manager) :
        timer(event, std::bind(std::mem_fn(&LampTest::timeOutHandler), this)),
        manager(manager)
    {}

    /** @brief the lamp test request handler
     *
     *  @param[in]  value    -  true: start lamptest
     *                          false: stop lamptest
     *  @return
     */
    void requestHandler(bool value);

  private:
    /** @brief Timer used for LEDs lamp test period */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> timer;

    /** @brief Reference to Manager object */
    Manager& manager;

    /** @brief Start and restart lamp test depending on what is the current
     *         state. */
    void start();

    /** @brief Stop lamp test. */
    void stop();

    /** @brief This method gets called when the lamp test procedure is done as
     *         part of timeout. */
    void timeOutHandler();
};

} // namespace led
} // namespace phosphor
