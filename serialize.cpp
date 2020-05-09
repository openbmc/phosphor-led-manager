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

bool Serialize::getSavedGroups(const std::string& objPath)
{
    for (auto& group : savedGroups)
    {
        if (group == objPath)
        {
            return true;
        }
    }

    return false;
}

void Serialize::storeGroups(const std::string& group, bool asserted)
{
    auto iter = savedGroups.find(group);
    if (iter != savedGroups.end() && asserted == false)
    {
        savedGroups.erase(iter);
    }

    if (iter == savedGroups.end() && asserted)
    {
        savedGroups.insert(group);
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
