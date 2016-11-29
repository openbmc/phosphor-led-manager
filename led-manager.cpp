#include <iostream>
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include <sdbusplus/message.hpp>
#include "led-manager.hpp"
#include "led-gen.hpp"
#include "config.h"

namespace phosphor
{
namespace led
{
// Initialize the globals
std::set<Group::group*> Group::gAssertedGroups;
Group::group Group::gCurrentState;

/** @brief Overloaded Property Setter function */
auto Group::asserted(bool value) -> bool
{
    auto group = strrchr(path.c_str(), '/');
    if (group)
    {
        // Removing the starting '/' in /group
        group++;
    }
    // Set the base class's asserted to 'true' since the getter
    // operation is handled there.
    return sdbusplus::xyz::openbmc_project::Led::server::
                      Group::asserted(setGroupState(group,value));
}

// Assert -or- De-assert
bool Group::setGroupState(const std::string& name, const bool& assert)
{
    if (assert)
    {
        gAssertedGroups.insert(&gLedMap[name]);
    }
    else
    {
        auto search = gAssertedGroups.find(&gLedMap[name]);
        if (search != gAssertedGroups.end())
        {
            gAssertedGroups.erase(&gLedMap[name]);
        }
        else
        {
            throw std::runtime_error("Group not present");
        }
    }
    return driveLEDs();
}

/** @brief Run through the map and apply action on the LEDs */
bool Group::driveLEDs()
{
    // This will contain the union of what's already in the asserted group
    group desiredState {};
    for(auto& grp : gAssertedGroups)
    {
        std::set_union(grp->begin(), grp->end(), grp->begin(), grp->end(),
                       std::inserter(desiredState, desiredState.begin()));
    }

    // Always Do execute Turn Off and then Turn on since we have the Blink
    // taking priority over -on-
    group ledsToAssert {};
    group ledsToTurnOff {};

    std::set_difference(gCurrentState.begin(), gCurrentState.end(),
                        desiredState.begin(), desiredState.end(),
                        std::inserter(ledsToTurnOff, ledsToTurnOff.begin()));
    if(ledsToTurnOff.size())
    {
        std::cout << "Turning off LEDs" << std::endl;
        for (auto& it: ledsToTurnOff)
        {
            std::cout << "\t{" << it.name << "::" << it.action << "}"
                      << std::endl;
        }

        // If we previously had a FRU in ON state , and then if there was a
        // request to make it blink, the end state would now be blink.
        // If we either turn off blink / fault, then we need to go back to its
        // previous state.
        std::set_intersection(desiredState.begin(), desiredState.end(),
                              ledsToTurnOff.begin(), ledsToTurnOff.end(),
                              std::inserter(ledsToAssert, ledsToAssert.begin()),
                              ledComp);

        if (ledsToAssert.size())
        {
            std::cout << "Asserting LEDs again" << std::endl;
            for (auto& it: ledsToAssert)
            {
                std::cout << "\t{" << it.name << "::" << it.action << "}"
                          << std::endl;
            }
        }
    }

    // Turn on these
    ledsToAssert.clear();
    std::set_difference(desiredState.begin(), desiredState.end(),
                        gCurrentState.begin(), gCurrentState.end(),
                        std::inserter(ledsToAssert, ledsToAssert.begin()));

    if(ledsToAssert.size())
    {
        std::cout << "Asserting LEDs" << std::endl;
        for (auto& it: ledsToAssert)
        {
            std::cout << "\t{" << it.name << "::" << it.action << "}"
                      << std::endl;
        }
    }

    // Done.. Save the latest and greatest.
    gCurrentState.clear();
    gCurrentState = desiredState;

    return true;
}

/** @brief Initialize the bus and announce services */
Group::Group(sdbusplus::bus::bus& bus, const char* objPath) :
    sdbusplus::server::object::object<
    sdbusplus::xyz::openbmc_project::Led::server::Group>(
        bus, objPath),
    path(objPath)
{
    // Nothing to do here
}

} // namespace led
} // namespace phosphor
