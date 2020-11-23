#include "config.h"

#include "lamptest.hpp"

#include <phosphor-logging/log.hpp>

#include <iomanip>

namespace phosphor
{
namespace led
{

void LampTest::stopLampTest()
{
    timer.setEnabled(false);

    // Set all the Physical action to Off for lamp test
    std::vector<std::string> paths =
        dBusHandler.getSubTreePaths(PHY_LED_PATH, PHY_LED_IFACE);
    for (auto& path : paths)
    {
        manager.drivePhysicalLED(path, Layout::Action::Off, 0, 0);
    }

    manager.isLampTestRunning = false;
    manager.restorePhysicalLedStates();
}

void LampTest::startLampTest()
{
    // initiate lamp test.
    timer.restart(std::chrono::seconds(LAMP_TEST_TIMEOUT_SECS));
    manager.isLampTestRunning = true;

    // Set all the Physical action to On for lamp test
    std::vector<std::string> paths =
        dBusHandler.getSubTreePaths(PHY_LED_PATH, PHY_LED_IFACE);
    for (auto& path : paths)
    {
        manager.drivePhysicalLED(path, Layout::Action::On, 0, 0);
    }
}

void LampTest::timeOutHandler()
{
    using namespace phosphor::logging;

    // set the Asserted property of lamp test to false
    if (!groupPtr)
    {
        log<level::ERR>("the Group object is nullptr");
        throw std::runtime_error("the Group object is nullptr");
    }

    groupPtr->asserted(false);
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
