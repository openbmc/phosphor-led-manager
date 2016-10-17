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
private:

    /** @brief Define possible actions on a given LED.
     *  For the BLINK operation, follow 50-50 duty cycle
     */
    enum Action
    {
        OFF,
        ON,
        BLINK,
    };

    /** @brief Dbus constructs used by LED manager */
    sdbusplus::bus::bus iv_bus;

public:
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

        // Needed for set union / intersect
        bool operator==(const LedAction& right) const
        {
            // Only if name and action are same, consider equal
            if (name == right.name &&
                    action == right.action)
            {
                return true;
            }
            return false;
        }
    };

    /** @brief static global map constructed at compile time */
    static const std::map<std::string,
           std::set<LedAction>> cv_LedMap;

    /** @brief sd_bus object manager */
    sdbusplus::server::manager::manager objManager;

    /** @brief Individual objects */
    std::vector<sdbusplus::server::interface::interface> intfContainer;

    void run();
};

} // namespace led

} // namespace phosphor
