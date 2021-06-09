#pragma once

#include "../utils.hpp"

namespace phosphor
{
namespace led
{
namespace multi
{
namespace purpose
{
namespace status
{

using namespace phosphor::led::utils;

class Status
{
  public:
    Status()
    {
        loadConfigValues();
    }

 private:
    DBusHandler dBusHandler;

    void selectPurpose(const std::string& purpose);

    const PropertyValue getPropertyValue(const std::string& objectPath,
                                         const std::string& interface,
                                         const std::string& propertyName);

    void setLedGroup(const std::string& objectPath, bool value);

    void selectLedGroup(uint16_t position, const std::string& powerStatus,
                        const std::string& healthStatus);

    void loadConfigValues();
};
} // namespace status
} // namespace purpose
} // namespace multi
} // namespace led
} // namespace phosphor
