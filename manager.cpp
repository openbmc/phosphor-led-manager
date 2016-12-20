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
bool Manager::setGroupState(const std::string& path, bool assert,
                            group& ledsAssert, group& ledsDeAssert,
                            group& ledsUpdate)
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

    // This will contain the union of what's already in the asserted group
    group desiredState {};
    for(const auto& grp : assertedGroups)
    {
        desiredState.insert(grp->cbegin(), grp->cend());
    }

    // Has the LEDs that are either to be turned off -or- want a new assertion
    group transient {};
    std::set_difference(currentState.begin(), currentState.end(),
                        desiredState.begin(), desiredState.end(),
                        std::inserter(transient, transient.begin()));
    if(transient.size())
    {
        // We really do not want the Manager to know how a particular LED
        // transitions from State-A --> State-B and all this must be handled by
        // the physical LED controller implementation.
        // So in this case, Manager really does not want to turn off the
        // LEDs and then turning it back on and let the physical LED controller
        // handle that.

        // If we previously had a FRU in ON state , and then if there was a
        // request to make it blink, the end state would now be blink.
        // If we either turn off blink / fault, then we need to go back to its
        // previous state.
        std::set_intersection(desiredState.begin(), desiredState.end(),
                              transient.begin(), transient.end(),
                              std::inserter(ledsUpdate, ledsUpdate.begin()),
                              ledComp);

        // These LEDs are only to be De-Asserted.
        std::set_difference(transient.begin(), transient.end(),
                            ledsUpdate.begin(), ledsUpdate.end(),
                            std::inserter(ledsDeAssert, ledsDeAssert.begin()),
                            ledComp);

    }

    // Turn on these
    std::set_difference(desiredState.begin(), desiredState.end(),
                        currentState.begin(), currentState.end(),
                        std::inserter(ledsAssert, ledsAssert.begin()));


    // Done.. Save the latest and greatest.
    currentState = std::move(desiredState);

    // If we survive, then set the state accordingly.
    return assert;
}

/** @brief Run through the map and apply action on the LEDs */
void Manager::driveLEDs(group& ledsAssert, group& ledsDeAssert,
                        group& ledsUpdate)
{
    // This order of LED operation is important.
    if (ledsUpdate.size())
    {
        std::cout << "Updating LED states between (On <--> Blink)"
                  << std::endl;
        for (const auto& it: ledsUpdate)
        {
            std::cout << "\t{" << it.name << "::" << it.action << "}"
                << std::endl;
        }
    }

    if (ledsDeAssert.size())
    {
        std::cout << "De-Asserting LEDs" << std::endl;
        for (const auto& it: ledsDeAssert)
        {
            std::cout << "\t{" << it.name << "::" << it.action << "}"
                << std::endl;
        }
    }

    if(ledsAssert.size())
    {
        std::cout << "Asserting LEDs" << std::endl;
        for (const auto& it: ledsAssert)
        {
            std::cout << "\t{" << it.name << "::" << it.action << "}"
                << std::endl;
        }
    }
    return;
}

} // namespace led
} // namespace phosphor
