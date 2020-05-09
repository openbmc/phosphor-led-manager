#include "serialize.hpp"

#include <gtest/gtest.h>

using namespace phosphor::led;

TEST(SerializeTest, testSimpleStoreGroups)
{
    static constexpr auto& path = "config/led-save-group.json";
    static constexpr auto& bmcBooted =
        "/xyz/openbmc_project/led/groups/bmc_booted";

    Serialize serialize(path);

    serialize.storeGroups(bmcBooted, true);

    ASSERT_EQ(true, serialize.getGroupSavedState(bmcBooted));
}

TEST(SerializeTest, testMultipleStoreGroups)
{
    static constexpr auto& path = "config/led-save-group.json";
    static constexpr auto& bmcBooted =
        "/xyz/openbmc_project/led/groups/bmc_booted";
    static constexpr auto& powerOn = "/xyz/openbmc_project/led/groups/power_on";
    static constexpr auto& enclosureIdentify =
        "/xyz/openbmc_project/led/groups/EnclosureIdentify";

    Serialize serialize(path);

    serialize.storeGroups(bmcBooted, true);
    serialize.storeGroups(powerOn, true);
    serialize.storeGroups(enclosureIdentify, true);

    ASSERT_EQ(true, serialize.getGroupSavedState(bmcBooted));
    ASSERT_EQ(true, serialize.getGroupSavedState(powerOn));
    ASSERT_EQ(true, serialize.getGroupSavedState(enclosureIdentify));
}

TEST(SerializeTest, testInvalidStoreGroups)
{
    static constexpr auto& path = "config/led-save-group.json";
    static constexpr auto& bmcBooted =
        "/xyz/openbmc_project/led/groups/bmc_booted";
    static constexpr auto& powerOn = "/xyz/openbmc_project/led/groups/power_on";
    static constexpr auto& enclosureIdentify =
        "/xyz/openbmc_project/led/groups/EnclosureIdentify";

    Serialize serialize(path);

    serialize.storeGroups(bmcBooted, false);
    serialize.storeGroups(powerOn, false);
    serialize.storeGroups(enclosureIdentify, false);

    ASSERT_EQ(false, serialize.getGroupSavedState(bmcBooted));
    ASSERT_EQ(false, serialize.getGroupSavedState(powerOn));
    ASSERT_EQ(false, serialize.getGroupSavedState(enclosureIdentify));
}
