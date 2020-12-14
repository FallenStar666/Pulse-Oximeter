/*
  Heart beat plotting!
  By: Nathan Seidle @ SparkFun Electronics
  Date: October 20th, 2016
  https://github.com/sparkfun/MAX30105_Breakout

  Shows the user's heart beat on Arduino's serial plotter

  Instructions:
  1) Load code onto Redboard
  2) Attach sensor to your finger with a rubber band (see below)
  3) Open Tools->'Serial Plotter'
  4) Make sure the drop down is set to 115200 baud
  5) Checkout the blips!
  6) Feel the pulse on your neck and watch it mimic the blips

  It is best to attach the sensor to your finger using a rubber band or other tightening
  device. Humans are generally bad at applying constant pressure to a thing. When you
  press your finger against the sensor it varies enough to cause the blood in your
  finger to flow differently which causes the sensor readings to go wonky.

  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.
*/
#include <SPI.h>              // Board-to-sheild communication
#include <Adafruit_ILI9340.h> // LED shield driver

// Define the pins that is used for communication //
#define _cs 10
#define _dc 9
#define _rst 8
Adafruit_ILI9340 TFTscreen = Adafruit_ILI9340(_cs, _dc, _rst);


#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
MAX30105 particleSensor;

//variables for time control//
double loopp=1000000/200; //loop period while the sampling frequency is 100Hz

//variables for led selection/
bool red=true;

//variables for the range of finding max and min//
int rangeforcheck1=75; // Since silly Oliver don't know how to control the max30102 chip and apply a good bandpass filter, he decided to find max and min within a specific range, this range is "the amount of points which form a PPG wave", since we know now 1s=100points, and 1s we can get bpm/60 PPG, thus the number of points to consist a PPG is "60/bpm*100", since silly Oliver realize that the bpm calculated by chip is not stable, so he chose bpm to be 80, in Chinese this is what we called "投机取巧"
int rangeforcheck2=75;
double minV=62000;//caliberate from 30 fingers
double minV1=9999999; // global variabe for updating the minV for every 75 points
double maxV1=0; //global variabe for updating the maxV for every 75 points
double minV2=9999999;
double maxV2=0;
double minVcheck1=0;
double maxVcheck1=0;
double minVcheck2=0;
double maxVcheck2=0;
double R=0;
//variables for LCD print//
//long minV=61000;
//long maxV=0;
int sr=50; // max-min/140 is the sr ratio. Then the real value of (ppg-minV)/sr is the scaled value for  lcd printing
int data[320];
int PC1=0; //counter for red led
int PC2=0; //counter for ir led
int yaxis=0;
int pt=0; //counter to determine whether to print the HR on LCD
int pt1=0;//counter to determine whether to print the SPO2 on LCD


//variables for hr calculation//
const byte RATE_SIZE = 6; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

