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

std::set<Group::group*> Group::gAssertedGroups;
Group::group Group::gCurrentState;

/** @brief Called when the group's property is read
 *         Signature as needed by sd_bus
 */
int getGroupState(sd_bus *bus, const char *path, const char *interface,
                  const char *property, sd_bus_message *reply,
                  void *userdata, sd_bus_error* error)
{
    auto ledMgr = static_cast<Group*>(userdata);
    auto state = ledMgr->getGroupState(path);

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
    auto msg = sdbusplus::message::message(value);
    sd_bus_message_ref(value);
    msg.read(state);

    auto ledMgr = static_cast<Group*>(userdata);
    return ledMgr->setGroupState(path, state);
}

// Get the asserted state
bool Group::getGroupState(const std::string& name)
{
    return gAssertedGroups.find(&gLedMap[name]) != gAssertedGroups.end();
}

// Assert -or- De-assert
int Group::setGroupState(const std::string& name,  bool assert)
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

// Run through the map and apply action
int Group::driveLEDs()
{
    // This will contain the union of what's already in the asserted group
    group desiredState {};
    for(auto& grp : gAssertedGroups)
    {
        desiredState.insert(grp->cbegin(), grp->cend());
    }

    // Always Do execute Turn Off and then Turn on since we have the Blink
    // taking priority over -on-
    group ledsToDeAssert {};

    std::set_difference(gCurrentState.begin(), gCurrentState.end(),
                        desiredState.begin(), desiredState.end(),
                        std::inserter(ledsToDeAssert, ledsToDeAssert.begin()));
    if(ledsToDeAssert.size())
    {
        std::cout << "Turning off LEDs" << std::endl;
        for (auto& it: ledsToDeAssert)
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
            for (auto& it: ledsToReAssert)
            {
                std::cout << "\t{" << it.name << "::" << it.action << "}"
                          << std::endl;
            }
        }
    }

    // Turn on these
    group ledsToAssert {};
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
    gCurrentState = std::move(desiredState);

    return 0;
}

/** @brief Users having to assert a group will just turn this property to TRUE
 *  similarly, setting this property to FALSE will deassert the group
 */
constexpr sdbusplus::vtable::vtable_t led_vtable[] =
{
    sdbusplus::vtable::start(),
    sdbusplus::vtable::property("Asserted", "b",
                                getGroupState, setGroupState,
                                sdbusplus::vtable::property_::emits_change),
                                sdbusplus::vtable::end()
};

/** @brief Initialize the bus and announce services */
Group::Group(const char* busName,
             const char* objPath,
             const char* intfName) :
    bus(sdbusplus::bus::new_system()),
    objManager(bus, objPath)
{
    // Like /org/openbmc/ledmanager/groups/
    auto path = std::string(objPath) + "/";

    /** Now create so many dbus objects as there are groups */
    for (auto &grp: Group::gLedMap)
    {
        auto grpPath = path + grp.first;
        intfContainer.emplace_back(bus, grpPath.c_str(),
                                   intfName, led_vtable, this);

        // These are now set of structs having LED name and the action. Do not
        // have anything to be done here at the moment but need to create a
        // mapping between led names to device strigs eventually
        //for (auto &set: grp.second)
        //{

        //}
    }
    // Once done, claim the bus and systemd will
    // consider this service started
    bus.request_name(busName);
}

/** @brief Wait for client requests */
void Group::run()
{
    while(true)
    {
        try
        {
            bus.process_discard();
            bus.wait();
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

} // namespace led

} // namespace phosphor
