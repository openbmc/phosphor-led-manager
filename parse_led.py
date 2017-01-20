#!/usr/bin/env python
import yaml
import os

if __name__ == '__main__':
    script_dir = os.path.dirname(os.path.realpath(__file__))
    with open(os.path.join(script_dir, 'led.yaml'), 'r') as f:
        ifile = yaml.safe_load(f)

    with open(os.path.join(script_dir, 'led-gen.hpp'), 'w') as ofile:
        ofile.write('/* !!! WARNING: This is a GENERATED Code..')
        ofile.write('Please do NOT Edit !!! */\n\n')

        ofile.write('static const std::map<std::string,')
        ofile.write(' std::set<phosphor::led::Layout::LedAction>>')
        ofile.write(' systemLedMap = {\n\n')
        for group in ifile.iterkeys():
            # This section generates an std::map of LedGroupNames to std::set
            # of LEDs containing the name and properties
            ledset = ifile[group]
            ofile.write('   {\"' + "/xyz/openbmc_project/ledmanager/groups/" + group + '\",{\n')

            for led_dict, list_dict in ledset.iteritems():
                # Need this to make sure the LED name is printed once
                name_printed = False
                for name, value in list_dict.iteritems():
                    if group and led_dict and name:
                        if name_printed is False:
                            ofile.write('        {\"' + led_dict + '\",')
                            name_printed = True
                        if name == 'Action':
                            ofile.write('phosphor::led::Layout::')
                        ofile.write(str(value) + ',')
                ofile.write('},\n')
            ofile.write('   }},\n')
        ofile.write('};\n')

