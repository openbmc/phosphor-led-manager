#pragma once

#include <map>
#include <set>
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
        /** @brief Define possible actions on a given LED.
         *  For the BLINK operation, follow 50-50 duty cycle
         */
        enum Action
        {
            Off,
            On,
            Blink,
        };

        /** @brief Only need the default Manager */
        Manager() = default;
        ~Manager() = default;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;
        Manager(Manager&&) = delete;
        Manager& operator=(Manager&&) = delete;

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

        /** @brief Given a group name, applies the action on the group
         *
         *  @param[in]  path    -  dbus path of group
         *  @param[in]  assert  -  Could be true or false
         *  @return             -  Success or exception thrown
         */
        bool setGroupState(const std::string& path, bool assert);

    private:
        /** @brief Pointers to groups that are in asserted state */
        std::set<const group*> assertedGroups;

        /** @brief Contains the LEDs that are in asserted state */
        group currentState;

        /** @brief Finds the set of LEDs to operate on and executes action
         *
         *  @return: None
         */
        void driveLEDs();
};

} // namespace led
} // namespace phosphor
