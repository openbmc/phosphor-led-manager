#include "config.h"

#include "lamptest.hpp"

#include <iomanip>

namespace phosphor
{
namespace led
{

void LampTest::stopLampTest()
{
    timer.setEnabled(false);
}

void LampTest::reStartLampTest()
{
    // restart lamp test, it contains initiate or reset the timer.
    timer.restart(std::chrono::seconds(LAMP_TEST_TIMEOUT_IN_SECS));
}

void LampTest::timeOutHandler()
{
    // set the Asserted property of lamp test to false
}

void LampTest::requestHandler(bool value)
{
    if (value)
    {
        reStartLampTest();
    }
    else
    {
        stopLampTest();
    }
}

} // namespace led
} // namespace phosphor
