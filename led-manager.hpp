#include <map>
#include <set>
#include <sdbusplus/bus.hpp>

/** @class LedManager
 *  @brief Manages group of LEDs and applies action on the elements of group
 */
class LedManager
{
private:
    /** Define possible actions on a given LED.
     *  For the BLINK operation, follow 50-50 duty cycle
     */
    enum action
    {
        OFF,
        ON,
        BLINK,
    };

    /** @brief Dbus constructs used by LED manager */
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

    /** Name of the LED and it's proposed action.
     * This structure is supplied as configuration at build time
     */
    struct LedAction
    {
        std::string name;
        LedManager::action action;
    };

    /** A static global map is created at once */
    static std::map<std::string,
           std::set<LedManager::LedAction>> cv_ledMap;

    /** Need these frined here to keep action enums private to this class
     *  and not having to mention name of the class in the data file */
    friend bool operator<(const LedManager::LedAction&,
                          const LedManager::LedAction&);

    friend bool operator==(const LedManager::LedAction&,
                           const LedManager::LedAction&);
    void run();
};
