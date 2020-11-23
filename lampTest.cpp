#include "config.h"

#include "lampTest.hpp"

#include <iomanip>

namespace phosphor
{
namespace led
{

bool LampTest::active(bool value)
{
    // If the value is already what is before, return right away
    if (value == sdbusplus::com::ibm::Led::server::LampTest::active())
    {
        return value;
    }

    if (!value)
    {
        stopLampTest();
    }
    else
    {
        constexpr auto LAMP_TEST_TIMEOUT_SECS = seconds(4 * 60);

        // initiate lamp test.
        timer.restart(LAMP_TEST_TIMEOUT_SECS);

        status(OperationStatus::InProgress);
        startTime(getTime().count());
        completedTime(0);
        manager.isLampTestInProgress = true;

        // Set all the Physical action to On for lamp test
        if (!updatePhysicalAction(Layout::Action::On))
        {
            timer.setEnabled(false);
            status(OperationStatus::Failed);
            completedTime(getTime().count());
            manager.isLampTestInProgress = false;
        }
    }

    return sdbusplus::com::ibm::Led::server::LampTest::active(value);
}

microseconds LampTest::getTime() const
{
    auto now = system_clock::now();
    return duration_cast<microseconds>(now.time_since_epoch());
}

bool LampTest::updatePhysicalAction(Layout::Action action)
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

    // Set all the Physical action to Off for lamp test
    if (!updatePhysicalAction(Layout::Action::Off))
    {
        status(OperationStatus::Failed);
    }
    else
    {
        status(OperationStatus::Completed);
    }

    completedTime(getTime().count());
    manager.isLampTestInProgress = false;
}

void LampTest::lampTestTimeout()
{
    active(false);
}

} // namespace led
} // namespace phosphor