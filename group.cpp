#include <sdbusplus/message.hpp>
#include "group.hpp"
namespace phosphor
{
namespace led
{

/** @brief Overloaded Property Setter function */
bool Group::asserted(bool value)
{
    // Introducing these to enable gtest.
    Manager::group ledsAssert {};
    Manager::group ledsDeAssert {};
    Manager::group ledsUpdate {};

    // Group management is handled by Manager. The populated leds* sets are not
    // really used by production code. They are there to enable gtest for
    // validation.
    auto result = manager.setGroupState(path, value, ledsAssert,
                                        ledsDeAssert, ledsUpdate);

    // Set the base class's asserted to 'true' since the getter
    // operation is handled there.
    return sdbusplus::xyz::openbmc_project::Led::server::
                      Group::asserted(result);
}

} // namespace led
} // namespace phosphor
