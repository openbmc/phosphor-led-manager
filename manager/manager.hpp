#pragma once

#include "grouplayout.hpp"
#include "ledlayout.hpp"
#include "utils.hpp"

#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>

#include <chrono>
#include <set>
#include <string>
#include <unordered_map>

// to better see what the string is representing
using LedName = std::string;

namespace phosphor
{
namespace led
{
using namespace phosphor::led::utils;

static constexpr auto phyLedPath = "/xyz/openbmc_project/led/physical/";
static constexpr auto phyLedIntf = "xyz.openbmc_project.Led.Physical";

/** @class Manager
 *  @brief Manages group of LEDs and applies action on the elements of group
 */
class Manager
{
  public:
    /** @brief Only need the default Manager */
    Manager() = delete;
    ~Manager() = default;
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(Manager&&) = delete;

    /** @brief Special comparator for finding set difference */
    static bool ledComp(const phosphor::led::Layout::LedAction& left,
                        const phosphor::led::Layout::LedAction& right)
    {
        // Example :
        // If FIRST_1 is {fan0, 1, 1} and FIRST_2 is {fan0, 2, 2},
        // with default priority of Blink, this comparator would return
        // false. But considering the priority, this comparator would need
        // to return true so that we consider appropriate set and in
        // this case its {fan0, 1, 1}
        if (left.name == right.name)
        {
            return left.action != right.action;
        }
        return left.name < right.name;
    }

    /** @brief Comparator for finding LEDs to be DeAsserted */
    static bool ledLess(const phosphor::led::Layout::LedAction& left,
                        const phosphor::led::Layout::LedAction& right)
    {
        return left.name < right.name;
    }

    /** @brief Comparator for helping unique_copy */
    static bool ledEqual(const phosphor::led::Layout::LedAction& left,
                         const phosphor::led::Layout::LedAction& right)
    {
        return left.name == right.name;
    }

    /** @brief static global map constructed at compile time */
    const GroupMap& ledMap;

    /** @brief Refer the user supplied LED layout and sdbusplus handler
     *
     *  @param [in] bus       - sdbusplus handler
     *  @param [in] GroupMap - LEDs group layout
     *  @param [in] Event    - sd event handler
     */
    Manager(
        sdbusplus::bus_t& bus, const GroupMap& ledLayout,
        const sdeventplus::Event& event = sdeventplus::Event::get_default()) :
        ledMap(ledLayout), bus(bus),
        timer(event, [this](auto&) { driveLedsHandler(); })
    {
        // Nothing here
    }

    /* create the resulting map from all currently asserted groups */
    static auto getNewMap(std::set<const Layout::GroupLayout*> assertedGroups)
        -> std::map<LedName, Layout::LedAction>;

    /** @brief Given a group name, applies the action on the group
     *
     *  @param[in]  path          -  dbus path of group
     *  @param[in]  assert        -  Could be true or false
     *  @param[in]  ledsAssert    -  LEDs that are to be asserted new
     *                               or to a different state
     *  @param[in]  ledsDeAssert  -  LEDs that are to be Deasserted
     *
     *  @return                   -  Success or exception thrown
     */
    bool setGroupState(const std::string& path, bool assert,
                       ActionSet& ledsAssert, ActionSet& ledsDeAssert);

    /** @brief Finds the set of LEDs to operate on and executes action
     *
     *  @param[in]  ledsAssert    -  LEDs that are to be asserted newly
     *                               or to a different state
     *  @param[in]  ledsDeAssert  -  LEDs that are to be Deasserted
     *
     *  @return: None
     */
    void driveLEDs(ActionSet& ledsAssert, ActionSet& ledsDeAssert);

    /** @brief Chooses appropriate action to be triggered on physical LED
     *  and calls into function that applies the actual action.
     *
     *  @param[in]  objPath   -  D-Bus object path
     *  @param[in]  action    -  Intended action to be triggered
     *  @param[in]  dutyOn    -  Duty Cycle ON percentage
     *  @param[in]  period    -  Time taken for one blink cycle
     *
     *  @return:              -  0: success, -1: LED set failed
     */
    int drivePhysicalLED(const std::string& objPath, Layout::Action action,
                         uint8_t dutyOn, uint16_t period);

    /** @brief Set lamp test callback when enabled lamp test.
     *
     *  @param[in]  callBack   -  Custom callback when enabled lamp test
     */
    void setLampTestCallBack(
        std::function<bool(ActionSet& ledsAssert, ActionSet& ledsDeAssert)>
            callBack);

  private:
    /** @brief sdbusplus handler */
    sdbusplus::bus_t& bus;

    /** Map of physical LED path to service name */
    std::unordered_map<std::string, std::string> phyLeds;

    /** @brief Pointers to groups that are in asserted state */
    std::set<const Layout::GroupLayout*> assertedGroups;

    /** Map of led name to current state */
    std::map<std::string, Layout::LedAction> ledStateMap;

    /** @brief Custom callback when enabled lamp test */
    std::function<bool(ActionSet& ledsAssert, ActionSet& ledsDeAssert)>
        lampTestCallBack;

    /** @brief Timer used for LEDs handler callback*/
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> timer;

    /** @brief Contains the required set of assert LEDs action */
    ActionSet reqLedsAssert;

    /** @brief Contains the required set of deassert LEDs action */
    ActionSet reqLedsDeAssert;

    /** @brief Map to store the last error time for physical LED paths */
    std::unordered_map<std::string,
                       std::chrono::time_point<std::chrono::steady_clock>>
        physicalLEDErrors;

    /** @brief LEDs handler callback */
    void driveLedsHandler();

    /** @brief Returns action string based on enum
     *
     *  @param[in]  action - Action enum
     *
     *  @return string equivalent of the passed in enumeration
     */
    static std::string getPhysicalAction(Layout::Action action);
};

} // namespace led
} // namespace phosphor
