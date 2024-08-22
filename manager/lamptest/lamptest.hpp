#pragma once

#include "config.h"

#include "group.hpp"
#include "manager.hpp"

#include <nlohmann/json.hpp>
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
    LampTest(LampTest&&) = delete;
    LampTest& operator=(LampTest&&) = delete;

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
    {
        // Get the force update and/or skipped physical LEDs names from the
        // lamp-test-led-overrides.json file during lamp
        getPhysicalLEDNamesFromJson(LAMP_TEST_LED_OVERRIDES_JSON);
    }

    /** @brief the lamp test request handler
     *
     * If lamp test is running (Asserted=true) and if user requests to stop lamp
     * test (Asserted input=false), Stop operation will not take place and set
     * the Asserted to true itself. LampTest Asserted is/can be set to false
     * only when the lamptest timer expires.
     *
     *  @param[in]  group    -  Pointer to Group object
     *  @param[in]  value    -  true: start lamptest
     *                          false: stop lamptest
     *
     *  @return Whether lamp test request is handled successfully or not.
     */
    bool requestHandler(Group* group, bool value);

    /** @brief Update physical LEDs states during lamp test and the lamp test is
     *         running
     *
     *  @param[in]  ledsAssert    -  LEDs that are to be asserted newly or to a
     *                               different state
     *  @param[in]  ledsDeAssert  -  LEDs that are to be Deasserted
     *
     *  @return Is running lamp test, true running
     */
    bool processLEDUpdates(const ActionSet& ledsAssert,
                           const ActionSet& ledsDeAssert);

    /** @brief Clear LEDs triggered by lamptest
     * When system reboots during lamptest, leds triggered by lamptest needs to
     * be cleared in the upcoming boot. This method clears all the leds along
     * with the persisted lamp test indicator file so that there is no sign of
     * lamptest.
     */
    void clearLamps();

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

    /** @brief Queue to save LED states during lamp test */
    std::queue<std::pair<ActionSet, ActionSet>> updatedLEDsDuringLampTest;

    /** @brief Get state of the lamp test operation */
    bool isLampTestRunning{false};

    /** @brief Physical LED states prior to lamp test */
    ActionSet physicalLEDStatesPriorToLampTest;

    /** @brief Vector of names of physical LEDs, whose changes will be forcibly
     *         updated even during lamp test. */
    std::vector<std::string> forceUpdateLEDs;

    /** @brief Vector of names of physical LEDs, that will be exempted from lamp
     *         test */
    std::vector<std::string> skipUpdateLEDs;

    /** @brief Start and restart lamp test depending on what is the current
     *         state. */
    void start();

    /** @brief Stop lamp test. */
    void stop();

    /** @brief This method gets called when the lamp test procedure is done as
     *         part of timeout. */
    void timeOutHandler();

    /** @brief Restore the physical LEDs states after the lamp test finishes */
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

    /** @brief Notify host to start / stop the lamp test
     *
     *  @param[in]  value   -  the Asserted property value
     */
    void doHostLampTest(bool value);

    /** @brief Get physical LED names from lamp test JSON config file
     *
     *  @param[in]  path - path of LED JSON file
     *
     *  return
     */
    void getPhysicalLEDNamesFromJson(const fs::path& path);
};

} // namespace led
} // namespace phosphor
