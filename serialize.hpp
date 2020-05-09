#pragma once

#include <filesystem>
#include <fstream>
#include <set>
#include <string>

namespace phosphor
{
namespace led
{

namespace fs = std::filesystem;

using SavedGroups = std::set<std::string>;

/** @class Serialize
 *  @brief Store and restore groups of LEDs
 */
class Serialize
{
  public:
    Serialize(const fs::path& path) : path(path)
    {
        restoreGroups();
    }

    /** @brief Store asserted state groups to
     * /var/lib/phosphor-led-manager/savedGroups
     *
     *  @param [in] group     - asserted state groups
     *  @param [in] asserted  - true or false, the default is false
     */
    void storeGroups(const std::string& group, bool asserted = false);

    /** @brief Restore asserted state groups from
     * /var/lib/phosphor-led-manager/savedGroups
     *
     */
    void restoreGroups();

    /** @brief Is there a group save state in SAVED_GROUPS_PATH
     *
     *  @param [in] objPath - The Dbus path that hosts LED group
     *
     *  @return             - true: exist, false: does not exist
     */
    bool getGroupSavedState(const std::string& objPath);

  private:
    /** @brief the set of asserted state groups*/
    SavedGroups savedGroups;

    fs::path path;
};

} // namespace led
} // namespace phosphor
