/* -------------------------------------------------------------------
 * AdminESP - ElectronicIOT 2022
 * Sitio WEB: https://electroniciot.com
 * Correo: admim@electroniciot.com
 * Cel_WSP: +591 71243395
 * Plataforma: ESP32
 * Framework:  Arduino
 * Proyecto Admin Panel Tool para el ESP32 con HTNL, JavaScript, CSS
 * ESP32 Admin Tool v2.0
 * -------------------------------------------------------------------
*/

#include "ESPAsyncWebServer.h"

// -------------------------------------------------------------------
// Instancia de AsyncWebServer puerto 80
// -------------------------------------------------------------------
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
// -------------------------------------------------------------------
// Declaracion de funciones
// -------------------------------------------------------------------
void ProcessRequest(AsyncWebSocketClient * client, String request);

// -------------------------------------------------------------------
// Variables generales WS
// -------------------------------------------------------------------
long lastWsSend = 0;           // Variable para tiempo de envio por WS

// -------------------------------------------------------------------
// Eventos del Websocket
// -------------------------------------------------------------------
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){ 
	if(type == WS_EVT_CONNECT){
		Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
		client->printf("{\"ClientID\": %u }", client->id());
		client->ping();
	} else if(type == WS_EVT_DISCONNECT){
		//Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
	} else if(type == WS_EVT_ERROR){
		//Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
	} else if(type == WS_EVT_PONG){
		//Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
	} else if(type == WS_EVT_DATA){
		AwsFrameInfo * info = (AwsFrameInfo*)arg;
		String msg = "";
		if(info->final && info->index == 0 && info->len == len){
			if(info->opcode == WS_TEXT){
				for(size_t i=0; i < info->len; i++) {
					msg += (char) data[i];
				}
			} else {
				char buff[3];
				for(size_t i=0; i < info->len; i++) {
					sprintf(buff, "%02x ", (uint8_t) data[i]);
					msg += buff ;
				}
			}

			if(info->opcode == WS_TEXT)
			   ProcessRequest(client, msg);
			
		} else {
			//message is comprised of multiple frames or the frame is split into multiple packets
			if(info->opcode == WS_TEXT){
				for(size_t i=0; i < len; i++) {
					msg += (char) data[i];
				}
			} else {
				char buff[3];
				for(size_t i=0; i < len; i++) {
					sprintf(buff, "%02x ", (uint8_t) data[i]);
					msg += buff ;
				}
			}
			Serial.printf("%s\n",msg.c_str());

			if((info->index + len) == info->len){
				if(info->final){
					if(info->message_opcode == WS_TEXT)
					   ProcessRequest(client, msg);
				}
			}
		}
	}
}

