#include "config.h"

#include "group.hpp"
#ifdef LED_USE_JSON
#include "json-config.hpp"
#include "json-parser.hpp"
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

#ifdef LED_USE_JSON
    fs::path path{};

    {
        // Get a new Dbus
        auto busP = sdbusplus::bus::new_bus();

        // Get a new event loop
        auto eventP = sdeventplus::Event::get_new();

        // Attach the bus to sd_event to service user requests
        busP.attach_event(eventP.get(), SD_EVENT_PRIORITY_IMPORTANT);
        phosphor::led::JsonConfig jsonConfig(busP, eventP);

        while (!jsonConfig.completed())
        {
            eventP.loop();
        }

        path = jsonConfig.getConfFile();

        // Detach the bus from its sd_event event loop object
        busP.detach_event();
    }

    auto systemLedMap = loadJsonConfig(path);
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

#ifdef USE_LAMP_TEST
    phosphor::led::LampTest lampTest(event, manager);

    groups.emplace_back(std::make_unique<phosphor::led::Group>(
        bus, LAMP_TEST_OBJECT, manager, serialize,
        std::bind(std::mem_fn(&phosphor::led::LampTest::requestHandler),
                  &lampTest, std::placeholders::_1, std::placeholders::_2)));
#endif

    // Attach the bus to sd_event to service user requests
    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);

    /** @brief Claim the bus */
    bus.request_name(BUSNAME);
    event.loop();

    return 0;
}
