# Software
## How to install the desktop software
1. Navigate to the the CricketApp folder.
2. (Optional) If this is the first time running the software on the computer run ```Install.bat```. This may take sometime to download the dependancies.


## Running the desktop Software
1. Run: ``` Run.bat ```. This should open a command promp with the server running. If this fails, repeat installs steps.
2. Open up a browser and navigate to ``` localhost:5000/ ```.
3. Files are saved in ``` CricketApp/Data/Scale```

Note:  
The scale must be running the firmware found in ``` scale/3psJavaComm ``` and the serial monitor in Arduino MUST be closed.

## Calibrating the Scale
To calibrate the scale you will need to change the firmware on the scale. The calibration will then save the settings into the EEPROM.

1. Open up Arduino
2. Go the root directory of the software and open ``` scale/calibration.ino ```. 
3. Press upload.
4. Open the Serial terminal and follow instructions ``` Tools -> Serial Monitor ```

Note:  
The server must be shut down to allow the arduino software to access the COMM ports.
If the upload button can't find the required libraries. Under the sketch menu select ``` include library -> manage libraries ``` and install the missing library that it is asking for. 
If the software states that the COMM is busy, shut down the server command prompt.
If the software can not find the correct COMM, select ``` Tools -> Port -> Arduino/Uno ```.

