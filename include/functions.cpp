
// ########################## HANDLE I/O ##########################
void readio(){
#ifdef SENDER1
btn_1 = !digitalRead(button1pin);
btn_2 = !digitalRead(button2pin);
btn_3 = !digitalRead(button3pin);
btn_4 = !digitalRead(button4pin);
btn_5 = !digitalRead(button5pin);

potval = analogRead(potpin);
if (btn_1 == 1 || btn_2 == 1 || btn_3 == 1 || btn_4 == 1 || btn_5 == 1){
buttonpressed=1;  
}
else
{buttonpressed=0;}
//Serial.write(potval);
//Serial.write(0x1B);
#endif
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {

  #ifdef BLINK_ON_SEND_SUCCESS
  if (status == ESP_NOW_SEND_SUCCESS) {
    led_status = ~led_status;
    // this function happens too fast to register a blink
    // instead, we latch on/off as data is successfully sent
    digitalWrite(LED_BUILTIN, led_status);
    return;
  }
  // turn off the LED if send fails
  led_status = 0;
  digitalWrite(LED_BUILTIN, led_status);
  #endif 
}
// ########################## RECEIVE ##########################
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) //Funktion von Eferu
{
led_status = !led_status;
 digitalWrite(LED_BUILTIN, led_status);

#ifdef SENDER1
  uint16_t checksum;
  memcpy(&buf_recv, incomingData, BUFFER_SIZE_RECEIVE); //Argumente: wohin, woher, anzahl bytes
  //--> buf_rec enthält bis zu max 250bytes, eigentlich werden aber immer nur 18 geschickt.
  // bufStartFrame = ((uint16_t)(buf_recv[1]) << 8) | buf_recv[0];       // Construct the start frame
  Feedback.start = (buf_recv[1] << 8) | buf_recv[0];
  if (Feedback.start == START_FRAME) {

            //unsigned int word = ((unsigned int)high_byte << 8) + low_byte
            Feedback.cmd1 = (buf_recv[3] << 8) | buf_recv[2];
            Feedback.cmd2 = (buf_recv[5] << 8) | buf_recv[4];
            Feedback.speedR_meas = (buf_recv[7] << 8) | buf_recv[6];
            Feedback.speedL_meas = (buf_recv[9] << 8) | buf_recv[8];
            Feedback.batVoltage = (buf_recv[11] << 8) | buf_recv[10];
            Feedback.boardTemp = (buf_recv[13] << 8) | buf_recv[12];
            Feedback.cmdLed = (buf_recv[15] << 8) | buf_recv[14];
            Feedback.checksum = (buf_recv[17] << 8) | buf_recv[16];
            
            checksum = (uint16_t)(Feedback.start ^ Feedback.cmd1 ^ Feedback.cmd2 ^ Feedback.speedR_meas ^ Feedback.speedL_meas
                            ^ Feedback.batVoltage ^ Feedback.boardTemp ^ Feedback.cmdLed);       
  }   
    
    
    
  if (Feedback.start == START_FRAME && checksum == Feedback.checksum) {
  
            //memcpy(&Feedback, &NewFeedback, sizeof(SerialFeedback)); // Copy the new data
            // Print data to built-in Serial
            Serial.print("1,Cmd1:");   Serial.print(Feedback.cmd1);
            Serial.print(" 2,Cmd2:");  Serial.print(Feedback.cmd2);
            Serial.print(" 3,SpeedR:");  Serial.print(Feedback.speedR_meas);
            Serial.print(" 4,SpeedL:");  Serial.print(Feedback.speedL_meas);
            Serial.print(" 5,Battery:");  Serial.print(Feedback.batVoltage);
            Serial.print(" 6,Temperatur:");  Serial.print(Feedback.boardTemp);
            Serial.print(" 7,LED:");  Serial.println(Feedback.cmdLed);
        } else {
          Serial.println("Non-valid data skipped");
        }
#endif

#ifdef ORGEL
memcpy(&buf_recv, incomingData, sizeof(buf_recv));
//#ifdef BLINK_ON_RECV
//#endif

#ifdef DEBUG
  Serial.print("\n Bytes received: ");
  Serial.print(len);
  Serial.print("Content: ");
  //Serial.println(incomingData);
#endif
#endif
    
}
// ########################## SEND ##########################
void makecommand(int16_t uSteer, int16_t uSpeed)
{
  // Create command
  Command.start    = (uint16_t)START_FRAME;
  Command.steer    = (int16_t)uSteer;
  Command.speed    = (int16_t)uSpeed;
  Command.checksum = (uint16_t)(Command.start ^ Command.steer ^ Command.speed);
   
  memcpy(buf_send, &Command, BUFFER_SIZE_SEND); //Argumente: wohin, woher, anzahl bytes   
    
  
  
  // bufStartFrame = ((uint16_t)(buf_recv[1]) << 8) | buf_recv[0];       // Construct the start frame
  // Feedback.start = (buf_recv[1] << 8) | buf_recv[0];
  
  //unsigned int word = ((unsigned int)high_byte << 8) + low_byte
  //Feedback.cmd1 = (buf_recv[3] << 8) | buf_recv[2];
  //memcpy(buf_send, &Command, BUFFER_SIZE_SEND); //Argumente: wohin, woher, anzahl bytes
  //esp_now_send(broadcastAddress, (uint8_t *) &buf_send, BUFFER_SIZE_SEND);
  #ifdef DEBUG
  #endif
  /*
  esp_now_send(broadcastAddress, (uint8_t *) &Command, BUFFER_SIZE_SEND); 
  
  Serial.write((uint8_t *) &Command, sizeof(Command)); 
  Serial.print(" ");
  Serial.print(Command.start);
  Serial.print(" ");
  Serial.print(Command.steer);
  Serial.print(" ");
  Serial.print(Command.speed);
  Serial.print(" ");
  Serial.println(Command.checksum);
  */ 
}
void steerlogic(){
  
  if (potval<2048){
    	speedchange = map(potval, 0, 2047, 10, 0);
    }
  if (potval>2048){
      speedchange = map(potval, 2049, 4095, 0, -11);
  }
  
  if (btn_1==1){ //Button über Poti --> Stop
    steer=0;
    speed=0;
  }
  
  if (btn_2==1){
    steer-=10;   
  }
  
  if (btn_3==1){
    steer+=10;   
  }

  if (btn_4==1 && speed>-MAX_SPEED){
    //speed-=speedchange; }
    speed--;
  }

  if (btn_5==1 && speed<MAX_SPEED){
    //speed+=speedchange; }
    speed++;
  }
  
  if (buttonpressed != 1){
    //speed=0;
    steer=0; }
  
  /*  
    if (buttonpressed == 1){
    //speed+=speedchange;
    }
    else
    {
      speed=0;
      speedchange=0;
      //steer=0;
    }    
  }
  */
}
void serialdebug(){
  Serial.print("Buffer Send: ");
  for(int i=0; i < BUFFER_SIZE_SEND; i++){
   Serial.write(buf_send[i]);
  }
  //Serial.write(0x1B);
  Serial.print("   ");
  Serial.print(" Btn 1:");
  Serial.print(btn_1);
  Serial.print(" Btn 2: ");
  Serial.print(btn_2);
  Serial.print(" Btn 3: ");
  Serial.print(btn_3);
  Serial.print(" Btn 4: ");
  Serial.print(btn_4);
  Serial.print(" Btn 5: ");
  Serial.print(btn_5);
  Serial.print(" Speed: ");
  Serial.print(speed);
  Serial.print(" Steer: ");
  Serial.print(steer);
  Serial.print(" Potval: ");
  Serial.print(potval);
  Serial.print(" ESPNOW Status: ");
  Serial.println(esp_err_to_name(result));
  //buf_send[0]=0x00;
  

 
}