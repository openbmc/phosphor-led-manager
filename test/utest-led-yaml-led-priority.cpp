#include "group.hpp"
#include "ledlayout.hpp"
#include "manager.hpp"
#include "test-led-priority.hpp"

#include <sdbusplus/bus.hpp>

#include <algorithm>
#include <set>

#include <gtest/gtest.h>

using namespace phosphor::led;

using Action = phosphor::led::Layout::Action;

// systemLedMap is generated code
// static const phosphor::led::GroupMap systemLedMap = {};

const std::string basePath = "/xyz/openbmc_project/led/groups/";

TEST(YamlLedPriorityTest, assertPriorityOn)
{
    const std::string groupPath = basePath + "group1";
    EXPECT_EQ(systemLedMap.contains(groupPath), true);

    phosphor::led::Layout::GroupLayout group = systemLedMap.at(groupPath);

    EXPECT_EQ(group.priority, 0);
    EXPECT_EQ(group.actionSet.size(), 1);

    for (const auto& led : group.actionSet)
    {
        EXPECT_EQ(led.name, "led1");
        EXPECT_EQ(led.action, Action::On);
        EXPECT_EQ(led.priority, Action::On);
    }
}

TEST(YamlLedPriorityTest, assertPriorityOff)
{
    const std::string groupPath = basePath + "group2";
    EXPECT_EQ(systemLedMap.contains(groupPath), true);

    phosphor::led::Layout::GroupLayout group = systemLedMap.at(groupPath);

    EXPECT_EQ(group.priority, 0);
    EXPECT_EQ(group.actionSet.size(), 1);

    for (const auto& led : group.actionSet)
    {
        EXPECT_EQ(led.name, "led2");
        EXPECT_EQ(led.action, Action::Off);
        EXPECT_EQ(led.priority, Action::Off);
    }
}

TEST(YamlLedPriorityTest, assertPriorityBlink)
{
    const std::string groupPath = basePath + "group3";
    EXPECT_EQ(systemLedMap.contains(groupPath), true);

    phosphor::led::Layout::GroupLayout group = systemLedMap.at(groupPath);

    EXPECT_EQ(group.priority, 0);
    EXPECT_EQ(group.actionSet.size(), 1);

    for (const auto& led : group.actionSet)
    {
        EXPECT_EQ(led.name, "led3");
        EXPECT_EQ(led.action, Action::Blink);
        EXPECT_EQ(led.priority, Action::Blink);
    }
}
