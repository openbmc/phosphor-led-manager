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
using namespace phosphor::led::sysfs;

TEST(DbusName, Has3Parts)
{
    LedDescr d = {"devicename", "color", "function"};

    std::string name = interface::InternalInterface::getDbusName(d);

    ASSERT_EQ("devicename_function_color", name);
}

TEST(DbusName, WithDash)
{
    LedDescr d = {"enclosure-identify", std::nullopt, std::nullopt};

    std::string name = interface::InternalInterface::getDbusName(d);

    ASSERT_EQ("enclosure_identify", name);
}

TEST(DbusName, With2Dashes)
{
    LedDescr d = {"enclosure-identify-fault", std::nullopt, std::nullopt};

    std::string name = interface::InternalInterface::getDbusName(d);

    ASSERT_EQ("enclosure_identify_fault", name);
}

TEST(DbusName, WithUnderscore)
{
    LedDescr d = {"enclosure_identify", std::nullopt, std::nullopt};

    std::string name = interface::InternalInterface::getDbusName(d);

    ASSERT_EQ("enclosure_identify", name);
}
