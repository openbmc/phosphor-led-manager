sources = [
    'group.cpp',
    'led-main.cpp',
    'manager.cpp',
    'serialize.cpp',
    '../utils.cpp',
    'config-validator.cpp',
]

if get_option('use-json').disabled()
    led_gen_hpp = custom_target(
        'led-gen.hpp',
        command: [
            prog_python,
            meson.project_source_root() + '/scripts/parse_led.py',
            '-i',
            meson.project_source_root(),
            '-o',
            meson.current_build_dir(),
        ],
        output: 'led-gen.hpp',
    )
    sources += [led_gen_hpp]
endif

if get_option('use-lamp-test').allowed()
    conf_data.set_quoted(
        'LAMP_TEST_OBJECT',
        '/xyz/openbmc_project/led/groups/lamp_test',
    )
    conf_data.set_quoted(
        'HOST_LAMP_TEST_OBJECT',
        '/xyz/openbmc_project/led/groups/host_lamp_test',
    )
    conf_data.set_quoted(
        'LAMP_TEST_LED_OVERRIDES_JSON',
        '/usr/share/phosphor-led-manager/lamp-test-led-overrides.json',
    )
    conf_data.set('LAMP_TEST_TIMEOUT_IN_SECS', 240)

    sources += ['lamptest/lamptest.cpp']
endif

executable(
    'phosphor-ledmanager',
    sources,
    include_directories: ['..'],
    dependencies: deps,
    install: true,
    install_dir: get_option('bindir'),
)
