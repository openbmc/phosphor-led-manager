#include "config-validator.hpp"
#include "group.hpp"
#include "ledlayout.hpp"
#include "test-empty-group.hpp"

#include <gtest/gtest.h>

using namespace phosphor::led;

// systemLedMap is generated code
// static const GroupMap systemLedMap = {};

const std::string basePath = "/xyz/openbmc_project/led/groups/";

TEST(YamlEmptyGroupTest, assertEmptyGroupExists)
{
    /* Empty led groups are supported since some boards may
     * not have the required leds to fill the expected groups.
     * Other software in openbmc (e.g bmcweb, led-manager itself)
     * expects certain groups and unintended error messages can result
     * if they are not present.
     */

    const std::string emptyGroupPath = basePath + "emptygroup";
    const std::string nonEmptyGroupPath = basePath + "nonemptygroup";

    EXPECT_EQ(systemLedMap.contains(emptyGroupPath), true);
    EXPECT_EQ(systemLedMap.contains(nonEmptyGroupPath), true);

    const Layout::GroupLayout& emptyGroup = systemLedMap.at(emptyGroupPath);
    const Layout::GroupLayout& nonEmptyGroup =
        systemLedMap.at(nonEmptyGroupPath);

    EXPECT_EQ(emptyGroup.actionSet.size(), 0);
    EXPECT_EQ(nonEmptyGroup.actionSet.size(), 1);

    // this should not throw
    phosphor::led::validateConfigV1(systemLedMap);
}
