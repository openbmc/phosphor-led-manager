#!/bin/sh

# This shell script sets operational status to false for guarded FRUs.
# It is being done only for DIMM and PROCs, as guard is done for dimm, proc and cores
# only and cores do not have LED and so are skipped.
# The script is required to establish the LED state of guarded DIMMs and PROCs in case
# it got cleared in the boot process.

# Skip running this script if the chassis is powered ON.
current_chassis_status=$(busctl get-property xyz.openbmc_project.State.Chassis \
        /xyz/openbmc_project/state/chassis0 xyz.openbmc_project.State.Chassis \
    CurrentPowerState | cut -d" " -f2)

if [ "${current_chassis_status}" = \
"\"xyz.openbmc_project.State.Chassis.PowerState.On\"" ]; then
    echo "Current chassis power state is , $current_chassis_status . \
    Exit set-guarded-fru-leds.sh \
    script successfully without setting operational status for guarded FRUs."
    exit 0
fi

#Extract list of FRUs that has been guarded.
busctl call xyz.openbmc_project.ObjectMapper \
    /xyz/openbmc_project/object_mapper \
    xyz.openbmc_project.ObjectMapper GetSubTreePaths sias \
    "/xyz/openbmc_project/hardware_isolation/entry" 0 1 \
    "xyz.openbmc_project.Association.Definitions" \
    | sed  's/ /\n/g' | tail -n+3 | awk -F "\"" '{print $2}' | \
    while read -r line
do
    busctl get-property org.open_power.HardwareIsolation "$line" \
        xyz.openbmc_project.Association.Definitions Associations \
        | sed  's/ /\n/g' | tail -n+3 | awk -F "\"" '{print $2}' | \
        while read -r line2
    do
        #check for guarded dimms and update its operational status.
        echo "$line2" | grep "dimm" >/dev/null
        rc=$?
        if [ $rc -eq 0 ]; then
            busctl set-property xyz.openbmc_project.Inventory.Manager \
                "$line2" xyz.openbmc_project.State.Decorator.OperationalStatus \
                Functional b false;
        fi

        #check for guarded processors and update its operational status.
        echo "$line2" | grep "cpu" >/dev/null
        rc=$?
        if [ $rc -eq 0 ]; then
            echo "$line2" | grep "unit\|core" >/dev/null
            rc=$?
            if [ $rc -ne 0 ]; then
                busctl set-property xyz.openbmc_project.Inventory.Manager \
                    "$line2" xyz.openbmc_project.State.Decorator.OperationalStatus \
                    Functional b false;
            fi
        fi
    done
done

#return success
exit 0
