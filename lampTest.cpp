#include "config.h"

#include "lampTest.hpp"

#include <iomanip>

namespace phosphor
{
namespace led
{

bool LampTest::drivephysicalLEDs(Layout::Action action)
{
    std::vector<std::string> paths =
        dBusHandler.getSubTreePaths(PHY_LED_PATH, PHY_LED_IFACE);

    for (auto& path : paths)
    {
        if (!manager.drivePhysicalLED(path, action, 0, 0))
        {
            return false;
        }
    }

    return true;
}

void LampTest::stopLampTest()
{
    timer.setEnabled(false);
    manager.restorePhysicalLedStates();
    manager.isLampTestRunning = false;
}

void LampTest::startLampTest()
{
    constexpr auto LAMP_TEST_TIMEOUT_SECS = seconds(4 * 60);

    // initiate lamp test.
    timer.restart(LAMP_TEST_TIMEOUT_SECS);
    manager.isLampTestRunning = true;

    // Set all the Physical LEDs to On for lamp test
    if (!drivephysicalLEDs(Layout::Action::On))
    {
        stopLampTest();
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
