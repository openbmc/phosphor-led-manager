#include <iostream>
#include <cstring>
#include <sdbusplus/vtable.hpp>
#include <sdbusplus/message.hpp>
#include <sdbusplus/bus.hpp>
#include "led-manager.hpp"
#include "led-gen.hpp"

namespace phosphor
{

namespace led
{

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

    //TODO : Need to see how to represent group specific asserted state
    // May be a new tuple / map ?
    sd_bus_message_append(reply, "b", 0);
    return 0;
}

/** @brief Called when the group's asserted state is updated
 *         Signature as needed by sd_bus
 */
int setGroupState(sd_bus *bus, const char *path, const char *interface,
                  const char *property, sd_bus_message *value,
                  void *data, sd_bus_error* error)
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
    sdbusplus::vtable::property("Assert", "b",
    getGroupState, setGroupState, sdbusplus::vtable::property_::emits_change),
    sdbusplus::vtable::end()
};

/** @brief Initialize the bus and announce services */
Manager::Manager(const char* busName,
                 const char* objPath,
                 const char* intfName) :
    iv_bus(sdbusplus::bus::new_system()),
    objManager(iv_bus, objPath)
{
    // Like /org/openbmc/ledmanager/groups/
    auto path = std::string(objPath) + "/";

    /** Now create so many dbus objects as there are groups */
    for (auto &grp: Manager::cv_LedMap)
    {
        auto grpPath = path + grp.first;
        intfContainer.emplace_back(sdbusplus::server::interface::interface(
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
