/*  
----------------------------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  salmin36@gmail.com wrote this file.  As long as you retain this notice you
  can do whatever you want with this stuff. If we meet some day, and you think
  this stuff is worth it, you can buy me a beer in return.  Pasi Salminen  
 ----------------------------------------------------------------------------
*/
#include <SoftwareSerial.h>
#include "DHT.h"
 
//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);


#define DHTPIN 3     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

String lines_of_new_text_messages[10]; 
unsigned int message_lines = 0;
String row = "";


void setup() {
//  pinMode(2, OUTPUT);
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
   
  //Being serial communication witj Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  //Transistor on
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
  
  
  dht.begin();
}

void clear_messages(){
  for(int i = 0; i < 10; i++){
    lines_of_new_text_messages[i] = "";
  }
  message_lines=0;
}
 
void loop() {
  delay(5000);
  
  get_text();
  //delay(300000);
  delay(240000);
}


void save_into_array(String row){
  if(message_lines < 10){
    if(row.length() > 0){
      lines_of_new_text_messages[message_lines] = row;
      message_lines++;
    }
    
  }
  else{
    Serial.println("Number of lines in exceeded in array");
  }
}

void read_array(){
  Serial.println("Starting to read from array");
  row = "";
  for(unsigned int i = 0; i < message_lines; i++){
    row = lines_of_new_text_messages[i];
    Serial.println(row);
  }
  Serial.println("Stopped to read from array");
}

String parse_number_from_row(String row){
  Serial.println("Starting parse_number_from_row");
  String number = "";
  int index = row.indexOf(",");
  if(index != -1){
    index = row.indexOf(",", (index + 1));
    if(index != -1){
      int index2 = row.indexOf(",", (index + 1));
      if(index2 != -1){
        return row.substring((index + 1),(index2));
      }
    }
  }
  return "";
}


void webasto_press(){
  digitalWrite(4, HIGH);   
  delay(1000);             
  digitalWrite(4, LOW);
}

void parse_message_content_from_array(){
  Serial.println("Starting to parse message content from array");
  row = "";
  if(message_lines > 2 ){
    for(unsigned int i = 0; i < message_lines; i++){
        row = lines_of_new_text_messages[i];
        //Then we have a message
        if((row.indexOf("+CMGL") != -1) && 
           (row.indexOf("=") == -1)){
          Serial.println("Now we propebly have a new message");
          Serial.println("Row contains: " + row );
          String number = parse_number_from_row(row);
          if(i < 8){
            row = lines_of_new_text_messages[i+1];
            Serial.println("This is it +1: " + lines_of_new_text_messages[i+1]);
            Serial.println("This is it +2: " + lines_of_new_text_messages[i+2]);
            Serial.println("Got message that has the following: " + row + ", from number: " + number);
            row.toLowerCase();
            if(row.indexOf("temp") != -1){
              Serial.println("We got the TEMP so sending text message as the response");
             
              send_text(number,false);         
            }
             else if(row.indexOf("on") != -1){
              Serial.println("We got the On so sending text message as the response");
              webasto_press();
              send_text(number,true);         
            }
            
            return;
          }
        }
    }
  }
  else{
    Serial.println("Propably no messages exists");
  }
}


void get_text(){
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  wait_and_read(6000, false);
  clear_messages();
  Serial.println("Trying to get text message");
  
  Serial.println("Clearing text messages");
  clear_messages();
  
  Serial.println("Setting pin!");
  serialSIM800.write("AT\r\n");
  wait_and_read(3000, false);
  
  serialSIM800.write("AT+CPIN=1234\r\n");
  wait_and_read(30000, false);
  
  serialSIM800.write("AT+CMGF=1\r\n");
  
  wait_and_read(2000, false);
  
  
  serialSIM800.write("AT+CREG=1\r\n");
  
  
  wait_and_read(2000, false);
  
  serialSIM800.write("AT+CMGL=\"REC UNREAD\"\r\n");
  
  wait_and_read(15000, true);

  Serial.println("Stopped reading!");
  read_array();
  parse_message_content_from_array();
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}

String read_sensor_data(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return "";
  }
  
  row = "Humidity: " + String(h, 2) + " % " + " Temperature: " + String(t,2) + " *C ";
  
  return row;
}

void wait_and_read(int time, bool write_to_array){
  unsigned long start_time = millis();
  unsigned long stop_time = start_time + time ;
  row = "";
  char c;
  while(millis() < stop_time){
    if(serialSIM800.available()){
      
      c = serialSIM800.read();
      row += c;
      Serial.write(c);
      if(c == '\n'){
        if(write_to_array){
          save_into_array(row);
        }
        row = "";
      }
    } 
  }
}


void send_text(String number, bool push_on){
  if(number.length() < 13){
    Serial.println("Not valid phone number, EXITING...");
    return;
  }
  Serial.println("Phone number is: " + number);

  Serial.println("Setting pin!");
  serialSIM800.write("AT\r\n");
  delay(1000); 
  
  serialSIM800.write("AT+CPIN=1234\r\n");
  delay(5000); 
 
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(2000); 

  serialSIM800.write("AT+CREG=1\r\n");
  delay(10000); 
  
  Serial.println("Setup Complete!");
  
  
  Serial.println("Sending SMS...");
   
  //Send new SMS command and message number
  serialSIM800.print("AT+CMGS=" + number + "\r\n");
  delay(3000);
 
  String message = "";
  if(push_on){
    message += "Webasto: on \r\n";
  }
  message += read_sensor_data();
  
  //Send SMS content
  serialSIM800.print(message);
  delay(2000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);  
  delay(7000);
 
  Serial.println("SMS Sent!");  
}
