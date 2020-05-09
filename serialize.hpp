#pragma once

#include <sdbusplus/bus.hpp>
#include <set>

namespace phosphor
{
namespace led
{

using AssertedGroups = std::set<std::string>;

/** @class Serialize
 *  @brief Store and restore groups of LEDs
 */
class Serialize
{
  public:
    /** @brief Refer the user supplied LED layout and sdbusplus handler
     *
     */
    Serialize()
    {
        restoreGroups();
    }

    /** @brief Store asserted state groups
     *
     *  @param [in] group     - asserted state groups
     *  @param [in] asserted  - true or false, the default is false
     */
    void storeGroups(std::string group, bool asserted = false);

    /** @brief Restore asserted state groups
     *
     */
    void restoreGroups();

    /** @brief Get asserted state groups
     *
     */
    AssertedGroups getAssertedGroups();

  private:
    /** @brief the set of asserted state groups*/
    AssertedGroups assertedGroups;
};

} // namespace led
} // namespace phosphor
