#include <RH_RF95.h>  //Radio Head, controls LoRa module
#include <LiquidCrystal.h>  //Used to control the LCD screen.
// Created By Jesse Duran
// for feather m0 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 900 // frequency
#define RecLED 13 //blink on recieve
#define RH_RF95_MAX_MESSAGE_LEN 20

RH_RF95 rf95(RFM95_CS, RFM95_INT);  //Initializes the LoRa module

LiquidCrystal lcd(6,9,10,11,12,13); //Initializes the LCD screen

int TX1 = 0; // storage for recieved data from Transmitters
int TX2 = 0;
int TX3 = 0;
int TX4 = 0;
int TXid = 0; // used to identify which reciever 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16,2);  //Sets the ratio of LCD

  pinMode(RecLED, OUTPUT);   
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

   // manual reset of LoRa module
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) { // Haults if iniitalization fails.
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("Reciever intialized");

  if (!rf95.setFrequency(RF95_FREQ)) {  //Halts if Frequency is not set scessfuly.
    lcd.print("setFrequency Failed");
    Serial.println("setFrequency failed");
    while (1);
  }
  
  Serial.print("Frequency set to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(5, false);  //Sets response transmission power
  
}

void loop() {

  if (rf95.available()) 
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; //used to store incoming message
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) //Function that recieves radio signals
    {
      TXid = int(buf[19]); //Stores transmitter's ID.
      switch((int)buf[19]){ //Stores data from appropiate TX.

        case 1: // '1' refers to TX identifier, not first case
       
            TX1 = buf[0];
            TX1 = TX1*1.8+32; //Conversion from deg C to F
                
         
          break;
          
        case 2:
            TX2 = buf[0];
            TX2 = TX2*1.8+32;
        
          break;
          
        case 3:
            TX3 = buf[0];
            TX3 = TX3*1.8+32;        
          break;
          
        case 4:   
            TX4 = buf[0];
            TX4 = TX4*1.8+32;
         
          break;
          
        default: //if none of the identifers match switch breaks
        break;
      }
      
    digitalWrite(RecLED, HIGH);
    delay(100);
    digitalWrite(RecLED,LOW);
    lcd.clear();  //Clears previous LCD display
    //prints recieved data to the LCD.
    lcd.setCursor(0,0);
    lcd.print("TX1 ");
    lcd.print(TX1);
    lcd.print(" TX2 ");
    lcd.print(TX2);
    lcd.setCursor(0,1); //Moves text to second row
    lcd.print("TX3 ");
    lcd.print(TX3);
    lcd.print(" TX4 ");
    lcd.print(TX4);
  
    // Send a reply
    uint8_t data[] = "Data recieved"; //An array of characters that make up a respone string.
    //rf95.send(data, sizeof(data)); //Sends response
    //rf95.waitPacketSent(); //Prevents intereupted send.
  
    //Printing recieved data to the serial monitor to later be stored to a text file.
    Serial.print(TX1);
    Serial.print(",");
    Serial.print(TX2);
    Serial.print(",");
    Serial.print(TX3);
    Serial.print(",");
    Serial.print(TX4);
    Serial.print(",");
    Serial.println(TXid);
 

  }
  else
  {
    lcd.clear();
    lcd.print("Receive failed");
  }
}
}
