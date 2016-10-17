#include <iostream>
#include <cstring>
#include <sdbusplus/vtable.hpp>
#include <sdbusplus/message.hpp>
#include <sdbusplus/bus.hpp>
#include "led-manager.hpp"
#include "led-gen.hpp"

/** @brief Called when the group's property is read
 *         Signature as needed by sd_bus
 */
int getGroupState(sd_bus *bus, const char *path, const char *interface,
                  const char *property, sd_bus_message *reply,
                  void *data, sd_bus_error* error)
{
    auto group = strrchr(path, '/');
    if (group)
    {
        // Removing the starting '/' in /group
        group++;
    }
    std::cout << "Reading property of group: " << group << std::endl;

    //TODO : Need to see how to represent group specific asserted state
    // May be a new tuple / map ?
    sd_bus_message_ref(reply);
    sd_bus_message_append(reply, "i", 0);
    return 0;
}

/** @brief Called when the group's asserted state is updated
 *         Signature as needed by sd_bus
 */
int setGroupState(sd_bus *bus, const char *path, const char *interface,
                  const char *property, sd_bus_message *value,
                  void *data, sd_bus_error* error)
{
    int state;
    auto group = strrchr(path, '/');
    if (group)
    {
        // Removing the starting '/' in /group
        group++;
    }
    std::cout << "Writing property of group: " << group << std::endl;
    auto msg = sdbusplus::message::message(value);
    sd_bus_message_ref(value);
    msg.read(state);

    //TODO : Need to see how to represent group specific asserted state
    // May be a new tuple / map ?
    return 1;
}

/** @brief Users having to assert a group will just turn this property to 1
 *  similarly, setting this property to 0 will deassert the group
 */
constexpr sdbusplus::vtable::vtable_t led_vtable[] =
{
    sdbusplus::vtable::start(),
    sdbusplus::vtable::property("Asserted", "i",
            getGroupState, setGroupState, 0),
    sdbusplus::vtable::end()
};

/** @brief Initialize the bus and announce services */
LedManager::LedManager():
    iv_bus(sdbusplus::bus::new_system())
{
    /** Best practice to have 1 object manager and then have it cater to
     *  GetManagedObjects than having to introspect on each element
     */
    iv_bus.add_object_manager(iv_path);

    /** Now create so many dbus objects as there are groups */
    for (auto &grp: LedManager::cv_ledMap)
    {
        auto grpPath = std::string(iv_path) + "/" + grp.first;
        iv_bus.add_object_vtable(grpPath.c_str(), iv_intf, *led_vtable);

        // These are now set of structs having LED name and the action. Do not
        // have anything to be done here at the moment but need to create a
        // mapping between led names to device strigs eventually
        //for (auto &set: grp.second)
        //{

        //}
    }
    // Once done, claim the bus and systemd will
    // consider this service started
    iv_bus.request_name(iv_name);
}

/** @brief Wait for client requests */
void LedManager::run()
{
    while(true)
    {
        try
        {
            iv_bus.process();

            /** Wait for 5 seconds (mentioned in usec) */
            iv_bus.wait(5000000);
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

int main(void)
{
    LedManager ledMgr;
    ledMgr.run();
    return 0;
}
