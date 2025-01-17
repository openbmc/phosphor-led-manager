/**
 * Copyright © 2024 9elements
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

#include "interfaces/internal_interface.hpp"
#include "sysfs.hpp"

#include <gtest/gtest.h>

using namespace phosphor::led;

static LedDescr runtest(const std::string& name)
{
    std::string path = devParent + name;
    SysfsLed led(path);
    return led.getLedDescr();
}

TEST(LEDDescr, Has4PartsInvalid)
{
    LedDescr d = runtest("devicename:color:function:part4");

    ASSERT_EQ("devicename", d.devicename);
    ASSERT_EQ("color", d.color);
    ASSERT_EQ("function", d.function);
}

TEST(LEDDescr, Has3Parts)
{
    LedDescr d = runtest("devicename:color:function");

    ASSERT_EQ("devicename", d.devicename);
    ASSERT_EQ("color", d.color);
    ASSERT_EQ("function", d.function);
}

TEST(LEDDescr, Has2PartsColorFunction)
{
    LedDescr d = runtest("red:fault");

    ASSERT_EQ(std::nullopt, d.devicename);
    ASSERT_EQ("red", d.color);
    ASSERT_EQ("fault", d.function);
}

TEST(LEDDescr, Has2PartsDevicenameFunction)
{
    LedDescr d = runtest("input9::capslock");

    ASSERT_EQ("input9", d.devicename);
    ASSERT_EQ(std::nullopt, d.color);
    ASSERT_EQ("capslock", d.function);
}

TEST(LEDDescr, Has1PartColor)
{
    LedDescr d = runtest("green:");

    ASSERT_EQ(std::nullopt, d.devicename);
    ASSERT_EQ("green", d.color);
    ASSERT_EQ(std::nullopt, d.function);
}

TEST(LEDDescr, Has1PartFunction)
{
    LedDescr d = runtest(":boot");

    ASSERT_EQ(std::nullopt, d.devicename);
    ASSERT_EQ(std::nullopt, d.color);
    ASSERT_EQ("boot", d.function);
}

TEST(LEDDescr, Has1PartLabel)
{
    LedDescr d = runtest("identify");

    ASSERT_EQ("identify", d.devicename);
    ASSERT_EQ(std::nullopt, d.color);
    ASSERT_EQ(std::nullopt, d.function);
}
