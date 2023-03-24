/****************************************************************************************************************************
  Async_WebSocketsServer.ino
  For W5500 LwIP Ethernet in ESP32 (ESP32 + W5500)
  AsyncWebServer_ESP32_W5500 is a library for the LwIP Ethernet W5500 in ESP32 to run AsyncWebServer
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_ESP32_W5500
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

#if !( defined(ESP32) )
  #error This code is designed for (ESP32 + W5500) to run on ESP32 platform! Please check your Tools->Board setting.
#endif

#include <Arduino.h>
#include <ArduinoJson.h>

#include "variables.hpp"
#include "Funciones.hpp"
// #include "Finger.hpp"

#include <AsyncWebServer_ESP32_W5500.h>
//WebServer
#include <SPIFFS.h>
#include <TimeLib.h>
#include "webpage.h"

#include "config.hpp"
#include "functions.hpp"
#include "Finger.hpp"

#include "settingsReset.hpp"
#include "settingsRead.hpp"
#include "settingsSave.hpp"

//Expansor IOs 
#include <Wire.h>
#include "PCF8574.h"
#include "Pcf8574.hpp"

//I2C Scanner
#include "I2CScanner.h"
#define SDA_PIN 21	
#define SCL_PIN 22
I2CScanner scanner;



#define _ASYNC_WEBSERVER_LOGLEVEL_       4

// Enter a MAC address and IP address for your controller below.
#define NUMBER_OF_MAC      20

byte mac[][NUMBER_OF_MAC] = {
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14 },
};

// Select the IP address according to your local network
IPAddress myIP(192, 168, 0, 17);   //IP
IPAddress myGW(192, 168, 0, 1);     //Puerta de enlace GateWay
IPAddress mySN(255, 255, 255, 0);   //Mascara de SubNet (SubRed)

// Google DNS Server IP
IPAddress myDNS(8, 8, 8, 8);

bool bloqueado = false;

//////////////////////////////////////////////////////////

// Optional values to override default settings
// Don't change unless you know what you're doing
//#define ETH_SPI_HOST        SPI3_HOST
//#define SPI_CLOCK_MHZ       25

// Must connect INT to GPIOxx or not working
//#define INT_GPIO            4

//#define MISO_GPIO           19
//#define MOSI_GPIO           23
//#define SCK_GPIO            18
//#define CS_GPIO             5

//////////////////////////////////////////////////////////




AsyncWebServer server(80);
// AsyncWebSocket ws("/ws");
AsyncWebSocket ws("/");

#include "esp32_server.hpp"

void do_cmd(int n_cmd);
void get_test_rst();

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
	if (type == WS_EVT_CONNECT)
	{
    digitalWrite(led_client, 1);
		Serial.printf("ws[Server: %s][ClientID: %u] WSClient connected\n", server->url(), client->id());
		client->text("Hello from ESP32 Server");
	}
	else if (type == WS_EVT_DISCONNECT)
	{
    digitalWrite(led_client, 0);
		Serial.printf("ws[Server: %s][ClientID: %u] WSClient disconnected\n", server->url(), client->id());
	}
	else if (type == WS_EVT_ERROR)
	{
		//error was received from the other end
		Serial.printf("ws[Server: %s][ClientID: %u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
	}
	else if (type == WS_EVT_PONG)
	{
		//pong message was received (in response to a ping request maybe)
		Serial.printf("ws[Server: %s][ClientID: %u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");
	}
	else if (type == WS_EVT_DATA)
	{
		//data packet
		AwsFrameInfo * info = (AwsFrameInfo*)arg;

		if (info->final && info->index == 0 && info->len == len)
		{
			//the whole message is in a single frame and we got all of it's data
			// Serial.printf("ws[Server: %s][ClientID: %u] %s-message[len: %llu]: ", server->url(), client->id(),
			//               (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

			if (info->opcode == WS_TEXT)
			{
				data[len] = 0;
        String d_data = String((char*)data);
        int i_data = get_cmd(d_data);
        do_cmd(i_data);
        log("Comando: " + d_data + "\tNum: " + String(i_data));

        client->text("Got your text message");
			}
			else
			{
				for (size_t i = 0; i < info->len; i++)
				{
					Serial.printf("%02x ", data[i]);
          client->binary("Got your binary message");
				}

				Serial.printf("\n");
			}

			// if (info->opcode == WS_TEXT)
			// 	client->text("Got your text message");
			// else
			// 	client->binary("Got your binary message");
		}
		else
		{
			//message is comprised of multiple frames or the frame is split into multiple packets
			if (info->index == 0)
			{
				if (info->num == 0)
				{
					Serial.printf("ws[Server: %s][ClientID: %u] %s-message start\n", server->url(), client->id(),
					              (info->message_opcode == WS_TEXT) ? "text" : "binary");
				}

				Serial.printf("ws[Server: %s][ClientID: %u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
			}

			Serial.printf("ws[Server: %s][ClientID: %u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(),
			              info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

			if (info->message_opcode == WS_TEXT)
			{
				data[len] = 0;
				Serial.printf("%s\n", (char*)data);
			}
			else
			{
				for (size_t i = 0; i < len; i++)
				{
					Serial.printf("%02x ", data[i]);
				}

				Serial.printf("\n");
			}

			if ((info->index + len) == info->len)
			{
				Serial.printf("ws[Server: %s][ClientID: %u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);

				if (info->final)
				{
					Serial.printf("ws[Server: %s][ClientID: %u] %s-message end\n", server->url(), client->id(),
					              (info->message_opcode == WS_TEXT) ? "text" : "binary");

					if (info->message_opcode == WS_TEXT)
						client->text("I got your text message");
					else
						client->binary("I got your binary message");
				}
			}
		}
	}
}

void handleRoot(AsyncWebServerRequest *request)
{
	request->send(200, "text/html", webpageCont);
}

void setup(){
  pinMode(led_power,OUTPUT);
  pinMode(led_finger,OUTPUT);
  pinMode(led_ethernet,OUTPUT);
  pinMode(led_client,OUTPUT);

  digitalWrite(led_power,1);
  digitalWrite(led_finger,1);
  digitalWrite(led_ethernet,1);
  digitalWrite(led_client,1);
  delay(750);
  digitalWrite(led_finger,0);
  digitalWrite(led_ethernet,0);
  digitalWrite(led_client,0);

	Serial.begin(115200);

	while (!Serial && millis() < 5000);

  //Inicializando SPIFFS
  if (!SPIFFS.begin()){
    log (F("ERROR: Falló la inicialización del SPIFFS"));
    //while(true);
  }
  log(F("SPIFFS OK"));

  // leer www_username/password
  if(!settingsReadAdmin()){
      // Salvar el usuario y Contraseña
      settingsSaveAdmin();
  } 


  // scanner.Init();
  // scanner.Scan();
	// delay(5000);

  ///////////////////
  exp0_init();
  exp1_init();

  unblock();
  
  fingerOK = fingerConfig();
  if (fingerOK){
    // digitalWrite(led_finger,1);
    fingerGetParameters();
    fingerPrintParameters();
  }

	delay(200);

	Serial.print("\nStarting Async_WebSocketsServer on ");
	Serial.print(ARDUINO_BOARD);
  Serial.print(" with ");
  Serial.println(SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_ESP32_W5500_VERSION);

  AWS_LOGWARN(F("Default SPI pinout:"));
  AWS_LOGWARN1(F("SPI_HOST:"), ETH_SPI_HOST);
  AWS_LOGWARN1(F("MOSI:"), MOSI_GPIO);
  AWS_LOGWARN1(F("MISO:"), MISO_GPIO);
  AWS_LOGWARN1(F("SCK:"),  SCK_GPIO);
  AWS_LOGWARN1(F("CS:"),   CS_GPIO);
  AWS_LOGWARN1(F("INT:"),  INT_GPIO);
  AWS_LOGWARN1(F("SPI Clock (MHz):"), SPI_CLOCK_MHZ);
  AWS_LOGWARN(F("========================="));

  ///////////////////////////////////

  // To be called before ETH.begin()
  ESP32_W5500_onEvent();

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  uint16_t index = millis() % NUMBER_OF_MAC;

  //bool begin(int MISO_GPIO, int MOSI_GPIO, int SCLK_GPIO, int CS_GPIO, int INT_GPIO, int SPI_CLOCK_MHZ,
  //           int SPI_HOST, uint8_t *W5500_Mac = W5500_Default_Mac);
  //ETH.begin( MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST );
  ETH.begin( MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST, mac[index] );

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  ETH.config(myIP, myGW, mySN, myDNS);

  // ESP32_W5500_waitForConnect();
  digitalWrite(led_ethernet,ESP32_W5500_eth_connected);

  ///////////////////////////////////

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);

	// server.on("/", handleRoot);
	// server.begin();
  InitServer();
}

void loop(){
  if (ESP32_W5500_eth_connected){
    digitalWrite(led_ethernet,HIGH);
    server.begin();
  }else{
    digitalWrite(led_ethernet,LOW);
  }
  get_test_rst();

  if(finger_add){
    
    // finger.LEDcontrol(finger_led);
    // finger_led != finger_led;
    // log("# Huellas: " + String(finger_templateCount));
    finger_ID = fingerEnroll();
    log(s_p);
    equals  = s_p.equals(s_p_ant);
    if ( !equals && finger_status < 0){
      // events.send(s_p.c_str(),"fP_text",millis());
      s_p_ant = s_p;
    }
    if (finger_status >= 0){
      if (finger_status == 0){
        // finger.LEDcontrol(FINGERPRINT_LED_OFF, 10, FINGERPRINT_LED_BLUE, 1);
        finger.LEDcontrol(FINGERPRINT_LED_ON, 10, FINGERPRINT_LED_RED, 1);
        // finger.LEDcontrol(FINGERPRINT_LED_OFF, 10, FINGERPRINT_LED_PURPLE, 1);
        turnOffLed = true;
        // delay(1000);
        log("NO SE ENCONTRO HUELLA");
      }
      if (finger_status == 1){
        finger.LEDcontrol(FINGERPRINT_LED_ON, 10, FINGERPRINT_LED_BLUE, 1);
        // finger.LEDcontrol(FINGERPRINT_LED_OFF, 10, FINGERPRINT_LED_RED, 1);
        // finger.LEDcontrol(FINGERPRINT_LED_OFF, 10, FINGERPRINT_LED_PURPLE, 1);
        // delay(1000);
        log("HUELLA  ENCONTRADA CON ID");
        
      } 
      log("Terminado");
      finger.getTemplateCount();
      // finger_templateCount = finger.templateCount;
      log("Finger ID: " + String(finger.templateCount));

      finger_add = false;
      finger_state = 0;
      finger_status = -1;
      s_p = "";
      s_p_ant = "";
    }
  }
  //Apaga el led Rojo del sensor de huella cuando el resultado es negativo
  if (turnOffLed){
    if(wait(3000)){
          // finger.LEDcontrol(FINGERPRINT_LED_OFF, 10, FINGERPRINT_LED_RED, 1);
          finger.LEDcontrol(FINGERPRINT_LED_ON, 10, FINGERPRINT_LED_BLUE, 1);
          turnOffLed = false;
        }
  }
 
  if(finger_clear){
    fingerEmptyDatabase();
    finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 30, FINGERPRINT_LED_RED, 5);
    turnOffLed = true;
    // events.send(String("Todas las huellas han sido borradas").c_str(),"fP_text",millis());
    finger_clear = false;
  }
  
  if (bloquear){
    block();
    // finger_check = true;
    bloqueado = true;
  }

  while(finger_check){
    finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 30, FINGERPRINT_LED_PURPLE, 2);
    // if (wait(50)){
    //   fingerParpadeo(2, 250);
    //   log("led");
    // }
    finger_ID= fingerGetID();
    // log("Checando huella ID:" + String(finger_ID));
    // log("s_P: " + s_p);
    // log ("s_p_ant: " + s_p_ant);
    equals  = s_p.equals(s_p_ant);
    // log("Igual:" + String(equals));
    if ( !equals && finger_status < 0){
      // events.send(s_p.c_str(),"fP_text",millis());
      log("Estado:" + s_p);
      s_p_ant = s_p;
    }
    if (finger_status >= 0){
      if (finger_status == 0){
        // events.send(String("NO SE ENCONTRO HUELLA").c_str(),"fP_text",millis());
        finger.LEDcontrol(FINGERPRINT_LED_ON, 25, FINGERPRINT_LED_RED, 1);
        turnOffLed = true;
      }
      if (finger_status == 1){
        // events.send(String("HUELLA  ENCONTRADA CON ID" + String(finger.fingerID)).c_str(),"fP_text",millis());
        finger.LEDcontrol(FINGERPRINT_LED_ON, 25, FINGERPRINT_LED_BLUE, 1);
        finger_check = false;
        finger.LEDcontrol(true);
        lamp_color(gr);
        delay(200);
        lamp_color(r);
        delay(200);
        lamp_color(gr);
        delay(200);
        lamp_color(r);
        unblock();
        wait_pza = true;
      } 
      log("Terminado");
      log("Finger ID: " + String(finger_status));

      
      s_p = "";
      s_p_ant = "";
    }
    delay(50);
  }

  while (wait_pza){
    // log("pieza");
    bool pz = !io_ext_0.digitalRead(P4);
    log("Pieza: " + String(pz));
    if (pz){
      // log("Pieza: " + String(pz));
      wait_pza = false;
      bloquear = false;
      lamp_color(off);
      unblock();
      delay(100);
    }
    lamp_color(yr);
    delay(100);
  }

  delay(500);
}
//Acciones de los comandos de websocket
void do_cmd(int n_cmd){
    switch (n_cmd){
        case 1: //C_OK
          Serial.println("cmd: C_OK");
          lamp_color(g);
          // wait_pza = true;
          break;
        case 2: //C_NG
          Serial.println("cmd: C_NG");
          lamp_color(r);
          block();
          finger_check = true;
          bloqueado = true;
          break;
        case 3: //F_ADD
          Serial.println("cmd: F_ADD");
          finger_led = false;
          finger_state = 0;
          finger_add = true;
          break;
        case 4: //F_CLEAR
          Serial.println("cmd: F_CLEAR");
          finger_clear = true;
          break;
        case 5: //F_CLEAR
          Serial.println("cmd: F_CHECK");
          finger_check = true;
          fLed2 = false;
          break;
        case 6: //C_RST
          Serial.println("cmd: C_RST");
          finger_add = false;
          finger_clear = false;
          finger_check = false;
          break;
        case 7: //C_TEST
          Serial.println("cmd: C_TEST");
          lamp_color(y);
          break;
        case 8: //S_RST
          log("Reseteando ESP32...");
          delay(500);
          ESP.restart();
          break;
        default:
            // Serial.println("cmd x");
          break;
        }
}

//Obtiene el resultado de la prueba segun torreta
void get_test_rst(){
  state2 = get_state();
  if(state2 != state){
    log ("Estado: " + String(state2));
    state = state2;
    switch(state2){
      case 0: //Idle
        lamp_color(off);
        log ("Idle");
        break;
      case 1: //Pieza OK
        bloquear = false;
        lamp_color(g);
        log("Pieza OK");
        // client->text("R-OK");
        // client.write("R-OK");  
        break;
      case 2: //Realizando prueba
        bloquear = true;
        lamp_color(y);
        log("Haciendo Prueba");
        break;
      case 4: //Pieza NG
        lamp_color(r);
        log("Pieza NG");
        finger_check = true;
        // client->text("R-NG");
        // client.write("R-NG"); 
        // bloquear = true; 
        break;
    }
    //state = 0;
  }
}