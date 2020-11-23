#include "config.h"

#include "lampTest.hpp"

#include <iomanip>

namespace phosphor
{
namespace led
{

void LampTest::stopLampTest()
{
    timer.setEnabled(false);
}

void LampTest::startLampTest()
{
    constexpr auto LAMP_TEST_TIMEOUT_SECS = seconds(4 * 60);

    // initiate lamp test.
    timer.restart(LAMP_TEST_TIMEOUT_SECS);
}

void LampTest::lampTestTimeOut()
{
    // set the Asserted property of lamp test to false
}

void LampTest::lampTestHandler(bool value)
{
    if (value)
    {
        startLampTest();
    }
    else
    {
        stopLampTest();
    }
}

} // namespace led
} // namespace phosphor