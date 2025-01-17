# phosphor-led-sysfs

This project exposes physical LEDs on dbus.

## Example

Say we have 'identify' LED in our system, which may be declared in devicetree.

```text
leds {
    compatible = "gpio-leds";

    identify {
        gpios = <&gpio ASPEED_GPIO(S, 6) GPIO_ACTIVE_HIGH>;
    };
};
```

We can add an LED via the helper program. The main service is then started via
dbus-activation.

```sh
./add-led-action --path /sys/class/leds/identify
```

which will expose following dbus objects:

```text
busctl tree xyz.openbmc_project.LED.Controller
`- /xyz
  `- /xyz/openbmc_project
    `- /xyz/openbmc_project/led
      `- /xyz/openbmc_project/led/physical
        `- /xyz/openbmc_project/led/physical/identify

busctl introspect  xyz.openbmc_project.LED.Controller /xyz/openbmc_project/led/physical/identify
NAME                                TYPE      SIGNATURE  RESULT/VALUE                             FLAGS
...
xyz.openbmc_project.Led.Physical    interface -          -                                        -
.Color                              property  s          "xyz.openbmc_project.Led.Physical.Pal... emits-change writable
.DutyOn                             property  y          50                                       emits-change writable
.Period                             property  q          1000                                     emits-change writable
.State                              property  s          "xyz.openbmc_project.Led.Physical.Act... emits-change writable
```

In the above output, the usual org.freedesktop.\* interfaces have been removed
to keep it readable.

## Example: using the dbus interface

Query the LED State

```text
busctl get-property  xyz.openbmc_project.LED.Controller \
/xyz/openbmc_project/led/physical/identify \
xyz.openbmc_project.Led.Physical State \
s "xyz.openbmc_project.Led.Physical.Action.Off"
```

Set the LED State

```text
busctl set-property  xyz.openbmc_project.LED.Controller \
/xyz/openbmc_project/led/physical/identify \
xyz.openbmc_project.Led.Physical State s \
"xyz.openbmc_project.Led.Physical.Action.Off"
```

## How to Build

```sh
meson setup build
cd build
ninja
```
