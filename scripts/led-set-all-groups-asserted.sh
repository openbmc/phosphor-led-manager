#!/bin/bash

# This shell script sets all the group D-Bus objects
# /xyz/openbmc_project/led/groups/ to true or false.

function usage()
{
    echo "set-all-group.sh [true/false]"
    return 0;
}

# User passed in argument
action=$1

#If no argument passed, then exit
if [ $# -eq 0 ]; then
    echo "No arguments passed";
    usage;
    exit -1;
fi

# If it is not "true" or "false", exit
if [ $action != "true" ] && [ $action != "false" ]; then
    echo "Bad argument $action passed";
    usage;
    exit -1;
fi

# Now, set the LED group to what has been requedsted
for line in $(busctl tree xyz.openbmc_project.LED.GroupManager | grep -e groups/ | grep -v bmc_booted | grep -v power_on | awk -F 'xyz' '{print "/xyz" $2}');
do
    busctl set-property xyz.openbmc_project.LED.GroupManager $line xyz.openbmc_project.Led.Group Asserted b $action;
done

#Return Success
exit 0
