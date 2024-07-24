#pragma once

#include <xyz/openbmc_project/Led/Physical/server.hpp>

#include <set>
#include <string>
#include <unordered_map>

namespace phosphor
{
namespace led
{
/** @namespace Layout
 *  @brief Depicts the LED and their mappings and group actions
 */
namespace Layout
{

using Action = sdbusplus::xyz::openbmc_project::Led::server::Physical::Action;

/** @brief Name of the LED and it's proposed action.
 *  This structure is supplied as configuration at build time
 */
struct LedAction
{
    std::string name;
    Action action;
    uint8_t dutyOn;
    uint16_t period;
    std::optional<Action> priority;

    // Order LEDs such that same LEDs are grouped next to
    // each other and the same LEDs are in priority order
    // with the highest priority coming first
    bool operator<(const LedAction& right) const
    {
        if (name == right.name)
        {
            if (action == right.action)
            {
                return false;
            }
            else if (action == priority)
            {
                return true;
            }
        }
        return name < right.name;
    }
};
} // namespace Layout

using ActionSet = std::set<Layout::LedAction>;

} // namespace led
} // namespace phosphor