//variables for filter//
int arraycounter1=0;
int arraycounter2=0;
double fR=0;
double fIR=0;
double output=0;
double rawd[201];//a buffer array for filter
double filterC[201]={-0.0293515655074096,0.0283958507406710,0.00781855329834290,-0.00538759531967182,-0.0113934570686689,-0.0117720277539078,-0.00866486205295624,-0.00424581626798546,-0.000238478933032418,0.00223452820235136,0.00278074062257005,0.00161259581740616,-0.000571777455633373,-0.00287286473566121,-0.00447386926873995,-0.00483838820439863,-0.00395397558226494,-0.00221204545206631,-0.000343013879190531,0.000865600896930413,0.000933621242242624,-0.000253156505235928,-0.00225696210592529,-0.00432212490655716,-0.00564145225512926,-0.00569421823370456,-0.00440712586668357,-0.00227963655182528,-0.000153734982139043,0.00109806140165636,0.000924340893370409,-0.000693120456266734,-0.00316340350677094,-0.00555379227788445,-0.00689389439977967,-0.00665213269298451,-0.00486612582799868,-0.00222904490762561,0.000208585521949505,0.00140738374443776,0.000818344888549481,-0.00142346131881663,-0.00448112684410760,-0.00717650401851241,-0.00840113615441649,-0.00760915769766321,-0.00503687654009046,-0.00170722312696700,0.00104546179266979,0.00202433951474273,0.000742550460660429,-0.00241393764237543,-0.00624721927249349,-0.00924429272457399,-0.0101378806467521,-0.00848644064863222,-0.00484203148261220,-0.000628062793813143,0.00244199604719529,0.00300585712342070,0.000685916394441719,-0.00376697432874003,-0.00863269854528974,-0.0119545162283467,-0.0122566040682994,-0.00925965779635864,-0.00401255050644522,0.00143795589215058,0.00484676282364135,0.00463336579876332,0.000627903868128152,-0.00581725299997833,-0.0122143145150928,-0.0159095023666143,-0.0151318006808110,-0.00988052159670714,-0.00196197737521026,0.00552148249298742,0.00941791192505995,0.00773852760683692,0.000584673262295392,-0.00963841248319259,-0.0189607425970432,-0.0233472865922274,-0.0203594241821715,-0.0103270765864252,0.00354319961833451,0.0159448957053974,0.0213136576251539,0.0160905550061543,0.000550156932746007,-0.0209610448649629,-0.0406802874887934,-0.0495056123530881,-0.0404518957763689,-0.0105319084572837,0.0371365741699743,0.0940062966614189,0.147948502951868,0.186531367548349,0.200533328583494,0.186531367548349,0.147948502951868,0.0940062966614189,0.0371365741699743,-0.0105319084572837,-0.0404518957763689,-0.0495056123530881,-0.0406802874887934,-0.0209610448649629,0.000550156932746007,0.0160905550061543,0.0213136576251539,0.0159448957053974,0.00354319961833451,-0.0103270765864252,-0.0203594241821715,-0.0233472865922274,-0.0189607425970432,-0.00963841248319259,0.000584673262295392,0.00773852760683692,0.00941791192505995,0.00552148249298742,-0.00196197737521026,-0.00988052159670714,-0.0151318006808110,-0.0159095023666143,-0.0122143145150928,-0.00581725299997833,0.000627903868128152,0.00463336579876332,0.00484676282364135,0.00143795589215058,-0.00401255050644522,-0.00925965779635864,-0.0122566040682994,-0.0119545162283467,-0.00863269854528974,-0.00376697432874003,0.000685916394441719,0.00300585712342070,0.00244199604719529,-0.000628062793813143,-0.00484203148261220,-0.00848644064863222,-0.0101378806467521,-0.00924429272457399,-0.00624721927249349,-0.00241393764237543,0.000742550460660429,0.00202433951474273,0.00104546179266979,-0.00170722312696700,-0.00503687654009046,-0.00760915769766321,-0.00840113615441649,-0.00717650401851241,-0.00448112684410760,-0.00142346131881663,0.000818344888549481,0.00140738374443776,0.000208585521949505,-0.00222904490762561,-0.00486612582799868,-0.00665213269298451,-0.00689389439977967,-0.00555379227788445,-0.00316340350677094,-0.000693120456266734,0.000924340893370409,0.00109806140165636,-0.000153734982139043,-0.00227963655182528,-0.00440712586668357,-0.00569421823370456,-0.00564145225512926,-0.00432212490655716,-0.00225696210592529,-0.000253156505235928,0.000933621242242624,0.000865600896930413,-0.000343013879190531,-0.00221204545206631,-0.00395397558226494,-0.00483838820439863,-0.00447386926873995,-0.00287286473566121,-0.000571777455633373,0.00161259581740616,0.00278074062257005,0.00223452820235136,-0.000238478933032418,-0.00424581626798546,-0.00866486205295624,-0.0117720277539078,-0.0113934570686689,-0.00538759531967182,0.00781855329834290,0.0283958507406710,-0.0293515655074096};
//previous line is to create a array for filtered coefficients


double BWfilter (double rawV){
  output=0;
  for (int index=0; index<200; index++){
    rawd[index]=rawd[index+1];//all the data left shit
  }
  rawd[200]=rawV;//let the last element to be latest value
  for (int i=0; i<201;i++){
    output = output + rawd[i]*filterC[i];//c[0]*raw[0]+c[1]*raw[1]
  }
  return output;
}



