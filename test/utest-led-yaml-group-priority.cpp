#include "group.hpp"
#include "ledlayout.hpp"
#include "manager.hpp"
#include "test-group-priority.hpp"

#include <sdbusplus/bus.hpp>

#include <algorithm>
#include <set>

#include <gtest/gtest.h>

using namespace phosphor::led;

using Action = phosphor::led::Layout::Action;

// systemLedMap is generated code
// static const phosphor::led::GroupMap systemLedMap = {};

const std::string basePath = "/xyz/openbmc_project/led/groups/";

TEST(YamlGroupPriorityTest, assertYAMLLedOn)
{
    const std::string groupPath = basePath + "group1";
    EXPECT_EQ(systemLedMap.contains(groupPath), true);

    phosphor::led::Layout::GroupLayout group = systemLedMap.at(groupPath);

    EXPECT_EQ(group.priority, 1);
    EXPECT_EQ(group.actionSet.size(), 1);

    for (const auto& led : group.actionSet)
    {
        EXPECT_EQ(led.name, "led1");
        EXPECT_EQ(led.action, Action::On);
        EXPECT_EQ(led.priority, std::nullopt);
    }
}

TEST(YamlGroupPriorityTest, assertYAMLLedOff)
{
    const std::string groupPath = basePath + "group2";
    EXPECT_EQ(systemLedMap.contains(groupPath), true);

    phosphor::led::Layout::GroupLayout group = systemLedMap.at(groupPath);

    EXPECT_EQ(group.priority, 2);
    EXPECT_EQ(group.actionSet.size(), 1);

    for (const auto& led : group.actionSet)
    {
        EXPECT_EQ(led.name, "led1");
        EXPECT_EQ(led.action, Action::Off);
        EXPECT_EQ(led.priority, std::nullopt);
    }
}

TEST(YamlGroupPriorityTest, assertYAMLLedBlink)
{
    const std::string groupPath = basePath + "group3";
    EXPECT_EQ(systemLedMap.contains(groupPath), true);

    phosphor::led::Layout::GroupLayout group = systemLedMap.at(groupPath);

    EXPECT_EQ(group.priority, 3);
    EXPECT_EQ(group.actionSet.size(), 1);

    for (const auto& led : group.actionSet)
    {
        EXPECT_EQ(led.name, "led1");
        EXPECT_EQ(led.action, Action::Blink);
        EXPECT_EQ(led.dutyOn, 50);
        EXPECT_EQ(led.period, 1000);
        EXPECT_EQ(led.priority, std::nullopt);
    }
}

TEST(YamlGroupPriorityTest, assertYAMLGroupPriority)
{
    const std::string groupPath = basePath + "group4";
    EXPECT_EQ(systemLedMap.contains(groupPath), true);

    phosphor::led::Layout::GroupLayout group = systemLedMap.at(groupPath);

    EXPECT_EQ(group.priority, 2);
    EXPECT_EQ(group.actionSet.size(), 2);

    int found = 0;
    for (const auto& led : group.actionSet)
    {
        if (led.name == "led1")
        {
            EXPECT_EQ(led.action, Action::On);
            EXPECT_EQ(led.priority, std::nullopt);
            found++;
        }
        if (led.name == "led2")
        {
            EXPECT_EQ(led.action, Action::Off);
            EXPECT_EQ(led.priority, std::nullopt);
            found++;
        }
    }

    EXPECT_EQ(found, group.actionSet.size());
}
