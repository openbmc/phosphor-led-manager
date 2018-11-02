#pragma once

#include "manager.hpp"

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include <xyz/openbmc_project/Led/Group/server.hpp>
namespace phosphor
{
namespace led
{

/** @class Group
 *  @brief Manages group of LEDs and applies action on the elements of group
 */
class Group : sdbusplus::server::object::object<
                  sdbusplus::xyz::openbmc_project::Led::server::Group>
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
     * @param[in] bus     - Handle to system dbus
     * @param[in] objPath - The Dbus path that hosts LED group
     * @param[in] manager - Reference to Manager
     */
    Group(sdbusplus::bus::bus& bus, const std::string& objPath,
          Manager& manager) :

        sdbusplus::server::object::object<
            sdbusplus::xyz::openbmc_project::Led::server::Group>(
            bus, objPath.c_str()),
        path(objPath), manager(manager)
    {
        // Nothing to do here
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
};

} // namespace led
} // namespace phosphor
