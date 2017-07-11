## OpenHAB module based on ESP8266 and homie 2.0 framework ##

http://www.hoeser-medien.de/2017/07/homie-2-0-with-openhab-on-esp8266-using-mqtt/
 

ESP8266 is a great device whenever you need a small and cheap module to be connected to the WiFi network. It is cheaper than an arduino with an additional LAN or WLAN module attached to it.

My first attempt for a lightshow was using an Arduino nano + nRF24. The second approach was based on an ESP8266. Nevertheless implementing a fail safe WiFI connection is sort of a pain.

For the time being, I will move on using the homie framework, as it does handle all the background work and provide an MQTT interface to communicate with OpenHAB.

It took quite some time to get homie 2.0 up and running, as the framework won’t work when using Eclipse  (Sloeber from bayens). It works when using platformIO which also does allow to upload the configuration in config.json to SPIFFS.

ESP8266

The module is providing two functions: one port will switch a door lock when receiving a message via MQTT or when the button is pressed. In addition the module will periodically send the temperature to the MQTT server.

OpenHAB

homie.items

This was the hardest piece as some requirements are not that obvious. I was able to see all the messages on the MQTT server via MQTT fx but nothing happened in openHAB. The key to success was to use the Log Viewer. You will see the error messages there and get a glimpse where to start hunting down the bugs.


The status of the switch „homie_switch“ is managed by a rule rather than reading the inbound message from MQTT. This was the only way the switch would change the sate when the button is pressed on the device.


myHome.sitemap

This does work,  but not as expected. My preferences would be to use sub-frames. The only way to display a sub-page was using a group. Unfortunately you have no control of the order the items are shown within the group.


homie.rules

This works, but I failed using „case“ or „if“ to comine both cases within one rule.
