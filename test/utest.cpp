#include <set>
#include <algorithm>
#include <gtest/gtest.h>
#include "manager.hpp"

using namespace phosphor::led;

/** @brief Assert FanA4 Fault */
TEST(setGroupState, FaultOnFanA4)
{
    Manager manager;
    {
        // Assert the LEDs.
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
    {
        // De-Assert
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/FanA4Fault";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);

        // Need just the ledsDeAssserted populated with these.
        std::set<Manager::LedAction> refDeAssert = {
            {"Fan_A4", Manager::Action::On},
            {"EnclosureFaultBack", Manager::Action::On},
            {"EnclosureFaultFront", Manager::Action::On}
        };
        EXPECT_EQ(0, ledsAssert.size());
        EXPECT_EQ(3, ledsDeAssert.size());
        EXPECT_EQ(0, ledsUpdate.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        Manager::group temp {};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
        EXPECT_EQ(false, result);
    }
}

/** @brief Assert FanA2 Fault after asserting Fan_4*/
TEST(setGroupState, FaultOnFanA4AndA2)
{
    Manager manager;
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
    {
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        // Now assert Fan_A2 and expect that only Fan_A2 gets asserted
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
}

/** @brief Fault Fan4 and also identify FanA4 */
TEST(setGroupState, FanA4FaultAndIdentifyOn)
{
    Manager manager;
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
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
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
    {
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        // Now IDENTIFY the LEDs.
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

        // difference of refAssert and ledsAssert must be null.
        Manager::group temp {};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
        EXPECT_EQ(true, result);
    }
}

/** @brief Fault Fan4 + identify FanA4 + remove Fault and
 * expect a transition to Identify
 */
TEST(setGroupState, FanA4FaultIdentifyUndoFault)
{
    Manager manager;
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
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
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
    {
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        // Now IDENTIFY the LEDs.
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

        // difference of refAssert and ledsAssert must be null.
        Manager::group temp {};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
        EXPECT_EQ(true, result);
    }
    {
        // Now Undo the fault and make sure that Fan_A4 directly
        // moves to Blinking state.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/FanA4Fault";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        // Need just the ledsUpdate populated with these.
        std::set<Manager::LedAction> refUpdate = {
            {"Fan_A4", Manager::Action::Blink}
        };
        EXPECT_EQ(0, ledsAssert.size());
        EXPECT_EQ(2, ledsDeAssert.size());
        EXPECT_EQ(refUpdate.size(), ledsUpdate.size());

        // difference of refAssert and ledsAssert must be null.
        Manager::group temp {};
        std::set_difference(ledsUpdate.begin(), ledsUpdate.end(),
                            refUpdate.begin(), refUpdate.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
        EXPECT_EQ(false, result);
    }
}

/** @brief DeAssert EnclosureFault group */
TEST(setGroupState, EnclosureFaultOff)
{
    // Just expect NULL sets.
    Manager manager;

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

/** @brief Assert PowerSupply1Identify */
TEST(setGroupState, PowerSupply1IdentifyOn)
{
    Manager manager;
    {
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/PowerSupply1Identify";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);

        // Need just the ledsAssserted populated with these.
        std::set<Manager::LedAction> refAssert = {
            {"PowerSupply_1", Manager::Action::Blink},
            {"EnclosureIdentifyFront", Manager::Action::On},
            {"EnclosureIdentifyBack", Manager::Action::On}
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
    {
        // Now Assert EnclosureIdentify and make sure Enclosure LEDs now blink
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/EnclosureIdentify";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);

        // Need just the ledsAssserted populated with these.
        std::set<Manager::LedAction> refAssert = {
            {"EnclosureIdentifyFront", Manager::Action::Blink},
            {"EnclosureIdentifyBack", Manager::Action::Blink}
        };
        EXPECT_EQ(2, ledsAssert.size());
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
    {
        // Now De-assert EnclosureIdentify and make sure Enclosure LEDs now turn ON
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/EnclosureIdentify";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);

        // Need just the ledsAssserted populated with these.
        std::set<Manager::LedAction> refUpdate = {
            {"EnclosureIdentifyFront", Manager::Action::On},
            {"EnclosureIdentifyBack", Manager::Action::On}
        };
        EXPECT_EQ(0, ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());
        EXPECT_EQ(2, ledsUpdate.size());

        // difference of refUpdate and ledsUpdate must be null.
        Manager::group temp {};
        std::set_difference(ledsUpdate.begin(), ledsUpdate.end(),
                            refUpdate.begin(), refUpdate.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
        EXPECT_EQ(false, result);
    }
}
