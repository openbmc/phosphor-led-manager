#include "config-validator.hpp"

#include "grouplayout.hpp"
#include "ledlayout.hpp"

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/bus.hpp>

using namespace phosphor::led;

namespace phosphor
{
namespace led
{

// Priority for a particular LED needs to stay SAME across all groups
using PriorityMap =
    std::unordered_map<std::string,
                       std::optional<phosphor::led::Layout::Action>>;

static bool isUsingGroupPriority(const phosphor::led::GroupMap& ledMap)
{
    for (const auto& [_, group] : ledMap)
    {
        if (group.priority != 0)
        {
            return true;
        }
    }
    return false;
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
        throw ConfigValidationException(
            error::LedPriorityMismatch, "?", name,
            "Priority of the LED is not same across groups. Old Priority = " +
                priorityToString(iter->second) +
                ", New Priority = " + priorityToString(priority));
    }
}

static void validateConfigV1GroupForLedPriority(
    const std::string groupName,
    const phosphor::led::Layout::GroupLayout& group, PriorityMap& priorityMap)
{
    if (group.priority != 0)
    {
        throw ConfigValidationException(
            error::MixedLedAndGroupPriority, groupName,
            "Cannot mix group priority and led priority");
    }

    for (const auto& ledAction : group.actionSet)
    {
        if (ledAction.priority == std::nullopt)
        {
            throw ConfigValidationException(error::MissingLedPriority,
                                            groupName, ledAction.name,
                                            "Need valid led priority");
        }

        // Same LEDs can be part of multiple groups. However, their
        // priorities across groups need to match.
        validatePriority(ledAction.name, ledAction.priority, priorityMap);
    }
}

static void validateConfigV1ForLedPriority(
    const phosphor::led::GroupMap& ledMap)
{
    PriorityMap priorityMap{};
    for (const auto& [groupName, group] : ledMap)
    {
        validateConfigV1GroupForLedPriority(groupName, group, priorityMap);
    }
}

static void validateConfigV1GroupForGroupPriority(
    const std::string groupName,
    const phosphor::led::Layout::GroupLayout& group)
{
    for (const auto& led : group.actionSet)
    {
        if (led.priority != std::nullopt)
        {
            throw ConfigValidationException(
                error::MixedLedAndGroupPriority, groupName, led.name,
                "Cannot mix group priority and led priority for LED");
        }
    }

    if (group.priority == 0)
    {
        // group priority 0 is for internal use
        throw ConfigValidationException(error::InvalidGroupPriority, groupName,
                                        "Group Priority cannot be 0");
    }
}

static void validateConfigV1ForGroupPriority(
    const phosphor::led::GroupMap& ledMap)
{
    std::set<int> groupPriorities;
    for (const auto& [_, group] : ledMap)
    {
        groupPriorities.insert(group.priority);
    }

    if (groupPriorities.size() != ledMap.size())
    {
        throw ConfigValidationException(
            error::DuplicateGroupPriority,
            "When using Group Priority, no 2 Groups may have the same priority");
    }

    for (const auto& [groupName, group] : ledMap)
    {
        validateConfigV1GroupForGroupPriority(groupName, group);
    }
}

void validateConfigV1(const GroupMap& ledMap)
{
    const bool useGroupPriority = isUsingGroupPriority(ledMap);

    if (useGroupPriority)
    {
        validateConfigV1ForGroupPriority(ledMap);
    }
    else
    {
        validateConfigV1ForLedPriority(ledMap);
    }
}

} // namespace led
} // namespace phosphor
