#include "config.h"

#include "lampTest.hpp"

#include <iomanip>

namespace phosphor
{
namespace led
{

bool LampTest::updatePhysicalAction(Layout::Action /* action */)
{

    return true;
}

void LampTest::stopLampTest()
{
    timer.setEnabled(false);
}

void LampTest::startLampTest()
{
    constexpr auto LAMP_TEST_TIMEOUT_SECS = seconds(4 * 60);

    // initiate lamp test.
    timer.restart(LAMP_TEST_TIMEOUT_SECS);

    // Set all the Physical action to On for lamp test
    if (!updatePhysicalAction(Layout::Action::On))
    {
        timer.setEnabled(false);
    }
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