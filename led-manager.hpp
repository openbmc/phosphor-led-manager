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
        OFF,
        ON,
        BLINK,
    };

    Manager() = delete;
    ~Manager() = default;
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager(Manager&&) = default;
    Manager& operator=(Manager&&) = default;

    /** @brief Constructs LED manager
     *
     * @param[in] busName   - The Dbus name to own
     * @param[in] objPath   - The Dbus path that hosts LED manager
     * @param[in] intfName  - The Dbus interface
     */
    Manager(const char* busName, const char* objPath, const char* intfName);

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

    /** @brief Dbus constructs used by LED manager */
    sdbusplus::bus::bus iv_bus;

    /** @brief sd_bus object manager */
    sdbusplus::server::manager::manager iv_ObjManager;

    /** @brief Individual objects */
    std::vector<sdbusplus::server::interface::interface> iv_IntfContainer;

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
    /** @brief Finds the set of LEDs to operate on and executes action
     *
     *  @return: Returns '0' for now.
     */
    int driveLEDs();
};

} // namespace led

} // namespace phosphor
