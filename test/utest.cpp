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

/** @brief Assert Office */
TEST_F(LedTest, TestOffice)
{
    Manager manager(testLedMap);
    {
        // Assert the LEDs.
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/Office";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Desk",phosphor::led::Layout::Blink},
            {"Room",phosphor::led::Layout::On},
            {"Chairs",phosphor::led::Layout::On},
            {"MeetingRoom",phosphor::led::Layout::On},
            {"Floor",phosphor::led::Layout::Blink},
            {"Phone",phosphor::led::Layout::On},
            {"Computer",phosphor::led::Layout::On},
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
        // De-Assert
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/Office";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(false, result);

        // Need just the ledsDeAssserted populated with these.
        std::set<Layout::LedAction> refDeAssert = {
            {"Desk",phosphor::led::Layout::Blink},
            {"Room",phosphor::led::Layout::On},
            {"Chairs",phosphor::led::Layout::On},
            {"MeetingRoom",phosphor::led::Layout::On},
            {"Floor",phosphor::led::Layout::Blink},
            {"Phone",phosphor::led::Layout::On},
            {"Computer",phosphor::led::Layout::On},
        };
        EXPECT_EQ(0, ledsAssert.size());
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsUpdate.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        Manager::group temp {};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // De-Assert again and see we get  empty
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/Office";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(false, result);

        EXPECT_EQ(0, ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());
        EXPECT_EQ(0, ledsUpdate.size());
    }
}

/** @brief Assert Office and School */
TEST_F(LedTest, TestSchool)
{
    Manager manager(testLedMap);
    {
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/Office";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Desk",phosphor::led::Layout::Blink},
            {"Room",phosphor::led::Layout::On},
            {"Chairs",phosphor::led::Layout::On},
            {"MeetingRoom",phosphor::led::Layout::On},
            {"Floor",phosphor::led::Layout::Blink},
            {"Phone",phosphor::led::Layout::On},
            {"Computer",phosphor::led::Layout::On},
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
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        // Now assert School and expect that Chair does not get asserted again
        auto group = "/xyz/openbmc_project/ledmanager/groups/School";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Desk",phosphor::led::Layout::On},
            {"Room",phosphor::led::Layout::Blink},
            {"MeetingRoom",phosphor::led::Layout::Blink},
            {"Floor",phosphor::led::Layout::On},
            {"Phone",phosphor::led::Layout::Blink},
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
        // Assert again and see we get  empty
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/School";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        EXPECT_EQ(0, ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());
        EXPECT_EQ(0, ledsUpdate.size());
    }
}

/** @brief Set Computer, phone and unset phone */
TEST_F(LedTest, SetComputerPhone)
{
    Manager manager(testLedMap);
    {
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/Computer";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Display",phosphor::led::Layout::On},
            {"CPU",phosphor::led::Layout::Blink},
            {"Memory",phosphor::led::Layout::On},
            {"keyboard",phosphor::led::Layout::Blink},
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
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        // Now assert Phone
        auto group = "/xyz/openbmc_project/ledmanager/groups/Phones";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Expect SET results
        std::set<Layout::LedAction> refAssert = {
            {"Display",phosphor::led::Layout::Blink},
            {"CPU",phosphor::led::Layout::On},
            {"Touch",phosphor::led::Layout::On},
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
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        // Now Unset Phone and expect any updates to common LEDs
        auto group = "/xyz/openbmc_project/ledmanager/groups/Phones";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(false, result);

        // Expect SET results
        std::set<Layout::LedAction> refDeAssert = {
            {"Touch",phosphor::led::Layout::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        Manager::group temp {};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        temp.clear();
        std::set<Layout::LedAction> refUpdate = {
            {"Display",phosphor::led::Layout::On},
            {"CPU",phosphor::led::Layout::Blink},
        };
        EXPECT_EQ(refUpdate.size(), ledsUpdate.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        std::set_difference(ledsUpdate.begin(), ledsUpdate.end(),
                            refUpdate.begin(), refUpdate.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Set Computer, phone and unset computer and phone */
TEST_F(LedTest, SetUnsetComputerPhone)
{
    Manager manager(testLedMap);
    {
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        auto group = "/xyz/openbmc_project/ledmanager/groups/Computer";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        std::set<Layout::LedAction> refAssert = {
            {"Display",phosphor::led::Layout::On},
            {"CPU",phosphor::led::Layout::Blink},
            {"Memory",phosphor::led::Layout::On},
            {"keyboard",phosphor::led::Layout::Blink},
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
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        // Now assert Phones
        auto group = "/xyz/openbmc_project/ledmanager/groups/Phones";
        auto result = manager.setGroupState(group, true, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(true, result);

        // Expect SET results
        std::set<Layout::LedAction> refAssert = {
            {"Display",phosphor::led::Layout::Blink},
            {"CPU",phosphor::led::Layout::On},
            {"Touch",phosphor::led::Layout::On},
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
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        // Now Unset Phone and expect any updates to common LEDs
        auto group = "/xyz/openbmc_project/ledmanager/groups/Phones";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(false, result);

        // Expect SET results
        std::set<Layout::LedAction> refDeAssert = {
            {"Touch",phosphor::led::Layout::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        Manager::group temp {};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        temp.clear();
        std::set<Layout::LedAction> refUpdate = {
            {"Display",phosphor::led::Layout::On},
            {"CPU",phosphor::led::Layout::Blink},
        };
        EXPECT_EQ(refUpdate.size(), ledsUpdate.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        std::set_difference(ledsUpdate.begin(), ledsUpdate.end(),
                            refUpdate.begin(), refUpdate.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        Manager::group ledsAssert {};
        Manager::group ledsDeAssert {};
        Manager::group ledsUpdate {};

        // Now Unset Phone and expect any updates to common LEDs
        auto group = "/xyz/openbmc_project/ledmanager/groups/Computer";
        auto result = manager.setGroupState(group, false, ledsAssert,
                                            ledsDeAssert, ledsUpdate);
        EXPECT_EQ(false, result);

        // Expect SET results
        std::set<Layout::LedAction> refDeAssert = {
            {"Display",phosphor::led::Layout::On},
            {"CPU",phosphor::led::Layout::Blink},
            {"Memory",phosphor::led::Layout::On},
            {"keyboard",phosphor::led::Layout::Blink},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsUpdate.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        Manager::group temp {};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}
