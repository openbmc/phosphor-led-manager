#include <iostream>
#include <string>
#include <algorithm>
#include <sdbusplus/message.hpp>
#include "led-manager.hpp"
#include "led-gen.hpp"
namespace phosphor
{
namespace led
{

// Initialize the globals
std::set<const Group::group*> Group::assertedGroups;
Group::group Group::currentState;

/** @brief Overloaded Property Setter function */
bool Group::asserted(bool value)
{
    // Set the base class's asserted to 'true' since the getter
    // operation is handled there.
    return sdbusplus::xyz::openbmc_project::Led::server::
                      Group::asserted(setGroupState(value));
}

/** @brief Called when the group's asserted state is updated
 *         Signature as needed by sd_bus
 */
bool setGroupState(sd_bus *bus, const char *path, const char *interface,
                  const char *property, sd_bus_message *value,
                  void *userdata, sd_bus_error* error)
{
    bool state {};
    auto msg = sdbusplus::message::message(value);
    sd_bus_message_ref(value);
    msg.read(state);

    auto ledMgr = static_cast<Group*>(userdata);
    return ledMgr->setGroupState(state);
}

// Assert -or- De-assert
bool Group::setGroupState(bool assert)
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
        else
        {
            std::cout <<"Object [ " << path << " ] Not present\n";
        }
    }
    return driveLEDs();
}

/** @brief Run through the map and apply action on the LEDs */
bool Group::driveLEDs()
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
        std::cout << "Turning off LEDs" << std::endl;
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

    return true;
}

/** @brief Initialize the bus and announce services */
Group::Group(sdbusplus::bus::bus& bus,
             const std::string& objPath) :
    sdbusplus::server::object::object<
    sdbusplus::xyz::openbmc_project::Led::server::Group>(
                            bus, objPath.c_str()),
    path(objPath)
{
    // Nothing to do here
}

} // namespace led
} // namespace phosphor
