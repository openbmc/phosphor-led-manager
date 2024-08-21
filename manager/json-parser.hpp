#include "config.h"

#include "grouplayout.hpp"
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

// Priority for a particular LED needs to stay SAME across all groups
using PriorityMap =
    std::unordered_map<std::string,
                       std::optional<phosphor::led::Layout::Action>>;

/** @brief Parse LED JSON file and output Json object
 *
 *  @param[in] path - path of LED JSON file
 *
 *  @return const Json - Json object
 */
Json readJson(const fs::path& path)
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
 *  @return Action - action enum (On/Off/Blink)
 */
phosphor::led::Layout::Action getAction(const std::string& action)
{
    if (action == "On")
    {
        return phosphor::led::Layout::Action::On;
    }
    if (action == "Off")
    {
        return phosphor::led::Layout::Action::Off;
    }
    if (action == "Blink")
    {
        return phosphor::led::Layout::Action::Blink;
    }

    assert(false);
    return phosphor::led::Layout::Action::Blink;
}

static std::string priorityToString(
    const std::optional<phosphor::led::Layout::Action>& priority)
{
    if (!priority.has_value())
    {
        return "none";
    }
    switch (priority.value())
    {
        case phosphor::led::Layout::Action::Off:
            return "Off";
        case phosphor::led::Layout::Action::On:
            return "On";
        case phosphor::led::Layout::Action::Blink:
            return "Blink";
    }
    return "?";
}

/** @brief Validate the Priority of an LED is same across ALL groups
 *
 *  @param[in] name - led name member of each group
 *  @param[in] priority - member priority of each group
 *  @param[out] priorityMap - std::unordered_map, key:name, value:priority
 *
 *  @return
 */
void validatePriority(
    const std::string& name,
    const std::optional<phosphor::led::Layout::Action>& priority,
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
            "NAME", name, "OLD_PRIO", priorityToString(iter->second),
            "NEW_PRIO", priorityToString(priority));

        throw std::runtime_error(
            "Priority of at least one LED is not same across groups");
    }
}

static void loadJsonConfigV1GroupMember(
    const Json& member, PriorityMap& priorityMap,
    phosphor::led::ActionSet& ledActions, const bool hasGroupPriority)
{
    auto name = member.value("Name", "");
    auto action = getAction(member.value("Action", ""));
    uint8_t dutyOn = member.value("DutyOn", 50);
    uint16_t period = member.value("Period", 0);

    const std::string priorityStr = member.value("Priority", "");
    std::optional<phosphor::led::Layout::Action> priority = std::nullopt;

    if (!priorityStr.empty())
    {
        priority = getAction(priorityStr);
    }

    if (priority != std::nullopt && hasGroupPriority)
    {
        lg2::error("Cannot mix group priority and led priority for LED: {NAME}",
                   "NAME", name);
        throw std::runtime_error(
            "Cannot mix group priority and led priority for LED: " + name);
    }

    if (priority == std::nullopt && !hasGroupPriority)
    {
        lg2::error("Need valid led priority or group priority for LED: {NAME}",
                   "NAME", name);
        throw std::runtime_error(
            "Need valid led priority or group priority for LED: " + name);
    }

    // Same LEDs can be part of multiple groups. However, their
    // priorities across groups need to match.
    validatePriority(name, priority, priorityMap);

    phosphor::led::Layout::LedAction ledAction{name, action, dutyOn, period,
                                               priority};
    ledActions.emplace(ledAction);
}

static void loadJsonConfigV1Group(const Json& entry,
                                  phosphor::led::GroupMap& ledMap,
                                  PriorityMap& priorityMap)
{
    const Json empty{};

    fs::path tmpPath("/xyz/openbmc_project/led/groups");

    const std::string groupName = entry.value("group", "");

    tmpPath /= groupName;
    auto objpath = tmpPath.string();
    auto members = entry.value("members", empty);
    int priority = entry.value("Priority", 0);

    lg2::debug("config for '{GROUP}'", "GROUP", groupName);

    const bool hasGroupPriority = entry.contains("Priority");

    phosphor::led::ActionSet ledActions{};
    phosphor::led::Layout::GroupLayout groupLayout{};
    for (const auto& member : members)
    {
        loadJsonConfigV1GroupMember(member, priorityMap, ledActions,
                                    hasGroupPriority);
    }

    // Generated an std::unordered_map of LedGroupNames to std::set of LEDs
    // containing the name and properties.
    groupLayout.actionSet = ledActions;
    groupLayout.priority = priority;

    ledMap.emplace(objpath, groupLayout);
}

/** @brief Load JSON config and return led map (JSON version 1)
 *
 *  @return phosphor::led::GroupMap
 */
const phosphor::led::GroupMap loadJsonConfigV1(const Json& json)
{
    phosphor::led::GroupMap ledMap{};
    PriorityMap priorityMap{};

    // define the default JSON as empty
    const Json empty{};
    auto leds = json.value("leds", empty);

    for (const auto& entry : leds)
    {
        loadJsonConfigV1Group(entry, ledMap, priorityMap);
    }

    return ledMap;
}

/** @brief Load JSON config and return led map
 *
 *  @return phosphor::led::GroupMap
 */
const phosphor::led::GroupMap loadJsonConfig(const fs::path& path)
{
    auto json = readJson(path);

    auto version = json.value("version", 1);
    switch (version)
    {
        case 1:
            return loadJsonConfigV1(json);

        default:
            lg2::error("Unsupported JSON Version: {VERSION}", "VERSION",
                       version);
            throw std::runtime_error("Unsupported version");
    }

    return phosphor::led::GroupMap{};
}

/** @brief Get led map from LED groups JSON config
 *
 *  @param[in] config - Path to the JSON config.
 *  @return phosphor::led::GroupMap
 *
 *  @note if config is an empty string, daemon will interrogate dbus for
 *        compatible strings.
 */
const phosphor::led::GroupMap getSystemLedMap(fs::path config)
{
    if (config.empty())
    {
        config = phosphor::led::getJsonConfig();
    }

    return loadJsonConfig(config);
}
