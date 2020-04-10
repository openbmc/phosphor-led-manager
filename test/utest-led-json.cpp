#include "json-config.hpp"

#include <gtest/gtest.h>

TEST(loadJsonConfig, testGoodPath)
{
    static constexpr auto jsonPath = "config/led-group-config.json";
    LedMap ledMap = loadJsonConfig(jsonPath);

    std::string objPath = "/xyz/openbmc_project/led/groups";
    std::string bmcBooted = objPath + "/bmc_booted";
    std::string powerOn = objPath + "/power_on";
    std::string enclosureIdentify = objPath + "/enclosure_identify";
    ASSERT_NE(ledMap.find(bmcBooted), ledMap.end());
    ASSERT_NE(ledMap.find(powerOn), ledMap.end());
    ASSERT_NE(ledMap.find(enclosureIdentify), ledMap.end());

    LedAction bmcBootedActions = ledMap.at(bmcBooted);
    LedAction powerOnActions = ledMap.at(powerOn);
    LedAction enclosureIdentifyActions = ledMap.at(enclosureIdentify);

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

    for (const auto& enclosureIdentifyAction : enclosureIdentifyActions)
    {
        if (enclosureIdentifyAction.name == "front_id")
        {
            ASSERT_EQ(enclosureIdentifyAction.action,
                      phosphor::led::Layout::Blink);
            ASSERT_EQ(enclosureIdentifyAction.dutyOn, 50);
            ASSERT_EQ(enclosureIdentifyAction.period, 1000);
            ASSERT_EQ(enclosureIdentifyAction.priority,
                      phosphor::led::Layout::Blink);
        }
        else if (enclosureIdentifyAction.name == "rear_id")
        {
            ASSERT_EQ(enclosureIdentifyAction.action,
                      phosphor::led::Layout::Blink);
            ASSERT_EQ(enclosureIdentifyAction.dutyOn, 50);
            ASSERT_EQ(enclosureIdentifyAction.period, 1000);
            ASSERT_EQ(enclosureIdentifyAction.priority,
                      phosphor::led::Layout::Blink);
        }
    }
}

TEST(loadJsonConfig, testBadPath)
{
    static constexpr auto jsonPath = "config/led-group-config-malformed.json";
    ASSERT_THROW(loadJsonConfig(jsonPath), std::exception);
}