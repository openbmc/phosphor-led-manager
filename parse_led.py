#!/usr/bin/env python
import yaml

if __name__ == '__main__':
    with open('led.yaml', 'r') as f:
        ifile = yaml.safe_load(f)

    with open('led-gen.hpp', 'w') as ofile:
        ofile.write('/* !!! WARNING: This is a GENERATED Code..')
        ofile.write('Please do NOT Edit !!! */\n\n')

        ofile.write('const std::map<std::string,')
        ofile.write(' std::set<phosphor::led::Manager::LedAction>>')
        ofile.write(' phosphor::led::Manager::cv_LedMap = {\n\n')
        for group in ifile.iterkeys():
            # Value of this group is a std::set<string, led structure>
            ledset = ifile[group]
            ofile.write('   {\"' + group + '\",{\n')

            for led_dict, list_dict in ledset.iteritems():
                for name, value in list_dict.iteritems():
                    if group and led_dict and name and value:
                        ofile.write('        {\"' + led_dict + '\",')
                        ofile.write(value.upper() + '},\n')
            ofile.write('   }},\n')
        ofile.write('};\n')

