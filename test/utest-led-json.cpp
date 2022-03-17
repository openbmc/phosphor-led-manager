#include "json-parser.hpp"

#include <gtest/gtest.h>

TEST(loadJsonConfig, testGoodPath)
{
    static constexpr auto jsonPath = "config/led-group-config.json";
    auto ledMap = loadJsonConfig(jsonPath);

    std::string objPath = "/xyz/openbmc_project/led/groups";
    std::string bmcBooted = objPath + "/bmc_booted";
    std::string powerOn = objPath + "/power_on";
    std::string enclosureIdentify = objPath + "/enclosure_identify";

    ASSERT_EQ(ledMap.contains(bmcBooted), true);
    ASSERT_EQ(ledMap.contains(powerOn), true);
    ASSERT_EQ(ledMap.contains(enclosureIdentify), true);

    auto& bmcBootedActions = ledMap.at(bmcBooted);
    auto& powerOnActions = ledMap.at(powerOn);
    auto& enclosureIdentifyActions = ledMap.at(enclosureIdentify);

    for (const auto& group : bmcBootedActions)
    {
        ASSERT_EQ(group.name, "heartbeat");
        ASSERT_EQ(group.action, phosphor::led::Layout::Action::On);
        ASSERT_EQ(group.dutyOn, 50);
        ASSERT_EQ(group.period, 0);
        ASSERT_EQ(group.priority, phosphor::led::Layout::Action::Blink);
    }

    for (const auto& group : powerOnActions)
    {
        ASSERT_EQ(group.name, "power");
        ASSERT_EQ(group.action, phosphor::led::Layout::Action::On);
        ASSERT_EQ(group.dutyOn, 50);
        ASSERT_EQ(group.period, 0);
        ASSERT_EQ(group.priority, phosphor::led::Layout::Action::On);
    }

    for (const auto& group : enclosureIdentifyActions)
    {
        if (group.name == "front_id")
        {
            ASSERT_EQ(group.action, phosphor::led::Layout::Action::Blink);
            ASSERT_EQ(group.dutyOn, 50);
            ASSERT_EQ(group.period, 1000);
            ASSERT_EQ(group.priority, phosphor::led::Layout::Action::Blink);
        }
        else if (group.name == "rear_id")
        {
            ASSERT_EQ(group.action, phosphor::led::Layout::Action::Blink);
            ASSERT_EQ(group.dutyOn, 50);
            ASSERT_EQ(group.period, 1000);
            ASSERT_EQ(group.priority, phosphor::led::Layout::Action::Blink);
        }
        else
        {
            ASSERT_TRUE(false);
        }
    }
}

TEST(loadJsonConfig, testBadPath)
{
    static constexpr auto jsonPath = "config/led-group-config-malformed.json";
    ASSERT_THROW(loadJsonConfig(jsonPath), std::exception);
}

TEST(validatePriority, testGoodPriority)
{
    PriorityMap priorityMap{};
    validatePriority("heartbeat", phosphor::led::Layout::Action::Blink,
                     priorityMap);
    validatePriority("power", phosphor::led::Layout::Action::On, priorityMap);

    ASSERT_EQ(priorityMap.at("heartbeat"),
              phosphor::led::Layout::Action::Blink);
    ASSERT_EQ(priorityMap.at("power"), phosphor::led::Layout::Action::On);
}

TEST(validatePriority, testBadPriority)
{
    PriorityMap priorityMap{};
    validatePriority("heartbeat", phosphor::led::Layout::Action::Blink,
                     priorityMap);

    ASSERT_EQ(priorityMap.at("heartbeat"),
              phosphor::led::Layout::Action::Blink);
    ASSERT_THROW(validatePriority("heartbeat",
                                  phosphor::led::Layout::Action::On,
                                  priorityMap),
                 std::runtime_error);
}
