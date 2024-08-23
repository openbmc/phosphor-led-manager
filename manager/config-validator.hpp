#pragma once

#include "grouplayout.hpp"
#include "ledlayout.hpp"

#include <phosphor-logging/lg2.hpp>
#include <sdbusplus/bus.hpp>

namespace phosphor
{
namespace led
{
namespace error
{
enum ConfigValidationError
{
    // An LED has different priorities assigned to it in different groups
    LedPriorityMismatch,

    // LED priority was needed but not assigned
    MissingLedPriority,

    // Mixup of the 2 configuration options
    MixedLedAndGroupPriority,

    // An invalid group priority was assigned
    InvalidGroupPriority,

    // Group priorities were not unique
    DuplicateGroupPriority,
};
}

class ConfigValidationException : std::runtime_error
{
  public:
    error::ConfigValidationError reason;

    ConfigValidationException(const error::ConfigValidationError& err,
                              const std::string& msg) :
        std::runtime_error(msg), reason(err)
    {
        lg2::error(msg.c_str());
    }

    ConfigValidationException(const error::ConfigValidationError& err,
                              const std::string& groupName,
                              const std::string& msg) :
        std::runtime_error(msg), reason(err)
    {
        lg2::error("Configuration Validation Error in Group {GROUP}: {MSG}",
                   "GROUP", groupName, "MSG", msg.c_str());
    }

    ConfigValidationException(const error::ConfigValidationError& err,
                              const std::string& groupName,
                              const std::string& ledName,
                              const std::string& msg) :
        std::runtime_error(msg), reason(err)
    {
        lg2::error(
            "Configuration Validation Error in Group {GROUP}, Led {LED}: {MSG}",
            "GROUP", groupName, "LED", ledName, "MSG", msg.c_str());
    }
};

void validateConfigV1(const phosphor::led::GroupMap& ledMap);

} // namespace led
} // namespace phosphor
