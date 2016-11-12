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

        ofile.write('const std::map<std::string,')
        ofile.write(' std::set<phosphor::led::Group::LedAction>>')
        ofile.write(' phosphor::led::Group::ledMap = {\n\n')
        for group in ifile.iterkeys():
            # Value of this group is a std::set<string, led structure>
            ledset = ifile[group]
            ofile.write('   {\"' + "/xyz/openbmc_project/ledmanager/groups/" + group + '\",{\n')

            for led_dict, list_dict in ledset.iteritems():
                for name, value in list_dict.iteritems():
                    if group and led_dict and name and value:
                        ofile.write('        {\"' + led_dict + '\",')
                        ofile.write(value.upper() + '},\n')
            ofile.write('   }},\n')
        ofile.write('};\n')

