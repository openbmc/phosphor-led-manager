#include "led-test-map.hpp"
#include "manager.hpp"
#include "mocked_utils.hpp"

#include <sdbusplus/test/sdbus_mock.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace phosphor::led::utils;
using namespace phosphor::led;

using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;

TEST(Utils, DrivePhysicalLED)
{
    MockdBusHandler mockedUtils;
    Manager manager(DBusHandler::getBus(), singleLedBlink, &mockedUtils);

    constexpr auto objectPath = "/xyz/openbmc_project/Led/Utest";

    constexpr auto DutyOn = "DutyOn";
    constexpr auto Period = "Period";
    constexpr auto State = "State";

    uint8_t dutyOn = 50;
    uint16_t period = 1000;
    std::string action = "xyz.openbmc_project.Led.Physical.Action.Blink";

    EXPECT_CALL(mockedUtils,
                getProperty(StrEq(objectPath), StrEq(DutyOn), PHY_LED_IFACE))
        .WillOnce(Return(PropertyValue(uint8_t(dutyOn))));

    EXPECT_CALL(mockedUtils,
                getProperty(StrEq(objectPath), StrEq(Period), PHY_LED_IFACE))
        .WillOnce(Return(PropertyValue(uint16_t(period))));

    EXPECT_CALL(mockedUtils,
                getProperty(StrEq(objectPath), StrEq(State), PHY_LED_IFACE))
        .WillOnce(Return(PropertyValue(std::string(action))));

    auto retDutyOn = manager.getPhysicalLED(objectPath, DutyOn);
    auto retPeriod = manager.getPhysicalLED(objectPath, Period);
    auto retState = manager.getPhysicalLED(objectPath, State);

    EXPECT_EQ(dutyOn, std::get<uint8_t>(retDutyOn));
    EXPECT_EQ(period, std::get<uint16_t>(retPeriod));
    EXPECT_EQ(action, std::get<std::string>(retState));
}