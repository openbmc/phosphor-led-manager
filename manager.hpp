#pragma once

#include <map>
#include <set>
#include "ledlayout.hpp"
namespace phosphor
{
namespace led
{

/** @class Manager
 *  @brief Manages group of LEDs and applies action on the elements of group
 */
class Manager
{
    public:
        /** @brief Only need the default Manager */
        Manager() = delete;
        ~Manager() = default;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;
        Manager(Manager&&) = delete;
        Manager& operator=(Manager&&) = delete;

        /** @brief For finding intersection */
        static bool ledComp(const phosphor::led::Layout::LedAction& left,
                            const phosphor::led::Layout::LedAction& right)
        {
            return left.name < right.name;
        }

        using group = std::set<phosphor::led::Layout::LedAction>;

        /** @brief static global map constructed at compile time */
        const std::map<std::string, group>& ledMap;

        /** @brief Refer the user supplied LED layout.
         *
         *  @param [in] ledLayout - LEDs group layout
         */
        Manager(const std::map<std::string, Manager::group>& ledLayout)
                : ledMap(ledLayout)
        {
            // Nothing here
        }

        /** @brief Given a group name, applies the action on the group
         *
         *  @param[in]  path          -  dbus path of group
         *  @param[in]  assert        -  Could be true or false
         *  @param[in]  ledsAssert    -  LEDs that are to be asserted newly
         *  @param[in]  ledsDeAssert  -  LEDs that are to be Deasserted
         *  @param[in]  ledsUpdate    -  LEDs that need a transition between
         *                               different types of asserted states.
         *
         *  @return                   -  Success or exception thrown
         */
        bool setGroupState(const std::string& path, bool assert,
                           group& ledsAssert, group& ledsDeAssert,
                           group& ledsUpdate);

        /** @brief Finds the set of LEDs to operate on and executes action
         *
         *  @param[in]  ledsAssert    -  LEDs that are to be asserted newly
         *  @param[in]  ledsDeAssert  -  LEDs that are to be Deasserted
         *  @param[in]  ledsUpdate    -  LEDs that need a transition between
         *                               different types of asserted states.
         *
         *  @return: None
         */
        void driveLEDs(group& ledsAssert, group& ledsDeAssert,
                       group& ledsUpdate);

    private:
        /** @brief Pointers to groups that are in asserted state */
        std::set<const group*> assertedGroups;

        /** @brief Contains the LEDs that are in asserted state */
        group currentState;

};

} // namespace led
} // namespace phosphor
