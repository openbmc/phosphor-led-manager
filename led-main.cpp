#include "led-manager.hpp"
#include "config.h"

int main(void)
{
    std::vector<std::unique_ptr<phosphor::led::Group>> groups;
    auto root = std::string(OBJPATH) + "/";

    /** Now create so many dbus objects as there are groups */
    for (auto &grp: phosphor::led::Group::cv_LedMap)
    {
        auto grpPath = root + grp.first;
        groups.emplace_back(std::make_unique<
                phosphor::led::Group>(grpPath.c_str()));
    }
    phosphor::led::Group::run();
    return 0;
}
