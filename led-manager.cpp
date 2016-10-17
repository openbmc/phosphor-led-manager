#include <iostream>
#include <sdbusplus/vtable.hpp>
#include <sdbusplus/message.hpp>
#include <sdbusplus/bus.hpp>
#include "led-manager.hpp"
#include "led-gen.hpp"

// Callback handler for processing user requests
int processMsg(sd_bus_message *m, void *data,
               sd_bus_error *retError)
{
    std::cout <<"Called into ProcessSdBus\n";
    try
    {
        auto msg = sdbusplus::message::message(m);
        sd_bus_message_ref(m);
        sd_bus_reply_method_return(m, "i", 0);
    }
    catch (std::exception &e)
    {
        sd_bus_reply_method_errorf(m, SD_BUS_ERROR_FAILED, e.what());
    }
    return 0;
}

// Function pointers on the services available.
constexpr sdbusplus::vtable::vtable_t led_vtable[] =
{
    sdbusplus::vtable::start(),
    sdbusplus::vtable::method("SetOn", "", "i", &processMsg),
    sdbusplus::vtable::method("SetOff", "", "i", &processMsg),
    sdbusplus::vtable::method("SetBlink", "", "i", &processMsg),
    sdbusplus::vtable::end()
};

// Initialize the bus and announce services
LedManager::LedManager():
    iv_bus(sdbusplus::bus::new_system())
{
    // Create a Dbus for every single group
    for (auto &grp: LedManager::gMap)
    {
        auto grpPath = std::string(iv_path) + "/" + grp.first;
        //std::cout << "Group: " << grpPath << std::endl;
        iv_bus.add_object_manager(grpPath.c_str());
        iv_bus.add_object_vtable(grpPath.c_str(), iv_intf, *led_vtable);

        for  (auto &set: grp.second)
        {
        //    std::cout << "\t" << set.name << "  ::  " << set.action << std::endl;
        }
    }

    // Once done, claim the bus
    iv_bus.request_name(iv_name);
}

// Needed for std::set
bool operator<(const LedManager::LedAction& left,
               const LedManager::LedAction& right)
{
    // If the Name is same, then go by the operation.
    // Always BLINK has the highest priority.
    if (left.name == right.name)
    {
        return left.action < right.action;
    }
    return left.name < right.name;
}

// Needed for set union / intersect
bool operator==(const LedManager::LedAction& left,
                const LedManager::LedAction& right)
{
    // Only if name and action are same, consider equal
    if (left.name == right.name &&
        left.action == right.action)
    {
        return true;
    }
    return false;
}

// Wait for client requests
void LedManager::run()
{
    while(true)
    {
        try
        {
            iv_bus.process();
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

    std::cout <<" WELCOME\n";
    return 0;
}
