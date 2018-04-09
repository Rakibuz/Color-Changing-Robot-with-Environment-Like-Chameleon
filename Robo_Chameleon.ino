
#include <SoftwareSerial.h>
 
#define DEBUG true
 
//SoftwareSerial esp8266(15, 14); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3
//always high
int CH_PD_8266 = 53;
//first code marge start
int redPin=22; // red ping
int greenPin=26; // green pin
int bluePin=30; // blue pin
int enA = 5;
int enB = 6;
int in1 = 42;
int in2 = 40;
int in3 = 38;
int in4 = 36;

int S2=2; // Color sensore pin S2 to Arduino pin 2
int S3=3; // Color sensor pin S3 to Arduino pin 3
int outPin=4; // color Sensor OUT to Arduino pin 4

int rColorStrength;
int gColorStrength;
int bColorStrength;

unsigned int pulseWidth;

void setup()
{
  //Serial.begin(115200);
  //esp8266.begin(115200); // your esp's baud rate might be different

  Serial.begin(115200);
  Serial3.begin(115200);
  
  pinMode(42,OUTPUT);
  digitalWrite(42,LOW);
  
  pinMode(40,OUTPUT);
  digitalWrite(40,LOW);
  
  pinMode(38,OUTPUT);
  digitalWrite(38,LOW);
  
  pinMode(36,OUTPUT);
  digitalWrite(36,LOW);
  
  pinMode (enA, OUTPUT);
  analogWrite (enA, 120);

  pinMode (enB, OUTPUT);
  analogWrite (enB, 120);
    
  
Serial.begin (9600); //Turn on serial port

pinMode(redPin, OUTPUT);
pinMode(greenPin, OUTPUT);
pinMode(bluePin, OUTPUT);

pinMode(S2, OUTPUT);
pinMode(S3, OUTPUT);
pinMode(outPin, INPUT);
pinMode (enA, OUTPUT);
pinMode (enB, OUTPUT);
pinMode (in1, OUTPUT);
pinMode (in2, OUTPUT);
pinMode (in3, OUTPUT);
pinMode (in4, OUTPUT);

  pinMode(CH_PD_8266, OUTPUT);
  digitalWrite(CH_PD_8266, HIGH);
   
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
}

void goForward(){
  digitalWrite(42,HIGH);
  digitalWrite(40,LOW);
  digitalWrite(38,HIGH);
  digitalWrite(36,LOW);
}
void goBackward(){
  digitalWrite(42,LOW);
  digitalWrite(40,HIGH);
  digitalWrite(38,LOW);
  digitalWrite(36,HIGH);
 
}

void goLeft(){
  digitalWrite(42,LOW);
  digitalWrite(40,LOW);
  digitalWrite(38,HIGH);
  digitalWrite(36,LOW);
}
void goRight(){
  digitalWrite(42,HIGH);
  digitalWrite(40,LOW);
  digitalWrite(38,LOW);
  digitalWrite(36,LOW);
}

void stopCar(){
  digitalWrite(42,LOW);
  digitalWrite(40,LOW);
  digitalWrite(38,LOW);
  digitalWrite(36,LOW);
}
void loop()
{
  if(Serial3.available()) // check if the esp is sending a message 
  {
 
    
    if(Serial3.find("+IPD,"))
    {
     delay(1000); // wait for the serial buffer to fill up (read all the serial data)
     // get the connection id so that we can then disconnect
     int connectionId = Serial3.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
          
     Serial3.find("pin="); // advance cursor to "pin="
     
     int pinNumber = (Serial3.read()-48)*10; // get first number i.e. if the pin 13 then the 1st number is 1, then multiply to get 10
     pinNumber += (Serial3.read()-48); // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
     
     //digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin    

     if(pinNumber == 11){
         goForward();
        
     }
      if(pinNumber == 9){
        //goBackward();
       goRight();

       
     }
     if(pinNumber == 12){
        stopCar();
      //  goBackward();
     }
     if(pinNumber == 13){
        //goRight();
        goBackward();
     }
      if(pinNumber == 10){
        goLeft();
       
     }
     // make close command
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     sendData(closeCommand,1000,DEBUG); // close connection
    }
  }

  //color sensor code start from here
  
  // S2 and S3 should be set LOW

digitalWrite(S2,LOW);
digitalWrite(S3,LOW);

pulseWidth = pulseIn(outPin,LOW);

rColorStrength = pulseWidth/400. -1;

rColorStrength = (255- rColorStrength);

// Lets read green component of the color
// S2 and S3 should be set LOW

digitalWrite (S2,HIGH);
digitalWrite (S3,HIGH);

pulseWidth = pulseIn(outPin,LOW);

gColorStrength = pulseWidth/400. -1;

gColorStrength = (255- gColorStrength);

// Lets read blue component of the color
// S2 and S3 should be set LOW and HIGH Repectively

digitalWrite(S2,LOW);
digitalWrite(S3,HIGH);

pulseWidth = pulseIn(outPin,LOW);

bColorStrength = pulseWidth/400. -1;

bColorStrength = (255- bColorStrength);

Serial.print(rColorStrength);
Serial.print(", ");
Serial.print(gColorStrength);
Serial.print(", ");
Serial.println(bColorStrength);
Serial.println(" ");
if(rColorStrength>bColorStrength && rColorStrength>gColorStrength){
  digitalWrite(redPin,LOW);
  digitalWrite(bluePin,HIGH);
  digitalWrite(greenPin,HIGH);
}
if(gColorStrength>bColorStrength && gColorStrength>rColorStrength){
  digitalWrite(redPin,HIGH);
  digitalWrite(bluePin,HIGH);
  digitalWrite(greenPin,LOW);
}
if(bColorStrength>gColorStrength&& bColorStrength>rColorStrength){
  digitalWrite(redPin,HIGH);
  digitalWrite(bluePin,LOW);
  digitalWrite(greenPin,HIGH);
}


}//void loop  ennds from here
 
/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    Serial3.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(Serial3.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = Serial3.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    //Serial.print("na");
    
    return response;
}
