#include "fru-fault-monitor.hpp"

namespace phosphor
{
namespace led
{
namespace inventory
{
namespace fault
{

void action(sdbusplus::bus::bus& bus,
            const std::string& path,
            const bool assert)
{
}

int Monitor::created(sd_bus_message* msg,
                     void* data,
                     sd_bus_error* retError)
{
    return 0;
}

int Monitor::removed(sd_bus_message* msg,
                     void* data,
                     sd_bus_error* retError)
{
    return 0;
}

}//namespace fault
}//namespace inventory
}//namespace led
}//namespace phosphor
