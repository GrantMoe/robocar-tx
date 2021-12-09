# BLE Robocar Controller

I'm building an autonomous RC car to participate in diy [DIYRobocar](https://diyrobocars.com/) events.

My 2-channel RC radio wasn't cutting it, so I decided to make my own BLE controller.


### Components

* [Adafruit Feather nRF52 Bluefruit LE - nRF52832](https://www.adafruit.com/product/3406)
* [Adafruit Mini Color TFT with Joystick FeatherWing](https://www.adafruit.com/product/3321)


### Todo
- [x] analog input for steering, throttle, trim pots, switches, etc
- [x] TFT/seesaw joystick and button input 
- [x] approximate Arduino's Serial dislpay via TFT screen
- [x] transmit control inputs to car
- [x] implement other car parameters (end stops, center point, max throttle, etc.) in software
- [ ] TFT menu to manipulate the above parameters
- [ ] send paramater changes to the car
- [ ] get state from car 


### tbd
* proper refresh rate
