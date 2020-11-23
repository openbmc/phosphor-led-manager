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

    // Set all the Physical action to Off for lamp test
    std::vector<std::string> paths =
        dBusHandler.getSubTreePaths(PHY_LED_PATH, PHY_LED_IFACE);
    for (auto& path : paths)
    {
        manager.drivePhysicalLED(path, Layout::Action::Off, 0, 0);
    }
}

void LampTest::startLampTest()
{
    // initiate lamp test.
    timer.restart(LAMP_TEST_TIMEOUT_SECS);

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
    // set the Asserted property of lamp test to false
    try
    {
        dBusHandler.setProperty(LAMP_TEST_PATH, "xyz.openbmc_project.Led.Group",
                                "Asserted", false);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Failed to set Assert property",
                        entry("ERROR=%s", e.what()),
                        entry("PATH=%s", LAMP_TEST_PATH));
    }
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
