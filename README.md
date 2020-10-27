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