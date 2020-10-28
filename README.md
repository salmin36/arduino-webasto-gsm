# arduino-webasto-gsm
This repository is for the project that contains gerber files and c code for system that is remotely controlling Webasto clock switch


## Arduino Shield
This project has working PCB and code for communicating between mobile phone and this component.
It is used to remotely control Webasto heater and to get the tempereature inside a car.

## PoC
Here we have actual prototype pcb:s and a photo from gerber file.

Circuit board has:
- Voltage regulator (LM10841T-5.0) with capacitors (10uF)
- Transistor (TIP41C)
- Diodes 
- Optocoupler (PC123)
- Some resistor

<img src="images/pcb-top.png" width="200" >
<img src="images/pcb-bottom.png" width="200" >
<img src="images/pcb-gsm.png" width="200" >



Optocoupler has a resistor added to the pin becouse I mistakely did not add it to the pcb. If this is not added then when you apply voltage from arduino it fries up the arduinos I/0-pin becouse it does not have a resistor to restrict the flow of current. 

<img src="images/optocoupler.png" width="50">

Communication with DHT temperature sensor happens via DHT arduino library. https://www.arduino.cc/reference/en/libraries/dht-sensor-library/


## How it works
Arduino starts up the SIM800 gsm module via transistor. Transistor is conneted via 2 diodes. This is becouse only one i/o pin driving the transistor is not giving enought controlling current for allowing enought current to flow trought SIM800 module. Using digital pins 11 & 10 and going trought diodes it allowes more controlling current to transistor that makes the transistor to provide more current to sim800 module.
<br><br>

After starting up the sim800 module we are reading all the new text messages. If there is new text messages we go trought the payloads. If the paylods contain either "temp" or "on" strings.

<br><br>
If the string is "on" then we put current trought the optocoupler via digital pin 4 for 1 second. After that we read the humidity and temperature values from digital pin 3 from DHT11 sensor.
After that we send text message to the number with <br><t>"Humidity: xx% Temperature: yy Arduino: on "
<br><br>
If the string is then we just read temperature and humidty and send message with corresponding values.

<br>
Last thing is that we are cutting of the current from SIM800 module stopping it. 



