#include "config.h"

#include "manager.hpp"

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/exception.hpp>
#include <xyz/openbmc_project/Led/Physical/server.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

namespace phosphor
{
namespace led
{

// apply the led action to the map
static void applyGroupAction(std::map<LedName, Layout::LedAction>& newState,
                             Layout::LedAction action)
{
    if (!newState.contains(action.name))
    {
        newState[action.name] = action;
        return;
    }

    auto currentAction = newState[action.name];

    const bool hasPriority = currentAction.priority.has_value();

    if (hasPriority && currentAction.action == action.priority)
    {
        // if the current action is already the priority action,
        // we cannot override it
        return;
    }

    newState[action.name] = action;
}

// create the resulting new map from all currently asserted groups
static auto getNewMapWithGroupPriorities(
    std::set<const Layout::GroupLayout*, Layout::CompareGroupLayout> sorted)
    -> std::map<LedName, Layout::LedAction>
{
    std::map<LedName, Layout::LedAction> newState;

    // update the new map with the desired state
    for (const auto* it : sorted)
    {
        // apply all led actions of that group to the map
        for (const Layout::LedAction& action : it->actionSet)
        {
            newState[action.name] = action;
        }
    }
    return newState;
}

static std::map<LedName, Layout::LedAction> getNewMapWithLEDPriorities(
    std::set<const Layout::GroupLayout*> assertedGroups)
{
    std::map<LedName, Layout::LedAction> newState;
    // update the new map with the desired state
    for (const Layout::GroupLayout* it : assertedGroups)
    {
        // apply all led actions of that group to the map
        for (const Layout::LedAction& action : it->actionSet)
        {
            applyGroupAction(newState, action);
        }
    }
    return newState;
}

// create the resulting new map from all currently asserted groups
std::map<LedName, Layout::LedAction> Manager::getNewMap(
    std::set<const Layout::GroupLayout*> assertedGroups)
{
    std::map<LedName, Layout::LedAction> newState;

    std::set<const Layout::GroupLayout*, Layout::CompareGroupLayout> sorted;

    bool groupPriorities = false;

    for (const Layout::GroupLayout* it : assertedGroups)
    {
        sorted.insert(it);

        if (it->priority != 0)
        {
            groupPriorities = true;
        }
    }

    if (groupPriorities)
    {
        newState = getNewMapWithGroupPriorities(sorted);
    }
    else
    {
        newState = getNewMapWithLEDPriorities(assertedGroups);
    }

    return newState;
}

// Assert -or- De-assert
bool Manager::setGroupState(const std::string& path, bool assert,
                            ActionSet& ledsAssert, ActionSet& ledsDeAssert)
{
    if (assert)
    {
        assertedGroups.insert(&ledMap.at(path));
    }
    else
    {
        if (assertedGroups.contains(&ledMap.at(path)))
        {
            assertedGroups.erase(&ledMap.at(path));
        }
    }

    // create the new map from the asserted groups
    auto newState = getNewMap(assertedGroups);

    // the ledsAssert are those that are in the new map and change state
    // + those in the new map and not in the old map
    for (const auto& [name, action] : newState)
    {
        if (ledStateMap.contains(name))
        {
            // check if the led action has changed
            auto& currentAction = ledStateMap[name];

            if (currentAction.action == action.action)
            {
                continue;
            }
        }

        ledsAssert.insert(action);
    }

    // the ledsDeAssert are those in the old map but not in the new map
    for (const auto& [name, action] : ledStateMap)
    {
        if (!newState.contains(name))
        {
            ledsDeAssert.insert(action);
        }
    }

    ledStateMap = newState;

    // If we survive, then set the state accordingly.
    return assert;
}

void Manager::setLampTestCallBack(
    std::function<bool(ActionSet& ledsAssert, ActionSet& ledsDeAssert)>
        callBack)
{
    lampTestCallBack = callBack;
}

/** @brief Run through the map and apply action on the LEDs */
void Manager::driveLEDs(ActionSet& ledsAssert, ActionSet& ledsDeAssert)
{
    if constexpr (USE_LAMP_TEST)
    {
        // Use the lampTestCallBack method and trigger the callback method in
        // the lamp test(processLEDUpdates), in this way, all lamp test
        // operations are performed in the lamp test class.
        if (lampTestCallBack(ledsAssert, ledsDeAssert))
        {
            return;
        }
    }

    ActionSet newReqChangedLeds;
    std::vector<std::pair<ActionSet&, ActionSet&>> actionsVec = {
        {reqLedsAssert, ledsAssert}, {reqLedsDeAssert, ledsDeAssert}};

    timer.setEnabled(false);
    std::set_union(ledsAssert.begin(), ledsAssert.end(), ledsDeAssert.begin(),
                   ledsDeAssert.end(),
                   std::inserter(newReqChangedLeds, newReqChangedLeds.begin()),
                   ledLess);

    // prepare reqLedsAssert & reqLedsDeAssert
    for (auto pair : actionsVec)
    {
        ActionSet tmpSet;

        // Discard current required LED actions, if these LEDs have new actions
        // in newReqChangedLeds.
        std::set_difference(pair.first.begin(), pair.first.end(),
                            newReqChangedLeds.begin(), newReqChangedLeds.end(),
                            std::inserter(tmpSet, tmpSet.begin()), ledLess);

        // Union the remaining LED actions with new LED actions.
        pair.first.clear();
        std::set_union(tmpSet.begin(), tmpSet.end(), pair.second.begin(),
                       pair.second.end(),
                       std::inserter(pair.first, pair.first.begin()), ledLess);
    }

    driveLedsHandler();
    return;
}

// Calls into driving physical LED post choosing the action
int Manager::drivePhysicalLED(const std::string& objPath, Layout::Action action,
                              uint8_t dutyOn, uint16_t period)
{
    try
    {
        // If Blink, set its property
        if (action == Layout::Action::Blink)
        {
            PropertyValue dutyOnValue{dutyOn};
            PropertyValue periodValue{period};

            phosphor::led::utils::DBusHandler::setProperty(
                objPath, phyLedIntf, "DutyOn", dutyOnValue);
            phosphor::led::utils::DBusHandler::setProperty(
                objPath, phyLedIntf, "Period", periodValue);
        }

        PropertyValue actionValue{getPhysicalAction(action)};
        phosphor::led::utils::DBusHandler::setProperty(objPath, phyLedIntf,
                                                       "State", actionValue);
    }
    catch (const sdbusplus::exception_t& e)
    {
        // This can be a really spammy event log, so we rate limit it to once an
        // hour per LED.
        auto now = std::chrono::steady_clock::now();

        if (auto it = physicalLEDErrors.find(objPath);
            it != physicalLEDErrors.end())
        {
            using namespace std::literals::chrono_literals;
            if ((now - it->second) < 1h)
            {
                return -1;
            }
        }

        lg2::error(
            "Error setting property for physical LED, ERROR = {ERROR}, OBJECT_PATH = {PATH}",
            "ERROR", e, "PATH", objPath);
        physicalLEDErrors[objPath] = now;
        return -1;
    }

    return 0;
}

/** @brief Returns action string based on enum */
std::string Manager::getPhysicalAction(Layout::Action action)
{
    namespace server = sdbusplus::xyz::openbmc_project::Led::server;

    // TODO: openbmc/phosphor-led-manager#5
    //    Somehow need to use the generated Action enum than giving one
    //    in ledlayout.
    if (action == Layout::Action::On)
    {
        return server::convertForMessage(server::Physical::Action::On);
    }
    else if (action == Layout::Action::Blink)
    {
        return server::convertForMessage(server::Physical::Action::Blink);
    }
    else
    {
        return server::convertForMessage(server::Physical::Action::Off);
    }
}

void Manager::driveLedsHandler(void)
{
    ActionSet failedLedsAssert;
    ActionSet failedLedsDeAssert;

    // This order of LED operation is important.
    for (const auto& it : reqLedsDeAssert)
    {
        std::string objPath = std::string(phyLedPath) + it.name;
        lg2::debug("De-Asserting LED, NAME = {NAME}, ACTION = {ACTION}", "NAME",
                   it.name, "ACTION", it.action);
        if (drivePhysicalLED(objPath, Layout::Action::Off, it.dutyOn,
                             it.period) != 0)
        {
            failedLedsDeAssert.insert(it);
        }
    }

    for (const auto& it : reqLedsAssert)
    {
        std::string objPath = std::string(phyLedPath) + it.name;
        lg2::debug("Asserting LED, NAME = {NAME}, ACTION = {ACTION}", "NAME",
                   it.name, "ACTION", it.action);
        if (drivePhysicalLED(objPath, it.action, it.dutyOn, it.period) != 0)
        {
            failedLedsAssert.insert(it);
        }
    }

    reqLedsAssert = failedLedsAssert;
    reqLedsDeAssert = failedLedsDeAssert;

    if (reqLedsDeAssert.empty() && reqLedsAssert.empty())
    {
        timer.setEnabled(false);
    }
    else
    {
        timer.restartOnce(std::chrono::seconds(1));
    }

    return;
}

} // namespace led
} // namespace phosphor
