#pragma once

#include <set>
#include <string>

namespace phosphor
{
namespace led
{

using AssertedStateGroups = std::set<std::string>;

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
    void storeGroups(const std::string& group, bool asserted = false);

    /** @brief Restore asserted state groups
     *
     */
    void restoreGroups();

    /** @brief Get asserted state groups
     *
     */
    const AssertedStateGroups& getAssertedStateGroups();

  private:
    /** @brief the set of asserted state groups*/
    AssertedStateGroups assertedStateGroups;
};

} // namespace led
} // namespace phosphor
