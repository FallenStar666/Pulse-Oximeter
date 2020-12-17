/*-------------------------------Arduino code for pulse oximetry-------------------------------

  [Objective]
  Develop a functional signal generator and detector on Arduino Nano with the following functions:
  1. Real-time signal sampling
  2. Waveform display in serial plotter

---------------------------------------------------------------------------------------------*/

#include <Adafruit_ILI9340.h>           // LED shield driver
#include <SPI.h>                        // Board-to-shield communication
#include <Wire.h>

#include "MAX30105.h"
#include "heartRate.h" //??what's this

//pins NO. assignment
#define CS 10
#define DC 9
#define RST 8

//==========================================================================================//
//								                1. Variable declaration  							                    //
//==========================================================================================//

// Master time control info //
int loopfreq = 100;			                              // Loop frequency (Hz)
float loopperiod = 1000000 / (2 * loopfreq);    		  // Loop period (us)

// Signal generation //
[......]                // ON/OFF of signal generation
[......]  			 		    // Signal frequency (Hz)
[......]   		          // No. of samples in a period
[......]                // Number of output channels (digital ouput)
[......]                // Loop counter

// Signal detection //
[......]                // ON/OFF of signal detection
[......]                // Input analog channel 
[......]                // Temporal input value


//==========================================================================================//
//								                2. Function declaration  							                    //
//==========================================================================================//

// serial controller
void serialcontroller(){
  Serial.begin(115200);
  Serial.println("Initializing...");
  
}
// LED emission controller
//================================== Signal generation ===================================//

  if [......] {   // ON/OFF of signal generation 

    for [......] {    // To generate two signals to two channels 
    
      // Output waveform //
      if (......) {
      	[......]    // output at digital pin #1
      }
      if (......) {
      	[......]    // output at digital pin #2
      }

      // Print output value to serial port to check waveform in serial plotter //
      [......]

    }
  }

// data reader
datareader(){
  if [......] {   // ON/OFF of signal detection
    
  // Sampling at analog channel defined above //
  [......] 
  
  // Print the reading to serial port for visualization //  
  [......] 

  }
}

// data processor

// screen controller



//==========================================================================================//
//					                  3. Main program (initialize environment)  			    	       	//
//==========================================================================================//
// Turn on serial port (baud, bit per second) //
void setup() {
  serialcontroller();


}

//==========================================================================================//
//							                   4. Main program (looping) 						                		//
//==========================================================================================//
void loop() {

  // Set variable to record start time for loop time monitoring (i.e sampling period) //
  [......]
    
  // Using counter to compute the phase of the waveform //
  [......]
    
//================================== Signal generation ===================================//
//================================== Signal detection ====================================//
  



  // Set variable to record end time for loop time monitoring (i.e sampling period) //
  [......] 

  // Set delay time equal to the difference between start and end times to control loop time //
  [......] 

}

//==========================================================================================//
//								                6. Function realization  							                    //
//==========================================================================================//



