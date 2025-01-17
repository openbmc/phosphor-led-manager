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

#include "sysfs.hpp"

#include "phosphor-logging/lg2.hpp"

#include <cstring>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace phosphor
{
namespace led
{
template <typename T>
T getSysfsAttr(const fs::path& path);

template <>
std::string getSysfsAttr(const fs::path& path)
{
    std::string content;
    std::ifstream file(path);
    std::getline(file, content);
    return content;
}

template <>
unsigned long getSysfsAttr(const fs::path& path)
{
    std::string content = getSysfsAttr<std::string>(path);
    return std::strtoul(content.c_str(), nullptr, 0);
}

template <typename T>
void setSysfsAttr(const fs::path& path, const T& value)
{
    std::ofstream file(path);
    file << value;
}

unsigned long SysfsLed::getBrightness()
{
    return getSysfsAttr<unsigned long>(root / attrBrightness);
}

void SysfsLed::setBrightness(unsigned long brightness)
{
    setSysfsAttr<unsigned long>(root / attrBrightness, brightness);
}

unsigned long SysfsLed::getMaxBrightness()
{
    return getSysfsAttr<unsigned long>(root / attrMaxBrightness);
}

std::string SysfsLed::getTrigger()
{
    // Example content for `/sys/class/leds/<led_name>/trigger`:
    //
    // * `[none] timer heartbeat default-on`
    // * `none [timer] heartbeat default-on`
    //
    // Refer to:
    //
    // * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/ABI/testing/sysfs-class-led?h=v6.6#n71
    // * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/ABI/stable/sysfs-block?h=v6.6#n558
    std::string triggerLine = getSysfsAttr<std::string>(root / attrTrigger);
    size_t start = triggerLine.find_first_of('[');
    size_t end = triggerLine.find_first_of(']');
    if (start >= end || start == std::string::npos || end == std::string::npos)
    {
        return "none";
    }

    std::string rc = triggerLine.substr(start + 1, end - start - 1);
    if (rc.empty())
    {
        return "none";
    }

    return rc;
}

void SysfsLed::setTrigger(const std::string& trigger)
{
    setSysfsAttr<std::string>(root / attrTrigger, trigger);
}

unsigned long SysfsLed::getDelayOn()
{
    return getSysfsAttr<unsigned long>(root / attrDelayOn);
}

void SysfsLed::setDelayOn(unsigned long ms)
{
    setSysfsAttr<unsigned long>(root / attrDelayOn, ms);
}

unsigned long SysfsLed::getDelayOff()
{
    return getSysfsAttr<unsigned long>(root / attrDelayOff);
}

void SysfsLed::setDelayOff(unsigned long ms)
{
    setSysfsAttr<unsigned long>(root / attrDelayOff, ms);
}

/* LED sysfs name can be any of
 *
 * - devicename:color:function
 * - devicename::function
 * - color:function (e.g. "red:fault")
 * - label (e.g. "identify")
 * - :function (e.g. ":boot")
 * - color: (e.g. "green:")
 *
 * but no one prevents us from making all of this up and creating
 * a label with colons inside, e.g. "mylabel:mynoncolorstring:lala".
 *
 * Reference: https://www.kernel.org/doc/html/latest/leds/leds-class.html
 *
 * Summary: It's bonkers (not my words, but describes it well)
 */
LedDescr SysfsLed::getLedDescr()
{
    std::string name = std::string(root).substr(strlen(devParent));
    LedDescr ledDescr;

    std::vector<std::optional<std::string>> words;
    std::stringstream ss(name);
    std::string item;

    while (std::getline(ss, item, ':'))
    {
        if (item.empty())
        {
            words.emplace_back(std::nullopt);
        }
        else
        {
            words.emplace_back(item);
        }
    }

    if (name.ends_with(":"))
    {
        words.emplace_back(std::nullopt);
    }

    if (name.empty())
    {
        lg2::warning("LED description '{DESC}' was empty", "DESC", name);
        throw std::out_of_range("expected non-empty LED name");
    }

    if (words.size() != 3)
    {
        lg2::warning(
            "LED description '{DESC}' not well formed, expected 3 parts but got {NPARTS}",
            "DESC", name, "NPARTS", words.size());
    }

    switch (words.size())
    {
        default:
        case 3:
            ledDescr.function = words.at(2);
            ledDescr.color = words.at(1);
            ledDescr.devicename = words.at(0);
            break;
        case 2:
            ledDescr.color = words.at(0);
            ledDescr.function = words.at(1);
            break;
        case 1:
            ledDescr.devicename = words.at(0);
            break;
        case 0:
            throw std::out_of_range("expected non-empty LED name");
    }

    // if there is more than 3 parts we ignore the rest
    return ledDescr;
}
} // namespace led
} // namespace phosphor
