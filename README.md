# phosphor-led-manager

This project manages LED groups on dbus. Sometimes many LEDs must be driven
together to indicate some system state.

For example, there can be multiple identify LEDs. When the user wants to
identify the system, they should all light up together.

## Configuration

The configuration can happen via json or yaml.

## Configuration Example (JSON)

This is our configuration file. It describes 2 LEDs for the
'_enclosure_identify_' group, with their respective states and duty cycles.

```
$ cat example.json
{
  "leds": [
    {
      "group": "enclosure_identify",
      "members": [
        {
          "Name": "pca955x_front_sys_id0",
          "Action": "On",
          "DutyOn": 50,
          "Period": 0,
          "Priority": "Blink"
        },
        {
          "Name": "led_rear_enc_id0",
          "Action": "On",
          "DutyOn": 50,
          "Period": 0,
          "Priority": "Blink"
        }
      ]
    }
  ]
}
```

Then start the program with

```
$ ./phosphor-led-manager --config example.json
```

## Dbus interface

When starting the program, our LED group shows up on dbus. Usually there will be
many more groups.

```
$ busctl tree xyz.openbmc_project.LED.GroupManager
`- /xyz
  `- /xyz/openbmc_project
    `- /xyz/openbmc_project/led
      `- /xyz/openbmc_project/led/groups
        `- /xyz/openbmc_project/led/groups/enclosure_identify


$ busctl introspect xyz.openbmc_project.LED.GroupManager /xyz/openbmc_project/led/groups/enclosure_identify
NAME                                TYPE      SIGNATURE RESULT/VALUE FLAGS
...
xyz.openbmc_project.Led.Group       interface -         -            -
.Asserted                           property  b         false        emits-change writable
```

In the above output, the usual org.freedesktop.\* interfaces have been removed
to keep it readable.

We can now drive the entire group by setting it's 'Asserted' property on dbus.

```
$ busctl set-property \
xyz.openbmc_project.LED.GroupManager \
/xyz/openbmc_project/led/groups/enclosure_identify \
xyz.openbmc_project.Led.Group Asserted b true
```

The program can then use the _xyz.openbmc_project.Led.Physical_ dbus interface
exposed by _phosphor-led-sysfs_ to set each LED state.

## How to Build

```
meson setup build
cd build
ninja
```
