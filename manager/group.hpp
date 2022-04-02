#pragma once

#include "manager.hpp"
#include "serialize.hpp"

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include <xyz/openbmc_project/Led/Group/server.hpp>

#include <string>

namespace phosphor
{
namespace led
{

namespace
{
using GroupInherit = sdbusplus::server::object_t<
    sdbusplus::xyz::openbmc_project::Led::server::Group>;
}

/** @class Group
 *  @brief Manages group of LEDs and applies action on the elements of group
 */
class Group : public GroupInherit
{
  public:
    Group() = delete;
    ~Group() = default;
    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;
    Group(Group&&) = default;
    Group& operator=(Group&&) = default;

    /** @brief Constructs LED Group
     *
     * @param[in] bus       - Handle to system dbus
     * @param[in] objPath   - The D-Bus path that hosts LED group
     * @param[in] manager   - Reference to Manager
     * @param[in] serialize - Serialize object
     * @param[in] callBack  - Custom callback when LED group is asserted
     */
    Group(sdbusplus::bus::bus& bus, const std::string& objPath,
          Manager& manager, Serialize& serialize,
          std::function<void(Group*, bool)> callBack = nullptr) :

        GroupInherit(bus, objPath.c_str(), GroupInherit::action::defer_emit),
        path(objPath), manager(manager), serialize(serialize),
        customCallBack(callBack)
    {
        // Initialize Asserted property value
        if (serialize.getGroupSavedState(objPath))
        {
            asserted(true);
        }

        // Emit deferred signal.
        emit_object_added();
    }

    /** @brief Property SET Override function
     *
     *  @param[in]  value   -  True or False
     *  @return             -  Success or exception thrown
     */
    bool asserted(bool value) override;

  private:
    /** @brief Path of the group instance */
    std::string path;

    /** @brief Reference to Manager object */
    Manager& manager;

    /** @brief The serialize class for storing and restoring groups of LEDs */
    Serialize& serialize;

    /** @brief Custom callback when LED group is asserted
     */
    std::function<void(Group*, bool)> customCallBack;
};

} // namespace led
} // namespace phosphor
