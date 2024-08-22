#include "ledlayout.hpp"
#include "manager.hpp"

#include <sdbusplus/bus.hpp>
#include <xyz/openbmc_project/Led/Physical/server.hpp>

#include <set>

#include <gtest/gtest.h>

using namespace phosphor::led;
class LedTest : public ::testing::Test
{
  public:
    sdbusplus::bus_t bus;
    LedTest() : bus(sdbusplus::bus::new_default())
    {
        // Nothing here
    }
    ~LedTest() = default;
};

static void assertMap(std::map<LedName, Layout::LedAction> map,
                      std::map<LedName, Layout::Action> expect)
{
    EXPECT_EQ(map.size(), expect.size());
    for (auto& [key, expect_value] : expect)
    {
        ASSERT_TRUE(map.contains(key));
        EXPECT_EQ(expect_value, map[key].action);
    }
}

static const phosphor::led::GroupMap groups1 = {
    {"/xyz/openbmc_project/ledmanager/groups/groupA",
     {0,
      {
          {"led1", Layout::Action::On, 0, 0, std::nullopt},
          {"led2", Layout::Action::On, 0, 0, std::nullopt},
      }}},
    {"/xyz/openbmc_project/ledmanager/groups/groupB",
     {0,
      {
          {"led3", Layout::Action::On, 0, 0, std::nullopt},
          {"led4", Layout::Action::On, 0, 0, std::nullopt},
      }}},
};

/** @brief Assert one group*/
TEST_F(LedTest, assertOneGroup)
{
    Manager manager(bus, groups1);

    std::set<const Layout::GroupLayout*> assertedGroups;

    Layout::GroupLayout mygroup =
        groups1.at("/xyz/openbmc_project/ledmanager/groups/groupA");

    assertedGroups.insert(&mygroup);

    std::map<LedName, Layout::LedAction> map =
        manager.getNewMap(assertedGroups);

    assertMap(map, {
                       {"led1", Layout::Action::On},
                       {"led2", Layout::Action::On},
                   });
}

static const phosphor::led::GroupMap groups2 = {
    {"/xyz/openbmc_project/ledmanager/groups/groupA",
     {0,
      {
          {"led1", Layout::Action::On, 0, 0, std::nullopt},
          {"led2", Layout::Action::On, 0, 0, std::nullopt},
      }}},
    {"/xyz/openbmc_project/ledmanager/groups/groupB",
     {2,
      {
          {"led2", Layout::Action::Off, 0, 0, std::nullopt},
          {"led3", Layout::Action::On, 0, 0, std::nullopt},
      }}},
    {"/xyz/openbmc_project/ledmanager/groups/groupC",
     {1,
      {
          {"led3", Layout::Action::Blink, 0, 0, std::nullopt},
          {"led4", Layout::Action::Blink, 0, 0, std::nullopt},
      }}},
};

/** @brief Assert multiple groups which overwrite each other*/
TEST_F(LedTest, assertMultipleGroups)
{
    Manager manager(bus, groups2);

    std::set<const Layout::GroupLayout*> assertedGroups;

    Layout::GroupLayout groupA =
        groups2.at("/xyz/openbmc_project/ledmanager/groups/groupA");
    Layout::GroupLayout groupB =
        groups2.at("/xyz/openbmc_project/ledmanager/groups/groupB");
    Layout::GroupLayout groupC =
        groups2.at("/xyz/openbmc_project/ledmanager/groups/groupC");

    assertedGroups.insert(&groupA);
    assertedGroups.insert(&groupB);
    assertedGroups.insert(&groupC);

    std::map<LedName, Layout::LedAction> map =
        manager.getNewMap(assertedGroups);

    assertMap(map, {
                       {"led1", Layout::Action::On},
                       {"led2", Layout::Action::Off},
                       {"led3", Layout::Action::On},
                       {"led4", Layout::Action::Blink},
                   });
}

