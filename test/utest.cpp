#include <set>
#include <algorithm>
#include <gtest/gtest.h>
#include "manager.hpp"
#include "led-test-map.hpp"

using namespace phosphor::led;
class LedTest : public ::testing::Test
{
    public:
        virtual void SetUp()
        {
            // Not having a need at the moment but for future.
        }
        virtual void TearDown()
        {
            // Leaving upto auto cleanup.
        }
};

/** @brief Assert Single LED to On */
TEST_F(LedTest, assertSingleLedOn)
{
    Manager manager(singleLedOn);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/SingleLed";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::On},
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
    }
}

/** @brief Assert Single LED to Blink */
TEST_F(LedTest, assertSingleLedBlink)
{
    Manager manager(singleLedBlink);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/SingleLed";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::Blink},
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
    }
}

/** @brief Assert Multiple LEDs to On */
TEST_F(LedTest, assertMultipleLedOn)
{
    Manager manager(multipleLedsOn);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::On},
            {"Two",phosphor::led::Layout::On},
            {"Three",phosphor::led::Layout::On},
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
    }
}

/** @brief Assert Multiple LEDs to Blink */
TEST_F(LedTest, assertMultipleLedBlink)
{
    Manager manager(multipleLedsBlink);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::Blink},
            {"Two",phosphor::led::Layout::Blink},
            {"Three",phosphor::led::Layout::Blink},
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
    }
}

/** @brief Assert Multiple LEDs to mix of On and Blink */
TEST_F(LedTest, assertMultipleLedOnAndBlink)
{
    Manager manager(multipleLedsOnAndBlink);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/MultipleLedsMix";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::Blink},
            {"Two",phosphor::led::Layout::On},
            {"Three",phosphor::led::Layout::Blink},
            {"Four",phosphor::led::Layout::On},
            {"Five",phosphor::led::Layout::On},
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
    }
}

/** @brief Assert 2 groups not having common LEDs*/
TEST_F(LedTest, assertTwoGroupsOnWithDistinctLEDOn)
{
    Manager manager(twoGroupsWithDistinctLEDsOn);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::On},
            {"Two",phosphor::led::Layout::On},
            {"Three",phosphor::led::Layout::On},
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
    }
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Four",phosphor::led::Layout::On},
            {"Five",phosphor::led::Layout::On},
            {"Six",phosphor::led::Layout::On},
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
    }
}

/** @brief Assert 2 groups having one of the LEDs common */
TEST_F(LedTest, asserttwoGroupsWithOneComonLEDOn)
{
    Manager manager(twoGroupsWithOneComonLEDOn);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::On},
            {"Two",phosphor::led::Layout::On},
            {"Three",phosphor::led::Layout::On},
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
    }
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Four",phosphor::led::Layout::On},
            {"Six",phosphor::led::Layout::On},
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
    }
}

/** @brief Assert 2 groups having one of the LEDs common but in different state */
TEST_F(LedTest,twoGroupsWithOneComonLEDInDifferentState)
{
    Manager manager(twoGroupsWithOneComonLEDInDifferentState);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::On},
            {"Two",phosphor::led::Layout::On},
            {"Three",phosphor::led::Layout::On},
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
    }
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Two",phosphor::led::Layout::Blink},
            {"Four",phosphor::led::Layout::On},
            {"Five",phosphor::led::Layout::On},
            {"Six",phosphor::led::Layout::On},
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
    }
}

/** @brief Assert 2 groups having one of the LEDs common but in different state */
TEST_F(LedTest, twoGroupsWithMultiplComonLEDOn)
{
    Manager manager(twoGroupsWithMultiplComonLEDOn);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Two",phosphor::led::Layout::On},
            {"Six",phosphor::led::Layout::On},
            {"Three",phosphor::led::Layout::On},
            {"Seven",phosphor::led::Layout::On},
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
    }
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::On},
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
    }
}

/** @brief Assert 2 groups having one of the LEDs common but in different state */
TEST_F(LedTest, assertTwoGroupsWithMultipleComonLEDInDifferen)
{
    Manager manager(twoGroupsWithMultipleComonLEDInDifferentState);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::On},
            {"Two",phosphor::led::Layout::Blink},
            {"Three",phosphor::led::Layout::On},
            {"Four",phosphor::led::Layout::On},
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
    }
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Two",phosphor::led::Layout::On},
            {"Three",phosphor::led::Layout::Blink},
            {"Five",phosphor::led::Layout::On},
            {"Six",phosphor::led::Layout::On},
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
    }
}

/** @brief Assert 2 groups having one of the LEDs common but in different state */
TEST_F(LedTest, assertTwoGroupsWithMultipleComonLEDAndDeAssertOne)
{
    Manager manager(twoGroupsWithMultiplComonLEDOn);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::On},
            {"Two",phosphor::led::Layout::On},
            {"Three",phosphor::led::Layout::On},
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
    }
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Six",phosphor::led::Layout::On},
            {"Seven",phosphor::led::Layout::On},
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
    }
    {
        // DeAssert A Set
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(false, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refDeAssert = {
            {"One",phosphor::led::Layout::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());
        EXPECT_EQ(0, ledsUpdate.size());

        // difference of refAssert and ledsAssert must be null.
        Manager::group temp {};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having one of the LEDs common but in different state */
TEST_F(LedTest, asserttwoGroupsWithMultipleComonLEDInDifferenStatesAndDeAssertOne)
{
    Manager manager(twoGroupsWithMultipleComonLEDInDifferentState);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Two",phosphor::led::Layout::On},
            {"Three",phosphor::led::Layout::Blink},
            {"Five",phosphor::led::Layout::On},
            {"Six",phosphor::led::Layout::On},
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
    }
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"One",phosphor::led::Layout::On},
            {"Two",phosphor::led::Layout::Blink},
            {"Three",phosphor::led::Layout::On},
            {"Four",phosphor::led::Layout::On},
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
    }
    {
        // DeAssert A Set
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group ="/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(false, result);

        // Need just the ledsUpdated populated with these.
        std::set<Layout::LedAction> refUpdate = {
            {"Two",phosphor::led::Layout::On},
            {"Three",phosphor::led::Layout::Blink},
        };
        EXPECT_EQ(refUpdate.size(), ledsUpdate.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refAssert and ledsAssert must be null.
        Manager::group temp {};
        std::set_difference(ledsUpdate.begin(), ledsUpdate.end(),
                            refUpdate.begin(), refUpdate.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());

        // Need just the ledsDeAsserted populated with these.
        std::set<Layout::LedAction> refDeAssert = {
            {"One",phosphor::led::Layout::On},
            {"Four",phosphor::led::Layout::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        temp.clear();
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}
