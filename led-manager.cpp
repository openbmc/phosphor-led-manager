#include <iostream>
#include <cstring>
#include <algorithm>
#include <sdbusplus/vtable.hpp>
#include <sdbusplus/message.hpp>
#include <sdbusplus/bus.hpp>
#include "led-manager.hpp"
#include "led-gen.hpp"

namespace phosphor
{

namespace led
{

using group = std::set<Manager::LedAction>;
std::set<group*> Manager::cv_AssertedGroups;
group Manager::cv_CurrentState;

/** @brief Called when the group's property is read
 *         Signature as needed by sd_bus
 */
int getGroupState(sd_bus *bus, const char *path, const char *interface,
                  const char *property, sd_bus_message *reply,
                  void *userdata, sd_bus_error* error)
{
    auto group = strrchr(path, '/');
    if (group)
    {
        // Removing the starting '/' in /group
        group++;
    }

    auto ledMgr = static_cast<Manager*>(userdata);
    auto state = ledMgr->getGroupState(group);

    sd_bus_message_append(reply, "b", state);
    return 0;
}

/** @brief Called when the group's asserted state is updated
 *         Signature as needed by sd_bus
 */
int setGroupState(sd_bus *bus, const char *path, const char *interface,
                  const char *property, sd_bus_message *value,
                  void *userdata, sd_bus_error* error)
{
    bool state {};
    auto group = strrchr(path, '/');
    if (group)
    {
        // Removing the starting '/' in /group
        group++;
    }

    auto msg = sdbusplus::message::message(value);
    sd_bus_message_ref(value);
    msg.read(state);

    auto ledMgr = static_cast<Manager*>(userdata);
    return ledMgr->setGroupState(group, state);
}

// Get the asserted state
bool Manager::getGroupState(const std::string& name)
{
    return cv_AssertedGroups.find(&cv_LedMap[name]) != cv_AssertedGroups.end();
}

// Assert -or- De-assert
int Manager::setGroupState(const std::string& name, const bool& assert)
{
    if (assert)
    {
        cv_AssertedGroups.insert(&cv_LedMap[name]);
    }
    else
    {
        auto search = cv_AssertedGroups.find(&cv_LedMap[name]);
        if (search != cv_AssertedGroups.end())
        {
            cv_AssertedGroups.erase(&cv_LedMap[name]);
        }
        else
        {
            throw std::runtime_error("Group not present");
        }
    }
    return driveLEDs();
}

// Run through the map and apply action
int Manager::driveLEDs()
{
    // This will contain the union of what's already in the asserted group
    group desiredState {};
    for(auto& grp : cv_AssertedGroups)
    {
        std::set_union(grp->begin(), grp->end(), grp->begin(), grp->end(),
                       std::inserter(desiredState, desiredState.begin()));
    }

    // Always Do execute Turn Off and then Turn on since we have the Blink
    // taking priority over -on-
    group ledsToAssert {};
    group ledsToTurnOff {};

    std::set_difference(cv_CurrentState.begin(), cv_CurrentState.end(),
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
                        cv_CurrentState.begin(), cv_CurrentState.end(),
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
    cv_CurrentState.clear();
    cv_CurrentState = desiredState;

    return 0;
}

/** @brief Users having to assert a group will just turn this property to TRUE
 *  similarly, setting this property to FALSE will deassert the group
 */
constexpr sdbusplus::vtable::vtable_t led_vtable[] =
{
    sdbusplus::vtable::start(),
    sdbusplus::vtable::property("Asserted", "b",
    getGroupState, setGroupState, sdbusplus::vtable::property_::emits_change),
    sdbusplus::vtable::end()
};

/** @brief Initialize the bus and announce services */
Manager::Manager(const char* busName,
                 const char* objPath,
                 const char* intfName) :
    iv_bus(sdbusplus::bus::new_system()),
    iv_ObjManager(iv_bus, objPath)
{
    // Like /org/openbmc/ledmanager/groups/
    auto path = std::string(objPath) + "/";

    /** Now create so many dbus objects as there are groups */
    for (auto &grp: Manager::cv_LedMap)
    {
        auto grpPath = path + grp.first;
        iv_IntfContainer.emplace_back(sdbusplus::server::interface::interface(
                      iv_bus, grpPath.c_str(), intfName, led_vtable, this));

        // These are now set of structs having LED name and the action. Do not
        // have anything to be done here at the moment but need to create a
        // mapping between led names to device strigs eventually
        //for (auto &set: grp.second)
        //{

        //}
    }
    // Once done, claim the bus and systemd will
    // consider this service started
    iv_bus.request_name(busName);
}

/** @brief Wait for client requests */
void Manager::run()
{
    while(true)
    {
        try
        {
            iv_bus.process_discard();
            iv_bus.wait();
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

} // namespace led

} // namespace phosphor
