#include "led-manager.hpp"
#include <iostream>
#include "config.h"

int main(void)
{
    /** @brief Dbus constructs used by LED Group manager */
    sdbusplus::bus::bus bus = std::move(sdbusplus::bus::new_system());

    /** @brief sd_bus object manager */
    sdbusplus::server::manager::manager objManager(bus, OBJPATH);

    /** @brief vector of led groups */
    std::vector<std::unique_ptr<phosphor::led::Group>> groups;

    /** Now create so many dbus objects as there are groups */
    auto root = std::string(OBJPATH) + "/";
    for (auto &grp: phosphor::led::Group::gLedMap)
    {
        auto grpPath = root + grp.first;
        groups.emplace_back(std::make_unique<
                            phosphor::led::Group>(bus, grpPath.c_str()));
    }

    /** @brief Claim the bus */
    bus.request_name(BUSNAME);

    /** @brief Wait for client requests */
    while(true)
    {
        try
        {
            /** @brief process dbus calls / signals discarding unhandled */
            bus.process_discard();
            bus.wait();
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
    return 0;
}
