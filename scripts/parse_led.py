#!/usr/bin/env python3
import argparse
import os

import yaml
from inflection import underscore


def check_led_priority(led_name, value, priority_dict):

    if led_name in priority_dict:
        if value != priority_dict[led_name]:
            # Priority for a particular LED needs to stay SAME
            # across all groups
            ofile.close()
            os.remove(ofile.name)
            raise ValueError(
                "Priority for [" + led_name + "] is NOT same across all groups"
            )
    else:
        priority_dict[led_name] = value

    return 0


def generate_file_single_led(ifile, led_name, list_dict, priority_dict, ofile):

    value = list_dict.get("Priority")

    check_led_priority(led_name, value, priority_dict)

    action = "phosphor::led::Layout::Action::" + str(
        list_dict.get("Action", "Off")
    )
    dutyOn = str(list_dict.get("DutyOn", 50))
    period = str(list_dict.get("Period", 0))
    priority = "phosphor::led::Layout::Action::" + str(
        list_dict.get("Priority", "Blink")
    )

    ofile.write('        {"' + underscore(led_name) + '",')
    ofile.write(action + ",")
    ofile.write(dutyOn + ",")
    ofile.write(period + ",")
    ofile.write(priority + ",")

    ofile.write("},\n")

    return 0


def generate_file_single_group(ifile, group, priority_dict, ofile):
    # This section generates an std::unordered_map of LedGroupNames to
    # std::set of LEDs containing the name and properties
    led_dict = ifile[group]

    group_priority = 0
    has_group_priority = "Priority" in led_dict

    if has_group_priority:
        group_priority = led_dict["Priority"]
        # we do not want to enumerate this as a led group
        del led_dict["Priority"]

    ofile.write(
        '   {"'
        + "/xyz/openbmc_project/led/groups/"
        + underscore(group)
        + '"'
        + ",{ "
        + str(group_priority)
        + ",\n"
        + "{\n"
    )

    for led_name, list_dict in list(led_dict.items()):
        generate_file_single_led(
            ifile, led_name, list_dict, priority_dict, ofile
        )

    ofile.write("   }}},\n")

    return 0


def generate_file(ifile, ofile):
    # Dictionary having [Name:Priority]
    priority_dict = {}

    ofile.write("/* !!! WARNING: This is a GENERATED Code..")
    ofile.write("Please do NOT Edit !!! */\n\n")

    ofile.write("static const phosphor::led::GroupMap")
    ofile.write(" systemLedMap = {\n\n")

    for group in list(ifile.keys()):
        generate_file_single_group(ifile, group, priority_dict, ofile)
    ofile.write("};\n")

    return 0


if __name__ == "__main__":
    script_dir = os.path.dirname(os.path.realpath(__file__))
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-f", "--filename", default="led.yaml", help="Input File Name"
    )
    parser.add_argument(
        "-i",
        "--input-dir",
        dest="inputdir",
        default=script_dir,
        help="Input directory",
    )
    parser.add_argument(
        "-o",
        "--output-dir",
        dest="outputdir",
        default=".",
        help="Output directory.",
    )

    args = parser.parse_args()

    # Default to the one that is in the current.
    yaml_dir = script_dir
    yaml_file = os.path.join(yaml_dir, "led.yaml")

    if args.inputdir:
        yaml_dir = args.inputdir

    if args.filename:
        yaml_file = os.path.join(yaml_dir, args.filename)

    with open(yaml_file, "r") as f:
        ifile = yaml.safe_load(f)

    with open(os.path.join(args.outputdir, "led-gen.hpp"), "w") as ofile:
        generate_file(ifile, ofile)
