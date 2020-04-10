#include "config.h"

#include "ledlayout.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/log.hpp>
#include <xyz/openbmc_project/Common/File/error.hpp>

namespace fs = std::filesystem;

using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::File::Error;

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
        log<level::ERR>("Wrong JSON file path",
                        entry("LED_JSON_FILE=%s", LED_JSON_FILE));
        elog<Open>(
            xyz::openbmc_project::Common::File::Open::ERRNO(errno),
            xyz::openbmc_project::Common::File::Open::PATH(LED_JSON_FILE));
    }

    std::ifstream jsonFile(path);
    if (!jsonFile.is_open())
    {
        log<level::ERR>("Failed to open LED JSON file",
                        entry("LED_JSON_FILE=%s", LED_JSON_FILE));
        elog<Open>(
            xyz::openbmc_project::Common::File::Open::ERRNO(errno),
            xyz::openbmc_project::Common::File::Open::PATH(LED_JSON_FILE));
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

    return ledMap;
}
