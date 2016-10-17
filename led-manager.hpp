#include <map>
#include <set>
#include <sdbusplus/bus.hpp>

/** @class LedManager
 *  @brief Manages group of LEDs and applies action on the elements of group
 */

class LedManager
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

    // Dbus constructs used by LED manager
    sdbusplus::bus::bus iv_bus;
    const char* iv_name = "org.openbmc.ledmanager";
    const char* iv_path = "/org/openbmc/ledmanager/groups";
    const char* iv_intf = "org.openbmc.ledmanager";

public:

    LedManager();
    ~LedManager() = default;
    LedManager(const LedManager&) = delete;
    LedManager& operator=(const LedManager&) = delete;
    LedManager(LedManager&&) = default;
    LedManager& operator=(LedManager&&) = default;

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

    // A static global map is created at once
    static std::map<std::string,
           std::set<LedManager::LedAction>> cv_ledMap;

    void run();
};
