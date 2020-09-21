#include "config.h"

#include "group.hpp"

#include <sdbusplus/message.hpp>
namespace phosphor
{
namespace led
{

/** @brief Overloaded Property Setter function */
bool Group::asserted(bool value)
{
    // Introducing these to enable gtest.
    LedsAssert ledsAssert{};
    LedsDeAssert ledsDeAssert{};

    // Group management is handled by Manager. The populated leds* sets are
    // not really used by production code. They are there to enable gtest for
    // validation.
    auto result = manager.setGroupState(path, value, ledsAssert, ledsDeAssert);

    // Store asserted state
    serialize.storeGroups(path, result);

    if (!manager.getLampTestStatus())
    {
        // If something does not go right here, then there should be an
        // sdbusplus exception thrown.
        manager.driveLEDs(ledsAssert, ledsDeAssert);
    }

    manager.setLedsAssertGroups(
        path, std::make_tuple(std::move(ledsAssert), std::move(ledsDeAssert)));

    // Set the base class's asserted to 'true' since the getter
    // operation is handled there.
    return sdbusplus::xyz::openbmc_project::Led::server::Group::asserted(value);
}

} // namespace led
} // namespace phosphor
