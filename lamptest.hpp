#pragma once

#include "group.hpp"
#include "manager.hpp"

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
     * Constructs timer and when the timeout occurs, the stop method is called
     * back to stop timer and also end the lamp test.
     *
     * @param[in] event   - sd event handler
     * @param[in] manager - reference to manager instance
     */
    LampTest(const sdeventplus::Event& event, const Manager& manager) :
        timer(event, std::bind(std::mem_fn(&LampTest::timeOutHandler), this)),
        manager(manager), groupObj(NULL)
    {
        // Get paths of all the Physical LED objects
        physicalLEDPaths =
            dBusHandler.getSubTreePaths(PHY_LED_PATH, PHY_LED_IFACE);
    }

    /** @brief the lamp test request handler
     *
     *  @param[in]  group    -  Pointer to Group object
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

    /** @brief Pointer to Group object */
    Group* groupObj;

    /** all the Physical paths */
    std::vector<std::string> physicalLEDPaths;

    /** @brief Start lamp test, it contains initiate or reset the timer. */
    void start();

    /** @brief Stop lamp test. */
    void stop();

    /** @brief The lamp test timeout. */
    void timeOutHandler();
};

} // namespace led
} // namespace phosphor
