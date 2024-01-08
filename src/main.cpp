//#define RECEIVER_1 //(Orgel)
//#define RECEIVER_2 //Wand
//#define RECEIVER_3 //Board
#define SENDERALL
//#define SENDER1
//#define SENDER2
//#define SENDER3
//#define DEBUG

#if defined SENDER1 || defined SENDER2 || defined SENDER3 || defined SENDERALL
#define DEBUG
#define SCREEN
#endif

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ezButton.h>

#ifdef SCREEN
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif

#include "../include/variable.cpp"
#include "../include/functions.cpp"
#ifdef SCREEN
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  Serial.begin(BAUD_RATE, SER_PARAMS, RX_PIN, TX_PIN);

  Serial.print("Mac Address set, Starting.....");
  if (esp_wifi_set_channel(WIFI_CHAN, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
    #ifdef DEBUG
    Serial.println("Error changing WiFi channel");
    #endif
  }

#if defined RECEIVER_1 || defined RECEIVER_2 || defined RECEIVER_3
  if (esp_wifi_set_channel(WIFI_CHAN, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
    #ifdef DEBUG
    Serial.println("Error changing WiFi channel");
    #endif
    return;
  }

if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("ESP-NOW initialized");
  }
    
#endif

  #ifdef RECEIVER_1
  memcpy(sender1.peer_addr, Sender1_Address, 6);
  sender1.channel = 13;  
  sender1.encrypt = false;
  if (esp_now_add_peer(&sender1) != ESP_OK){
    Serial.println("Failed to add Sender2");
    return;
  }
  
  #endif

  #ifdef RECEIVER_2
  memcpy(sender2.peer_addr, Sender2_Address, 6);
  sender2.channel = 13;  
  sender2.encrypt = false;
  if (esp_now_add_peer(&sender2) != ESP_OK){
    Serial.println("Failed to add Sender2");
    return;
  }
  #endif
//Also dies ist eine Seite Din A4 voll mit Text in Libre Office, Version komma pi.  
  #ifdef RECEIVER_3
  memcpy(sender3.peer_addr, Sender3_Address, 6);
  sender3.channel = 13;  
  sender3.encrypt = false;
  if (esp_now_add_peer(&sender3) != ESP_OK){
    Serial.println("Failed to add Sender3");
    return;
  }
  
  #endif

  #if defined SENDER1 || defined SENDER2 || defined SENDER3 || defined SENDERALL
  button1.setDebounceTime(50); // set debounce time to 50 milliseconds
  button2.setDebounceTime(50); // set debounce time to 50 milliseconds
  button3.setDebounceTime(50); // set debounce time to 50 milliseconds
  button4.setDebounceTime(50); // set debounce time to 50 milliseconds
  button5.setDebounceTime(50); // set debounce time to 50 milliseconds
  
  if (esp_wifi_set_channel(WIFI_CHAN, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
    #ifdef DEBUG
    Serial.println("Error changing WiFi channel");
    #endif
    return;
  }


  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  #endif

  #ifdef SENDER1
  memcpy(orgel.peer_addr, Orgel_Address, 6);
  orgel.channel = 13;  
  orgel.encrypt = false;
  if (esp_now_add_peer(&orgel) != ESP_OK){
    Serial.println("Failed to add Orgel");
    return;
  }
  #endif
  
  #ifdef SENDER2
  memcpy(wand.peer_addr, Wand_Address, 6);
  wand.channel = 13;  
  wand.encrypt = false;
  if (esp_now_add_peer(&wand) != ESP_OK){
    Serial.println("Failed to add Wand");
    return;
  }

  #endif
  
  #ifdef SENDER3
  memcpy(board.peer_addr, Board_Address, 6);
  board.channel = 13;  
  board.encrypt = false;
  if (esp_now_add_peer(&board) != ESP_OK){
    Serial.println("Failed to add Board");
    return;
  }

  #endif
 
 

  #ifdef DEBUG
  Serial.print("Setup done...");
  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
  #endif

  if (esp_now_init() != ESP_OK) {
    #ifdef DEBUG
    Serial.println("Error initializing ESP-NOW");
    for(;;); // Don't proceed, loop forever
    #endif
    return;
  }
  
  #ifdef SCREEN
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.drawBitmap((display.width()  - LOGO_WIDTH ) / 2,(display.height() - LOGO_HEIGHT) / 2,logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(500);
  display.clearDisplay();
  #endif

  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  

}

#ifdef SCREEN

void drawinterface(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2); // Großer Text
  #ifdef SENDER1
  display.println("Orgel");
  #endif
  #ifdef SENDER2
  display.println("Wand");
  #endif
  #ifdef SENDER3
  display.println("Board");
  #endif
  display.setTextSize(1); // kleiner Text
  switch (modus){
    case 1: //direkt steuern
    display.println("Direkt");  
    display.setTextSize(1); 
    display.print("Speedchange: ");
    display.println(speedchange);
    display.print("Speed: ");
    display.println(speed);
    display.print("Steer: ");
    display.println(steer);
    
  
    break;
    case 2: //automatik gerade
    display.println("Gerade");
    display.setTextSize(1); 
    display.print("Laenge: ");
    display.println(distanz);
    display.print("Geschwindigkeit: ");
    display.println(speed);
    display.print("Status: ");
    if (running)display.println("running...");
    if (!running)display.println("Stopped!");
   
    break;
    case 3: //automatik kreis
    display.println("Kreis");
    display.setTextSize(1); 
    display.print("Durchmesser: ");
    display.println(steer);
    display.print("Geschwindigkeit: ");
    display.println(speed);
    display.print("Status: ");
    if (running)display.println("running...");
    if (!running)display.println("Stopped!");
    break;

  }
  
  //display.print(modus[mode]);
  display.print("Battery: ");
  display.println(Feedback.batVoltage);
  display.print("Temp: ");
  display.println(Feedback.boardTemp);
  display.display();
  //display.setCursor(0, 10);
}
#endif

void loop() {
#if defined RECEIVER_1 || defined RECEIVER_2 || defined RECEIVER_3

if (Serial.available()) {
    while (Serial.available() && buf_size < BUFFER_SIZE) {
      buf_send[buf_size] = Serial.read();
      send_timeout = micros() + timeout_micros;
      buf_size++;
    }
  }
  #endif
   
  #ifdef RECEIVER_1
  if (buf_size == BUFFER_SIZE_SEND || (buf_size > 0 && micros() >= send_timeout)) {
  esp_err_t result = esp_now_send(Sender1_Address, (uint8_t *) &buf_send, buf_size);
  buf_size = 0;
  }
  #endif
  
  #ifdef RECEIVER_2
  if (buf_size == BUFFER_SIZE_SEND || (buf_size > 0 && micros() >= send_timeout)) {
  esp_err_t result = esp_now_send(Sender2_Address, (uint8_t *) &buf_send, buf_size);
  buf_size = 0;
  }
  #endif

  #ifdef RECEIVER_3
  if (buf_size == BUFFER_SIZE_SEND || (buf_size > 0 && micros() >= send_timeout)) {
  esp_err_t result = esp_now_send(Sender3_Address, (uint8_t *) &buf_send, sizeof(buf_send));
  buf_size = 0;
  }
  #endif
  
  
  
      
#if defined SENDER1 || defined SENDER2 || defined SENDER3
  button1.loop();
  button2.loop();
  button3.loop();
  button4.loop();
  button5.loop();
  int btn1State = button1.getState();
  int btn2State = button2.getState();
  int btn3State = button3.getState();
  int btn4State = button4.getState();
  int btn5State = button5.getState();
  
  readio();
  steerlogic();
  makecommand(steer,speed);
  #endif

  #ifdef SCREEN
  drawinterface();
  #endif

  #ifdef SENDER1
  unsigned long timeNow = millis();
  if (timeNow - prevsend >= TIME_SEND ) { //&& buf_send[0] != 0x00 
  prevsend = timeNow;
  result = esp_now_send(Orgel_Address, (uint8_t *) &Command, sizeof(Command));     
  }
  #endif
  
  #ifdef SENDER2
  unsigned long timeNow = millis();
  if (timeNow - prevsend >= TIME_SEND ) { //&& buf_send[0] != 0x00 
  prevsend = timeNow;
  result = esp_now_send(Wand_Address, (uint8_t *) &Command, sizeof(Command));     
  }
  #endif
  
  #ifdef SENDER3
  unsigned long timeNow = millis();
  if (timeNow - prevsend >= TIME_SEND ) { //&& buf_send[0] != 0x00 
  prevsend = timeNow;
  result = esp_now_send(Board_Address, (uint8_t *) &Command, sizeof(Command));     
  }
  #endif

  #ifdef DEBUG
  serialdebug();
  #endif
}
