#!/bin/bash

# This shell script sets all the group D-Bus objects
# in /xyz/openbmc_project/led/groups/ to true or false.
# If the group is in excluded list, then, they are not
# altered

function usage()
{
    echo "led-set-all-groups-asserted.sh [true/false] [optional groups to be excluded]"
    echo "Example: led-set-all-groups-asserted.sh false bmc_booted power_on"
    return 0;
}

# We need at least 1 argument
if [ $# -lt 1 ]; then
    echo "Atleast ONE argument needed";
    usage;
    exit -1;
fi

# User passed in argument [true/false]
action=$1

# If it is not "true" or "false", exit
if [ $action != "true" ] && [ $action != "false" ]; then
    echo "Bad argument $action passed";
    usage;
    exit -1;
fi

# Get the excluded groups, where $@ is all the agruments passed
index=2;
excluded_groups=""

for arg in "$@"
do
   excluded_groups="${excluded_groups}$arg|"
   let "index+=1"
done

# Remove the last | from exclude_list
length=${#excluded_groups}
if [ $length -gt 0 ]
then
    endindex=$(expr $length - 1)
    excluded_groups=${excluded_groups:0:$endindex}
fi

# Now, set the LED group to what has been requested
if [ ${#excluded_groups} -eq 0 ]
then
    for line in $(busctl tree xyz.openbmc_project.LED.GroupManager | grep -e groups/ | awk -F 'xyz' '{print "/xyz" $2}');
    do
        busctl set-property xyz.openbmc_project.LED.GroupManager $line xyz.openbmc_project.Led.Group Asserted b $action;
    done
else
    for line in $(busctl tree xyz.openbmc_project.LED.GroupManager | grep -e groups/ | grep -Ev $excluded_groups | awk -F 'xyz' '{print "/xyz" $2}');
    do
        busctl set-property xyz.openbmc_project.LED.GroupManager $line xyz.openbmc_project.Led.Group Asserted b $action;
    done
fi

# Return Success
exit 0
