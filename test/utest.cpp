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

/** @brief Assert Single LED to On and Try Assert Again */
TEST_F(LedTest, assertSingleLedOnAndreAssert)
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
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/SingleLed";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        EXPECT_EQ(0, ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());
        EXPECT_EQ(0, ledsUpdate.size());
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

/** @brief Assert Multiple LEDs to Blink, DeAssert */
TEST_F(LedTest, assertMultipleLedBlinkAndDeAssert)
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
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(false, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refDeAssert = {
            {"One",phosphor::led::Layout::Blink},
            {"Two",phosphor::led::Layout::Blink},
            {"Three",phosphor::led::Layout::Blink},
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
    }
}

/** @brief Assert Multiple LEDs to Blink, DeAssert Twice */
TEST_F(LedTest, assertMultipleLedBlinkAndDeAssertTwice)
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
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(false, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refDeAssert = {
            {"One",phosphor::led::Layout::Blink},
            {"Two",phosphor::led::Layout::Blink},
            {"Three",phosphor::led::Layout::Blink},
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
    }
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(false, result);
        EXPECT_EQ(0, ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());
        EXPECT_EQ(0, ledsUpdate.size());
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

/** @brief Assert 2 groups having distinct LEDs */
TEST_F(LedTest, assertTwoGroupsOnWithDistinctLEDOn)
{
    Manager manager(twoGroupsWithDistinctLEDsOn);
    {
        // Assert Set-A
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
        // Assert Set-B
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
        // Assert Set-A
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
        // Assert Set-B
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

/** @brief Assert 2 groups having one of the LEDs common in different state */
TEST_F(LedTest, assertTwoGroupsWithOneComonLEDInDifferentState)
{
    Manager manager(twoGroupsWithOneComonLEDInDifferentState);
    {
        // Assert Set-A
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
        // Assert Set-B
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

/** @brief Assert 2 groups having multiple common LEDs in Same State */
TEST_F(LedTest, assertTwoGroupsWithMultiplComonLEDOn)
{
    Manager manager(twoGroupsWithMultiplComonLEDOn);
    {
        // Assert Set-B
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
        // Assert Set-A
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

/** @brief Assert 2 groups having multiple LEDs common in different state */
TEST_F(LedTest, assertTwoGroupsWithMultipleComonLEDInDifferentStates)
{
    Manager manager(twoGroupsWithMultipleComonLEDInDifferentState);
    {
        // Assert Set-A
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
        // Assert Set-B
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

/** @brief Assert 2 groups having multiple LEDs common in Same State
 *         and then De-Assert One
 */
TEST_F(LedTest, assertTwoGroupsWithMultipleComonLEDAndDeAssertOne)
{
    Manager manager(twoGroupsWithMultiplComonLEDOn);
    {
        // Assert Set-A
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
        // Assert Set-B
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
        // DeAssert Set-A
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

/** @brief Assert 2 groups having multiple LEDs common but in
 *         different state and De-Assert one*/
TEST_F(LedTest, assertTwoGroupsWithMultipleComonLEDInDifferentStateDeAssertOne)
{
    Manager manager(twoGroupsWithMultipleComonLEDInDifferentState);
    {
        // Assert Set-B
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
        // Assert Set-A
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
        // DeAssert Set-A
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
