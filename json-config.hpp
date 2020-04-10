#include "config.h"

#include "ledlayout.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

using Json = nlohmann::json;
using LedAction = std::set<phosphor::led::Layout::LedAction>;
using LedMap = std::map<std::string, LedAction>;

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
        throw std::invalid_argument("Wrong file path");
    }

    std::ifstream jsonFile(path);
    if (!jsonFile.is_open())
    {
        std::cerr << "Error opening LED JSON file, PATH=" << path << "\n";
	throw std::system_error();
    }

    return Json::parse(jsonFile);
}

/** @brief Returns action enum based on string
 *
 *  @param[in] action - action string
 *
 *  @return Action - action enum (On/Blink)
 */
phosphor::led::Layout::Action getAction(const std::string& action)
{
    assert(action == "On" || action == "Blink");

    return action == "Blink" ? phosphor::led::Layout::Blink
                             : phosphor::led::Layout::On;
}

/** @brief Load JSON config and return led map
 *
 *  @return LedMap - Generated an std::map of LedAction
 */
LedMap loadJsonConfig()
{
    LedMap ledMap{};

    // define the default JSON as empty
    const Json empty{};

    try
    {
        auto json = readJson(LED_JSON_FILE);
        auto leds = json.value("leds", empty);
        for (const auto& entry : leds)
        {
            fs::path tmpPath(std::string{OBJPATH});
            tmpPath /= entry.value("group", "");
            auto objpath = tmpPath.string();
            auto members = entry.value("members", empty);

            LedAction ledActions{};
            for (const auto& member : members)
            {
                auto name = member.value("name", "");
                auto action = getAction(member.value("Action", ""));
                uint8_t dutyOn = member.value("DutyOn", 50);
                uint16_t period = member.value("Period", 0);

                // Since only have Blink/On and default priority is Blink
                auto priority = getAction(member.value("Priority", "Blink"));
                phosphor::led::Layout::LedAction ledAction{name, action, dutyOn,
                                                           period, priority};
                ledActions.emplace(ledAction);
            }

            // Generated an std::map of LedGroupNames to std::set of LEDs
            // containing the name and properties.
            ledMap.emplace(objpath, ledActions);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        throw std::runtime_error("Load JSON file failed");
    }

    return ledMap;
}