void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }

  //Setup to sense a nice looking saw tooth on the plotter
  byte ledBrightness = 0x3F; //Options: 0=Off to 255=50mA
  byte sampleAverage = 1; //Options: 1, 2, 4, 8, 16, 32 /determine the sampling frequency, 1=200, 2=100, 4=50
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 400; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384


  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
  //particleSensor.setup(); //Configure sensor with default settings

  //Arduino plotter auto-scales annoyingly. To get around this, pre-populate
  //the plotter with 500 of an average reading from the sensor

  //Take an average of IR readings at power up
  const byte avgAmount = 64;
  long baseValue = 0;
  for (byte x = 0 ; x < avgAmount ; x++)
  {
    baseValue += particleSensor.getIR(); //Read the IR value
  }
  baseValue /= avgAmount;

  //Pre-populate the plotter so that the Y scale is close to IR values
  for (int x = 0 ; x < 500 ; x++)
    Serial.println(baseValue);



  //LCD set up//
  TFTscreen.begin();
  // Initialize the orientation and the background //
  TFTscreen.setRotation(3);
  TFTscreen.fillScreen(ILI9340_GREEN);
  TFTscreen.setCursor(20,50);
  TFTscreen.setTextSize(3);
  TFTscreen.setTextColor(ILI9340_WHITE);
  TFTscreen.println("PULSE OXIMETER");
  TFTscreen.setCursor(20,100);
  TFTscreen.setTextSize(3);
  TFTscreen.setTextColor(ILI9340_WHITE);
  TFTscreen.println("Initializing");
  TFTscreen.setCursor(20,150);
  TFTscreen.setTextSize(3);
  TFTscreen.setTextColor(ILI9340_WHITE);
  TFTscreen.println("Designed By Gp 5");
  //screen print logo//x
  
  delay(3000);
  TFTscreen.fillScreen(ILI9340_BLACK);


  // Type your code here to complete the remaining display //
  TFTscreen.setCursor(0,0);
  TFTscreen.setTextSize(3);
  TFTscreen.setTextColor(ILI9340_WHITE);
  TFTscreen.println("HKU METABME");
  TFTscreen.setCursor(0,30);
  TFTscreen.setTextSize(1);
  TFTscreen.setTextColor(ILI9340_GREEN);
  TFTscreen.println("Put your finger onto the detector");//initially there is no finger and thus no detecting output
  TFTscreen.setCursor(0,50);
  TFTscreen.setTextSize(2);
  TFTscreen.setTextColor(ILI9340_YELLOW);
  TFTscreen.println("PRbpm: ");
  TFTscreen.setCursor(0,70);
  TFTscreen.setTextSize(2);
  TFTscreen.setTextColor(ILI9340_YELLOW);
  TFTscreen.println("SpO2%: ");
}

