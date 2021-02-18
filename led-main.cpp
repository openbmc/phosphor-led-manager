#include "config.h"

#include "group.hpp"
#ifdef LED_USE_JSON
#include "json-parser.hpp"
#else
#include "led-gen.hpp"
#endif
#include "ledlayout.hpp"
#include "manager.hpp"
#include "serialize.hpp"
#include "utils.hpp"

#include <iostream>

int main(void)
{

#ifdef LED_USE_JSON
    auto systemLedMap = getSystemLedMap();
#endif

    // Get a default event loop
    auto event = sdeventplus::Event::get_default();

    /** @brief Dbus constructs used by LED Group manager */
    auto& bus = phosphor::led::utils::DBusHandler::getBus();

    /** @brief Group manager object */
    phosphor::led::Manager manager(bus, systemLedMap);

    /** @brief sd_bus object manager */
    sdbusplus::server::manager::manager objManager(bus, OBJPATH);

    /** @brief vector of led groups */
    std::vector<std::unique_ptr<phosphor::led::Group>> groups;

    /** @brief store and re-store Group */
    phosphor::led::Serialize serialize(SAVED_GROUPS_FILE);

    /** Now create so many dbus objects as there are groups */
    for (auto& grp : systemLedMap)
    {
        groups.emplace_back(std::make_unique<phosphor::led::Group>(
            bus, grp.first, manager, serialize));
    }

    /** @brief Claim the bus */
    bus.request_name(BUSNAME);
    event.loop();

    return 0;
}