// -------------------------------------------------------------------
// Inicializar el Websocket
// -------------------------------------------------------------------
void InitWebSockets(){
	ws.onEvent(onWsEvent);
	server.addHandler(&ws);
    log(F("Info: WebSocket server iniciado"));
}
// -------------------------------------------------------------------
// Manejador de ordenes enviada por Websocket
// -------------------------------------------------------------------
void ProcessRequest(AsyncWebSocketClient * client, String request){
   	String command = request;
	log ("Command Ws: " + command);
	command.trim();    
	if(strcmp(command.c_str(), "restore") == 0){
		log("Info: Commando por WS => " + command);
		settingsResetWiFi();
		settingsResetMQTT();
		settingsResetAdmin();
		settingsResetRelays();
		settingsResetColors();
		if(settingsSaveWiFi() && settingsSaveMQTT() && settingsSaveAdmin() && settingsSaveRelays() && settingsSaveColors()){
			WsMessage("¡Equipo restablecido correctamente!","success","info");
			log("Info: ¡Equipo restablecido correctamente!");
			//vTaskDelay(1000);
			Serial.flush();
			ESP.restart();
		}		
	}else if (strcmp(command.c_str(), "restart") == 0){
		log("Info: Commando por WS => " + command);
		WsMessage("¡Equipo reiniciado correctamente!","success","info");
		log("Info: ¡Equipo reiniciado correctamente!");
		//vTaskDelay(1000);
		Serial.flush(); 		
		ESP.restart(); 
	}else if (strcmp(command.c_str(), "huella") == 0){
		log("Info: Lleer Huella");
		log("Command: " + String(command));
		//OnOffRelays(command); 
	}else if (strcmp(command.c_str(), "sC_read") == 0){
		log("Info: Inicializar lectura del sensor de Color");
		log("Command: " + String(command));
		// log("Muestras: " + String(sC_samples) + "\tTiempo: " + String(sC_time));
		events.send(String("").c_str(),"sC_clear",millis());
		muestreo(sC_samples, sC_time);
	}
	else if (strcmp(command.c_str(), "sC_clear") == 0){
		log("Info: Limpiando monitor serial");
		log("Command: " + String(command));
		events.send(String("").c_str(),"sC_clear",millis());
	}
	else if (strcmp(command.c_str(), "fP_add") == 0){
		log("Info: Agregar huella");
		log("Command: " + String(command));
		events.send(String("").c_str(),"fP_clear",millis());
		finger_add=true;
		finger_state = 0;
	}
	else if (strcmp(command.c_str(), "fP_check") == 0){
		log("Info: Revisar huella");
		log("Command: " + String(command));
		events.send(String("").c_str(),"fP_clear",millis());
		finger_check = true;
	}
	else if (strcmp(command.c_str(), "fP_clear") == 0){
		log("Info: Borrar huellas");
		log("Command: " + String(command));
		events.send(String("").c_str(),"fP_clear",millis());
		finger_clear = true;
	}
		
    // TODO: Agregar cuando sea restart
	
	
	// if(command != "restore" && command != "restart" && command != "huella" && command != "color" && command != "sC_read" && command != "sC_clear"){
	if(strcmp(command.c_str(), "Relays") == 0){
		log("Info: Relays");
		OnOffRelays(command);
	} 
	
	
}
// -------------------------------------------------------------------
// Función enviar JSON por Websocket 
// -------------------------------------------------------------------
void WsMessage(String msg, String icon, String Type){
  	if(strcmp(Type.c_str(), "info") == 0){
		String response;
		StaticJsonDocument<300> doc;
		doc["type"] = Type;
		doc["msg"] = msg;
		doc["icon"] = icon;
		serializeJson(doc, response);
		ws.textAll(response);
	}else{
		ws.textAll(msg);
	}
}
// -------------------------------------------------------------------
// Empaquetar el JSON para enviar por WS
// -------------------------------------------------------------------
String GetJson(){
	String response;
	DynamicJsonDocument jsonDoc(3000);
	jsonDoc["type"] = "data";
	jsonDoc["wifi_status"] =  WiFi.status() == WL_CONNECTED ? F("CONECTADO") : F("DESCONECTADO");
	jsonDoc["wifi_dbm"] = WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0;
	jsonDoc["wifi_percent"] = WiFi.status() == WL_CONNECTED ? getRSSIasQuality(WiFi.RSSI()) : 0; 
	jsonDoc["temp_cpu"] = String(round(TempCPUValue()),2);  
	jsonDoc["ram_available"] = ESP.getFreeHeap() * 100 / ESP.getHeapSize(); 
	jsonDoc["spiffs_used"] = SPIFFS.usedBytes() * 100 / SPIFFS.totalBytes();
	jsonDoc["mqtt_status"] = mqttclient.connected() ? F("CONECTADO") : F("DESCONECTADO");
	jsonDoc["mqtt_server"] = mqttclient.connected() ? F(mqtt_server) : F("server not connected");
	jsonDoc["active"] = longTimeStr(millis() / 1000);
	jsonDoc["relay1_status"] = Relay01_status ? true : false;
	jsonDoc["relay2_status"] = Relay02_status ? true : false;
	serializeJson(jsonDoc, response);
	return response;
}

