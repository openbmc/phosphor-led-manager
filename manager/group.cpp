#include "config.h"

#include "group.hpp"

#include "ledlayout.hpp"

#include <sdbusplus/message.hpp>
namespace phosphor
{
namespace led
{

/** @brief Overloaded Property Setter function */
bool Group::asserted(bool value)
{
    if (customCallBack != nullptr)
    {
        // Custom callback method tells if the lamptest request is handled
        // successfully or not.
        if (customCallBack(this, value))
        {
            // If the lamp test request is handled successfully, update the
            // asserted property.
            return sdbusplus::xyz::openbmc_project::Led::server::Group::
                asserted(value);
        }

        // If the lamp test request is not handled successfully, return the
        // existing asserted value without any change.
        return sdbusplus::xyz::openbmc_project::Led::server::Group::asserted();
    }

    // If the value is already what is before, return right away
    if (value ==
        sdbusplus::xyz::openbmc_project::Led::server::Group::asserted())
    {
        return value;
    }

    // Introducing these to enable gtest.
    ActionSet ledsAssert{};
    ActionSet ledsDeAssert{};

    // Group management is handled by Manager. The populated leds* sets are not
    // really used by production code. They are there to enable gtest for
    // validation.
    auto result = manager.setGroupState(path, value, ledsAssert, ledsDeAssert);

    // Store asserted state
    if (serializePtr)
    {
        serializePtr->storeGroups(path, result);
    }

    // If something does not go right here, then there should be an sdbusplus
    // exception thrown.
    manager.driveLEDs(ledsAssert, ledsDeAssert);

    // Set the base class's asserted to 'true' since the getter
    // operation is handled there.
    return sdbusplus::xyz::openbmc_project::Led::server::Group::asserted(
        result);
}

} // namespace led
} // namespace phosphor
