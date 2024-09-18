/* !!! WARNING: This is a GENERATED Code..Please do NOT Edit !!! */

static const phosphor::led::GroupMap systemLedMap = {

   {"/xyz/openbmc_project/led/groups/bmc_booted",{ 0,
{
        {"heartbeat",phosphor::led::Layout::Action::On,50,0,phosphor::led::Layout::Action::On,},
   }}},
   {"/xyz/openbmc_project/led/groups/power_on",{ 0,
{
        {"power",phosphor::led::Layout::Action::On,50,0,phosphor::led::Layout::Action::On,},
   }}},
   {"/xyz/openbmc_project/led/groups/enclosure_fault",{ 0,
{
        {"front_fault",phosphor::led::Layout::Action::On,50,0,phosphor::led::Layout::Action::On,},
        {"rear_fault",phosphor::led::Layout::Action::On,50,0,phosphor::led::Layout::Action::On,},
   }}},
   {"/xyz/openbmc_project/led/groups/enclosure_identify",{ 0,
{
        {"front_id",phosphor::led::Layout::Action::Blink,50,1000,phosphor::led::Layout::Action::On,},
        {"rear_id",phosphor::led::Layout::Action::Blink,50,1000,phosphor::led::Layout::Action::On,},
   }}},
   {"/xyz/openbmc_project/led/groups/fan_fault",{ 0,
{
        {"front_fault",phosphor::led::Layout::Action::On,50,0,phosphor::led::Layout::Action::On,},
        {"fan0",phosphor::led::Layout::Action::On,50,0,phosphor::led::Layout::Action::On,},
        {"rear_fault",phosphor::led::Layout::Action::On,50,0,phosphor::led::Layout::Action::On,},
   }}},
   {"/xyz/openbmc_project/led/groups/fan_identify",{ 0,
{
        {"front_id",phosphor::led::Layout::Action::Blink,50,1000,phosphor::led::Layout::Action::On,},
        {"rear_id",phosphor::led::Layout::Action::Blink,50,1000,phosphor::led::Layout::Action::On,},
        {"fan0",phosphor::led::Layout::Action::Blink,50,1000,phosphor::led::Layout::Action::On,},
   }}},
};
