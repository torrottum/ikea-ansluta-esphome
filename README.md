# ikea-ansluta-esphome
IKEA Ansluta integration for Home Assistant using [ESPHome](https://esphome.io) with a D1 Mini and CC2500.

Based upon the work of [@NDBCK](https://github.com/NDBCK/Ansluta-Remote-Controller), [@ksjoberg](https://github.com/ksjoberg/ikea-ansluta-bridge), and [@matlen67](https://github.com/matlen67/ansluta-control).

## Hardware
- I'm using a [D1 mini](https://docs.wemos.cc/en/latest/d1/d1_mini.html), but should work with other 3.3v boards
- I ended up buying a [CC2500 breakout board](https://www.ebay.co.uk/itm/CC2500-2-4GHz-Module-on-Breakout-Board-with-0-1-Header-Pins-UK-Stock-/262966744730) since it wasn't easy soldering the CC2500 with my shaky hands and subpar soldering skills.

# TODO
- [ ] CC2500 component
- [ ] Light component
- [ ] Pairing mode (Intercept remote ID from remote, not pairing with transformer)
- [ ] Intercept commands from paired remote and set state
- [ ] Send command as remote
- [ ] Support both dimming levels? (I just care about 100% myself, but the option for doing 50% would be nice)