TEST_F(LedTest, test_OCP_Panel_Indicator_6_1_System_Power_Status)
{
    const int dutyon = 50; // Spec says 50% duty cycle
    const int period = 1;  // Spec says 1Hz

    // Example from OCP Panel Indicator Specification rev 1.0, Section 6.1
    // "System Power Control / Status"
    // The group priorities here are chosen arbitrarily, assuming that locating
    // the hw has highest priority
    const std::string groupOffServiceAction =
        "/xyz/openbmc_project/ledmanager/groups/SysOffServiceAction";
    const std::string groupSysOnOk =
        "/xyz/openbmc_project/ledmanager/groups/SysOnOK";
    const std::string groupSysOffFault =
        "/xyz/openbmc_project/ledmanager/groups/SysOffFault";
    const std::string groupSysOnLocate =
        "/xyz/openbmc_project/ledmanager/groups/SysOnLocate";
    const std::string groupSysOffLocate =
        "/xyz/openbmc_project/ledmanager/groups/SysOffLocate";
    const std::string groupSysOnFault =
        "/xyz/openbmc_project/ledmanager/groups/SysOnFault";

    const std::string pwr = "pwr";
    const std::string fault = "fault";

    static const phosphor::led::GroupMap groups_ocp_6_1_power_control = {
        {groupOffServiceAction,
         {2,
          {
              {pwr, Layout::Action::Off, 0, 0, std::nullopt},
              {fault, Layout::Action::Off, 0, 0, std::nullopt},
          }}},
        {groupSysOnOk,
         {3,
          {
              {pwr, Layout::Action::On, 0, 0, std::nullopt},
              {fault, Layout::Action::Off, 0, 0, std::nullopt},
          }}},
        {groupSysOffFault,
         {38,
          {
              {pwr, Layout::Action::Off, 0, 0, std::nullopt},
              {fault, Layout::Action::On, 0, 0, std::nullopt},
          }}},
        {groupSysOnLocate,
         {99,
          {
              {pwr, Layout::Action::On, 0, 0, std::nullopt},
              {fault, Layout::Action::Blink, dutyon, period, std::nullopt},
          }}},
        {groupSysOffLocate,
         {98,
          {
              {pwr, Layout::Action::Off, 0, 0, std::nullopt},
              {fault, Layout::Action::Blink, dutyon, period, std::nullopt},
          }}},
        {groupSysOnFault,
         {39,
          {
              {pwr, Layout::Action::On, 0, 0, std::nullopt},
              {fault, Layout::Action::On, 0, 0, std::nullopt},
          }}},
    };

    const phosphor::led::GroupMap* groups = &groups_ocp_6_1_power_control;

    Manager manager(bus, *groups);

    std::set<const Layout::GroupLayout*> assertedGroups;

    // Off Service Action
    assertedGroups.insert(&groups->at(groupOffServiceAction));

    assertMap(manager.getNewMap(assertedGroups),
              {
                  {pwr, Layout::Action::Off},
                  {fault, Layout::Action::Off},
              });

    // On Ok
    assertedGroups.insert(&groups->at(groupSysOnOk));
    assertMap(manager.getNewMap(assertedGroups),
              {
                  {pwr, Layout::Action::On},
                  {fault, Layout::Action::Off},
              });

    // Off Fault
    assertedGroups.insert(&groups->at(groupSysOffFault));
    assertMap(manager.getNewMap(assertedGroups),
              {
                  {pwr, Layout::Action::Off},
                  {fault, Layout::Action::On},
              });

    // Off Fault
    assertedGroups.insert(&groups->at(groupSysOffFault));
    assertMap(manager.getNewMap(assertedGroups),
              {
                  {pwr, Layout::Action::Off},
                  {fault, Layout::Action::On},
              });

    // On Fault
    assertedGroups.insert(&groups->at(groupSysOnFault));
    assertMap(manager.getNewMap(assertedGroups),
              {
                  {pwr, Layout::Action::On},
                  {fault, Layout::Action::On},
              });

    // Off Locate
    assertedGroups.insert(&groups->at(groupSysOffLocate));
    assertMap(manager.getNewMap(assertedGroups),
              {
                  {pwr, Layout::Action::Off},
                  {fault, Layout::Action::Blink},
              });

    // On Locate
    assertedGroups.insert(&groups->at(groupSysOnLocate));
    assertMap(manager.getNewMap(assertedGroups),
              {
                  {pwr, Layout::Action::On},
                  {fault, Layout::Action::Blink},
              });
}

