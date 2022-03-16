#include "config.h"

#include "json-config.hpp"
#include "ledlayout.hpp"

#include <nlohmann/json.hpp>
#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/bus.hpp>
#include <sdeventplus/event.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

using Json = nlohmann::json;
using LedAction = std::set<phosphor::led::Layout::LedAction>;
using LedMap = std::map<std::string, LedAction>;

// Priority for a particular LED needs to stay SAME across all groups
// phosphor::led::Layout::Action can only be one of `Blink` and `On`
using PriorityMap = std::map<std::string, phosphor::led::Layout::Action>;

/** @brief Parse LED JSON file and output Json object
 *
 *  @param[in] path - path of LED JSON file
 *
 *  @return const Json - Json object
 */
const Json readJson(const fs::path& path)
{

    if (!fs::exists(path) || fs::is_empty(path))
    {
        lg2::error("Incorrect File Path or empty file, FILE_PATH = {PATH}",
                   "PATH", path);
        throw std::runtime_error("Incorrect File Path or empty file");
    }

    try
    {
        std::ifstream jsonFile(path);
        return Json::parse(jsonFile);
    }
    catch (const std::exception& e)
    {
        lg2::error(
            "Failed to parse config file, ERROR = {ERROR}, FILE_PATH = {PATH}",
            "ERROR", e, "PATH", path);
        throw std::runtime_error("Failed to parse config file");
    }
}

/** @brief Returns action enum based on string
 *
 *  @param[in] action - action string
 *
 *  @return Action - action enum (On/Blink)
 */
phosphor::led::Layout::Action getAction(const std::string& action)
{
    assert(action == "On" || action == "Blink");

    return action == "Blink" ? phosphor::led::Layout::Blink
                             : phosphor::led::Layout::On;
}

/** @brief Validate the Priority of an LED is same across ALL groups
 *
 *  @param[in] name - led name member of each group
 *  @param[in] priority - member priority of each group
 *  @param[out] priorityMap - std::map, key:name, value:priority
 *
 *  @return
 */
void validatePriority(const std::string& name,
                      const phosphor::led::Layout::Action& priority,
                      PriorityMap& priorityMap)
{

    auto iter = priorityMap.find(name);
    if (iter == priorityMap.end())
    {
        priorityMap.emplace(name, priority);
        return;
    }

    if (iter->second != priority)
    {
        lg2::error(
            "Priority of LED is not same across all, Name = {NAME}, Old Priority = {OLD_PRIO}, New Priority = {NEW_PRIO}",
            "NAME", name, "OLD_PRIO", int(iter->second), "NEW_PRIO",
            int(priority));

        throw std::runtime_error(
            "Priority of at least one LED is not same across groups");
    }
}

/** @brief Load JSON config and return led map
 *
 *  @return LedMap - Generated an std::map of LedAction
 */
const LedMap loadJsonConfig(const fs::path& path)
{
    LedMap ledMap{};
    PriorityMap priorityMap{};

    // define the default JSON as empty
    const Json empty{};
    auto json = readJson(path);
    auto leds = json.value("leds", empty);

    for (const auto& entry : leds)
    {
        fs::path tmpPath(std::string{OBJPATH});
        tmpPath /= entry.value("group", "");
        auto objpath = tmpPath.string();
        auto members = entry.value("members", empty);

        LedAction ledActions{};
        for (const auto& member : members)
        {
            auto name = member.value("Name", "");
            auto action = getAction(member.value("Action", ""));
            uint8_t dutyOn = member.value("DutyOn", 50);
            uint16_t period = member.value("Period", 0);

            // Since only have Blink/On and default priority is Blink
            auto priority = getAction(member.value("Priority", "Blink"));

            // Same LEDs can be part of multiple groups. However, their
            // priorities across groups need to match.
            validatePriority(name, priority, priorityMap);

            phosphor::led::Layout::LedAction ledAction{name, action, dutyOn,
                                                       period, priority};
            ledActions.emplace(ledAction);
        }

        // Generated an std::map of LedGroupNames to std::set of LEDs
        // containing the name and properties.
        ledMap.emplace(objpath, ledActions);
    }

    return ledMap;
}

/** @brief Get led map from LED groups JSON config
 *
 *  @return LedMap - Generated an std::map of LedAction
 */
const LedMap getSystemLedMap()
{
    // Get a new Dbus
    auto bus = sdbusplus::bus::new_bus();

    // Get a new event loop
    auto event = sdeventplus::Event::get_new();

    // Attach the bus to sd_event to service user requests
    bus.attach_event(event.get(), SD_EVENT_PRIORITY_IMPORTANT);
    phosphor::led::JsonConfig jsonConfig(bus, event);

    // The event loop will be terminated from inside of a function in JsonConfig
    // after finding the configuration file
    if (jsonConfig.getConfFile().empty())
    {
        event.loop();
    }

    // Detach the bus from its sd_event event loop object
    bus.detach_event();

    return loadJsonConfig(jsonConfig.getConfFile());
}
