#include "led-test-map.hpp"
#include "manager.hpp"

#include <sdbusplus/bus.hpp>

#include <algorithm>
#include <set>

#include <gtest/gtest.h>
using namespace phosphor::led;
class LedTest : public ::testing::Test
{
  public:
    sdbusplus::bus_t bus;
    LedTest() : bus(sdbusplus::bus::new_default())
    {
        // Nothing here
    }
    ~LedTest() override = default;
};

/** @brief Assert Single LED to On */
TEST_F(LedTest, assertSingleLedOn)
{
    Manager manager(bus, singleLedOn);
    {
        // Assert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/SingleLed";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert Single LED to Blink */
TEST_F(LedTest, assertSingleLedBlink)
{
    Manager manager(bus, singleLedBlink);
    {
        // Assert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/SingleLed";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert Single LED to On and Try Assert Again */
TEST_F(LedTest, assertSingleLedOnAndreAssert)
{
    Manager manager(bus, singleLedOn);
    {
        // Assert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/SingleLed";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/SingleLed";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        EXPECT_EQ(0, ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());
    }
}

/** @brief Assert Multiple LEDs to On */
TEST_F(LedTest, assertMultipleLedOn)
{
    Manager manager(bus, multipleLedsOn);
    {
        // Assert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert Multiple LEDs to Blink */
TEST_F(LedTest, assertMultipleLedBlink)
{
    Manager manager(bus, multipleLedsBlink);
    {
        // Assert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert Multiple LEDs to Blink, DeAssert */
TEST_F(LedTest, assertMultipleLedBlinkAndDeAssert)
{
    Manager manager(bus, multipleLedsBlink);
    {
        // Assert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refDeAssert = {
            {"One", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert Multiple LEDs to Blink, DeAssert Twice */
TEST_F(LedTest, assertMultipleLedBlinkAndDeAssertTwice)
{
    Manager manager(bus, multipleLedsBlink);
    {
        // Assert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // DeAssert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refDeAssert = {
            {"One", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // DeAssert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLeds";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);
        EXPECT_EQ(0, ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());
    }
}

/** @brief Assert Multiple LEDs to mix of On and Blink */
TEST_F(LedTest, assertMultipleLedOnAndBlink)
{
    Manager manager(bus, multipleLedsOnAndBlink);
    {
        // Assert the LEDs.
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsMix";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having distinct LEDs */
TEST_F(LedTest, assertTwoGroupsOnWithDistinctLEDOn)
{
    Manager manager(bus, twoGroupsWithDistinctLEDsOn);
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having one of the LEDs common */
TEST_F(LedTest, asserttwoGroupsWithOneComonLEDOn)
{
    Manager manager(bus, twoGroupsWithOneComonLEDOn);
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having one of the LEDs common but having Blink as
 * priority and Deassert*/
TEST_F(LedTest, asserttwoGroupsWithOneComonLEDOnOneLEDBlinkPriorityAndDeAssertB)
{
    Manager manager(bus, twoGroupsWithOneComonLEDOnOneLEDBlinkPriority);
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // Does not action on [Three] since  priority is [Blink]
        ActionSet refAssert = {
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // De-Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsDeAssserted populated with these.
        ActionSet refDeAssert = {
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        // [Three] is not touched since its already [Blink]
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having one of the LEDs common but having Blink as
 * priority and Deassert A */
TEST_F(LedTest, asserttwoGroupsWithOneComonLEDOnOneLEDBlinkPriorityAndDeAssertA)
{
    Manager manager(bus, twoGroupsWithOneComonLEDOnOneLEDBlinkPriority);
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // [Three] does not get actioned since it has Blink priority
        ActionSet refAssert = {
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // De-Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsDeAssserted populated with these.
        ActionSet refDeAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());

        // Need just the ledsAssert populated with these.
        ActionSet refAssert = {
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp1{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp1, temp1.begin()));
        EXPECT_EQ(0, temp1.size());
    }
}

/** @brief Assert 2 groups having one of the LEDs common but having ON as
 * priority And Deassert A */
TEST_F(LedTest, asserttwoGroupsWithOneComonLEDOnOneLEDOnPriorityAndDeAssertA)
{
    Manager manager(bus, twoGroupsWithOneComonLEDOnPriority);
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // Three is set to ON due to ON priority.
        ActionSet refAssert = {
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
    }
    {
        // De-Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsDeAssserted populated with these.
        // [Three] stays in [On] since [B] has it [On]
        ActionSet refDeAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having one of the LEDs common but having ON as
 * priority And Deassert B */
TEST_F(LedTest, asserttwoGroupsWithOneComonLEDOnOneLEDOnPriorityAndDeAssertB)
{
    Manager manager(bus, twoGroupsWithOneComonLEDOnPriority);
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // Three is set to ON due to ON priority.
        ActionSet refAssert = {
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
    }
    {
        // De-Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsDeAssserted populated with these.
        ActionSet refDeAssert = {
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());

        // Need just the ledsAssert populated with these.
        // Since [Three] stood [On], need to go back to [Blink]
        ActionSet refAssert = {
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp1{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having multiple common LEDs in Same State */
TEST_F(LedTest, assertTwoGroupsWithMultiplComonLEDOnAndDeAssert)
{
    Manager manager(bus, twoGroupsWithMultiplComonLEDOn);
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Seven", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // De-Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsDeAssserted populated with these.
        ActionSet refDeAssert = {
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Seven", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having multiple LEDs common in different state */
TEST_F(LedTest, assertTwoGroupsWithMultipleComonLEDInDifferentStateBandA)
{
    Manager manager(bus, twoGroupsWithMultipleComonLEDInDifferentState);
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these
        // [Two] remains [On] due to higher priority.
        // [Three] remains [Blink]
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having multiple LEDs common in different state */
TEST_F(LedTest, assertTwoGroupsWithMultipleComonLEDInDifferentStateAtoB)
{
    Manager manager(bus, twoGroupsWithMultipleComonLEDInDifferentState);
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.'Two' gets to Blink
        // due to higher priority.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // [Three] remains [Blink] from previous
        // [Two] moves to [On] from [Blink] due to [On] priority
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having multiple LEDs common in different state
 *  DeAssert twice
 */
TEST_F(LedTest,
       assertTwoGroupsWithMultipleComonLEDInDifferentStateAtoBDeAssertTwice)
{
    Manager manager(bus, twoGroupsWithMultipleComonLEDInDifferentState);
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // [Two] turns [On] due to priority
        // [Three] remains [Blink]
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // DeAssert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refDeAssert = {
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());

        // Need just the ledsAssert populated with these.
        // [Two] will go back to [Blink] from [On]
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp1{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp1, temp1.begin()));
        EXPECT_EQ(0, temp1.size());
    }
    {
        // DeAssert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refDeAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // DeAssert Set-A again and make sure we get all empty
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);
        EXPECT_EQ(0, ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());
    }
}

/** @brief Assert 2 groups having multiple LEDs common in different state and
 *  mixed priority. DeAssert-A
 */
TEST_F(LedTest,
       assertTwoGroupsWithMultipleComonLEDInDifferentStateDiffPriorityAandB)
{
    Manager manager(bus,
                    twoGroupsWithMultipleComonLEDInDifferentStateDiffPriority);
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Ten", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // [Two] gets to [ON] due to higher priority.
        // [Three] remains on since it never was in [Blink] before
        // [Ten] remains [Blink] due to priority: [Blink]
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // De-Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsDeAsssert populated with these.
        ActionSet refDeAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());

        // Need just the ledsAsssert populated with these.
        // [Ten] Moves to [On] since there is no prior [Blink]
        // [Three] remains [On] since it never changed state.
        // [Two] remains [On] since it did not go back
        ActionSet refAssert = {
            {"Ten", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having multiple LEDs common in different state and
 *  mixed priority. DeAssert-B
 */
TEST_F(
    LedTest,
    assertTwoGroupsWithMultipleComonLEDInDifferentStateDiffPriorityAandBDeAssertB)
{
    Manager manager(bus,
                    twoGroupsWithMultipleComonLEDInDifferentStateDiffPriority);
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Ten", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // [Two] gets to [ON] due to higher priority.
        // [Three] remains on since it never was in [Blink] before
        // [Ten] remains [Blink] due to priority: [Blink]
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // De-Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsDeAsssert populated with these.
        ActionSet refDeAssert = {
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());

        // Need just the ledsAsssert populated with these.
        // [Ten] remains [Blink] since it did not move to [On]
        // [Three] remains [On] since it never changed state.
        // [Two] moves to [Blink] since there is no prior [On]
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having multiple LEDs common in different state and
 *  mixed priority.
 */
TEST_F(LedTest,
       assertTwoGroupsWithMultipleComonLEDInDifferentStateDiffPriorityBandA)
{
    Manager manager(bus,
                    twoGroupsWithMultipleComonLEDInDifferentStateDiffPriority);
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Ten", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // [Two] remains [ON] due to higher priority.
        // [Three] remains on since it never was in [Blink] before
        // [Ten] moves to [Blink] due to priority: [Blink]
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Ten", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having multiple LEDs common in different state and
 *  mixed priority and De-Assert-A
 */
TEST_F(
    LedTest,
    assertTwoGroupsWithMultipleComonLEDInDifferentStateDiffPriorityBandADeAssertA)
{
    Manager manager(bus,
                    twoGroupsWithMultipleComonLEDInDifferentStateDiffPriority);
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Ten", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // [Two] remains [ON] due to higher priority.
        // [Three] remains on since it never was in [Blink] before
        // [Ten] moves to [Blink] due to priority: [Blink]
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Ten", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // De-Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsAssserted populated with these.
        // [Ten] remains [Blink] due to priority.
        ActionSet refDeAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());

        // Need just the ledsAssserted populated with these.
        // [Two] remains [ON] due to higher priority.
        // [Three] remains [On] since it never was in [Blink] before
        // [Ten] moves to [On] due to priority: [Blink]
        ActionSet refAssert = {
            {"Ten", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
}

/** @brief Assert 2 groups having multiple LEDs common in different state and
 *  mixed priority and then DeAssert twice.
 */
TEST_F(LedTest,
       assertTwoGroupsWithMultipleComonLEDInDifferentStateOnBlinkPriorityBandA)
{
    Manager manager(bus,
                    twoGroupsWithMultipleComonLEDInDifferentStateDiffPriority);
    {
        // Assert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Ten", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // Assert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, true, ledsAssert, ledsDeAssert);
        EXPECT_EQ(true, result);

        // Need just the ledsAssserted populated with these.
        // [Two] remains [ON] due to higher priority.
        // [Three] remains on since it never was in [Blink] before
        // [Ten] moves to [Blink] due to priority: [Blink]
        ActionSet refAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Ten", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());
        EXPECT_EQ(0, ledsDeAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // DeAssert Set-B
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsAssserted populated with these.
        // [Ten] remains [Blink] due to priority.
        ActionSet refDeAssert = {
            {"Five", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Six", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());

        // Need just the ledsAssert populated with these.
        // [Two] will move to [Blink]
        ActionSet refAssert = {
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
        };
        EXPECT_EQ(refAssert.size(), ledsAssert.size());

        // difference of refAssert and ledsAssert must be null.
        ActionSet temp1{};
        std::set_difference(ledsAssert.begin(), ledsAssert.end(),
                            refAssert.begin(), refAssert.end(),
                            std::inserter(temp1, temp1.begin()));
        EXPECT_EQ(0, temp1.size());
    }
    {
        // DeAssert Set-A
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);

        // Need just the ledsAssserted populated with these.
        ActionSet refDeAssert = {
            {"One", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Three", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::Blink},
            {"Four", phosphor::led::Layout::Action::On, 0, 0,
             phosphor::led::Layout::Action::On},
            {"Ten", phosphor::led::Layout::Action::Blink, 0, 0,
             phosphor::led::Layout::Action::Blink},
        };
        EXPECT_EQ(refDeAssert.size(), ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());

        // difference of refDeAssert and ledsDeAssert must be null.
        ActionSet temp{};
        std::set_difference(ledsDeAssert.begin(), ledsDeAssert.end(),
                            refDeAssert.begin(), refDeAssert.end(),
                            std::inserter(temp, temp.begin()));
        EXPECT_EQ(0, temp.size());
    }
    {
        // DeAssert Set-B again and make sure we get all empty
        ActionSet ledsAssert{};
        ActionSet ledsDeAssert{};

        static constexpr auto group =
            "/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet";
        auto result =
            manager.setGroupState(group, false, ledsAssert, ledsDeAssert);
        EXPECT_EQ(false, result);
        EXPECT_EQ(0, ledsDeAssert.size());
        EXPECT_EQ(0, ledsAssert.size());
    }
}
