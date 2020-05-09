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

/** Path of file for storing the current state of asserted*/
static fs::path path{"/var/lib/phosphor-led-manager/assertedStateGroups"};

const AssertedStateGroups& Serialize::getAssertedStateGroups()
{
    return assertedStateGroups;
}

void Serialize::storeGroups(const std::string& group, bool asserted)
{
    auto iter = assertedStateGroups.find(group);
    if (iter != assertedStateGroups.end() && !asserted)
    {
        assertedStateGroups.erase(iter);
    }

    if (iter == assertedStateGroups.end() && asserted)
    {
        assertedStateGroups.insert(group);
    }

    auto dir = path.parent_path();
    if (!fs::exists(dir))
    {
        fs::create_directories(dir);
    }

    std::ofstream os(path.c_str(), std::ios::binary);
    cereal::JSONOutputArchive oarchive(os);
    oarchive(assertedStateGroups);
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
        iarchive(assertedStateGroups);
    }
    catch (cereal::Exception& e)
    {
        log<level::ERR>(e.what());
        fs::remove(path);
    }
}

} // namespace led
} // namespace phosphor
