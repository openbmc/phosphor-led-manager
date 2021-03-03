#include "config.h"

#include "lamptest.hpp"

#include <phosphor-logging/log.hpp>

namespace phosphor
{
namespace led
{

using namespace phosphor::logging;
using Json = nlohmann::json;

bool LampTest::processLEDUpdates(const Manager::group& ledsAssert,
                                 const Manager::group& ledsDeAssert)
{
    // If the physical LED status is updated during the lamp test, it should be
    // saved to Queue, and Queue will be updated after the lamp test is stopped.
    if (isLampTestRunning)
    {
        // Physical LEDs will be updated during lamp test
        for (const auto& it : ledsDeAssert)
        {
            std::string path = std::string(PHY_LED_PATH) + it.name;
            auto iter = std::find_if(
                forceUpdateLEDs.begin(), forceUpdateLEDs.end(),
                [&path](const auto& name) { return name == path; });

            if (iter != forceUpdateLEDs.end())
            {
                manager.drivePhysicalLED(path, Layout::Action::Off, it.dutyOn,
                                         it.period);
            }
        }

        for (const auto& it : ledsAssert)
        {
            std::string path = std::string(PHY_LED_PATH) + it.name;
            auto iter = std::find_if(
                forceUpdateLEDs.begin(), forceUpdateLEDs.end(),
                [&path](const auto& name) { return name == path; });

            if (iter != forceUpdateLEDs.end())
            {
                manager.drivePhysicalLED(path, it.action, it.dutyOn, it.period);
            }
        }

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
        auto iter = std::find_if(
            skipUpdateLEDs.begin(), skipUpdateLEDs.end(),
            [&path](const auto& skipLed) { return skipLed == path; });

        if (iter != skipUpdateLEDs.end())
        {
            // Physical LEDs will be skipped
            continue;
        }

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
        auto iter = std::find_if(
            skipUpdateLEDs.begin(), skipUpdateLEDs.end(),
            [&path](const auto& skipLed) { return skipLed == path; });

        if (iter != skipUpdateLEDs.end())
        {
            // Physical LEDs will be skipped
            continue;
        }

        // Reverse intercept path, Get the name of each member of the LED group
        // e.g: path = /xyz/openbmc_project/led/physical/front_fan
        //      name = front_fan
        sdbusplus::message::object_path object_path(path);
        auto name = object_path.filename();
        if (name.empty())
        {
            log<level::ERR>(
                "Failed to get the name of each member of the LED group",
                entry("PATH=%s", path.c_str()), entry("NAME=%s", name.c_str()));
            continue;
        }

        std::string state{};
        uint16_t period{};
        uint8_t dutyOn{};
        try
        {
            auto properties = dBusHandler.getAllProperty(path, PHY_LED_IFACE);

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
        auto iter =
            std::find_if(skipUpdateLEDs.begin(), skipUpdateLEDs.end(),
                         [&path](const auto& skip) { return skip == path; });

        if (iter != skipUpdateLEDs.end())
        {
            // Skip update physical path
            continue;
        }

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
    Manager::group ledsDeAssert{};
    manager.driveLEDs(physicalLEDStatesPriorToLampTest, ledsDeAssert);
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

void LampTest::getPhysicalLEDNamesFromJson(const fs::path& path)
{
    if (!fs::exists(path) || fs::is_empty(path))
    {
        log<level::INFO>("The file does not exist or is empty",
                         entry("FILE_PATH=%s", path.c_str()));
        return;
    }

    try
    {
        std::ifstream jsonFile(path);
        auto json = Json::parse(jsonFile);

        // define the default JSON as empty
        const Json empty{};
        std::vector<std::string> forceLEDs = json.value("forceLEDs", empty);
        for (auto& member : forceLEDs)
        {
            forceUpdateLEDs.push_back(PHY_LED_PATH + member);
        }

        std::vector<std::string> skipLEDs = json.value("skipLEDs", empty);
        for (auto& member : skipLEDs)
        {
            skipUpdateLEDs.push_back(PHY_LED_PATH + member);
        }
    }
    catch (const std::exception& e)
    {
        log<level::ERR>("Failed to parse config file",
                        entry("ERROR=%s", e.what()),
                        entry("FILE_PATH=%s", path.c_str()));
    }
    return;
}

} // namespace led
} // namespace phosphor
