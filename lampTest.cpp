#include "config.h"

#include "lampTest.hpp"

#include <phosphor-logging/elog.hpp>

namespace phosphor
{
namespace led
{

using namespace phosphor::logging;

ObjectPaths LampTest::getLedPhysicalPahts()
{
    constexpr auto MAPPER_BUSNAME = "xyz.openbmc_project.ObjectMapper";
    constexpr auto MAPPER_OBJ_PATH = "/xyz/openbmc_project/object_mapper";
    constexpr auto MAPPER_IFACE = "xyz.openbmc_project.ObjectMapper";
    constexpr auto INVENTORY_OBJ_PATH = "/xyz/openbmc_project/led/physical";
    constexpr auto PHYSICAL_IFACE = "xyz.openbmc_project.Led.Physical";

    ObjectPaths paths;

    auto method = bus.new_method_call(MAPPER_BUSNAME, MAPPER_OBJ_PATH,
                                      MAPPER_IFACE, "GetSubTreePaths");
    method.append(INVENTORY_OBJ_PATH);
    method.append(0); // Depth 0 to search all
    method.append(ObjectPaths({PHYSICAL_IFACE}));
    auto reply = bus.call(method);

    reply.read(paths);

    return paths;
}

ObjectPaths LampTest::getLedGroupPahts()
{
    constexpr auto MAPPER_BUSNAME = "xyz.openbmc_project.ObjectMapper";
    constexpr auto MAPPER_OBJ_PATH = "/xyz/openbmc_project/object_mapper";
    constexpr auto MAPPER_IFACE = "xyz.openbmc_project.ObjectMapper";
    constexpr auto INVENTORY_OBJ_PATH = "/xyz/openbmc_project/led/groups";
    constexpr auto PHYSICAL_IFACE = "xyz.openbmc_project.Led.Group";

    ObjectPaths paths;

    auto method = bus.new_method_call(MAPPER_BUSNAME, MAPPER_OBJ_PATH,
                                      MAPPER_IFACE, "GetSubTreePaths");
    method.append(INVENTORY_OBJ_PATH);
    method.append(0); // Depth 0 to search all
    method.append(ObjectPaths({PHYSICAL_IFACE}));
    auto reply = bus.call(method);

    reply.read(paths);

    return paths;
}

void LampTest::setAssertedStatus(const std::string& path, bool status)
{
    auto iter = groups.find(path);
    if (iter != groups.end())
    {
        const auto& group = iter->second;
        group->asserted(status);
    }
}

void LampTest::updatePhysicalAction()
{
    ObjectPaths paths = getLedPhysicalPahts();
    for (auto& path : paths)
    {
        manager.drivePhysicalLED(path, Layout::Action::On, 0, 0);
    }
}

void LampTest::lampTestHandler(sdbusplus::message::message& /*msg*/)
{
    constexpr auto LAMP_TEST_TIMEOUT_SECS = std::chrono::seconds(4 * 60);

    timer.restart(LAMP_TEST_TIMEOUT_SECS);

    manager.setLampTestStatus(true);

    // Set all the Physical action to On for lamp test
    updatePhysicalAction();
}

void LampTest::lampTestTimeout()
{
    timer.setEnabled(false);

    // lamptest is not running and call the restore function
    manager.setLampTestStatus(false);

    ObjectPaths paths = getLedPhysicalPahts();
    for (auto& path : paths)
    {
        manager.drivePhysicalLED(path, Layout::Action::Off, 0, 0);
    }

    ObjectPaths groups = getLedGroupPahts();
    for (auto& group : groups)
    {
        if (serialize.getGroupSavedState(group))
        {
            setAssertedStatus(group);
        }
        else
        {
            setAssertedStatus(group, false);
        }
    }
}

} // namespace led
} // namespace phosphor
