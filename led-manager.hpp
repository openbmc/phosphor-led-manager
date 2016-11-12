#pragma once

#include <map>
#include <set>
#include <vector>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/interface.hpp>
#include <sdbusplus/server/manager.hpp>
namespace phosphor
{
namespace led
{

/** @class Group
 *  @brief Manages group of LEDs and applies action on the elements of group
 */

class Group
{
    public:
        /** @brief Define possible actions on a given LED.
         *  For the BLINK operation, follow 50-50 duty cycle
         */
        enum Action
        {
            OFF,
            ON,
            BLINK,
        };

        Group() = delete;
        ~Group() = default;
        Group(const Group&) = delete;
        Group& operator=(const Group&) = delete;
        Group(Group&&) = delete;
        Group& operator=(Group&&) = delete;

        /** @brief Constructs LED manager
         *
         * @param[in] busName   - The Dbus name to own
         * @param[in] objPath   - The Dbus path that hosts LED manager
         * @param[in] intfName  - The Dbus interface
         */
        Group(const char* busName, const char* objPath, const char* intfName);

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

        using group = std::set<LedAction>;

        /** @brief static global map constructed at compile time */
        static const std::map<std::string, group> ledMap;

        /** @brief Dbus constructs used by LED manager */
        sdbusplus::bus::bus bus;

        /** @brief sd_bus object manager */
        sdbusplus::server::manager::manager objManager;

        /** @brief Individual objects */
        std::vector<sdbusplus::server::interface::interface> intfContainer;

        /** @brief Pointers to groups that are in asserted state */
        static std::set<const group*> assertedGroups;

        /** @brief Contains the LEDs that are in asserted state */
        static group currentState;

        /** @brief Waits on the client request and processes them */
        void run();

        /** @brief Given a group name, tells if its in asserted state or not.
         *
         *  @param[in]  name    -  Group name
         *  @return             -  Whether in asserted state or not
         */
        bool getGroupState(const std::string& name);

        /** @brief Given a group name, applies the action on the group
         *
         *  @param[in]  name    -  Group name
         *  @param[in]  assert  -  Could be 0 or 1
         *  @return             -  Success or exception thrown
         */
        int setGroupState(const std::string& name, bool assert);

    private:
        /** @brief Finds the set of LEDs to operate on and executes action
         *
         *  @return: Returns '0' for now.
         */
        int driveLEDs();
};

} // namespace led

} // namespace phosphor
