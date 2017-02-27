#include <iostream>
#include <string>
#include <algorithm>
#include <phosphor-logging/log.hpp>
#include "manager.hpp"
namespace phosphor
{
namespace led
{

// Assert -or- De-assert
bool Manager::setGroupState(const std::string& path, bool assert,
                            group& ledsAssert, group& ledsDeAssert,
                            group& ledsUpdate)
{
    if (assert)
    {
        assertedGroups.insert(&ledMap.at(path));
    }
    else
    {
        auto search = assertedGroups.find(&ledMap.at(path));
        if (search != assertedGroups.end())
        {
            assertedGroups.erase(&ledMap.at(path));
        }
    }

    // This will contain the union of what's already in the asserted group
    group desiredState {};
    for(const auto& grp : assertedGroups)
    {
        desiredState.insert(grp->cbegin(), grp->cend());
    }

    // Has the LEDs that are either to be turned off -or- want a new assertion
    group transient {};
    std::set_difference(currentState.begin(), currentState.end(),
                        desiredState.begin(), desiredState.end(),
                        std::inserter(transient, transient.begin()));
    if(transient.size())
    {
        // We really do not want the Manager to know how a particular LED
        // transitions from State-A --> State-B and all this must be handled by
        // the physical LED controller implementation.
        // So in this case, Manager really does not want to turn off the
        // LEDs and then turning it back on and let the physical LED controller
        // handle that.

        // If we previously had a FRU in ON state , and then if there was a
        // request to make it blink, the end state would now be blink.
        // If we either turn off blink / fault, then we need to go back to its
        // previous state.
        std::set_intersection(desiredState.begin(), desiredState.end(),
                              transient.begin(), transient.end(),
                              std::inserter(ledsUpdate, ledsUpdate.begin()),
                              ledComp);

        // These LEDs are only to be De-Asserted.
        std::set_difference(transient.begin(), transient.end(),
                            ledsUpdate.begin(), ledsUpdate.end(),
                            std::inserter(ledsDeAssert, ledsDeAssert.begin()),
                            ledComp);

    }

    // Turn on these
    std::set_difference(desiredState.begin(), desiredState.end(),
                        currentState.begin(), currentState.end(),
                        std::inserter(ledsAssert, ledsAssert.begin()));


    // Done.. Save the latest and greatest.
    currentState = std::move(desiredState);

    // If we survive, then set the state accordingly.
    return assert;
}

/** @brief Run through the map and apply action on the LEDs */
void Manager::driveLEDs(group& ledsAssert, group& ledsDeAssert,
                        group& ledsUpdate)
{
    // For now, physical LED is driven by xyz.openbmc_project.Led.Controller
    // at /xyz/openbmc_project/led/physical. However, its possible that in the
    // future, the physical LEDs are driven by different dbus services.
    // when that happens, service name needs to be obtained everytime a
    // particular LED would be targeted as opposed to getting one now and then
    // using it for all

    // This order of LED operation is important.
    if (ledsUpdate.size())
    {
        std::cout << "Updating LED states between (On <--> Blink)"
                  << std::endl;
        for (const auto& it: ledsUpdate)
        {
            std::string objPath = std::string(PHY_LED_PATH) + it.name;
            drivePhysicalLED(objPath, it.action, it.dutyOn);
        }
    }

    if (ledsDeAssert.size())
    {
        std::cout << "De-Asserting LEDs" << std::endl;
        for (const auto& it: ledsDeAssert)
        {
            std::string objPath = std::string(PHY_LED_PATH) + it.name;
            drivePhysicalLED(objPath, Layout::Action::Off, it.action);
        }
    }

    if(ledsAssert.size())
    {
        std::cout << "Asserting LEDs" << std::endl;
        for (const auto& it: ledsAssert)
        {
            std::string objPath = std::string(PHY_LED_PATH) + it.name;
            drivePhysicalLED(objPath, it.action, it.dutyOn);
        }
    }
    return;
}

// Calls into driving physical LED post choosing the action
void Manager::drivePhysicalLED(const std::string& objPath,
                               Layout::Action action,
                               uint8_t dutyOn)
{
    auto service = getServiceName(objPath, PHY_LED_IFACE);
    if (!service.empty())
    {
        // If Blink, set its property
        if (action == Layout::Action::Blink)
        {
            drivePhysicalLED(service, objPath, "DutyOn", dutyOn);
        }
        drivePhysicalLED(service, objPath, "State",
                getPhysicalAction(action));
    }
}

/** @brief Returns action string based on enum */
const char* const Manager::getPhysicalAction(Layout::Action action)
{
    // TODO : When this is moved over to using libdus interfaces, this code will
    // away. https://github.com/openbmc/phosphor-led-manager/issues/2
    if(action == Layout::Action::On)
    {
        return "xyz.openbmc_project.Led.Physical.Action.On";
    }
    else if(action == Layout::Action::Blink)
    {
        return "xyz.openbmc_project.Led.Physical.Action.Blink";
    }
    else
    {
        return "xyz.openbmc_project.Led.Physical.Action.Off";
    }
}

/** Given the LED dbus path and interface, returns the service name */
std::string Manager::getServiceName(const std::string& objPath,
                                    const std::string& interface) const
{
    using namespace phosphor::logging;

    // Mapper dbus constructs
    constexpr auto MAPPER_BUSNAME   = "xyz.openbmc_project.ObjectMapper";
    constexpr auto MAPPER_OBJ_PATH  = "/xyz/openbmc_project/ObjectMapper";
    constexpr auto MAPPER_IFACE     = "xyz.openbmc_project.ObjectMapper";

    // Make a mapper call
    auto mapperCall = bus.new_method_call(MAPPER_BUSNAME, MAPPER_OBJ_PATH,
                                          MAPPER_IFACE, "GetObject");
    // Cook rest of the things.
    mapperCall.append(objPath);
    mapperCall.append(std::vector<std::string>({interface}));

    auto reply = bus.call(mapperCall);
    if (reply.is_method_error())
    {
        // Its okay if we do not see a corresponding physical LED.
        log<level::INFO>("Error looking up Physical LED service",
                entry("PATH=%s",objPath.c_str()));
        return "";
    }

    // Response by mapper in the case of success
    std::map<std::string, std::vector<std::string>> serviceNames;

    // This is the service name for the passed in objpath
    reply.read(serviceNames);
    if (serviceNames.empty())
    {
        log<level::INFO>("Physical LED lookup did not return any service",
                entry("PATH=%s",objPath.c_str()));
        return "";
    }

    return serviceNames.begin()->first;
}

} // namespace led
} // namespace phosphor
