#include "config.h"

#include "lamptest.hpp"

#include <phosphor-logging/log.hpp>

namespace phosphor
{
namespace led
{

using namespace phosphor::logging;

void LampTest::updatePhysicalLedStates(group& ledsAssert, group& ledsDeAssert)
{
    if (isLampTestRunning)
    {
        savedPhysicalLedStates.emplace(
            std::make_tuple(ledsAssert, ledsDeAssert));
        return;
    }
}

void LampTest::stop()
{
    if (!isLampTestRunning)
    {
        return;
    }

    timer.setEnabled(false);

    // Set all the Physical action to Off
    for (const auto& path : physicalLEDPaths)
    {
        manager.drivePhysicalLED(path, Layout::Action::Off, 0, 0);
    }

    isLampTestRunning = false;
    restorePhysicalLedStates();
}

Layout::Action LampTest::getActionFromString(const std::string& str)
{
    Layout::Action action = Layout::Off;

    if (str == "xyz.openbmc_project.Led.Physical.Action.On")
    {
        action = Layout::On;
    }
    else if (str == "xyz.openbmc_project.Led.Physical.Action.Blink")
    {
        action = Layout::Blink;
    }

    return action;
}

void LampTest::storePhysicalLEDsStates()
{
    lastLedsAssert.clear();
    lastLedsDeAssert.clear();

    for (const auto& path : physicalLEDPaths)
    {
        sdbusplus::message::object_path object_path(path);
        std::string name = object_path.filename();
        if (name.empty())
        {
            log<level::ERR>("Failed to get name",
                            entry("PATH=%s", path.c_str()));
            continue;
        }

        std::string ledState{};
        uint16_t period{};
        uint8_t dutyOn{};
        try
        {
            auto properties = dBusHandler.getAllProperty(path, PHY_LED_IFACE);

            ledState = std::get<std::string>(properties["State"]);
            period = std::get<uint16_t>(properties["Period"]);
            dutyOn = std::get<uint8_t>(properties["DutyOn"]);
        }
        catch (const sdbusplus::exception::SdBusError& e)
        {
            log<level::ERR>("Failed to get All properties",
                            entry("ERROR=%s", e.what()),
                            entry("PATH=%s", path.c_str()));
            continue;
        }

        phosphor::led::Layout::Action action = getActionFromString(ledState);
        phosphor::led::Layout::LedAction ledAction{name, action, dutyOn, period,
                                                   phosphor::led::Layout::On};

        if (action == phosphor::led::Layout::Off)
        {
            lastLedsDeAssert.emplace(ledAction);
            continue;
        }
        lastLedsAssert.emplace(ledAction);
    }
}

void LampTest::start()
{
    if (isLampTestRunning)
    {
        return;
    }

    // Get paths of all the Physical LED objects
    physicalLEDPaths = dBusHandler.getSubTreePaths(PHY_LED_PATH, PHY_LED_IFACE);

    // Get physical LEDs states before lamp test
    storePhysicalLEDsStates();

    // restart lamp test, it contains initiate or reset the timer.
    timer.restart(std::chrono::seconds(LAMP_TEST_TIMEOUT_IN_SECS));
    isLampTestRunning = true;

    // Set all the Physical action to On for lamp test
    for (const auto& path : physicalLEDPaths)
    {
        manager.drivePhysicalLED(path, Layout::Action::On, 0, 0);
    }
}

void LampTest::timeOutHandler()
{
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

void LampTest::restorePhysicalLedStates()
{
    // restore physical LEDs states before lamp test
    manager.driveLEDs(lastLedsAssert, lastLedsDeAssert);
    lastLedsAssert.clear();
    lastLedsDeAssert.clear();

    // restore physical LEDs states during lamp test
    while (!savedPhysicalLedStates.empty())
    {
        auto& [ledsAssert, ledsDeAssert] = savedPhysicalLedStates.front();
        manager.driveLEDs(ledsAssert, ledsDeAssert);
        savedPhysicalLedStates.pop();
    }
}

} // namespace led
} // namespace phosphor
