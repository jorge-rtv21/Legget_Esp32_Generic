//---------------------------------------------------
// Definiciones
//---------------------------------------------------
// #define RELAY1  27                  // GPIO27 para salida de Relay 1
// #define RELAY2  26                  // GPIO26 para salida de Relay 2
// #define WIFILED 12                  // GPIO12 LED INDICADOR WIFI 
// #define MQTTLED 13                  // GPIO13 LED INDICADOR MQTT
//#define fs_tx               //Finger Print Sensor Tx
//#define fs_rx               //Finger Print Sensor Rx
//#define fs_baudrate         //Finger Print Sensor BaudRate
//#define WIFI_LED 13

// -------------------------------------------------------------------
// CALCULAR LA CAPACIDAD DEL JSON
// Asistente ArduinoJson: https://arduinojson.org/v6/assistant/
// Documentación: https://arduinojson.org/v6/api/json/deserializejson/
// -------------------------------------------------------------------
// const size_t capacitywifi = JSON_OBJECT_SIZE(16)+424;
// const size_t capacitymqtt = JSON_OBJECT_SIZE(7)+166;
// const size_t capacityrelays = JSON_OBJECT_SIZE(2)+30;
const size_t capacityadmin = JSON_OBJECT_SIZE(2)+42;

// const size_t capacitycolors = JSON_OBJECT_SIZE(16)+384;
// -------------------------------------------------------------------
// Versión de Firmware desde las variables de entorno platformio.ini
// -------------------------------------------------------------------
#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)
String currentfirmware = ESCAPEQUOTE(BUILD_TAG);

//---------------------------------------------------
// HardWare
//---------------------------------------------------
#define HW "ESP32-RTV v2"   //Version del HardWare
#define MF "RTV 2023"         // fabricante
//---------------------------------------------------
// General
//---------------------------------------------------
char id[30];                //ID del Dispositivo
int bootCount;              //Numero de reinicios

//---------------------------------------------------
// Wifi
//---------------------------------------------------
bool wifi_staticIP;         //Uso de IP Estatica
char wifi_ssid[30];         //Red Wifi
char wifi_pass[30];         //PassWord Red Wifi
char wifi_ip_static[15];    //IP Estatico
char wifi_gateway[15];      //Gateway
char wifi_subnet[15];       //Subred
char wifi_primaryDNS[15];   //DNS Primario
char wifi_secondaryDNS[15]; //DNS Secundario

//---------------------------------------------------
// AP
//---------------------------------------------------
bool ap_AccessPoint;        //Uso de Modo AP
char ap_nameAP[31];         //SSID AP
char ap_pass[63];           //PassWord AP
int ap_canalAP;             //Canal AP
int ap_hiddenAP;            //Visibilidad del AP
int ap_connAP;              //Numero de conexiones en el AP

// -------------------------------------------------------------------
// Zona configuración MQTT 
// -------------------------------------------------------------------          
boolean mqtt_enable;                // Habilitar MQTT Broker
char    mqtt_user[30];              // Usuario MQTT Broker 
char    mqtt_id[30];                // ID MQTT Broker       
char    mqtt_passw[30];             // Password MQTT Broker 
char    mqtt_server[39];            // Servidor del MQTT Broker
int     mqtt_port;                  // Puerto servidor MQTT Broker
int     mqtt_time;                  // Tiempo de envio por MQTT

//---------------------------------------------------
// Misenaleos
//---------------------------------------------------
uint8_t ip[4];              //Variable funcion convertir String a IP
float   temp_cpu;                   // Temperatura del CPU en °C
// bool fingerOK = false;      //Presensia del sensor de huella
// bool s_colorOK= false;

// Zona Relays
// -------------------------------------------------------------------
bool Relay01_status, Relay02_status; // Estados de los Relay de Salida
// -------------------------------------------------------------------
// Zona www_username & www_password
// -------------------------------------------------------------------
char www_username[15];              // Usuario para acceso al servidor Web
char www_password[15];              // Contraseña del usuario servidor Web
// -------------------------------------------------------------------
// Zona Firmware Update
// ------------------------------------------------------------------- 
size_t content_len;
#define U_PART U_SPIFFS

//---------------------------------------------------
// Finger Print
//---------------------------------------------------
// uint8_t p;
// String s_p, s_p_ant;
// uint8_t finger_ID;
// uint16_t finger_status_reg;         //< The status register
// uint16_t finger_system_id;          //< The system identifier
// uint16_t finger_capacity;           //< The fingerprint capacity
// uint16_t finger_security_level;     //< The security level
// uint32_t finger_device_addr;        //< The device address
// uint16_t finger_packet_len;         //< The max packet length
// uint16_t finger_baud_rate;          //< The UART baud rate
// uint16_t finger_templateCount;      // The number of stored templates in the sensor
// bool finger_check, finger_add, finger_clear;
// int finger_status, finger_state;
// bool equals;
//---------------------------------------------------
// Sensor de Color TCS32752
//---------------------------------------------------
String sC_Time = "TCS34725_INTEGRATIONTIME_154MS";
String sC_Gain = "TCS34725_GAIN_16X";
String sC_lectura;
struct TCS32752{
    uint16_t r;
    uint16_t g; 
    uint16_t b;
    uint16_t c;
    uint16_t colorTemp;
    uint16_t lux;
};
int black_R, black_G, black_B, black_T;
int blue_R, blue_G, blue_B, blue_T;
int green_R, green_G, green_B, green_T;
int red_R, red_G, red_B, red_T;
int sC_samples, sC_conteo;
// unsigned long currentTime=0;
// unsigned long previousTime=0;
unsigned long sC_time;
bool sC_muestreo;
//---------------------------------------------------
// Comunicacion Inalambrica ESP-Now
//---------------------------------------------------
// bool mesh_status;
// String success;
// uint8_t mac_remote[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// String mac_local, mach_linea, mach_op, mach_desc, s_mac_remote;
// typedef struct struct_message {
//     String command;
//     String ID_motor;
//     String station;
// } struct_message;
// struct_message incomingReadings;


//---------------------------------------------------
// Comunicacion RTC
//---------------------------------------------------
bool rtc_ok, rtc_runnin;
String rtc_stamp;
uint16_t  rtc_year;
uint8_t rtc_month, rtc_day;
uint16_t rtc_hour, rtc_minute, rtc_second;