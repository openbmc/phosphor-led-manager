#pragma once

#include "group.hpp"
#include "manager.hpp"

#include <sdeventplus/utility/timer.hpp>

#include <queue>
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
        manager(manager), groupObj(NULL)
    {}

    using group = std::set<phosphor::led::Layout::LedAction>;

    /** @brief the lamp test request handler
     *
     *  @param[in]  group    -  Pointer to Group object
     *  @param[in]  value    -  true: start lamptest
     *                          false: stop lamptest
     *  @return
     */
    void requestHandler(Group* group, bool value);

    /** @brief Update physical LEDs states during lamp test
     *
     *  @param[in]  ledsAssert    -  LEDs that are to be asserted newly
     *                               or to a different state
     *  @param[in]  ledsDeAssert  -  LEDs that are to be Deasserted
     *  @return
     */
    void updatePhysicalLedStates(group& ledsAssert, group& ledsDeAssert);

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

    /** @brief Queue to save physical LED state */
    std::queue<std::tuple<group, group>> savedPhysicalLedStates;

    /** @brief Get state of the lamp test operation */
    bool isLampTestRunning{false};

    /** @brief LEDs that are to be Asserted before start lamp test */
    group lastLedsAssert;

    /** @brief LEDs that are to be Deasserted before start lamp test */
    group lastLedsDeAssert;

    /** @brief Start and restart lamp test depending on what is the current
     *         state. */
    void start();

    /** @brief Stop lamp test. */
    void stop();

    /** @brief This method gets called when the lamp test procedure is done as
     *         part of timeout. */
    void timeOutHandler();

    /** @brief Restore the physical LEDs states after the lamp test finished */
    void restorePhysicalLedStates();

    /** @brief Store the physical LEDs states before the lamp test start */
    void storePhysicalLEDsStates();

    /** @brief Returns action enum based on string
     *
     *  @param[in]  str - Action string
     *
     *  @return enumeration equivalent of the passed in string
     */
    Layout::Action getActionFromString(const std::string& str);

    /** @brief Notify PHYP to start the lamp test
     *
     *  @param[in]  value   -  the Asserted property value
     */
    void doHostLampTest(bool value);
};

} // namespace led
} // namespace phosphor
