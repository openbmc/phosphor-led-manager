#include "config.h"

#include "lamptest.hpp"

#include <phosphor-logging/log.hpp>

namespace phosphor
{
namespace led
{

void LampTest::stop()
{
    if (!manager.isLampTestRunning)
    {
        return;
    }

    timer.setEnabled(false);

    // Set all the Physical action to Off
    for (const auto& path : physicalLEDPaths)
    {
        manager.drivePhysicalLED(path, Layout::Action::Off, 0, 0);
    }

    manager.isLampTestRunning = false;
    manager.restorePhysicalLedStates();
}

void LampTest::start()
{
    // restart lamp test, it contains initiate or reset the timer.
    timer.restart(std::chrono::seconds(LAMP_TEST_TIMEOUT_IN_SECS));
    manager.isLampTestRunning = true;

    // Set all the Physical action to On for lamp test
    for (auto& path : physicalLEDPaths)
    {
        manager.drivePhysicalLED(path, Layout::Action::On, 0, 0);
    }
}

void LampTest::timeOutHandler()
{
    using namespace phosphor::logging;

    // set the Asserted property of lamp test to false
    if (!groupObj)
    {
        log<level::ERR>("the Group object is nullptr");
        throw std::runtime_error("the Group object is nullptr");
    }

    groupObj->asserted(false);
}

void LampTest::requestHandler(Group* group, bool value)
{
    if (groupObj == NULL)
    {
        groupObj = std::move(group);
    }

    if (value)
    {
        start();
    }
    else
    {
        stop();
    }
}

} // namespace led
} // namespace phosphor
