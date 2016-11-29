#include <iostream>
#include <string>
#include <algorithm>
#include "manager.hpp"
#include "led-gen.hpp"
namespace phosphor
{
namespace led
{

// Assert -or- De-assert
bool Manager::setGroupState(const std::string& path, bool assert)
{
    if (assert)
    {
        assertedGroups.insert(&ledMap.at(path));
    }
    else
    {
        auto search = assertedGroups.find(&ledMap.at(path));
        if (search != assertedGroups.end())
        {
            assertedGroups.erase(&ledMap.at(path));
        }
    }
    // If something does not go right here, then there should be an sdbusplus
    // exception thrown.
    driveLEDs();

    // If we survive, then set the state accordingly.
    return assert;
}

/** @brief Run through the map and apply action on the LEDs */
void Manager::driveLEDs()
{
    // This will contain the union of what's already in the asserted group
    group desiredState {};
    for(const auto& grp : assertedGroups)
    {
        desiredState.insert(grp->cbegin(), grp->cend());
    }

    // Always Do execute Turn Off and then Turn on since we have the Blink
    // taking priority over -on-
    group ledsToDeAssert {};

    std::set_difference(currentState.begin(), currentState.end(),
                        desiredState.begin(), desiredState.end(),
                        std::inserter(ledsToDeAssert, ledsToDeAssert.begin()));

    if(ledsToDeAssert.size())
    {
        // We really do not want the Manager to know how a particular LED
        // transitions from State-A --> State-B and all this must be handled by
        // the physical LED controller implementation.
        // So in this case, Manager really does not want to turn off the
        // LEDs and then turning it back on and let the physical LED controller
        // handle that.

        // I am still experimenting on the algo..
        std::cout << "De asserting the LEDs" << std::endl;
        for (const auto& it: ledsToDeAssert)
        {
            std::cout << "\t{" << it.name << "::" << it.action << "}"
                               << std::endl;
        }

        // If we previously had a FRU in ON state , and then if there was a
        // request to make it blink, the end state would now be blink.
        // If we either turn off blink / fault, then we need to go back to its
        // previous state.
        group ledsToReAssert {};
        std::set_intersection(desiredState.begin(), desiredState.end(),
                              ledsToDeAssert.begin(), ledsToDeAssert.end(),
                              std::inserter(ledsToReAssert, ledsToReAssert.begin()),
                              ledComp);

        if (ledsToReAssert.size())
        {
            std::cout << "Asserting LEDs again" << std::endl;
            for (const auto& it: ledsToReAssert)
            {
                std::cout << "\t{" << it.name << "::" << it.action << "}"
                          << std::endl;
            }
        }
    }

    // Turn on these
    group ledsToAssert {};
    std::set_difference(desiredState.begin(), desiredState.end(),
                        currentState.begin(), currentState.end(),
                        std::inserter(ledsToAssert, ledsToAssert.begin()));

    if(ledsToAssert.size())
    {
        std::cout << "Asserting LEDs" << std::endl;
        for (const auto& it: ledsToAssert)
        {
            std::cout << "\t{" << it.name << "::" << it.action << "}"
                      << std::endl;
        }
    }

    // Done.. Save the latest and greatest.
    currentState = std::move(desiredState);
    return;
}

} // namespace led
} // namespace phosphor
