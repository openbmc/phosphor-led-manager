#include <set>
#include <algorithm>
#include <gtest/gtest.h>
#include "manager.hpp"

using namespace phosphor::led;

// Global object to retain states.
Manager manager;

// NOTE : This SEQUENCE is a HARD requirement. Using shuffle option will give
//        incorrect results.

/** @brief Assert FanA4 Fault */
TEST(setGroupState, FanA4FaultOn)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/FanA4Fault";
    auto result = manager.setGroupState(group, true, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    // Need just the ledsAssserted populated with these.
    std::set<Manager::LedAction> refAssert = {
            {"Fan_A4", Manager::Action::On},
            {"EnclosureFaultBack", Manager::Action::On},
            {"EnclosureFaultFront", Manager::Action::On}
    };
    EXPECT_EQ(3, ledsAssert.size());
    EXPECT_EQ(0, ledsDeAssert.size());
    EXPECT_EQ(0, ledsUpdate.size());

    // difference of refAssert and ledsAssert must be null.
    Manager::group temp {};
    std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                        refAssert.begin(), refAssert.end(),
                        std::inserter(temp, temp.begin()));
    EXPECT_EQ(0, temp.size());
    EXPECT_EQ(true, result);
}

/** @brief Identify FanA4 */
TEST(setGroupState, FanA4IdentifyOn)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/FanA4Identify";
    auto result = manager.setGroupState(group, true, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    // Expect SET results
    std::set<Manager::LedAction> refAssert = {
            {"Fan_A4", Manager::Action::Blink},
            {"EnclosureIdentifyFront", Manager::Action::On},
            {"EnclosureIdentifyBack", Manager::Action::On}
    };
    EXPECT_EQ(refAssert.size(), ledsAssert.size());
    EXPECT_EQ(0, ledsDeAssert.size());
    EXPECT_EQ(0, ledsUpdate.size());

    // difference of refAssert and ledsDeAssert must be null.
    Manager::group temp {};
    std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                        refAssert.begin(), refAssert.end(),
                        std::inserter(temp, temp.begin()));
    EXPECT_EQ(0, temp.size());
    EXPECT_EQ(true, result);
}

/** @brief Assert FanA2 Fault */
TEST(setGroupState, FanA2FaultOn)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/FanA2Fault";
    auto result = manager.setGroupState(group, true, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    // Need just the ledsAssserted populated with these.
    std::set<Manager::LedAction> refAssert = {
            {"Fan_A2", Manager::Action::On}
    };
    EXPECT_EQ(1, ledsAssert.size());
    EXPECT_EQ(0, ledsDeAssert.size());
    EXPECT_EQ(0, ledsUpdate.size());

    // difference of refAssert and ledsAssert must be null.
    Manager::group temp {};
    std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                        refAssert.begin(), refAssert.end(),
                        std::inserter(temp, temp.begin()));
    EXPECT_EQ(0, temp.size());
    EXPECT_EQ(true, result);
}

/** @brief Assert PowerSupply1Identify */
TEST(setGroupState, PowerSupply1IdentifyOn)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/PowerSupply1Identify";
    auto result = manager.setGroupState(group, true, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    // Need just the ledsAssserted populated with these.
    std::set<Manager::LedAction> refAssert = {
            {"PowerSupply_1", Manager::Action::Blink}
    };
    EXPECT_EQ(1, ledsAssert.size());
    EXPECT_EQ(0, ledsDeAssert.size());
    EXPECT_EQ(0, ledsUpdate.size());

    // difference of refAssert and ledsAssert must be null.
    Manager::group temp {};
    std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                        refAssert.begin(), refAssert.end(),
                        std::inserter(temp, temp.begin()));
    EXPECT_EQ(0, temp.size());
    EXPECT_EQ(true, result);
}

/** @brief DeAssert EnclosureFault group */
TEST(setGroupState, EnclosureFaultOff)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/EnclosureFault";
    auto result = manager.setGroupState(group, false, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    EXPECT_EQ(0, ledsAssert.size());
    EXPECT_EQ(0, ledsDeAssert.size());
    EXPECT_EQ(0, ledsUpdate.size());
    EXPECT_EQ(false, result);
}

