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

void LampTest::startLampTest()
{
    // initiate lamp test.
    timer.restart(std::chrono::seconds(LAMP_TEST_TIMEOUT_SECS));
}

void LampTest::timeOutHandler()
{
    // set the Asserted property of lamp test to false
}

void LampTest::requestHandler(Group* group, bool value)
{
    if (groupPtr == NULL)
    {
        groupPtr = std::move(group);
    }

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
