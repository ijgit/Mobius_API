#include <SPI.h>
#include <WiFi101.h>
#include <SPI.h>
#include <SD.h>
#include <string.h>

// save to sd card
const int chip_select = 10;
File datalog;

int file_idx = 0;
String file_name = "1.csv";

// connect to WiFi
#define SECRET_SSID "IoT416"
#define SECRET_PASS "iot10041004"

char ssid[] = "IoT416";      // your network SSID (name)
char pass[] = "iot10041004";    // your network password (use for WPA, or use as key for WEP)
WiFiClient client;


// Mobius server 
int status = WL_IDLE_STATUS;
IPAddress server(203, 253, 128, 161); 
String AE = "ubi_jeong";



// json data format
String data_format1 = "{\"m2m:cin\": {\"con\": [";
// example
//int data1_len = dataFormat1.length();
String data_format2 = "]}}";
int init_data_len = data_format1.length() + data_format2.length();

int senging_data_len = 0; // length of pir_sensing data



// pir sensing data
// data memory

// change to string for save and post
const int arr_len = 600;

// origin data
char **pir_mat;

float *pir_log_time_arr;
float pir_log_time = 0;
int pir_log_idx = -1;


unsigned long pir_sensing_timer = 0; 
unsigned long pir_sensing_interval = 100;  // sensing every 100 milliseconds (10hz)
unsigned long pir_log_timer = 0;
unsigned long pir_log_interval = 6*1000;  // save to sd and post to mobius every 1 min 

const int pir0 = A0;
const int pir1 = A1;
const int pir2 = A2;
const int pir3 = A3;


// user input through serial transmission
char input;
int sensing_flag = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
/*
  if (!SD.begin(chip_select)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
*/

  // IP Address: 192.168.1.1
  
  WiFi.setPins(8,7,4,2);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();

  // assign memory 


  pir_mat = (char **) malloc(sizeof(short*)*arr_len);
  for(int i =0; i<arr_len; i++){
    pir_mat[i] = (char*)malloc(sizeof(char)*24);
    pir_mat[i][0] = 0;
  }
}

void initCharArr(){
  for(int i =0; i<arr_len; i++){
    pir_mat[i][0] = 0;
  }
}


void loop() {
  userInput();
  
  if(sensing_flag == 1){
    pirSensing();
    if(client.available()) {
      char c = client.read();
      Serial.write(c);
    }
  }
}

void userInput(){
  if(Serial.available()){
    input = Serial.read();
    if(input == '1'){            // start pir sensing
      //client.connect(server, 7579);
      sensing_flag = 1;
      Serial.println("start senging");
      
    }else if(input == '2'){      // stop pir sensing
      client.stop();
    
      if(pir_log_idx > -1){ 
        saveAndPost();
      }
      Serial.println("quit senging");
      sensing_flag = 0;
      
    }else if(input == '3'){      // change file name
      file_idx++;
      file_name = String(file_idx) + ".csv";
      Serial.println("change file name");
      Serial.print("file idx: ");
      Serial.println(file_idx);
      
      
    }
  }
}

void postMobius(){
  client.stop();
  String data;
  int i;
  int data_len = init_data_len + (senging_data_len);
  if(client.connect(server, 7579)){
    Serial.println("connected to server");
    
    client.println("POST /Mobius/ubi_jeong/PIR HTTP/1.1");
    client.println("Host: 203.253.128.161:7579");
    client.println("Accept: application/json");
    client.println("X-M2M-RI: 12345");
    client.println("X-M2M-Origin:Subi_jeong");
    client.print("Content-Length: ");
    client.println(data_len);
    client.println("Content-Type: application/vnd.onem2m-res+json; ty=4");
    client.println();

    client.print(data_format1);
    for(i =0; i<pir_log_idx+1; i++){
      client.print(pir_mat[i]);
    }
    client.print(data_format2);
    client.println();

    
    Serial.println("sended post request to Mobius");
    senging_data_len = 0;
  }
}

void saveFile(){
  ;
}

void saveAndPost(){
  saveFile();
  postMobius();
  pir_log_idx = -1;
}

char pir_temp[4][8];

void pirSensing(){
  if(millis() - pir_sensing_timer > pir_sensing_interval){
    pir_sensing_timer = millis();
    pir_log_idx ++;
    pir_log_time += 0.1;
    pir_log_time_arr[pir_log_idx] = pir_log_time;

    // sensing
//    String var = String(pir_log_time) + "," +  String(analogRead(pir0))+ "," + 
//                    String(analogRead(pir1)) + "," + String(analogRead(pir2)) + "," + String(analogRead(pir3));
   
    sprintf(pir_temp[0], "\"%d,", analogRead(pir0));
    sprintf(pir_temp[1], "%d,", analogRead(pir1));
    sprintf(pir_temp[2], "%d,", analogRead(pir2));
    sprintf(pir_temp[3], "%d\"\0", analogRead(pir3));

    for(int i = 0; i<4; i++){
      strcat(pir_mat[pir_log_idx], pir_temp[i]);
    }
    if(pir_log_idx < arr_len-1)
      strcat(pir_mat[pir_log_idx], ",\0");

    Serial.println(pir_mat[pir_log_idx]);

    senging_data_len += strlen(pir_mat[pir_log_idx]);
   
    // gen data len for Content-length
//    pir_arr[pir_log_idx] = var;
//    senging_data_len += var.length() + 3; // add  {},
    Serial.println(pir_log_idx);
    if(pir_log_idx >= arr_len-1){
      Serial.println("save and post");
      saveAndPost();
      pir_log_idx = -1;
      initCharArr();
    }
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
