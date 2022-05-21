#define DEBUG
#define BLINK_ON_SEND
#define BLINK_ON_RECV
//#define ORGEL
#define SCREEN
#define SENDER1

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

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
  Serial.print("Starting.....");
  #ifdef ORGEL
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  #endif
  

  #ifdef SENDER1
  pinMode(button1pin, INPUT_PULLUP);
  pinMode(button2pin, INPUT_PULLUP);
  pinMode(button3pin, INPUT_PULLUP);
  pinMode(button4pin, INPUT_PULLUP);
  pinMode(button5pin, INPUT_PULLUP);
  
  memcpy(orgel.peer_addr, ORGEL_MAC, 6);
  orgel.channel = 13;  
  orgel.encrypt = false;
  
  #endif

  #ifdef DEBUG
  Serial.begin(BAUD_RATE, SER_PARAMS, RX_PIN, TX_PIN);
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
  /*
  if (esp_wifi_set_channel(WIFI_CHAN, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
    #ifdef DEBUG
    Serial.println("Error changing WiFi channel");
    #endif
    return;
  }
  */

  esp_now_register_recv_cb(OnDataRecv);
  //#if defined(DEBUG) || defined(BLINK_ON_SEND_SUCCESS)
  esp_now_register_send_cb(OnDataSent);
  //#endif


  #ifdef SCREEN
  //Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.drawBitmap((display.width()  - LOGO_WIDTH ) / 2,(display.height() - LOGO_HEIGHT) / 2,logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(500);
  display.clearDisplay();
   
    /*  
    (display.width()  - FLAG_WIDTH ) / 2,
    (display.height() - FLAG_HEIGHT) / 2,
    flag_bmp, FLAG_WIDTH, FLAG_HEIGHT, 1);
    delay(2000);
    display.clearDisplay();
  */
  #endif
 
}

#ifdef SCREEN
void drawinterface(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2); // Draw 2X-scale text
  display.println("Orgel");
  display.setTextSize(1); // Draw 2X-scale text
  display.print("Speedchange: ");
  display.println(speedchange);
  display.print("Speed: ");
  display.println(speed);
  display.print("Steer: ");
  display.println(steer);
  display.print("Battery: ");
  display.println(Feedback.batVoltage);
  display.print("Temp: ");
  display.println(Feedback.boardTemp);
  display.display();      // Show initial text
  //display.setCursor(0, 10);
}
#endif
void loop() {
//serialdebug();


#ifdef ORGEL
if (Serial.available()) {
    while (Serial.available() && buf_size < BUFFER_SIZE) {
      buf_send[buf_size] = Serial.read();
      send_timeout = micros() + timeout_micros;
      buf_size++;
    }
  }
  // send buffer contents when full or timeout has elapsed
  if (buf_size == BUFFER_SIZE_SEND || (buf_size > 0 && micros() >= send_timeout)) {
    
  memcpy(sender.peer_addr, ORGEL_MAC, 6);
  sender.channel = 13;  
  sender.encrypt = false;
  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &buf_send, buf_size);
  buf_size = 0;
    
    #ifdef BLINK_ON_SEND
  led_status = !led_status;
  digitalWrite(LED_BUILTIN, led_status);
  #endif
  
  #ifdef DEBUG
  Serial.println(esp_err_to_name(result));
  // esp_err_to_name_r(result)
    if (result == ESP_OK) {
      Serial.println("Sent!");
       }
    else {
      // Serial.println("Send error");
      //led_status = !led_status;
      //digitalWrite(LED_BUILTIN, led_status);
      }
    
    
    }
    
     #endif
    #endif
    
#ifdef SENDER1
  #ifdef SCREEN
  drawinterface();
  #endif
  readio();
  steerlogic();
  makecommand(steer,speed);
   
  //if (micros() >= send_timeout) {
  //send_timeout = micros() + timeout_micros;
  //result = esp_now_send(broadcastAddress, (uint8_t *) &Command, sizeof(Command));     
  
  unsigned long timeNow = millis();
  if (timeNow - prevsend >= TIME_SEND && buf_send[0] != 0x00) { // 
  prevsend = timeNow;
  //serialdebug();
  
  result = esp_now_send(broadcastAddress, (uint8_t *) &Command, sizeof(Command));     
  led_status = !led_status;
  digitalWrite(LED_BUILTIN, led_status);
  Serial.println(esp_err_to_name(result));
  buf_send[0]=0x00;
  }

#endif 
}

