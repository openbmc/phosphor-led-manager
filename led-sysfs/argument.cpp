/**
 * Copyright © 2016 IBM Corporation
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

#include "argument.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>

namespace phosphor
{
namespace led
{
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
ArgumentParser::ArgumentParser(int argc, char* argv[])
{
    int option = 0;
    while (-1 !=
           (option = getopt_long(argc, argv, optionstr, &options[0], nullptr)))
    {
        switch (option)
        {
            case '?':
            case 'h':
                usage(argv);
                exit(-1);
                break;
            case 'p':
                arguments["path"] = optarg;
                break;
        }
    }
}

const std::string& ArgumentParser::operator[](const std::string& opt)
{
    static const std::string emptyString{};

    auto i = arguments.find(opt);
    if (i == arguments.end())
    {
        return emptyString;
    }

    return i->second;
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
void ArgumentParser::usage(char* argv[])
{
    // NOLINTNEXTLINE
    std::cerr << "Usage: " << argv[0] << " [options]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "    --help               Print this menu" << std::endl;
    std::cerr << "    --path=<path>        absolute path of LED in sysfs; like";
    std::cerr << " /sys/class/leds/<name>" << std::endl;
}
} // namespace led
} // namespace phosphor
