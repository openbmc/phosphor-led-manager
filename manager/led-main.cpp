#include "config.h"

#include "group.hpp"
#include "ledlayout.hpp"
#ifdef LED_USE_JSON
#include "json-parser.hpp"
#else
#include "led-gen.hpp"
#endif
#include "config-validator.hpp"
#include "manager.hpp"
#include "serialize.hpp"
#include "utils.hpp"
#ifdef USE_LAMP_TEST
#include "lamptest/lamptest.hpp"
#endif

#include <CLI/CLI.hpp>
#include <sdeventplus/event.hpp>

#include <algorithm>
#include <iostream>
#include <memory>

int main(int argc, char** argv)
{
    CLI::App app("phosphor-led-manager");

#ifdef LED_USE_JSON
    std::string configFile{};
    app.add_option("-c,--config", configFile, "Path to JSON config");
#endif

    CLI11_PARSE(app, argc, argv);

    // Get a default event loop
    auto event = sdeventplus::Event::get_default();

    /** @brief Dbus constructs used by LED Group manager */
    auto& bus = phosphor::led::utils::DBusHandler::getBus();

#ifdef LED_USE_JSON
    auto optionalMap = getSystemLedMap(configFile);
    if (!optionalMap.has_value())
    {
        lg2::warning("No configuration found, exiting");
        return 0;
    }
    auto systemLedMap = *optionalMap;
#endif

    phosphor::led::validateConfigV1(systemLedMap);

    /** @brief Group manager object */
    phosphor::led::Manager manager(bus, systemLedMap, event);

    /** @brief sd_bus object manager */
    sdbusplus::server::manager_t objManager(bus,
                                            "/xyz/openbmc_project/led/groups");

    /** @brief vector of led groups */
    std::vector<std::unique_ptr<phosphor::led::Group>> groups;

    std::shared_ptr<phosphor::led::Serialize> serializePtr = nullptr;
#ifdef PERSISTENT_LED_ASSERTED
    /** @brief store and re-store Group */
    serializePtr =
        std::make_shared<phosphor::led::Serialize>(SAVED_GROUPS_FILE);
#endif

#ifdef USE_LAMP_TEST
    phosphor::led::LampTest lampTest(event, manager);

    // Clear leds triggered by lamp test in previous boot
    lampTest.clearLamps();

    groups.emplace_back(std::make_unique<phosphor::led::Group>(
        bus, LAMP_TEST_OBJECT, manager, serializePtr,
        [&lampTest](auto&& arg1, auto&& arg2) {
            return lampTest.requestHandler(std::forward<decltype(arg1)>(arg1),
                                           std::forward<decltype(arg2)>(arg2));
        }));

    // Register a lamp test method in the manager class, and call this method
    // when the lamp test is started
    manager.setLampTestCallBack([&lampTest](auto&& arg1, auto&& arg2) {
        return lampTest.processLEDUpdates(std::forward<decltype(arg1)>(arg1),
                                          std::forward<decltype(arg2)>(arg2));
    });
#endif

    /** Now create so many dbus objects as there are groups */
    std::ranges::transform(systemLedMap, std::back_inserter(groups),
                           [&bus, &manager, serializePtr](auto& grp) {
                               return std::make_unique<phosphor::led::Group>(
                                   bus, grp.first, manager, serializePtr);
                           });

    // Attach the bus to sd_event to service user requests
    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);

    /** @brief Claim the bus */
    bus.request_name("xyz.openbmc_project.LED.GroupManager");
    event.loop();

    return 0;
}
