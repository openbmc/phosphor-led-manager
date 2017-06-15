#!/usr/bin/env python
import yaml
import os
import argparse
from inflection import underscore

if __name__ == '__main__':
    script_dir = os.path.dirname(os.path.realpath(__file__))
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-f", "--filename",
        default='led.yaml',
        help="Input File Name")
    parser.add_argument(
        "-i", "--input-dir",
        dest='inputdir',
        default=script_dir,
        help="Input directory")
    parser.add_argument(
        '-o', '--output-dir',
        dest='outputdir',
        default='.',
        help='Output directory.')

    args = parser.parse_args()

    # Default to the one that is in the current.
    yaml_dir = script_dir
    yaml_file = os.path.join(yaml_dir, 'led.yaml')

    if args.inputdir:
        yaml_dir = args.inputdir

    if args.filename:
        yaml_file = os.path.join(yaml_dir, args.filename)

    with open(yaml_file, 'r') as f:
        ifile = yaml.safe_load(f)

    # Dictionary having [Name:Priority]
    priority_dict = {}

    with open(os.path.join(args.outputdir, 'led-gen.hpp'), 'w') as ofile:
        ofile.write('/* !!! WARNING: This is a GENERATED Code..')
        ofile.write('Please do NOT Edit !!! */\n\n')

        ofile.write('static const std::map<std::string,')
        ofile.write(' std::set<phosphor::led::Layout::LedAction>>')
        ofile.write(' systemLedMap = {\n\n')
        for group in ifile.keys():
            # This section generates an std::map of LedGroupNames to std::set
            # of LEDs containing the name and properties
            ledset = ifile[group]
            ofile.write(
                '   {\"' +
                "/xyz/openbmc_project/led/groups/" +
                underscore(group) +
                '\",{\n')

            # Some LED groups could be empty
            if not ledset:
                ofile.write('   }},\n')
                continue

            for led_dict, list_dict in ledset.items():
                value = list_dict.get('priority')
                if led_dict in priority_dict:
                    if value != priority_dict[led_dict]:
                        # Priority for a particular LED needs to stay SAME
                        # across all groups
                        ofile.close()
                        os.remove(ofile.name)
                        raise ValueError("Priority for [" +
                                         led_dict +
                                         "] is NOT same across all groups")
                else:
                    priority_dict[led_dict] = value

                ofile.write('        {\"' + underscore(led_dict) + '\",')
                ofile.write('phosphor::led::Layout::' +
                            str(list_dict.get('Action', 'Off')) + ',')
                ofile.write(str(list_dict.get('DutyOn', 50)) + ',')
                ofile.write(str(list_dict.get('Period', 0)) + ',')
                priority = str(list_dict.get('priority', 'Blink'))
                ofile.write('phosphor::led::Layout::' + priority + ',')
                ofile.write('},\n')
            ofile.write('   }},\n')
        ofile.write('};\n')
