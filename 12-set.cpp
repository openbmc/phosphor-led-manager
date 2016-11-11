#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include <set>

enum Action
{
    OFF,    //0
    ON,     // 1
    BLINK,  // 2
};

/** @brief Name of the LED and it's proposed action.
 *  This structure is supplied as configuration at build time
 */
struct LedAction
{
    std::string name;
    Action action;

    // Needed for inserting elements into sets
    bool operator<(const LedAction& right) const
    {
        if(name == right.name)
        {
            return action < right.action;
        }
        return name < right.name;
    }
};

/** @brief For finding intersection */
bool ledComp(const LedAction& left, const LedAction& right)
{
    if (left.name < right.name)
    {
        return true;
    }
    return false;
}

// Convenient declarations.
using group = std::set<LedAction>;
std::map<std::string, group> groups = {
    {   "FanA1Fault",{
            {"Fan_A1",ON},
            {"EnclosureFaultBack",ON},
            {"EnclosureFaultFront",ON},
        }
    },
    {   "FanA2Fault",{
            {"Fan_A2",ON},
            {"EnclosureFaultBack",ON},
            {"EnclosureFaultFront",ON},
        }
    },
    {   "FanA1Identify",{
            {"Fan_A1",BLINK},
            {"EnclosureIdentifyFront",ON},
            {"EnclosureIdentifyBack",ON},
        }
    },
    {   "FanA2Identify",{
            {"Fan_A2",BLINK},
            {"EnclosureIdentifyFront",ON},
            {"EnclosureIdentifyBack",ON},
        }
    },
    {   "Proc0Identify",{
            {"Proc_0",BLINK},
            {"EnclosureIdentifyFront",ON},
            {"EnclosureIdentifyBack",ON},
        }
    },
};

// This has set of LEDs telling their current state.
group Current;

// Everytime user request comes in, we add the set pointer to this
std::set<group*> assertedGroups;

void recalculate(const bool& op)
{
    // This will contain the union of what's already in the asserted group
    group Desired {};
    for(auto& g : assertedGroups)
    {
        std::set_union(g->begin(), g->end(), g->begin(), g->end(),
                       std::inserter(Desired, Desired.begin()));
    }

    std::cout <<"\n ---DESIRED_SET---\n";
    for (auto& st: Desired)
    {
        std::cout << "\t{" << st.name << "::" << st.action << "}" << std::endl;
    }

    // Always Do execute Turn Off and then Turn on since we have the Blink
    // taking priority over -on-
    group ledsToAssert {};
    group ledsToTurnOff {};
    std::set_difference(Current.begin(), Current.end(),
                        Desired.begin(), Desired.end(),
                        std::inserter(ledsToTurnOff, ledsToTurnOff.begin()));
    if(ledsToTurnOff.size())
    {
        std::cout << "Turning off LEDs" << std::endl;
        for (auto& it: ledsToTurnOff)
        {
            std::cout << "\t{" << it.name << "::" << it.action << "}"
                << std::endl;
        }

        // If we previously had a FRU in ON state , and then if there was a
        // request to make it blink, the end state would now be blink.
        // If we either turn off blink / fault, then we need to go back to its
        // previous state.
        std::set_intersection(Desired.begin(), Desired.end(),
                              ledsToTurnOff.begin(), ledsToTurnOff.end(),
                              std::inserter(ledsToAssert, ledsToAssert.begin()),
                              ledComp);

        std::cout << "Asserting LEDs again" << std::endl;
        for (auto& it: ledsToAssert)
        {
            std::cout << "\t{" << it.name << "::" << it.action << "}"
                << std::endl;
        }
    }

    // Turn on these
    ledsToAssert.clear();
    std::set_difference(Desired.begin(), Desired.end(),
                        Current.begin(), Current.end(),
                        std::inserter(ledsToAssert, ledsToAssert.begin()));

    std::cout << "Asserting LEDs" << std::endl;
    for (auto& it: ledsToAssert)
    {
        std::cout << "\t{" << it.name << "::" << it.action << "}"
            << std::endl;
    }
    Current.clear();
    Current = Desired;
}

void assertGroup(const std::string& g_name, const bool& op)
{
    // Extract the ::set from this group name
    auto req = groups[g_name];
    if (op)
    {
        assertedGroups.insert(&groups[g_name]);
    }
    else
    {
        auto search = assertedGroups.find(&groups[g_name]);
        if (search != assertedGroups.end())
        {
            assertedGroups.erase(&groups[g_name]);
        }
    }
    recalculate(op);
}

int main(void)
{
    assertGroup("FanA1Fault", true);
    assertGroup("FanA2Fault", true);
//    assertGroup("FanA1Fault", true);
    assertGroup("FanA1Identify", true);
//    assertGroup("FanA1Identify", false);
    assertGroup("FanA1Fault", false);
//    assertGroup("FanA2Identify", true);
//    assertGroup("FanA1Identify", false);
//    assertGroup("FanA2Fault", true);
//    assertGroup("FanA2Fault", false);
//    assertGroup("FanA1Identify", false);
//    assertGroup("FanA2Identify", false);
//    assertGroup("FanA1Fault", true);
//    assertGroup("FanA2Identify", false);

//    assertGroup("FanA1Identify", true);
//    assertGroup("FanA2Identify", true);
//    assertGroup("Proc0Identify", true);
//   assertGroup("FanA2Identify", false);
    // To know if the element is inside the asserted group, get std::inersect
    // and see if we get the entries same as this and if so, we are good.
    bool Asserted = assertedGroups.find(&groups["FanA2Fault"]) != assertedGroups.end();
    if (Asserted)
    {
        std::cout << " \nGroup : [ " << "FanA2Fault ]" << "Is Present \n";
    }
    else
    {
        std::cout << " \nGroup : [ " << "FanA2Fault ]" << "Is _NOT_Present \n";
    }
    return 0;
}
