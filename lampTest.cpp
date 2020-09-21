#include "config.h"

#include "lampTest.hpp"

#include <phosphor-logging/elog.hpp>

namespace phosphor
{
namespace led
{

using namespace phosphor::logging;

void LampTest::updatePhysicalAction(Layout::Action action)
{
    ObjectPaths paths =
        manager.getSubTreePahts("/xyz/openbmc_project/led/physical",
                                "xyz.openbmc_project.Led.Physical");
    for (auto& path : paths)
    {
        manager.drivePhysicalLED(path, action, 0, 0);
    }
}

void LampTest::lampTestHandler(sdbusplus::message::message& /*msg*/)
{
    constexpr auto LAMP_TEST_TIMEOUT_SECS = std::chrono::seconds(4 * 60);

    timer.restart(LAMP_TEST_TIMEOUT_SECS);

    manager.setLampTestStatus(true);

    // Set all the Physical action to On for lamp test
    updatePhysicalAction(Layout::Action::On);
}

void LampTest::lampTestTimeout()
{
    timer.setEnabled(false);

    // Set all the Physical action to Off for lamp test
    updatePhysicalAction(Layout::Action::Off);

    manager.restoreLedsAssert();

    // lamptest is not running and call the restore function
    manager.setLampTestStatus(false);
}

} // namespace led
} // namespace phosphor