#pragma once

#include <map>
#include <set>
#include <sdbusplus/bus.hpp>

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

    Manager(const char*, const char*, const char*);
    Manager() = delete;
    ~Manager() = default;
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager(Manager&&) = default;
    Manager& operator=(Manager&&) = default;

    /** @brief Define possible actions on a given LED.
     *  For the BLINK operation, follow 50-50 duty cycle
     */
    enum Action
    {
        OFF,
        ON,
        BLINK,
    };

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

    /** @brief static global map constructed at compile time */
    static std::map<std::string,
           std::set<LedAction>> cv_LedMap;

    using group = std::set<LedAction>;

    /** @brief Pointers to groups that are in asserted state */
    static std::set<group*> cv_AssertedGroups;

    /** @brief Contains the LEDs that are in asserted state */
    static group cv_CurrentState;

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
    int setGroupState(const std::string& name, const bool& assert);

private:
    /** @brief Dbus constructs used by LED manager */
    sdbusplus::bus::bus iv_bus;

    /** @brief Finds the set of LEDs to operate on and executes action */
    int driveLEDs();
};

} // namespace led

} // namespace phosphor
