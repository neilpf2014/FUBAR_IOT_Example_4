# Control WS2812 LED with MQTT

This is the ESP8366 code for receiving commands via MQTT to control LED.  We wil be using NodeRed on a Raspberry Pi as the UI.  The code expect a hex string that contains the color tuple.  Format is RGB_HexString, Example **_RGB0xffffff_** to turn on full brightness white.

Need to install the Mqtt helper library included in this repo.  Also need the PubSubClient and FastLed libraries using the PIO library manager.