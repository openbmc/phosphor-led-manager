#include "config.h"

#include "ledlayout.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

using Json = nlohmann::json;
using group = std::set<phosphor::led::Layout::LedAction>;
using LedLayout = std::map<std::string, group>;

static const Json empty{};

/** @brief Parse LED JSON file and output Json object
 *
 *  @param[in] path - path of LED JSON file
 *
 *  @return Json - Json object
 */
inline Json readJson(const std::string& path)
{
    fs::path dir(path);
    if (!fs::exists(dir) || fs::is_empty(dir))
    {
        return {};
    }

    std::ifstream jsonFile(path);
    if (!jsonFile.is_open())
    {
        std::cerr << "Error opening LED JSON file, PATH=" << path << "\n";
        return {};
    }

    return Json::parse(jsonFile);
}

phosphor::led::Layout::Action getAction(const std::string& action)
{
    if (action == "On")
    {
        return phosphor::led::Layout::On;
    }
    else if (action == "Off")
    {
        return phosphor::led::Layout::Off;
    }
    else
    {
        return phosphor::led::Layout::Blink;
    }
}

LedLayout loadLedJson()
{
    LedLayout ledLayout{};
    try
    {
        auto json = readJson(LED_JSON_FILE);
        if (!json.empty())
        {
            auto leds = json.value("leds", empty);
            for (const auto& led : leds)
            {
                auto objpath = led.value("objpath", "");
                auto groups = led.value("groups", empty);

                group ledActions{};
                for (const auto& group : groups)
                {
                    auto name = group.value("name", "");
                    auto action = getAction(group.value("Action", "Off"));
                    uint8_t dutyOn = group.value("DutyOn", 50);
                    uint16_t period = group.value("Period", 0);
                    auto priority = getAction(group.value("Priority", "Blink"));
                    phosphor::led::Layout::LedAction ledAction{
                        name, action, dutyOn, period, priority};
                    ledActions.emplace(ledAction);
                }

                ledLayout.emplace(objpath, ledActions);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return ledLayout;
}