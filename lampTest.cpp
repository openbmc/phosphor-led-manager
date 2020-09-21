#include "config.h"

#include "lampTest.hpp"

#include <phosphor-logging/elog.hpp>

namespace phosphor
{
namespace led
{

using namespace phosphor::logging;

void LampTest::updatePhysicalAction()
{
    ObjectPaths paths =
        manager.getSubTreePahts("/xyz/openbmc_project/led/physical",
                                "xyz.openbmc_project.Led.Physical");
    for (auto& path : paths)
    {
        manager.drivePhysicalLED(path, Layout::Action::On, 0, 0);
    }
}

void LampTest::lampTestHandler(sdbusplus::message::message& /*msg*/)
{
    constexpr auto LAMP_TEST_TIMEOUT_SECS = std::chrono::seconds(4 * 60);

    timer.restart(LAMP_TEST_TIMEOUT_SECS);

    manager.setLampTestStatus(true);

    // Set all the Physical action to On for lamp test
    updatePhysicalAction();
}

void LampTest::lampTestTimeout()
{
    timer.setEnabled(false);

    ObjectPaths physicalPaths =
        manager.getSubTreePahts("/xyz/openbmc_project/led/physical",
                                "xyz.openbmc_project.Led.Physical");
    for (auto& path : physicalPaths)
    {
        manager.drivePhysicalLED(path, Layout::Action::Off, 0, 0);
    }

    ObjectPaths groupPaths = manager.getSubTreePahts(
        "/xyz/openbmc_project/led/groups", "xyz.openbmc_project.Led.Group");
    for (auto& path : groupPaths)
    {
        auto assertGroups = manager.getLedsAssertGroups(path);
        if (assertGroups != std::nullopt)
        {
            auto [ledsAssert, ledsDeAssert] = *assertGroups;
            manager.driveLEDs(ledsAssert, ledsDeAssert);
        }
    }

    // lamptest is not running and call the restore function
    manager.setLampTestStatus(false);
}

} // namespace led
} // namespace phosphor