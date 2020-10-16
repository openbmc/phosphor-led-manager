#include "utils.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace phosphor::led::utils;

class MockdBusHandler : public DBusHandler
{
  public:
    MOCK_METHOD(std::string, getService, (const char*, const char*),
                (const override));

    MOCK_METHOD(PropertyValue, getProperty,
                (const char*, const char*, const char*), (const override));

    MOCK_METHOD(void, setProperty,
                (const std::string&, const std::string&, const std::string&,
                 const PropertyValue&),
                (const override));
};
