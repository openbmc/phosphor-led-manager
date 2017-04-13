#include "fru-fault-monitor.hpp"
namespace phosphor
{
namespace led
{
namespace fru
{
namespace fault
{
namespace monitor
{

void action(sdbusplus::bus::bus& bus,
            const std::string& unit,
            bool assert)
{
    return;
}

int Add::created(sd_bus_message* msg,
                 void* data,
                 sd_bus_error* retError)
{
    return 0;
}

int Remove::removed(sd_bus_message* msg,
                    void* data,
                    sd_bus_error* retError)
{
    return 0;
}

}//namespace monitor
}//namespace fault
}//namespace fru
}//namespace led
}//namespace phosphor
