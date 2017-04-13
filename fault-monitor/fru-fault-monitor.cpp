#include "fru-fault-monitor.hpp"
namespace phosphor
{
namespace led
{
namespace inventory
{
namespace fault
{

std::string getService(sdbusplus::bus::bus& bus,
                       const std::string& path)
{
    return "none";
}

void action(sdbusplus::bus::bus& bus,
            const std::string& unit,
            const bool assert)
{
    return;
}

int Monitor::created(sd_bus_message* msg,
                     void* data,
                     sd_bus_error* retError)
{
    return 0;
}

int RemoveMonitor::removed(sd_bus_message* msg,
                           void* data,
                           sd_bus_error* retError)
{
    return 0;
}

void Monitor::addFaultRemovedWatch(sdbusplus::bus::bus& bus,
                                   const std::string& unit)
{
    return;
}

}//namespace fault
}//namespace inventory
}//namespace led
}//namespace phosphor
