#include "config.h"

#include "serialize.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>
#include <phosphor-logging/log.hpp>

#include <filesystem>
#include <fstream>

namespace phosphor
{
namespace led
{

namespace fs = std::filesystem;

bool Serialize::getGroupSavedState(const std::string& objPath)
{
    auto iter = std::find_if(
        savedGroups.begin(), savedGroups.end(),
        [&objPath](const auto& group) { return group == objPath; });

    return iter == savedGroups.end() ? false : true;
}

void Serialize::storeGroups(const std::string& group, bool asserted)
{
    // the names of asserted groups exist in the list, the Asserted property is
    // true, and it is inserted into list.
    // the names of asserted groups does not exist in the list, the Asserted
    // property is true, and it is removed into list.
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
    using namespace phosphor::logging;

    if (!fs::exists(path))
    {
        log<level::ERR>("File does not exist",
                        entry("FILE_PATH=%s", path.c_str()));
        return;
    }

    try
    {
        std::ifstream is(path.c_str(), std::ios::in | std::ios::binary);
        cereal::JSONInputArchive iarchive(is);
        iarchive(savedGroups);
    }
    catch (cereal::Exception& e)
    {
        log<level::ERR>(e.what());
        fs::remove(path);
    }
}

} // namespace led
} // namespace phosphor
