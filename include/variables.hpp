int led_power = 26;
int led_finger = 25;
int led_ethernet = 33;
int led_client = 32;
bool fLed2 = false;
bool wait_pza = false;
bool currentLineIsBlank = true;

unsigned long currentTime=0;
unsigned long previousTime=0;
String data_tmp,data_in;
char cx[2];

const int n_cmds = 8;
String commands[n_cmds] = {"C_OK","C_NG","F_ADD","F_CLEAR","F_CHECK","C_RST","C_TEST", "S_RST"};
// comands cmds;// = comands;
int16_t state = 0;
int16_t state2 = 0;
bool bloquear;


bool iniciado = true;

//Sensor de Huella
// bool fingerOK = false;      //Presensia del sensor de huella

//---------------------------------------------------
// Variables Finger Print
//---------------------------------------------------
uint8_t p;
String s_p, s_p_ant;
uint8_t finger_ID;
uint16_t finger_status_reg;         //< The status register
uint16_t finger_system_id;          //< The system identifier
uint16_t finger_capacity;           //< The fingerprint capacity
uint16_t finger_security_level;     //< The security level
uint32_t finger_device_addr;        //< The device address
uint16_t finger_packet_len;         //< The max packet length
uint16_t finger_baud_rate;          //< The UART baud rate
uint16_t finger_templateCount;      // The number of stored templates in the sensor
bool finger_check, finger_add, finger_clear,fingerOK;
short  finger_status;
int finger_state;
bool equals, finger_led = false;
bool turnOffLed = false;