TEST_F(LedTest, test_OCP_Panel_Indicator_6_5_BBU_status)
{
    // Example from OCP Panel Indicator Specification rev 1.0, Section 6.5
    // "BBU Status"
    // The group priorities here are chosen arbitrarily, assuming that locating
    // the hw has highest priority
    const std::string gBBUSleep =
        "/xyz/openbmc_project/ledmanager/groups/BBUSleep";
    const std::string gBBUOn = "/xyz/openbmc_project/ledmanager/groups/BBUOn";
    const std::string gBBUFault =
        "/xyz/openbmc_project/ledmanager/groups/BBUFault";
    const std::string gBBUUnderVolt =
        "/xyz/openbmc_project/ledmanager/groups/BBUUnderVolt";
    const std::string gBBUEOL = "/xyz/openbmc_project/ledmanager/groups/BBUEOL";
    const std::string gBBUOffLocate =
        "/xyz/openbmc_project/ledmanager/groups/BBUOffLocate";
    const std::string gBBUOnLocate =
        "/xyz/openbmc_project/ledmanager/groups/BBUOnLocate";

    const std::string bbu_ok = "bbu_ok";
    const std::string bbu_fault = "bbu_fault";
    const std::string bbu_lowv = "bbu_lowv";
    const std::string bbu_eol = "bbu_eol";

    static const phosphor::led::GroupMap groups_ocp_6_5_bbu_status = {
        {gBBUSleep,
         {9,
          {
              {bbu_ok, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_fault, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_lowv, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_eol, Layout::Action::Off, 0, 0, std::nullopt},
          }}},
        {gBBUOn,
         {10,
          {
              {bbu_ok, Layout::Action::On, 0, 0, std::nullopt},
              {bbu_fault, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_lowv, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_eol, Layout::Action::Off, 0, 0, std::nullopt},
          }}},
        {gBBUFault,
         {38,
          {
              {bbu_ok, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_fault, Layout::Action::On, 0, 0, std::nullopt},
              {bbu_lowv, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_eol, Layout::Action::Off, 0, 0, std::nullopt},
          }}},
        {gBBUUnderVolt,
         {39,
          {
              {bbu_ok, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_fault, Layout::Action::On, 0, 0, std::nullopt},
              {bbu_lowv, Layout::Action::On, 0, 0, std::nullopt},
              {bbu_eol, Layout::Action::Off, 0, 0, std::nullopt},
          }}},
        {gBBUEOL,
         {40,
          {
              {bbu_ok, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_fault, Layout::Action::On, 0, 0, std::nullopt},
              {bbu_lowv, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_eol, Layout::Action::On, 0, 0, std::nullopt},
          }}},
        {gBBUOffLocate,
         {98,
          {
              {bbu_ok, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_fault, Layout::Action::Blink, 0, 0, std::nullopt},
              {bbu_lowv, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_eol, Layout::Action::Off, 0, 0, std::nullopt},
          }}},
        {gBBUOnLocate,
         {99,
          {
              {bbu_ok, Layout::Action::On, 0, 0, std::nullopt},
              {bbu_fault, Layout::Action::Blink, 0, 0, std::nullopt},
              {bbu_lowv, Layout::Action::Off, 0, 0, std::nullopt},
              {bbu_eol, Layout::Action::Off, 0, 0, std::nullopt},
          }}},
    };

    const phosphor::led::GroupMap* groups = &groups_ocp_6_5_bbu_status;

    Manager manager(bus, *groups);

    std::set<const Layout::GroupLayout*> assertedGroups;

    // Sleep
    assertedGroups.insert(&groups->at(gBBUSleep));
    assertMap(manager.getNewMap(assertedGroups),
              {
                  {bbu_ok, Layout::Action::Off},
                  {bbu_fault, Layout::Action::Off},
                  {bbu_lowv, Layout::Action::Off},
                  {bbu_eol, Layout::Action::Off},
              });

    // Fault
    assertedGroups.insert(&groups->at(gBBUFault));
    assertMap(manager.getNewMap(assertedGroups),
              {
                  {bbu_ok, Layout::Action::Off},
                  {bbu_fault, Layout::Action::On},
                  {bbu_lowv, Layout::Action::Off},
                  {bbu_eol, Layout::Action::Off},
              });

    // EOL
    assertedGroups.insert(&groups->at(gBBUEOL));
    assertMap(manager.getNewMap(assertedGroups),
              {
                  {bbu_ok, Layout::Action::Off},
                  {bbu_fault, Layout::Action::On},
                  {bbu_lowv, Layout::Action::Off},
                  {bbu_eol, Layout::Action::On},
              });

    // On + Locate
    assertedGroups.insert(&groups->at(gBBUOnLocate));
    assertMap(manager.getNewMap(assertedGroups),
              {
                  {bbu_ok, Layout::Action::On},
                  {bbu_fault, Layout::Action::Blink},
                  {bbu_lowv, Layout::Action::Off},
                  {bbu_eol, Layout::Action::Off},
              });
}
