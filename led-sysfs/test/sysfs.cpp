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

#include <sys/param.h>

#include <cerrno>
#include <cstdlib>
#include <fstream>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

constexpr unsigned long maxBrightnessVal = 128;

class FakeSysfsLed : public phosphor::led::SysfsLed
{
  public:
    FakeSysfsLed(FakeSysfsLed& other) = delete;
    FakeSysfsLed(FakeSysfsLed&& other) = delete;
    FakeSysfsLed& operator=(FakeSysfsLed& other) = delete;
    FakeSysfsLed&& operator=(FakeSysfsLed&& other) = delete;

    static FakeSysfsLed create()
    {
        static constexpr auto tmplt = "/tmp/FakeSysfsLed.XXXXXX";
        std::array<char, MAXPATHLEN> buffer = {0};

        strncpy(buffer.data(), tmplt, buffer.size() - 1);
        char* dir = mkdtemp(buffer.data());
        if (dir == nullptr)
        {
            throw std::system_error(errno, std::system_category());
        }

        return FakeSysfsLed(fs::path(dir));
    }

    ~FakeSysfsLed() override
    {
        fs::remove_all(root);
    }

  private:
    explicit FakeSysfsLed(fs::path&& path) : SysfsLed(std::move(path))
    {
        static constexpr auto attrs = {attrBrightness, attrTrigger, attrDelayOn,
                                       attrDelayOff};
        for (const auto& attr : attrs)
        {
            fs::path p = root / attr;
            std::ofstream f(p, std::ios::out);
            f.exceptions(std::ofstream::failbit);
        }

        fs::path p = root / attrMaxBrightness;
        std::ofstream f(p, std::ios::out);
        f.exceptions(std::ofstream::failbit);
        f << maxBrightnessVal;
    }
};

TEST(Sysfs, getBrightness)
{
    constexpr unsigned long brightness = 127;
    FakeSysfsLed fsl = FakeSysfsLed::create();

    fsl.setBrightness(brightness);
    ASSERT_EQ(brightness, fsl.getBrightness());
}

TEST(Sysfs, getMaxBrightness)
{
    FakeSysfsLed fsl = FakeSysfsLed::create();

    ASSERT_EQ(maxBrightnessVal, fsl.getMaxBrightness());
}

TEST(Sysfs, getTrigger)
{
    FakeSysfsLed fsl = FakeSysfsLed::create();

    // We need to set the complete attribute value in UT, Because the LED driver
    // is not called in UT to automatically set `none` to `[none] xxx yyy`
    fsl.setTrigger("[none] timer heartbeat default-on");
    ASSERT_EQ("none", fsl.getTrigger());
}

TEST(Sysfs, getTriggerBlink)
{
    FakeSysfsLed fsl = FakeSysfsLed::create();
    fsl.setTrigger("none [timer] heartbeat default-on");
    ASSERT_EQ("timer", fsl.getTrigger());
}

TEST(Sysfs, getDelayOn)
{
    constexpr unsigned long delayOn = 250;
    FakeSysfsLed fsl = FakeSysfsLed::create();

    fsl.setDelayOn(delayOn);
    ASSERT_EQ(delayOn, fsl.getDelayOn());
}

TEST(Sysfs, getDelayOff)
{
    constexpr unsigned long delayOff = 750;
    FakeSysfsLed fsl = FakeSysfsLed::create();

    fsl.setDelayOff(delayOff);
    ASSERT_EQ(delayOff, fsl.getDelayOff());
}
