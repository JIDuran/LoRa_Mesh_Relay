#include <SimpleDHT.h>          //DHT11 Library
#include <RH_RF95.h>            //Radio Head library
#include <Adafruit_SleepyDog.h> // Watchdog is a member of this (sleep)

/*Transmitter Tutorial code
 * Created By Jesse Duran
 */
//Defs for feather m0
#define RFM95_CS 8 //Chip select pin
#define RFM95_RST 4 //reset pin
#define RFM95_INT 3 //Interupt request pin
#define Freq 900    //frequcny set... Dependent of LoRa module
#define RH_RF95_MAX_MESSAGE_LEN 20 // Sets the max length of arrays that are sent.

#define DHT_Pin 12 //The data pin the DHT sensor is connected to  
#define rS_LED 9  //recive sucessful LED
#define rF_LED 6 //recive failed LED


SimpleDHT11 dht11; //Initilaization of the DHT11 sensor
RH_RF95 rf95(RFM95_CS, RFM95_INT); //LoRa intialization

void setup() {

  Serial.begin(9600);
  
  pinMode(RFM95_RST,OUTPUT);
  pinMode(rS_LED,OUTPUT); 
  pinMode(rF_LED,OUTPUT);
  digitalWrite(RFM95_RST,HIGH);
  
  //Manual reset using restet pin to ensure proper intialization
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
  
  rf95.setTxPower(5,false); // The power of transmitter can be changed by the first parameter(int). The power ranges from 5-23dB.
  Serial.println("Setup complete");
}

void loop() {
  byte temp ,humid = 0;
  byte data_2_Send[20] = {0}; // Array of bytes that consists of data to be sent
  dht11.read(DHT_Pin,&temp,&humid,data_2_Send); //Data collection function from the DHT
  
  data_2_Send[0]=char(temp); //Converts from byte to char. Assigns recorded temp to first byte of array.
  data_2_Send[1]=char(humid); // adds humidity to the data sent. Assigns recorded humidity to second byte of array. 
  data_2_Send[19]= 1; // [19] of the array is assigned to the TX identifier.

  Serial.print("temp ");
  Serial.println((int)data_2_Send[0]);
  
  rf95.send((uint8_t*)data_2_Send,20); // Sends the array of data as integers, "20" refers to max size of the array
  rf95.waitPacketSent(); // makes sure the send doesn't get interuppted before continuing.
  
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; // declares array 'buf' that will be used to store recieved data
  uint8_t len = sizeof(buf); 
  Serial.println("sent");
  
  if (rf95.waitAvailableTimeout(2000))  //Waits for a response from the reciever
  { 
    if (rf95.recv(buf, &len)) // recieves radio response and assignes message to 'buf'
   {
      Serial.print("succesful");
      digitalWrite(rS_LED,HIGH); //Blinks recieve successful LED
      delay(200);
      digitalWrite(rS_LED,LOW);
    }
    else
    {
     Serial.println("failed");
    //Blinks recieve failed LED
    digitalWrite(rF_LED,HIGH);
    delay(200);
    digitalWrite(rF_LED,LOW);
    }
  }
  else
  {
    Serial.println("Responce time out");
    //Blinks recieve failed LED
    digitalWrite(rF_LED,HIGH);
    delay(200);
    digitalWrite(rF_LED,LOW);
  }
  delay(2000);
  /* Sleeping the Feather if you desire
   *   rf95.sleep(); //sleeps the LoRa module itself 
   *   Watchdog.sleep(3000); //sleeps the Feather !!!this will disable serial communication if used. */
}
