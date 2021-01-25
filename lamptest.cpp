#include "config.h"

#include "lamptest.hpp"

#include <phosphor-logging/log.hpp>

namespace phosphor
{
namespace led
{

using namespace phosphor::logging;

bool LampTest::processLEDUpdates(const Manager::group& ledsAssert,
                                 const Manager::group& ledsDeAssert)
{
    // If the physical LED status is updated during the lamp test, it should be
    // saved to Queue, and the queue will be processed after the lamp test is
    // stopped.
    if (isLampTestRunning)
    {
        updatedLEDsDuringLampTest.emplace(
            std::make_pair(ledsAssert, ledsDeAssert));
        return true;
    }
    return false;
}

void LampTest::stop()
{
    if (!isLampTestRunning)
    {
        return;
    }

    timer.setEnabled(false);

    // Stop host lamp test
    doHostLampTest(false);

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
    physicalLEDStatesPriorToLampTest.clear();

    for (const auto& path : physicalLEDPaths)
    {
        // Reverse intercept path, Get the name of each member of physical led
        // e.g: path = /xyz/openbmc_project/led/physical/front_fan
        //      name = front_fan
        sdbusplus::message::object_path object_path(path);
        auto name = object_path.filename();
        if (name.empty())
        {
            log<level::ERR>(
                "Failed to get the name of member of physical LED path",
                entry("PATH=%s", path.c_str()), entry("NAME=%s", name.c_str()));
            continue;
        }

        std::string state{};
        uint16_t period{};
        uint8_t dutyOn{};
        try
        {
            auto properties = dBusHandler.getAllProperties(path, PHY_LED_IFACE);

            state = std::get<std::string>(properties["State"]);
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

        phosphor::led::Layout::Action action = getActionFromString(state);
        if (action != phosphor::led::Layout::Off)
        {
            phosphor::led::Layout::LedAction ledAction{
                name, action, dutyOn, period, phosphor::led::Layout::On};
            physicalLEDStatesPriorToLampTest.emplace(ledAction);
        }
    }
}

void LampTest::start()
{
    if (isLampTestRunning)
    {
        // reset the timer and then return
        timer.restart(std::chrono::seconds(LAMP_TEST_TIMEOUT_IN_SECS));
        return;
    }

    // Get paths of all the Physical LED objects
    physicalLEDPaths = dBusHandler.getSubTreePaths(PHY_LED_PATH, PHY_LED_IFACE);

    // Get physical LEDs states before lamp test
    storePhysicalLEDsStates();

    // restart lamp test, it contains initiate or reset the timer.
    timer.restart(std::chrono::seconds(LAMP_TEST_TIMEOUT_IN_SECS));
    isLampTestRunning = true;

    // Notify PHYP to start the lamp test
    doHostLampTest(true);

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
    Manager::group savedLEDStatesDeAssert{};
    manager.driveLEDs(physicalLEDStatesPriorToLampTest, savedLEDStatesDeAssert);
    physicalLEDStatesPriorToLampTest.clear();

    // restore physical LEDs states during lamp test
    while (!updatedLEDsDuringLampTest.empty())
    {
        auto& [ledsAssert, ledsDeAssert] = updatedLEDsDuringLampTest.front();
        manager.driveLEDs(ledsAssert, ledsDeAssert);
        updatedLEDsDuringLampTest.pop();
    }
}

void LampTest::doHostLampTest(bool value)
{
    try
    {
        PropertyValue assertedValue{value};
        dBusHandler.setProperty(HOST_LAMP_TEST_OBJECT,
                                "xyz.openbmc_project.Led.Group", "Asserted",
                                assertedValue);
    }
    catch (const sdbusplus::exception::SdBusError& e)
    {
        log<level::ERR>("Failed to set Asserted property",
                        entry("ERROR=%s", e.what()),
                        entry("PATH=%s", HOST_LAMP_TEST_OBJECT));
    }
}

} // namespace led
} // namespace phosphor
