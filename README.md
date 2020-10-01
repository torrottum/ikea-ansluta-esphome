# ikea-ansluta-esphome
IKEA Ansluta integration for Home Assistant using [ESPHome](https://esphome.io) with a D1 Mini and CC2500.

Based upon the work of [NDBCK](https://github.com/NDBCK/Ansluta-Remote-Controller), [ksjoberg](https://github.com/ksjoberg/ikea-ansluta-bridge), and [matlen67](https://github.com/matlen67/ansluta-control).

## Hardware
- I'm using a [D1 mini](https://docs.wemos.cc/en/latest/d1/d1_mini.html), but should work with other 3.3v boards
- I ended up buying a [CC2500 breakout board](https://www.ebay.co.uk/itm/CC2500-2-4GHz-Module-on-Breakout-Board-with-0-1-Header-Pins-UK-Stock-/262966744730) since it wasn't easy soldering the CC2500 with my shaky hands and subpar soldering skills.

## How to use
1. [Create your esphome project](https://esphome.io/guides/getting_started_command_line.html#creating-a-project)
1. Download the [latest release](https://github.com/torrottum/ikea-ansluta-esphome/releases)
1. If you don't have it already, create a custom_components directory
1. Copy `custom_components/ikea_ansluta` into your `custom_components/`
1. Add the `spi` and `ikea_ansluta` components. For example (for the pins, you need to know the SPI pins for your esp):
    ```yaml
      spi:
        clk_pin: GPIO14
        miso_pin: GPIO12
        mosi_pin: GPIO13

      ikea_ansluta:
        cs_pin: GPIO15
    ```
1. Upload your configuration to your esp
1. View the logs and press your remote and you'll find the remote address for your light
1. Add the configuration for your light(s):
    ```yaml
      light:
        - platform: ikea_ansluta
          name: 'IKEA Ansluta'
          remote_address: <your address>
    ```
    **NOTE:** I'm not sure how well this works with multiple remotes/paired to multiple lights. I only have one remote, so I have no way of testing it. 
1. Upload your configuration again
1. Your light should now be in Home Assistant

There is an [example here](example/) that you can look at.

## Upgrading from 0.1.x to 1.0.0
1. Follow the how to use above, step 1-5
1. Note the name and entity id and remote address for the old MQTT light in Home Assistant
1. Set the name in your esphome configuration. For example, if your name was `Ansluta Light` and the remote address was `0x35c0`:
    ```yaml
    light:
      - platform: ikea_ansluta
        name: 'IKEA Ansluta '
        remote_address: 0x35c0
    ```
1. Remove the MQTT light config from Home Assistant
1. Upload your configuration to your esp
1. If you need to, change the entity id of the light to the old id in the Home Assistant UI.
