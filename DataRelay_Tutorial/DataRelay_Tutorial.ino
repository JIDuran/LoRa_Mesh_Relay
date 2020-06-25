#include <RH_RF95.h>
#include <Adafruit_SleepyDog.h> // Watchdog is a member of this

/*Intermediate relay code
 */ Created By Jesse Duran
//Defs for feather m0
#define RFM95_CS 8 //Chip select pin
#define RFM95_RST 4 //reset pin
#define RFM95_INT 3 //Interupt request pin
#define Freq 900
#define RH_RF95_MAX_MESSAGE_LEN 20
#define buttonPin 9

RH_RF95 rf95(RFM95_CS, RFM95_INT); //LoRa intialization

int rS_LED = 5;  //recive sucessful LED
int rF_LED = 5; //recive failed LED
int butLED = 6;

int TX = 0; // used as Transmitter ID
int temp =0;
int humid=0;

int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0; 

void setup() {
  Serial.begin(9600);
  
  pinMode(RFM95_RST,OUTPUT);
  pinMode(rS_LED,OUTPUT); 
  pinMode(rF_LED,OUTPUT);
  pinMode(buttonPin,INPUT); //Reads in voltage from button press
  pinMode(butLED,OUTPUT);
  
  //Manual reset using restet pin to ensure proper intialization
  digitalWrite(RFM95_RST,HIGH);
  digitalWrite(RFM95_RST,LOW);
  delay(10);
  digitalWrite(RFM95_RST,HIGH);
  delay(10);
  
  while (!rf95.init()) { // Program will halt if initilaization of LoRa fails
      Serial.println("LoRa radio init failed");
      while(1);
  }

  Serial.print("LoRa Radio intilization succesful!");
  Serial.print("Set Freq to: "); Serial.println(Freq);
  if (!rf95.setFrequency(Freq)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  
  rf95.setTxPower(5,false); // The power of transmitter can be changed by the first int. The power ranges from 5-23 (dB).

  Serial.println("Setup complete");

}

void loop() {

  buttonState = digitalRead(buttonPin); //reads in button state
  
  if(buttonState != lastButtonState){ //If there has been a press
    
    if(buttonState==HIGH){  //if button is pressed
      Serial.println("button high");
      
      if(TX<=4){  //4 is highest ID# for this Ex.
        TX++; //Increases TXid# 
        
        for(int i=0;i<TX;i++){  //Blinks LED i times same as TXid
          digitalWrite(butLED,HIGH);
          delay(100);
          digitalWrite(butLED,LOW);
          delay(100);
          }
       }
      else{
        TX=1; //resets TX to loweset ID
      }
    } 
  }
  if (rf95.available()) //If the radio module is ready.
  {
    uint8_t MSG[RH_RF95_MAX_MESSAGE_LEN]; //stores incoming message
    uint8_t len = sizeof(MSG);
    
    if (rf95.recv(MSG, &len)) //Functinon to listen for incoming transmission
      {
        uint8_t D2R[20]; //Data to relay
        D2R[0]=MSG[0]; //Stores recieved temp data
        D2R[1]=MSG[1];  //Store recieved humidity data
        D2R[19]=TX; //Sets TXid for relay.
   
        rf95.send((uint8_t*)D2R,20);  //Sends the recived message to next relay/Reciever
        rf95.waitPacketSent();  //ensures send is not intterupted
      if (rf95.waitAvailableTimeout(3000))  //Waits for a response from the reciever
        { 
            //Check for reply from next stage
          if (rf95.recv(MSG, &len))
            {
              for(int i=0;i<2;i++){   //Double blinks recieve successful LED
                digitalWrite(rS_LED,HIGH); 
                delay(200);
                digitalWrite(rS_LED,LOW);
              }
            }
          else
          {
            digitalWrite(rF_LED,HIGH);//Blinks recieve failed LED
            delay(200);
            digitalWrite(rF_LED,LOW);
          }
        }
      else
      {
      //Blinks recieve failed LED
      digitalWrite(rF_LED,HIGH);
      delay(200);
      digitalWrite(rF_LED,LOW);
      }
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
   delay(1000);
}
