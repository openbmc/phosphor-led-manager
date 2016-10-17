#!/usr/bin/python -u
import yaml

with open('led.yaml', 'r') as f:
    ifile = yaml.safe_load(f)

with open('led-gen.hpp', 'w') as ofile:
    ofile.write('/* !!! WARNING: This is a GENERATED Code..')
    ofile.write('Please do NOT Edit !!! */\n\n')

    ofile.write('std::map<std::string, std::set<LedManager::LedAction>>')
    ofile.write(' LedManager::cv_ledMap = {\n\n')
    for group in ifile.iterkeys():
        # Value of this group is a std::set<string, led structure>
        ledset = ifile[group]
        ofile.write ('   {\"'  + group + '\",{\n')

        for led_dict in ledset:
            list_dict =  ledset[led_dict]

            for led in list_dict.keys():
                name = led
                value = list_dict[led];

            if group and led_dict and name and value:
                ofile.write ('        {\"' + led_dict + '\",')
                ofile.write ((value.upper() + '},\n')
        ofile.write('   }},\n')
    ofile.write('};\n')
