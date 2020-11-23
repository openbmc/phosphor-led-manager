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
    // initiate lamp test.
    timer.restart(LAMP_TEST_TIMEOUT_SECS);
}

void LampTest::timeOutHandler()
{
    // set the Asserted property of lamp test to false
}

void LampTest::requestHandler(bool value)
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