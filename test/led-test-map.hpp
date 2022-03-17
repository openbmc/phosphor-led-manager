#include "ledlayout.hpp"

#include <set>
#include <string>
#include <unordered_map>

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    singleLedOn = {
        {"/xyz/openbmc_project/ledmanager/groups/SingleLed",
         {
             {"One", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    singleLedBlink = {
        {"/xyz/openbmc_project/ledmanager/groups/SingleLed",
         {
             {"One", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::Blink},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    singleLedBlinkOverrideOn = {
        {"/xyz/openbmc_project/ledmanager/groups/SingleLed",
         {
             {"One", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    multipleLedsOn = {
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLeds",
         {
             {"One", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Two", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    multipleLedsBlink = {
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLeds",
         {
             {"One", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::Blink},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    multipleLedsOnAndBlink = {
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsMix",
         {
             {"One", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Two", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Four", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Five", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    twoGroupsWithDistinctLEDsOn = {
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet",
         {
             {"One", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Two", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
         }},
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet",
         {
             {"Four", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Five", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Six", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    twoGroupsWithOneComonLEDOn = {
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet",
         {
             {"One", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Two", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet",
         {
             {"Four", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Six", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    twoGroupsWithOneComonLEDOnOneLEDBlinkPriority = {
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet",
         {
             {"One", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Two", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::Blink},
         }},
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet",
         {
             {"Four", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Six", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    twoGroupsWithOneComonLEDOnPriority = {
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet",
         {
             {"One", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Two", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet",
         {
             {"Four", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Six", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    twoGroupsWithMultiplComonLEDOn = {
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet",
         {
             {"One", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Two", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet",
         {
             {"Two", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Six", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Seven", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    twoGroupsWithMultipleComonLEDInDifferentState = {
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet",
         {
             {"One", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Four", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet",
         {
             {"Two", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Five", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Six", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
         }},
};

static const std::unordered_map<std::string,
                                std::set<phosphor::led::Layout::LedAction>>
    twoGroupsWithMultipleComonLEDInDifferentStateDiffPriority = {
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsASet",
         {
             {"One", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Two", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Four", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Ten", phosphor::led::Layout::Action::Blink, 0, 0,
              phosphor::led::Layout::Action::Blink},
         }},
        {"/xyz/openbmc_project/ledmanager/groups/MultipleLedsBSet",
         {
             {"Two", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Three", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
             {"Five", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Six", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::On},
             {"Ten", phosphor::led::Layout::Action::On, 0, 0,
              phosphor::led::Layout::Action::Blink},
         }},
};
