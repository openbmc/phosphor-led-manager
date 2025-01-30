# phosphor-led-manager

This project manages LED groups on dbus. Sometimes many LEDs must be driven
together to indicate some system state.

For example, there can be multiple identify LEDs. When the user wants to
identify the system, they should all light up together.

## Configuration

The configuration can happen via json.

### Configuration: LED Priority

Each LED can have "Priority" as "Blink", "Off" or "On". If this property is
defined, it should be defined on each instance of the LED in the config.

When multiple LED groups are asserted and contain the same LED, "Priority"
determines the state of the LED.

For example, Group 1 says LED1 should be "Blink", and Group 2 says it should be
"On". LED1 will then have the state declared in "Priority".

## Configuration: LED Group Priority

Using LED Priority is fine for simple configurations, but when group state needs
to always be consistent, Group Priority can be used to enforce the consistent
representation.

The Group `Priority` is optional and a higher priority means that when 2 groups
are asserted, the one with highest `Priority` will be represented consistently.
Meaning all its LEDs will have the state as per the configuration.

## Configuration Example with Group Priorities (JSON)

Here we prioritize the locating group above the fault group since locating may
be required to fix the fault.

So independent of the order that these groups are asserted, if both are
asserted, "sys_id" should be in "Blink" state.

The "unrelated" group will have the default group priority of 0.

```json
{
  "leds": [
    {
      "group": "enclosure_identify",
      "Priority": 2,
      "members": [
        {
          "Name": "sys_id",
          "Action": "Blink"
        },
        {
          "Name": "rear_id",
          "Action": "Blink"
        }
      ]
    },
    {
      "group": "fault",
      "Priority": 1,
      "members": [
        {
          "Name": "sys_id",
          "Action": "On"
        },
        {
          "Name": "fault",
          "Action": "On"
        }
      ]
    },
    {
      "group": "unrelated",
      "members": [
        {
          "Name": "rear_id",
          "Action": "On"
        }
      ]
    }
  ]
}
```

### Configuration Example (JSON)

This is our configuration file. It describes 2 LEDs for the
'_enclosure_identify_' group, with their respective states and duty cycles.

```json
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

```text
~# ./phosphor-led-manager --config example.json
```

## Dbus interface

When starting the program, our LED group shows up on dbus. Usually there will be
many more groups.

```text
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

```text
$ busctl set-property \
xyz.openbmc_project.LED.GroupManager \
/xyz/openbmc_project/led/groups/enclosure_identify \
xyz.openbmc_project.Led.Group Asserted b true
```

The program can then use the _xyz.openbmc_project.Led.Physical_ dbus interface
exposed by _phosphor-led-sysfs_ to set each LED state.

## How to Build

```text
meson setup build
cd build
ninja
```
