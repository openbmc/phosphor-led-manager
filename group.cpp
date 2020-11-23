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
    // If the value is already what is before, return right away
    if (value ==
        sdbusplus::xyz::openbmc_project::Led::server::Group::asserted())
    {
        return value;
    }

#ifdef USE_LAMP_TEST
    if (callBack != nullptr)
    {
        callBack();
    }
#else
    // Introducing these to enable gtest.
    Manager::group ledsAssert{};
    Manager::group ledsDeAssert{};

    // Group management is handled by Manager. The populated leds* sets are not
    // really used by production code. They are there to enable gtest for
    // validation.
    auto result = manager.setGroupState(path, value, ledsAssert, ledsDeAssert);

    // Store asserted state
    serialize.storeGroups(path, result);

    // Set OperationalStatus according to the status of asserted property.
    // If the group is asserted, then the functional status is false and
    // vice-versa.
    manager.setOperationalStatus(path, !value);

    // If something does not go right here, then there should be an
    // sdbusplus exception thrown.
    manager.driveLEDs(ledsAssert, ledsDeAssert);
#endif

    // Set the base class's asserted to 'true' since the getter
    // operation is handled there.
    return sdbusplus::xyz::openbmc_project::Led::server::Group::asserted(
        result);
}

} // namespace led
} // namespace phosphor
