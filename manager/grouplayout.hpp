#pragma once

#include "ledlayout.hpp"

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

/** @brief Name of the Group and its properties
 */
struct GroupLayout
{
    int priority = 0;
    ActionSet actionSet;
};

struct CompareGroupLayout
{
    bool operator()(const Layout::GroupLayout* lhs,
                    const Layout::GroupLayout* rhs) const
    {
        return lhs->priority < rhs->priority;
    };
};

} // namespace Layout

using GroupMap = std::unordered_map<std::string, Layout::GroupLayout>;

} // namespace led
} // namespace phosphor
