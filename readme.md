# PSA VAN mileage reader

### What is it ?

This code uses my [PSA VAN CAN bridge hardware from this repository][psavancanbridgehw] to read the mileage from the odometer from cars with VAN bus. It uses the same command which is used by the BSI to read the mileage at startup.

### Why ?

I wanted to replace my odometer in my car with a different one. But unfortunately there is no safe way to know what mileage does the replacement unit has. And if it has a higher value than what the BSI stores then the value in the BSI gets overwritten. If it has a lower value than the BSI has then we are good, as the odometer will adjusts itself to the higher value, which means I will see the same value as my old odometer had.

### Usage

You need to connect the DATA and DATAB pins on the VAN CAN bridge hardware to the appropiate pins of your odometer (it is different for each car model, so I can't help with that info). You should look for the numbers on the wiring loom of your car. DATAB is marked with number 9005 and DATA is marked with number 9004

### Displaying the value on the serial console

![odometer_mileage](https://github.com/morcibacsi/PSAVanCanBridge/raw/master/images/odometer_mileage.png)

### Building the project

#### From Arduino
Follow these steps to build the project:
 - Install the ESP32 boards into the Arduino IDE (follow the [instructions here][install_esp32])
 - Install the libraries from the Used libraries section
	 - They should be installed under your documents folder. Which should be something like this: 
		 - C:\Users\YOUR_NAME\Documents\Arduino\libraries
	 - At the end you should have a folder structure similar to this:
		 - C:\Users\YOUR_NAME\Documents\Arduino\libraries\esp32_arduino_rmt_van_rx\
		 - C:\Users\YOUR_NAME\Documents\Arduino\libraries\tss463_van\
 - Extract the contents of the zip file
 - Open the empty **read_van_mileage\read_van_mileage.ino** file from the Arduino IDE *(do not rename any file or whatsoever)*
 - Select ESP32 Dev module from Tools\Board menu
 - Now you should be able to compile it by clicking on the menu Sketch\Verify/Compile

#### From PlatformIO
You can also open the project from PlatformIO. It will download the necessary libraries so you don't have to worry about them.

### Used libraries

- [Arduino abstract serial][lib_abstract_serial] (you don't need to install this one as the required files are included in the src/SerialPort folder)
- [ESP32 RMT peripheral VAN bus reader][lib_esp32_van_rx] (can be installed from the library manager from the Arduino IDE)
- [TSS463C VAN interface library][lib_tss463c_van] (can be installed from the library manager from the Arduino IDE)


[lib_abstract_serial]: https://github.com/computergeek125/arduino-abstract-serial
[lib_tss463c_van]: https://github.com/morcibacsi/arduino_tss463_van
[lib_esp32_van_rx]: https://github.com/morcibacsi/esp32_rmt_van_rx
[psavancanbridgehw]: https://github.com/morcibacsi/PSAVanCanBridgeHW
[install_esp32]: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions
