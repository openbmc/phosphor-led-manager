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

using GroupInherit = sdbusplus::server::object_t<
    sdbusplus::xyz::openbmc_project::Led::server::Group>;

/** @class Group
 *  @brief Manages group of LEDs and applies action on the elements of group
 */
class Group : public GroupInherit
{
  public:
    Group() = delete;
    ~Group() override = default;
    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;
    Group(Group&&) = delete;
    Group& operator=(Group&&) = delete;

    /** @brief Constructs LED Group
     *
     * @param[in] bus           - Handle to system dbus
     * @param[in] objPath       - The D-Bus path that hosts LED group
     * @param[in] manager       - Reference to Manager
     * @param[in] serializePtr  - Serialize object
     * @param[in] callBack      - Custom callback when LED group is asserted
     */
    Group(sdbusplus::bus_t& bus, const std::string& objPath, Manager& manager,
          std::shared_ptr<Serialize> serializePtr,
          std::function<bool(Group*, bool)> callBack = nullptr) :

        GroupInherit(bus, objPath.c_str(), GroupInherit::action::defer_emit),
        path(objPath), manager(manager), serializePtr(serializePtr),
        customCallBack(callBack)
    {
        // Initialize Asserted property value
        if (serializePtr && serializePtr->getGroupSavedState(objPath))
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
    std::shared_ptr<Serialize> serializePtr;

    /** @brief Custom callback when LED group is asserted
     * Callback that holds LED group method which handles lamp test request.
     *
     * @param[in] Group object - Pointer to Group object
     * @param[in] bool - Input value (true/false)
     *
     * @return bool which tells if execution succeeds(true) or fails(false).
     */
    std::function<bool(Group*, bool)> customCallBack;
};

} // namespace led
} // namespace phosphor
