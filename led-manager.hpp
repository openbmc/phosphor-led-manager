#pragma once

#include <map>
#include <set>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include "xyz/openbmc_project/Led/Group/server.hpp"
namespace phosphor
{
namespace led
{

/** @class Group
 *  @brief Manages group of LEDs and applies action on the elements of group
 */
class Group : sdbusplus::server::object::object<
              sdbusplus::xyz::openbmc_project::Led::server::Group>
{
    public:
        /** @brief Define possible actions on a given LED.
         *  For the BLINK operation, follow 50-50 duty cycle
         */
        enum Action
        {
            Off,
            On,
            Blink,
        };

        Group() = delete;
        ~Group() = default;
        Group(const Group&) = delete;
        Group& operator=(const Group&) = delete;
        Group(Group&&) = default;
        Group& operator=(Group&&) = default;

        /** @brief Constructs LED Group
         *
         * @param[in] bus     - Handle to system dbus
         * @param[in] objPath - The Dbus path that hosts LED group
         */
        Group(sdbusplus::bus::bus& bus, const std::string& objPath);

        /** @brief Name of the LED and it's proposed action.
         *  This structure is supplied as configuration at build time
         */
        struct LedAction
        {
            std::string name;
            Action action;

            // Needed for inserting elements into sets
            bool operator<(const LedAction& right) const
            {
                if (name == right.name)
                {
                    return action < right.action;
                }
                return name < right.name;
            }
        };

        /** @brief For finding intersection */
        static bool ledComp(const LedAction& left, const LedAction& right)
        {
            return left.name < right.name;
        }

        /** @brief Path of the group instance */
        std::string path;

        using group = std::set<LedAction>;

        /** @brief static global map constructed at compile time */
        static std::map<std::string, group> ledMap;

        /** @brief Pointers to groups that are in asserted state */
        static std::set<group*> assertedGroups;

        /** @brief Contains the LEDs that are in asserted state */
        static group currentState;

        /** @brief Given a group name, applies the action on the group
         *
         *  @param[in]  assert  -  Could be 0 or 1
         *  @return             -  Success or exception thrown
         */
        bool setGroupState(bool assert);

        /** @brief Property SET Override function
         *
         *  @param[in]  value   -  True or False
         *  @return             -  Success or exception thrown
         */
        bool asserted(bool value) override;

    private:
        /** @brief Finds the set of LEDs to operate on and executes action
         *
         *  @return: Returns '0' for now.
         */
        bool driveLEDs();
};

} // namespace led
} // namespace phosphor
