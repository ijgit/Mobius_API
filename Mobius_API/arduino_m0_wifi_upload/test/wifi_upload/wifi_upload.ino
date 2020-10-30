#include <SPI.h>
#include <WiFi101.h>
#include <string.h>


// connect to WiFi
char ssid[] = "SVR-150N";      // your network SSID (name)
char pass[] = "";             // your network password (use for WPA, or use as key for WEP)
WiFiClient client;


// Mobius server 
int status = WL_IDLE_STATUS;
IPAddress server(203, 253, 128, 161);   // server ip address


// json data format
// cin json data format
String cin_format_1 = "{\"m2m:cin\": {\"con\": [";
String cin_format_2 = "]}}";
int init_data_len = cin_format_1.length() + cin_format_2.length();
int senging_data_len = 0; // length of pir_sensing data



// pir sensing data

// data length 
// const int arr_len = 600;    // 1 min (60 * 10 - 10hz)
const int arr_len = 100;  // debug

// sensing data 
char **pir_mat;
float *pir_log_time_arr;
float pir_log_time = 0;
int pir_log_idx = -1;


unsigned long pir_sensing_timer = 0; 
unsigned long pir_sensing_interval = 100;  // sensing every 100 milliseconds (10hz)

// analog pin num
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
  
    WiFi.setPins(8,7,4,2);
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        while (true);
    }
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid); 
        status = WiFi.begin(ssid);  // don't need wifi password
        //status = WiFi.begin(ssid, pass);  // need wifi password

        // wait 10 seconds for connection:
        delay(10000);
    }
    Serial.println("Connected to wifi");
    printWiFiStatus();

    // assign memory 
    // pir_mat = (char **) malloc(sizeof(short*)*arr_len);
    pir_mat = (char **) malloc(sizeof(char*)*arr_len);
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
  }
  // get http response message from server
  if(client.available()) {
      char c = client.read();
      Serial.write(c);
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
            if(pir_log_idx > -1){ 
                Serial.println(pir_log_idx);     // debug
                uploadAndInit();
            }else{
                client.stop();
            }
            Serial.println("quit senging");
            sensing_flag = 0;
        }
    }
}

void postMobius(){
    client.stop();


    if(pir_log_idx != arr_len-1){
        int len = strlen(pir_mat[pir_log_idx]);
        pir_mat[pir_log_idx][len-1] = 0;
        senging_data_len = senging_data_len-1;
        
        Serial.print(pir_mat[pir_log_idx]);
    }
    
    int data_len = init_data_len + (senging_data_len);
    
    if(client.connect(server, 7579)){
        Serial.println("connected to server");

        // request line        
        // change ae and cnt name here
        client.println("POST /Mobius/sch20171518/pir HTTP/1.1");

        // header
        client.println("Host: 203.253.128.161:7579");
        client.println("Accept: application/json");
        client.println("X-M2M-RI: 12345");
        client.println("X-M2M-Origin:SOrigin");
        client.print("Content-Length: ");
        client.println(data_len);
        client.println("Content-Type: application/vnd.onem2m-res+json; ty=4");
        client.println();

        // write data
        client.print(cin_format_1);
        for(int i =0; i<pir_log_idx+1; i++){
            client.print(pir_mat[i]);
        }

        
        client.print(cin_format_2);
        client.println();

        Serial.println("sended post request to Mobius");
        senging_data_len = 0;
    }
}


char pir_temp[4][8];

void pirSensing(){
    if(millis() - pir_sensing_timer > pir_sensing_interval){
        pir_sensing_timer = millis();
        pir_log_idx ++;
        pir_log_time += 0.1;
        pir_log_time_arr[pir_log_idx] = pir_log_time;
    
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
    
        Serial.println(pir_log_idx);
        if(pir_log_idx >= arr_len-1){
            Serial.println("post req");
            uploadAndInit();
        }
    }
}

void uploadAndInit(){
    postMobius();
    pir_log_idx = -1;
    initCharArr();
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
