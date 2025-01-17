#pragma once

#include <getopt.h>

#include <map>
#include <string>

namespace phosphor
{
namespace led
{
/** @brief Class - Encapsulates parsing command line options and
 *                 populating arguments
 */
class ArgumentParser
{
  public:
    ArgumentParser() = delete;
    ~ArgumentParser() = default;
    ArgumentParser(const ArgumentParser&) = delete;
    ArgumentParser& operator=(const ArgumentParser&) = delete;
    ArgumentParser(ArgumentParser&&) = default;
    ArgumentParser& operator=(ArgumentParser&&) = default;

    /** @brief Constructs Argument object
     *
     *  @param argc - the main function's argc passed as is
     *  @param argv - the main function's argv passed as is
     *  @return Object constructed
     */
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    ArgumentParser(int argc, char* argv[]);

    /** @brief Given a option, returns its argument(optarg) */
    const std::string& operator[](const std::string& opt);

    /** @brief Displays usage */
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    static void usage(char* argv[]);

  private:
    /** @brief Option to argument mapping */
    std::map<const std::string, std::string> arguments;

    /** @brief Array of struct options as needed by getopt_long */
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    static inline const option options[] = {
        {"path", required_argument, nullptr, 'p'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0},
    };

    /** @brief optstring as needed by getopt_long */
    static inline const char* const optionstr = "p:?h";
};

} // namespace led
} // namespace phosphor
