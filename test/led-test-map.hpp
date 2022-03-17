#include "ledlayout.hpp"

static const phosphor::led::GroupMap singleLedOn = {
    {"/xyz/openbmc_project/ledmanager/groups/SingleLed",
     {
         {"One", phosphor::led::Layout::Action::On, 0, 0,
          phosphor::led::Layout::Action::Blink},
     }},
};

static const phosphor::led::GroupMap singleLedBlink = {
    {"/xyz/openbmc_project/ledmanager/groups/SingleLed",
     {
         {"One", phosphor::led::Layout::Action::Blink, 0, 0,
          phosphor::led::Layout::Action::Blink},
     }},
};

static const phosphor::led::GroupMap singleLedBlinkOverrideOn = {
    {"/xyz/openbmc_project/ledmanager/groups/SingleLed",
     {
         {"One", phosphor::led::Layout::Action::Blink, 0, 0,
          phosphor::led::Layout::Action::On},
     }},
};

static const phosphor::led::GroupMap multipleLedsOn = {
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

static const phosphor::led::GroupMap multipleLedsBlink = {
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

static const phosphor::led::GroupMap multipleLedsOnAndBlink = {
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

static const phosphor::led::GroupMap twoGroupsWithDistinctLEDsOn = {
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

static const phosphor::led::GroupMap twoGroupsWithOneComonLEDOn = {
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

static const phosphor::led::GroupMap
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

static const phosphor::led::GroupMap twoGroupsWithOneComonLEDOnPriority = {
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

static const phosphor::led::GroupMap twoGroupsWithMultiplComonLEDOn = {
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

static const phosphor::led::GroupMap
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

static const phosphor::led::GroupMap
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
