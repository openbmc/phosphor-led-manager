#include "json-config.hpp"

#include <gtest/gtest.h>

TEST(loadJsonConfig, testGoodPath)
{
    static constexpr auto jsonPath = "led/good/led-group-config.json";
    LedMap ledMap = loadJsonConfig(jsonPath);

    std::string objPath = "/xyz/openbmc_project/led/groups";
    std::string bmcBooted = objPath + "/bmc_booted";
    std::string powerOn = objPath + "/power_on";
    std::string enclosure = objPath + "/enclosure_identify";
    ASSERT_NE(ledMap.find(bmcBooted), ledMap.end());
    ASSERT_NE(ledMap.find(powerOn), ledMap.end());
    ASSERT_NE(ledMap.find(enclosure), ledMap.end());

    LedAction bmcBootedActions = ledMap.at(bmcBooted);
    LedAction powerOnActions = ledMap.at(powerOn);
    LedAction enclosureActions = ledMap.at(enclosure);

    for (const auto& bmcBootedAction : bmcBootedActions)
    {
        if (bmcBootedAction.name == "heartbeat")
        {
            ASSERT_EQ(bmcBootedAction.action, phosphor::led::Layout::On);
            ASSERT_EQ(bmcBootedAction.dutyOn, 50);
            ASSERT_EQ(bmcBootedAction.period, 0);
            ASSERT_EQ(bmcBootedAction.priority, phosphor::led::Layout::Blink);
        }
    }

    for (const auto& powerOnAction : powerOnActions)
    {
        if (powerOnAction.name == "power")
        {
            ASSERT_EQ(powerOnAction.action, phosphor::led::Layout::On);
            ASSERT_EQ(powerOnAction.dutyOn, 50);
            ASSERT_EQ(powerOnAction.period, 0);
            ASSERT_EQ(powerOnAction.priority, phosphor::led::Layout::On);
        }
    }

    for (const auto& enclosureAction : enclosureActions)
    {
        if (enclosureAction.name == "front_id")
        {
            ASSERT_EQ(enclosureAction.action, phosphor::led::Layout::Blink);
            ASSERT_EQ(enclosureAction.dutyOn, 50);
            ASSERT_EQ(enclosureAction.period, 1000);
            ASSERT_EQ(enclosureAction.priority, phosphor::led::Layout::Blink);
        }
        else if (enclosureAction.name == "rear_id")
        {
            ASSERT_EQ(enclosureAction.action, phosphor::led::Layout::Blink);
            ASSERT_EQ(enclosureAction.dutyOn, 50);
            ASSERT_EQ(enclosureAction.period, 1000);
            ASSERT_EQ(enclosureAction.priority, phosphor::led::Layout::Blink);
        }
    }
}

TEST(loadJsonConfig, testBadPath)
{
    static constexpr auto jsonPath = "led/malformed/led-group-config.json";
    ASSERT_THROW(loadJsonConfig(jsonPath), std::exception);
}