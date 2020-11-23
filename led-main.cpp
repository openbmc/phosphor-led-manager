#include "config.h"

#include "group.hpp"
#ifdef LED_USE_JSON
#include "json-config.hpp"
#else
#include "led-gen.hpp"
#endif
#include "ledlayout.hpp"
#include "manager.hpp"
#include "serialize.hpp"
#include "utils.hpp"
#ifdef USE_LAMP_TEST
#include "lamptest.hpp"
#endif

#include <sdeventplus/event.hpp>

#include <iostream>

int main(void)
{
    // Get a default event loop
    auto event = sdeventplus::Event::get_default();

    /** @brief Dbus constructs used by LED Group manager */
    auto& bus = phosphor::led::utils::DBusHandler::getBus();

#ifdef LED_USE_JSON
    auto systemLedMap = loadJsonConfig(LED_JSON_FILE);
#endif

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

#ifdef USE_LAMP_TEST
    phosphor::led::LampTest lampTest(event, manager);

    groups.emplace_back(std::make_unique<phosphor::led::Group>(
        bus, LAMP_TEST_OBJECT, manager, serialize,
        std::bind(std::mem_fn(&phosphor::led::LampTest::requestHandler),
                  &lampTest, std::placeholders::_1, std::placeholders::_2)));

    // Attach the bus to sd_event to service user requests
    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);
#endif

    /** @brief Claim the bus */
    bus.request_name(BUSNAME);
    event.loop();

    return 0;
}
