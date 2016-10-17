#include <map>
#include <set>
#include <sdbusplus/bus.hpp>

class LedManager
{
private:
    // Possible actions on a given LED. For now, we are not
    // considering custom blink rates. Default blink is at
    // a 50% duty cycle
    enum action
    {
        OFF = 0,
        ON = 1,
        BLINK = 2,
    };

    // LED Dbus service
    sdbusplus::bus::bus iv_bus;
    const char* iv_name = "org.openbmc.ledmanager";
    const char* iv_path = "/org/openbmc/ledmanager";
    const char* iv_intf = "org.openbmc.ledmanager";

public:
    LedManager();
    LedManager(const LedManager&) = delete;
    struct LedAction
    {
        std::string name;
        LedManager::action action;
    };
    static std::map<std::string,
           std::set<LedManager::LedAction>> gMap;

    friend bool operator<(const LedManager::LedAction&,
                          const LedManager::LedAction&);
    friend bool operator==(const LedManager::LedAction&,
                          const LedManager::LedAction&);
    void run();
};
