
// ########################## HANDLE I/O ##########################
void readio(){
#if defined SENDER1 || defined SENDER2 || defined SENDER3
/*
btn_1 = !digitalRead(button1pin);
btn_2 = !digitalRead(button2pin);
btn_3 = !digitalRead(button3pin);
btn_4 = !digitalRead(button4pin);
btn_5 = !digitalRead(button5pin);
*/

potval = analogRead(potpin);
if (potprev == potval || potprev == potval-1 || potprev == potval+1){
potchanged = false;
}

if (potprev != potval){
potchanged = true;
potprev = potval;
}



//|| potprev == potval-1 || potprev == potval+1

if (button1.isPressed() || button2.isPressed() || button3.isPressed() || button4.isPressed() || button5.isPressed()){
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


#if defined SENDER1 || defined SENDER2 || defined SENDER3
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

#if defined RECEIVER_1 || defined RECEIVER_2 || defined RECEIVER_3
memcpy(&buf_recv, incomingData, sizeof(buf_recv));

for(int i=0; i < BUFFER_SIZE_RECEIVE; i++){
   Serial.write(buf_recv[i]);
  }


#ifdef DEBUG
  Serial.print("\n Bytes received: ");
  Serial.print(len);
  Serial.print("Content: ");
  for(int i=0; i < BUFFER_SIZE_RECEIVE; i++){
   Serial.write(buf_recv[i]);
  }
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
  
}
void steerlogic(){
  if(button2.isReleased()){
    modus++;
    if (modus >3) modus=1;
    }

  if (button3.isReleased()){ //Navigate from Mode 1-3
    modus--;
    if (modus <1) modus=3;
     }
   
   if (button1.isReleased()){ //Button über Poti --> Stop
    running = !running;
     }
   
   switch (modus){
    case 1: //#################   direkt steuern
    
    if (running == true){
   
    if (potval<2048 && potchanged){
    //	speedchange = map(potval, 0, 2047, MAX_SPEED, 0);
    steer=map(potval, 0, 2047, MAX_STEER, 0);
    }
    
    if (potval>2048 && potchanged){
    //  speedchange = map(potval, 2049, 4095, 0, -MAX_SPEED);
    steer=map(potval, 2048, 4095, 0,-MAX_STEER);
    }
      
    if (!button4.getState() && speed>-MAX_SPEED){
    //speed-=speedchange; }
    speed--;
    }

    if (!button5.getState() && speed<MAX_SPEED){
    //speed+=speedchange; }
    speed++;
    }

    if (buttonpressed != 1){ //rücksetzen, falls nichts gedrückt
    //speed=0;
    //steer=0; 
    }
    
    }else{
    speed=0;
    steer=0; 
    }
    break;
    case 2: ////#################   automatik gerade

    if (button4.getState()){ 
    distanz++;
    }
    
    if (button5.getState()){ 
    distanz--;
    }

    if (button2.isPressed()){ 
    maxspeed_linie++;
    }
    if (button3.isPressed()){ 
    maxspeed_linie--;
    }
    
    if (running == true){
      unsigned long startzeit = millis();
      
      if (richtung == true){ //vorwärts
          if (speed < maxspeed_linie){
          speed++;
                   
      if (startzeit - prevstart >= distanz ) { //triggert alle distanz ms
        prevstart = startzeit;
        richtung = !richtung;
        } 
        }
      
      if (richtung == false){ //Rückwärts
          if (speed > -maxspeed_linie){
          speed--;
                   
        if (startzeit - prevstart >= distanz ) { //triggert alle distanz ms
        prevstart = startzeit;
        richtung =!richtung;
        }
        }
      } 

    
    } else {
    speed = 0;
    steer = 0;    
    }

    break;
    }
   
   
    case 3: ////#################   automatik kreis
    
    if (running == true){
   	steer = map(potval, 0, 4095, 0, MAX_STEER);
    if (!button4.getState() && speed>-MAX_SPEED){
    speed--;
    }
    if (!button5.getState() && speed<MAX_SPEED){
    speed++;
    }

    } else
    {
    speed = 0;
    steer = 0;  
    }
  break;

  }
  
  
  
}


void serialdebug(){
  #if defined SENDER1 || defined SENDER2 || defined SENDER3
  Serial.print("Sender! ");
  Serial.print("Buffer Send: ");
  for(int i=0; i < BUFFER_SIZE_SEND; i++){
   Serial.write(buf_send[i]);
  }
  Serial.print("Buffer Recv: ");
  for(int i=0; i < BUFFER_SIZE_SEND; i++){
   Serial.write(buf_recv[i]);
  }
 /*
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
  */
  Serial.print(" Speed: ");
  Serial.print(speed);
  Serial.print(" Steer: ");
  Serial.print(steer);
  Serial.print(" Potval: ");
  Serial.print(potval);
  //buf_send[0]=0x00;
 
  Serial.print(" ESPNOW Status: ");
  Serial.print(esp_err_to_name(result));
 
  #endif


  #if defined RECEIVER_1 || defined RECEIVER_2 || defined RECEIVER_3
  Serial.print("Receiver! "); 
  Serial.print("Buffer Send: ");
  for(int i=0; i < BUFFER_SIZE_SEND; i++){
   Serial.write(buf_send[i]);
  }
  Serial.print("Buffer Recv: ");
  for(int i=0; i < BUFFER_SIZE_RECEIVE; i++){
   Serial.write(buf_recv[i]);
  }
  Serial.print(" ESPNOW Status: ");
  Serial.print(esp_err_to_name(result));
 
  #endif
  //Serial.write(0x1B);
  Serial.println("");
 
}