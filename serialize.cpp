#include "config.h"

#include "serialize.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>
#include <filesystem>
#include <fstream>
#include <phosphor-logging/elog.hpp>
#include <phosphor-logging/log.hpp>

namespace phosphor
{
namespace led
{

namespace fs = std::filesystem;
using namespace phosphor::logging;

/** Path of file for storing the current state of asserted*/
static constexpr auto ASSERTED_STATE_GROUPS_PATH =
    "/var/lib/phosphor-led-manager/assertedStateGroups";

AssertedStateGroups Serialize::getAssertedStateGroups()
{
    return assertedStateGroups;
}

void serialize(const AssertedStateGroups& assertedStateGroups)
{
    fs::path path{ASSERTED_STATE_GROUPS_PATH};

    auto dir = path.parent_path();
    if (!fs::exists(dir))
    {
        fs::create_directories(dir);
    }

    std::ofstream os(path.c_str(), std::ios::binary);
    cereal::JSONOutputArchive oarchive(os);
    oarchive(assertedStateGroups);
}

bool deserialize(AssertedStateGroups& assertedStateGroups)
{
    fs::path path{ASSERTED_STATE_GROUPS_PATH};

    try
    {
        if (fs::exists(path))
        {
            std::ifstream is(path.c_str(), std::ios::in | std::ios::binary);
            cereal::JSONInputArchive iarchive(is);
            iarchive(assertedStateGroups);
            return true;
        }
        return false;
    }
    catch (cereal::Exception& e)
    {
        log<level::ERR>(e.what());
        fs::remove(path);
        return false;
    }

    return true;
}

void Serialize::storeGroups(std::string group, bool asserted)
{
    auto iter = assertedStateGroups.find(group);
    if (iter != assertedStateGroups.end())
    {
        if (!asserted)
        {
            assertedStateGroups.erase(iter);
        }
    }
    else
    {
        if (!asserted)
        {
            return;
        }
        assertedStateGroups.insert(group);
    }

    serialize(assertedStateGroups);
}

void Serialize::restoreGroups()
{
    assertedStateGroups.clear();
    if (!deserialize(assertedStateGroups))
    {
        return;
    }
}

} // namespace led
} // namespace phosphor