/** @brief Assert EnclosureFault group */
TEST(setGroupState, EnclosureFaultOn)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/EnclosureFault";
    auto result = manager.setGroupState(group, true, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    EXPECT_EQ(0, ledsAssert.size());
    EXPECT_EQ(0, ledsDeAssert.size());
    EXPECT_EQ(0, ledsUpdate.size());
    EXPECT_EQ(true, result);
}

/** @brief DeAssert FanA4 Fault */
TEST(setGroupState, FanA4FaultOff)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/FanA4Fault";
    auto result = manager.setGroupState(group, false, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    std::set<Manager::LedAction> refUpdate = {
            {"Fan_A4", Manager::Action::Blink}
    };
    EXPECT_EQ(refUpdate.size(), ledsUpdate.size());
    EXPECT_EQ(0, ledsDeAssert.size());
    EXPECT_EQ(0, ledsAssert.size());

    // difference of refUpdate and ledsUpdate must be null.
    Manager::group temp {};
    std::set_difference(ledsUpdate.begin(), ledsUpdate.end(),
                        refUpdate.begin(), refUpdate.end(),
                        std::inserter(temp, temp.begin()));
    EXPECT_EQ(0, temp.size());
    EXPECT_EQ(false, result);
}

/** @brief DeAssert PowerSupply1Identify */
TEST(setGroupState, PowerSupply1IdentifyOff)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/PowerSupply1Identify";
    auto result = manager.setGroupState(group, false, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    std::set<Manager::LedAction> refDeAssert = {
            {"PowerSupply_1", Manager::Action::Blink}
    };
    EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
    EXPECT_EQ(0, ledsAssert.size());
    EXPECT_EQ(0, ledsUpdate.size());

    // difference of refDeAssert and ledsDeAssert must be null.
    Manager::group temp {};
    std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                        refDeAssert.begin(), refDeAssert.end(),
                        std::inserter(temp, temp.begin()));
    EXPECT_EQ(0, temp.size());
    EXPECT_EQ(false, result);
}

/** @brief DeAssert FanA4 Identify */
TEST(setGroupState, FanA4IdentifyOff)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/FanA4Identify";
    auto result = manager.setGroupState(group, false, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    std::set<Manager::LedAction> refDeAssert = {
            {"Fan_A4", Manager::Action::Blink},
            {"EnclosureIdentifyFront", Manager::Action::On},
            {"EnclosureIdentifyBack", Manager::Action::On}
    };
    EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
    EXPECT_EQ(0, ledsAssert.size());
    EXPECT_EQ(0, ledsUpdate.size());

    // difference of refDeAssert and ledsDeAssert must be null.
    Manager::group temp {};
    std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                        refDeAssert.begin(), refDeAssert.end(),
                        std::inserter(temp, temp.begin()));
    EXPECT_EQ(0, temp.size());
    EXPECT_EQ(false, result);
}

/** @brief DeAssert FanA2 Fault */
TEST(setGroupState, FanA2FaultOff)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/FanA2Fault";
    auto result = manager.setGroupState(group, false, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    std::set<Manager::LedAction> refDeAssert = {
            {"Fan_A2", Manager::Action::On}
    };
    EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
    EXPECT_EQ(0, ledsAssert.size());
    EXPECT_EQ(0, ledsUpdate.size());

    // difference of refDeAssert and ledsDeAssert must be null.
    Manager::group temp {};
    std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                        refDeAssert.begin(), refDeAssert.end(),
                        std::inserter(temp, temp.begin()));
    EXPECT_EQ(0, temp.size());
    EXPECT_EQ(false, result);
}

/** @brief DeAssert EnclosureFault group */
TEST(setGroupState, EnclosureFaultOffAgain)
{
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    auto group = "/xyz/openbmc_project/ledmanager/groups/EnclosureFault";
    auto result = manager.setGroupState(group, false, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    std::set<Manager::LedAction> refDeAssert = {
            {"EnclosureFaultBack", Manager::Action::On},
            {"EnclosureFaultFront", Manager::Action::On}
    };
    EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
    EXPECT_EQ(0, ledsAssert.size());
    EXPECT_EQ(0, ledsUpdate.size());

    // difference of refDeAssert and ledsDeAssert must be null.
    Manager::group temp {};
    std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                        refDeAssert.begin(), refDeAssert.end(),
                        std::inserter(temp, temp.begin()));
    EXPECT_EQ(0, temp.size());
    EXPECT_EQ(false, result);
}
