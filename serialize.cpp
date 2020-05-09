#include "config.h"

#include "serialize.hpp"

#include <cereal/archives/binary.hpp>
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

AssertedGroups Serialize::getAssertedGroups()
{
    return assertedGroups;
}

void serialize(const AssertedGroups& assertedGroups)
{
    fs::path path{ASSERTED_STATE_GROUPS_PATH};

    auto dir = path.parent_path();
    if (!fs::exists(dir))
    {
        fs::create_directories(dir);
    }

    std::ofstream os(path.c_str(), std::ios::binary);
    cereal::BinaryOutputArchive oarchive(os);
    oarchive(assertedGroups);
}

bool deserialize(AssertedGroups& assertedGroups)
{
    fs::path path{ASSERTED_STATE_GROUPS_PATH};

    try
    {
        if (fs::exists(path))
        {
            std::ifstream is(path.c_str(), std::ios::in | std::ios::binary);
            cereal::BinaryInputArchive iarchive(is);
            iarchive(assertedGroups);
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
    auto iter = assertedGroups.find(group);
    if (iter != assertedGroups.end())
    {
        if (!asserted)
        {
            assertedGroups.erase(iter);
        }
    }
    else
    {
        if (!asserted)
        {
            return;
        }
        assertedGroups.insert(group);
    }

    serialize(assertedGroups);
}

void Serialize::restoreGroups()
{
    assertedGroups.clear();
    if (!deserialize(assertedGroups))
    {
        return;
    }
}

} // namespace led
} // namespace phosphor
