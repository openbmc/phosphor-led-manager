#include <sdbusplus/message.hpp>
#include "group.hpp"
namespace phosphor
{
namespace led
{

/** @brief Overloaded Property Setter function */
bool Group::asserted(bool value)
{
    // Group management is handled by Manager
    auto result = manager.setGroupState(path, value);

    // Set the base class's asserted to 'true' since the getter
    // operation is handled there.
    return sdbusplus::xyz::openbmc_project::Led::server::
                      Group::asserted(result);
}

} // namespace led
} // namespace phosphor
