#include "config-validator.hpp"
#include "grouplayout.hpp"
#include "ledlayout.hpp"

#include <gtest/gtest.h>

using namespace phosphor::led;

static void assertValidationException(const GroupMap& ledMap,
                                      error::ConfigValidationError err)
{
    try
    {
        validateConfigV1(ledMap);
        ASSERT_FALSE(true);
    }
    catch (ConfigValidationException& e)
    {
        ASSERT_EQ(e.reason, err);
    }
}

TEST(validateConfig, testGoodPathLedPriority)
{
    ActionSet group1ActionSet = {
        {"led1", Layout::Action::On, 0, 0, Layout::Action::On},
        {"led2", Layout::Action::On, 0, 0, Layout::Action::Blink},
    };
    Layout::GroupLayout group1 = {0, group1ActionSet};
    GroupMap ledMap = {{"group1", group1}};

    validateConfigV1(ledMap);
}

TEST(validateConfig, testGoodPathGroupPriority)
{
    ActionSet group1ActionSet = {
        {"led1", Layout::Action::On, 0, 0, std::nullopt},
        {"led2", Layout::Action::On, 0, 0, std::nullopt},
    };
    ActionSet group2ActionSet = {
        {"led1", Layout::Action::On, 0, 0, std::nullopt},
        {"led2", Layout::Action::On, 0, 0, std::nullopt},
    };
    Layout::GroupLayout group1 = {1, group1ActionSet};
    Layout::GroupLayout group2 = {2, group1ActionSet};
    GroupMap ledMap = {
        {"group1", group1},
        {"group2", group2},
    };

    validateConfigV1(ledMap);
}

TEST(validateConfig, testLedPriorityMismatch)
{
    ActionSet group1ActionSet = {
        {"led1", Layout::Action::On, 0, 0, Layout::Action::On},
    };
    ActionSet group2ActionSet = {
        {"led1", Layout::Action::On, 0, 0, Layout::Action::Off},
    };
    Layout::GroupLayout group1 = {0, group1ActionSet};
    Layout::GroupLayout group2 = {0, group2ActionSet};
    GroupMap ledMap = {
        {"group1", group1},
        {"group2", group2},
    };

    assertValidationException(ledMap, error::LedPriorityMismatch);
}

TEST(validateConfig, testMissingLedPriority)
{
    ActionSet group1ActionSet = {
        {"led1", Layout::Action::On, 0, 0, Layout::Action::On},
    };
    ActionSet group2ActionSet = {
        {"led1", Layout::Action::On, 0, 0, std::nullopt},
    };
    Layout::GroupLayout group1 = {0, group1ActionSet};
    Layout::GroupLayout group2 = {0, group2ActionSet};
    GroupMap ledMap = {
        {"group1", group1},
        {"group2", group2},
    };

    assertValidationException(ledMap, error::MissingLedPriority);
}

TEST(validateConfig, testMixedLedAndGroupPriority)
{
    ActionSet group1ActionSet = {
        {"led1", Layout::Action::On, 0, 0, Layout::Action::On},
    };
    ActionSet group2ActionSet = {
        {"led1", Layout::Action::On, 0, 0, Layout::Action::On},
    };
    Layout::GroupLayout group1 = {0, group1ActionSet};
    Layout::GroupLayout group2 = {1, group2ActionSet};
    GroupMap ledMap = {
        {"group1", group1},
        {"group2", group2},
    };

    assertValidationException(ledMap, error::MixedLedAndGroupPriority);
}

TEST(validateConfig, testInvalidGroupPriority)
{
    ActionSet group1ActionSet = {
        {"led1", Layout::Action::On, 0, 0, std::nullopt},
    };
    ActionSet group2ActionSet = {
        {"led1", Layout::Action::On, 0, 0, std::nullopt},
    };
    Layout::GroupLayout group1 = {0, group1ActionSet};
    Layout::GroupLayout group2 = {1, group2ActionSet};
    GroupMap ledMap = {
        {"group1", group1},
        {"group2", group2},
    };

    assertValidationException(ledMap, error::InvalidGroupPriority);
}

TEST(validateConfig, testDuplicateGroupPriority)
{
    ActionSet group1ActionSet = {
        {"led1", Layout::Action::On, 0, 0, std::nullopt},
    };
    ActionSet group2ActionSet = {
        {"led1", Layout::Action::On, 0, 0, std::nullopt},
    };
    Layout::GroupLayout group1 = {1, group1ActionSet};
    Layout::GroupLayout group2 = {1, group2ActionSet};
    GroupMap ledMap = {
        {"group1", group1},
        {"group2", group2},
    };

    assertValidationException(ledMap, error::DuplicateGroupPriority);
}
