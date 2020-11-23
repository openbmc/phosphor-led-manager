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
    if (!isRunning)
    {
        return;
    }

    timer.setEnabled(false);

    // Set all the Physical action to Off for lamp test
    std::vector<std::string> paths =
        dBusHandler.getSubTreePaths(PHY_LED_PATH, PHY_LED_IFACE);
    for (auto& path : paths)
    {
        manager.drivePhysicalLED(path, Layout::Action::Off, 0, 0);
    }
}

void LampTest::reStartLampTest()
{
    // restart lamp test, it contains initiate or reset the timer.
    timer.restart(std::chrono::seconds(LAMP_TEST_TIMEOUT_IN_SECS));
    isRunning = true;

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
        reStartLampTest();
    }
    else
    {
        stopLampTest();
    }
}

} // namespace led
} // namespace phosphor
