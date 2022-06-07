#include "config.h"

#include "serialize.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>
#include <phosphor-logging/lg2.hpp>

#include <filesystem>
#include <fstream>

// Register class version with Cereal
CEREAL_CLASS_VERSION(phosphor::led::Serialize, CLASS_VERSION)

namespace phosphor
{
namespace led
{

namespace fs = std::filesystem;

bool Serialize::getGroupSavedState(const std::string& objPath) const
{
    return savedGroups.contains(objPath);
}

void Serialize::storeGroups(const std::string& group, bool asserted)
{
    // If the name of asserted group does not exist in the archive and the
    // Asserted property is true, it is inserted into archive.
    // If the name of asserted group exist in the archive and the Asserted
    // property is false, entry is removed from the archive.
    auto iter = savedGroups.find(group);
    if (iter != savedGroups.end() && asserted == false)
    {
        savedGroups.erase(iter);
    }

    if (iter == savedGroups.end() && asserted)
    {
        savedGroups.emplace(group);
    }

    auto dir = path.parent_path();
    if (!fs::exists(dir))
    {
        fs::create_directories(dir);
    }

    std::ofstream os(path.c_str(), std::ios::binary);
    cereal::JSONOutputArchive oarchive(os);
    oarchive(savedGroups);
}

void Serialize::restoreGroups()
{
    if (!fs::exists(path))
    {
        lg2::info("File does not exist, FILE_PATH = {PATH}", "PATH", path);
        return;
    }

    try
    {
        std::ifstream is(path.c_str(), std::ios::in | std::ios::binary);
        cereal::JSONInputArchive iarchive(is);
        iarchive(savedGroups);
    }
    catch (const cereal::Exception& e)
    {
        lg2::error("Failed to restore groups, ERROR = {ERROR}", "ERROR", e);
        fs::remove(path);
    }
}

} // namespace led
} // namespace phosphor
