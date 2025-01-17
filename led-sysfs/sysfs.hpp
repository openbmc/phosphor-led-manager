/**
 * Copyright © 2019 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <filesystem>
#include <optional>

static constexpr auto devParent = "/sys/class/leds/";

namespace phosphor
{
namespace led
{

struct LedDescr
{
    // at least one of the members shall be non-empty
    // after initialization

    std::optional<std::string> devicename;
    std::optional<std::string> color;
    std::optional<std::string> function;
};

class SysfsLed
{
  public:
    explicit SysfsLed(std::filesystem::path&& root) : root(std::move(root)) {}
    SysfsLed() = delete;
    SysfsLed(const SysfsLed& other) = delete;
    SysfsLed(const SysfsLed&& other) = delete;
    SysfsLed& operator=(const SysfsLed& other) = delete;
    SysfsLed&& operator=(const SysfsLed&& other) = delete;

    virtual ~SysfsLed() = default;

    virtual unsigned long getBrightness();
    virtual void setBrightness(unsigned long brightness);
    virtual unsigned long getMaxBrightness();
    virtual std::string getTrigger();
    virtual void setTrigger(const std::string& trigger);
    virtual unsigned long getDelayOn();
    virtual void setDelayOn(unsigned long ms);
    virtual unsigned long getDelayOff();
    virtual void setDelayOff(unsigned long ms);

    /** @brief parse LED name in sysfs
     *  Parse sysfs LED name and sets corresponding
     *  fields in LedDescr struct.
     */
    LedDescr getLedDescr();

  protected:
    static constexpr const char* attrBrightness = "brightness";
    static constexpr const char* attrMaxBrightness = "max_brightness";
    static constexpr const char* attrTrigger = "trigger";
    static constexpr const char* attrDelayOn = "delay_on";
    static constexpr const char* attrDelayOff = "delay_off";

    std::filesystem::path root;
};
} // namespace led
} // namespace phosphor