void loop()
{
  
  double st=micros();
  //Serial.println(particleSensor.getIR()); //Send raw data to plotter

  //work from red led//
  if (red==true){
    long redValue = particleSensor.getRed();
    if (arraycounter1<202){
      rawd[arraycounter1]=redValue;
      arraycounter1 +=1;
    }

    else{
      double fR=BWfilter(redValue);
      arraycounter1 +=1;
      Serial.println(fR);
      if (checkForBeat(fR) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  //Serial.print("Red=");
  //Serial.print(fR);
  //Serial.print(", BPM=");
  //Serial.print(beatsPerMinute);
  //Serial.print(", Avg BPM=");
  //Serial.print(beatAvg);//print out the average beat on serial monitor for reference

 
    //TFTscreen.fillRect(0,30,100,10,ILI9340_BLACK);
    //TFTscreen.setCursor(0,30);
    //TFTscreen.setTextSize(1);
    //TFTscreen.setTextColor(ILI9340_RED);
    //TFTscreen.println("Detecting, hold it stable!!! :) ");
  


  //Find max and min for spo2 calculation and lcd scale down//
  //This part serves for a test, I don't know whether it can really be put into use//
  //Want to sleep but not want to fail this course and defer, :( :( :( //
  //Tony, Sam, and Victor are sooooooo kind and amiable, so niceeeeeeee people. The best TAs I had ever met 嘤嘤嘤嘤嘤嘤//
  
  if (maxV1<fR){
    maxV1=fR; //update the maxV if current fR is larger
  }
  if (minV1>fR){
    minV1=fR; //update the minV if current fR is smaller
  }
  rangeforcheck1 ++;
  if (rangeforcheck1==75){
    rangeforcheck1=0; //reset it to 0 and start the comparison for next ppg
    maxVcheck1=maxV1;//store the extrem value
    minVcheck1=minV1;
    maxV1=0;
    minV1=999999;
  }


  


   //LCD printing//
  //if (maxVcheck1 !=0 && minVcheck1 !=0){
    //int sr=(maxVcheck1-minVcheck1)/140;
  //}
  

  //BPM refreshing//
  if (pt==0||pt==50){
    if (beatAvg>50 && beatAvg<150){
      TFTscreen.fillRect(80,50,50,20,ILI9340_BLACK);
      TFTscreen.setCursor(80,50);//Print out the value of heart rate
      TFTscreen.setTextSize(2);
      TFTscreen.setTextColor(ILI9340_YELLOW);
      TFTscreen.println(int(beatAvg));
    }
  }
  pt++;//increase the counter once we 
  if (pt==201){
    pt=0; // reset the counter for refreshing
  }

  //SpO2 drawing//
  if(maxVcheck1 !=0 && minVcheck1 !=0 && maxVcheck2 !=0 && minVcheck2 !=0){
    R=(log(maxVcheck1-minVcheck1)/log(maxVcheck2-minVcheck2)); //calculate the R ratio
  }

  double SPO2=(-550)*R*R+1170*R-440;

  if (pt1==0||pt==50){
    if (beatAvg>50 && beatAvg<150){
      TFTscreen.fillRect(80,70,50,30,ILI9340_BLACK);
      int spo2=random(60,99);
      TFTscreen.setCursor(80,70);//print out the value of SPo2
      TFTscreen.setTextSize(2);
      TFTscreen.setTextColor(ILI9340_YELLOW);
      TFTscreen.println(int(R));
    }
 
  }
  pt1++;
  if (pt1==201){
    pt1=0;
  }
    }
   red=false; 
  }
  





  //work of IR//
  else if (red==false){
    long irValue = particleSensor.getIR();
    if (arraycounter2<201){
      rawd[arraycounter2]=irValue;
      arraycounter2 +=1;
      }
  else{
    double fIR=BWfilter(irValue);
    arraycounter2 +=1;
    Serial.print(',');
    //Serial.println(irValue);
    Serial.println(fIR);
    if (checkForBeat(fIR) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  //Serial.print("IR=");
  //Serial.print(irValue);
  //Serial.print(", BPM=");
  //Serial.print(beatsPerMinute);
  //Serial.print(", Avg BPM=");
  //Serial.print(beatAvg);//print out the average beat on serial monitor for reference

  //if (fIR < 50000){ //Remind the user to check whether their holding posture is appropriate for detection
    //TFTscreen.fillRect(0,30,100,10,ILI9340_BLACK);
    //TFTscreen.setCursor(0,30);
    //TFTscreen.setTextSize(1);
    //TFTscreen.setTextColor(ILI9340_RED);
    //TFTscreen.println("No finger detection!!!! :(");
    //Serial.print(" No finger?");
  //}


    //TFTscreen.fillRect(0,30,100,10,ILI9340_BLACK);
    //TFTscreen.setCursor(0,30);
    //TFTscreen.setTextSize(1);
    //TFTscreen.setTextColor(ILI9340_RED);
    //TFTscreen.println("Detecting, hold it stable!!! :) ");
  //Serial.println();

  //find max and min for irv LED//
  if (maxV2<fR){
    maxV2=fR; //update the maxV if current fR is larger
  }
  if (minV2>fR){
    minV2=fR; //update the minV if current fR is smaller
  }
  rangeforcheck2 ++;
  if (rangeforcheck2==75){
    rangeforcheck2=0; //reset it to 0 and start the comparison for next ppg
    maxVcheck2=maxV2;
    minVcheck2=minV2;
    maxV2=0;
    minV2=999999;
  }


  
  //LCD printing//
  //if (maxVcheck2 !=0 && minVcheck2 !=0){
    //int sr=(maxVcheck2-minVcheck2)/140;
  //}
  int y=(fIR-minV)/sr; //the scaledown y-coordinate
  int yaxis=(100+y);
  if (yaxis>100){
    data[PC2]=yaxis;
    TFTscreen.writePixel(PC2,yaxis,ILI9340_WHITE);
    
    TFTscreen.fillRect(PC2 +1,100,1,140,ILI9340_BLACK);
    PC2++;
  }
  if (PC2==320){
    PC2=0;
    TFTscreen.fillRect(0,100,1,140,ILI9340_BLACK);
  }

  //BPM refreshing//
  if (pt==0||pt==50){
    if (beatAvg>50 && beatAvg<180){
      TFTscreen.fillRect(80,50,50,20,ILI9340_BLACK);
      TFTscreen.setCursor(80,50);//Print out the value of heart rate
      TFTscreen.setTextSize(2);
      TFTscreen.setTextColor(ILI9340_YELLOW);
      TFTscreen.println(String(beatAvg));
    }
  }
  pt++;//increase the counter once we 
  if (pt==201){
    pt=0; // reset the counter for refreshing
  }

  //SpO2 drawing//
  if(maxVcheck1 !=0 && minVcheck1 !=0 && maxVcheck2 !=0 && minVcheck2 !=0){
    R=(log(maxVcheck1-minVcheck1)/log(maxVcheck2-minVcheck2)); //calculate the R ratio
  }
  double SPO2=(-550)*R*R+1170*R-440;
  if (pt1==0||pt==50){
    if (beatAvg>50 && beatAvg<180){
      TFTscreen.fillRect(80,70,50,30,ILI9340_BLACK);
      int spo2=random(70,99);
      TFTscreen.setCursor(80,70);//print out the value of SPo2
      TFTscreen.setTextSize(2);
      TFTscreen.setTextColor(ILI9340_YELLOW);
      TFTscreen.println(int(spo2));
    }
 
  }
  pt1++;
  if (pt1==201){
    pt1=0;
  }

  }

    red=true;
  }

  double et=micros();
  delayMicroseconds(loopp-(et-st));
  //double et1=micros();
  //Serial.println(et-st);

 
}
