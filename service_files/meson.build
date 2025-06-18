systemd = dependency('systemd')
systemd_system_unit_dir = systemd.get_variable(pkgconfig: 'systemdsystemunitdir')

foreach svc : [
    'obmc-fru-fault-monitor.service',
    'obmc-led-group-start@.service',
    'obmc-led-group-stop@.service',
    'xyz.openbmc_project.LED.GroupManager.service',
]
    install_data(svc, install_dir: systemd_system_unit_dir)
endforeach
