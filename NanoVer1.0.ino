/*-------------------------------Arduino code for pulse oximetry-------------------------------

  [Objective]
  Develop a functional signal generator and detector on Arduino Nano with the following functions:
  1. Real-time signal sampling
  2. Waveform display in serial plotter

---------------------------------------------------------------------------------------------*/

//==========================================================================================//
//								                1. Variable declaration  							                    //
//==========================================================================================//

// Master time control info //
[......]			 		      // Loop frequency (Hz)
[......];    		        // Loop period (us)

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
//					                  2. Main program (initialize environment)  			    	       	//
//==========================================================================================//
void setup() {

  // Turn on serial port (baud, bit per second) //
  [......]

  // Set analog read resolution (bit) //
  [......]

}

//==========================================================================================//
//							                   3. Main program (looping) 						                		//
//==========================================================================================//
void loop() {

  // Set variable to record start time for loop time monitoring (i.e sampling period) //
  [......]
    
  // Using counter to compute the phase of the waveform //
  [......]
    
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

  //================================== Signal detection ===================================//
  
  if [......] {   // ON/OFF of signal detection
    
    // Sampling at analog channel defined above //
    [......] 
  
    // Print the reading to serial port for visualization //  
    [......] 

  }


  // Set variable to record end time for loop time monitoring (i.e sampling period) //
  [......] 

  // Set delay time equal to the difference between start and end times to control loop time //
  [......] 

}
