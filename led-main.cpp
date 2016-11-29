#include <iostream>
#include "manager.hpp"
#include "group.hpp"
#include "config.h"

int main(void)
{
    /** @brief Group manager object */
    phosphor::led::Manager manager;

    /** @brief Dbus constructs used by LED Group manager */
    sdbusplus::bus::bus bus = sdbusplus::bus::new_default();

    /** @brief sd_bus object manager */
    sdbusplus::server::manager::manager objManager(bus, OBJPATH);

    /** @brief vector of led groups */
    std::vector<std::unique_ptr<phosphor::led::Group>> groups;

    /** Now create so many dbus objects as there are groups */
    for (auto &grp: phosphor::led::Manager::ledMap)
    {
        groups.emplace_back(std::make_unique<phosphor::led::Group>(
                    bus, grp.first, manager));
    }

    /** @brief Claim the bus */
    bus.request_name(BUSNAME);

    /** @brief Wait for client requests */
    while(true)
    {
        /** @brief process dbus calls / signals discarding unhandled */
        bus.process_discard();
        bus.wait();
    }
    return 0;
}
