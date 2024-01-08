// ########################## Bewegungseigenschaften ##########################
#define MAX_SPEED  500
#define MAX_STEER  50
// ########################## Wireless Parameter ##########################
esp_now_peer_info_t sender1;
esp_now_peer_info_t sender2;
esp_now_peer_info_t sender3;
esp_now_peer_info_t orgel;
esp_now_peer_info_t wand;
esp_now_peer_info_t board;
esp_err_t result;
#define SENDER1_MAC {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}  // Sender dev mit Keypad
#define SENDER2_MAC {0x00, 0x00, 0x00, 0x00, 0x00, 0x03}  // Sender dev mit Keypad
#define SENDER3_MAC {0x00, 0x00, 0x00, 0x00, 0x00, 0x05}  // Sender dev mit Keypad
#define ORGEL_MAC {0x00, 0x00, 0x00, 0x00, 0x00, 0x02}  // Empfänger an der Orgel
#define WAND_MAC {0x00, 0x00, 0x00, 0x00, 0x00, 0x4}       //Empfänger an der fahrenden Wand
#define SIDEBOARD_MAC {0x00, 0x00, 0x00, 0x00, 0x00, 0x06}  //Empfänger am Sideboard

#define WIFI_CHAN  13 // 12-13 only legal in US in lower power mode, do not use 14
#define BAUD_RATE  115200
#define TX_PIN     1 // default UART0 is pin 1 (shared by USB)
#define RX_PIN     3 // default UART0 is pin 3 (shared by USB)
#define SER_PARAMS SERIAL_8N1 // SERIAL_8N1: start/stop bits, no parity
#define BUFFER_SIZE 250 // max of 250 bytes
#define START_FRAME         0xABCD       // [-] Start frme definition for reliable serial communication
#define TIME_SEND           100         // [ms] Sending time interval

#ifdef SENDER1
uint8_t newMACAddress[] = SENDER1_MAC;
#define BUFFER_SIZE_SEND 8 // max of 250 bytes
#define BUFFER_SIZE_RECEIVE 18 // max of 250 bytes
uint8_t Orgel_Address[] = ORGEL_MAC;
uint8_t Wand_Address[] = WAND_MAC;
uint8_t Board_Address[] = SIDEBOARD_MAC;
#endif

#ifdef SENDER2
uint8_t newMACAddress[] = SENDER2_MAC;
#define BUFFER_SIZE_SEND 8 // max of 250 bytes
#define BUFFER_SIZE_RECEIVE 18 // max of 250 bytes
uint8_t Orgel_Address[] = ORGEL_MAC;
uint8_t Wand_Address[] = WAND_MAC;
uint8_t Board_Address[] = SIDEBOARD_MAC;
#endif

#ifdef SENDER3
uint8_t newMACAddress[] = SENDER3_MAC;
#define BUFFER_SIZE_SEND 8 // max of 250 bytes
#define BUFFER_SIZE_RECEIVE 18 // max of 250 bytes
uint8_t Orgel_Address[] = ORGEL_MAC;
uint8_t Wand_Address[] = WAND_MAC;
uint8_t Board_Address[] = SIDEBOARD_MAC;
#endif

#ifdef RECEIVER_1
uint8_t newMACAddress[] = ORGEL_MAC; 
#define BUFFER_SIZE_SEND 18 // max of 250 bytes
#define BUFFER_SIZE_RECEIVE 8 // max of 250 bytes
uint8_t Sender1_Address[] = SENDER1_MAC;
#endif

#ifdef RECEIVER_2
uint8_t newMACAddress[] = WAND_MAC; 
#define BUFFER_SIZE_SEND 18 // max of 250 bytes
#define BUFFER_SIZE_RECEIVE 8 // max of 250 bytes
uint8_t Sender2_Address[] = SENDER2_MAC;
#endif

#ifdef RECEIVER_3
uint8_t newMACAddress[] = SIDEBOARD_MAC; 
#define BUFFER_SIZE_SEND 18 // max of 250 bytes
#define BUFFER_SIZE_RECEIVE 8 // max of 250 bytes
uint8_t Sender3_Address[] = SENDER3_MAC;
#endif



// ########################## Hardware Belegung ##########################
//Kein Pullup bei: GPIO34, GPIO35, GPIO36, GPIO37,  GPIO38, GPIO39 !!!

const int SHORT_PRESS_TIME = 1000; // 1 sek
const int LONG_PRESS_TIME  = 1000; // 1 sek
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;


ezButton button1(27);
ezButton button2(14);
ezButton button3(26);
ezButton button4(25);
ezButton button5(33);

uint8_t potpin = 32;
int8_t speedchange;
int8_t steerchange;
int16_t speed = 0;
int16_t steer = 0;
bool buttonpressed;
uint16_t potval;
uint16_t potprev;
bool potchanged;
uint16_t linevalue;
uint8_t led_status = 0;

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // some boards don't have an LED or have it connected elsewhere
#endif

unsigned long prevsend;
unsigned long prevstart;

const uint32_t timeout_micros = (int)(1.0 / BAUD_RATE * 1E6) * 20;
uint32_t send_timeout = 0;
uint8_t buf_recv[BUFFER_SIZE_RECEIVE];
uint8_t buf_send[BUFFER_SIZE_SEND];
uint8_t buf_size = 0;

typedef struct{
   uint16_t start;
   int16_t  steer;
   int16_t  speed;
   uint16_t checksum;
} SerialCommand;
SerialCommand Command;
SerialCommand Command_1;
SerialCommand Command_2;

typedef struct{
   uint16_t start;
   int16_t  cmd1;
   int16_t  cmd2;
   int16_t  speedR_meas;
   int16_t  speedL_meas;
   int16_t  batVoltage;
   int16_t  boardTemp;
   uint16_t cmdLed;
   uint16_t checksum;
} SerialFeedback;
SerialFeedback Feedback;
SerialFeedback Feedback_1;
SerialFeedback Feedback_2;



// ########################## Bildschirm Parameter ##########################
#ifdef SCREEN
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
#define FLAG_HEIGHT   64
#define FLAG_WIDTH    128
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };


#endif

static uint8_t modus =1;
unsigned long distanz = 1000;
static uint8_t maxspeed_linie = 100;
static bool richtung =1;
static bool richtungwechsel =0;
static bool running;