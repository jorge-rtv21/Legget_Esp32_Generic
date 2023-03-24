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

// #include "LedBlink.hpp"

void WsMessage(String msg, String icon, String Type);
// -------------------------------------------------------------------
// Sensor Temp Interno CPU
// -------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif
    uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

// -------------------------------------------------------------------
// Genera un log en el puerto Serial
// -------------------------------------------------------------------
// void log(String s){
//     Serial.println(s);
// }
// -------------------------------------------------------------------
// Definir la Plataforma
// -------------------------------------------------------------------
String platform(){
// Optiene la plataforma de hardware
#ifdef ARDUINO_ESP32_DEV
    return "ESP32_V2";
#endif
}
// -------------------------------------------------------------------
// Convierte un char a IP
// -------------------------------------------------------------------
IPAddress CharToIP(const char *str){
    sscanf(str, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
    return IPAddress(ip[0], ip[1], ip[2], ip[3]);
}
// -------------------------------------------------------------------
// Retorna segundos como "d:hh:mm:ss"
// -------------------------------------------------------------------
String longTimeStr(const time_t &t){        
    String s = String(t / SECS_PER_DAY) + ':';
    if (hour(t) < 10)
    {
        s += '0';
    }
    s += String(hour(t)) + ':';
    if (minute(t) < 10)
    {
        s += '0';
    }
    s += String(minute(t)) + ':';
    if (second(t) < 10)
    {
        s += '0';
    }
    s += String(second(t));
    return s;
}
// -------------------------------------------------------------------
// Retorna IPAddress en formato "n.n.n.n" de IP a String
// -------------------------------------------------------------------
String ipStr(const IPAddress &ip){    
    String sFn = "";
    for (byte bFn = 0; bFn < 3; bFn++)
    {
        sFn += String((ip >> (8 * bFn)) & 0xFF) + ".";
    }
    sFn += String(((ip >> 8 * 3)) & 0xFF);
    return sFn;
}
// -------------------------------------------------------------------
// De HEX a String
// -------------------------------------------------------------------
// String hexStr(const unsigned long &h, const byte &l = 8){
//     String s;
//     s = String(h, HEX);
//     s.toUpperCase();
//     s = ("00000000" + s).substring(s.length() + 8 - l);
//     return s;
// }
// -------------------------------------------------------------------
// Crear un ID unico desde la direccion MAC
// -------------------------------------------------------------------
String idUnique(){
    // Retorna los ultimos 4 Bytes del MAC rotados
    char idunique[15]; 
    uint64_t chipid = ESP.getEfuseMac();           
    uint16_t chip = (uint16_t)(chipid >> 32);
    snprintf(idunique, 15, "%04X", chip);
    return idunique;
}
// -------------------------------------------------------------------
// ID del Dispositivo para La Base de Datos
// -------------------------------------------------------------------
const String device_id = hexStr(ESP.getEfuseMac()) + "CE" + String(idUnique()); 
// -------------------------------------------------------------------
// ID del Dispositivo para La Base de Datos
// -------------------------------------------------------------------
String deviceID(){
    return String(platform()) + hexStr(ESP.getEfuseMac()) + String(idUnique());
}
// -------------------------------------------------------------------
// Configurar los Pines de Salida
// -------------------------------------------------------------------
// void settingPines(){
//     // Pines de Salida
//     pinMode(WIFILED, OUTPUT);
//     pinMode(MQTTLED, OUTPUT);
//     pinMode(RELAY1, OUTPUT);  
//     pinMode(RELAY2, OUTPUT);  
//     // Los iniciamos en nivel bajo
//     setOffSingle(RELAY1);
//     setOffSingle(RELAY2); 
//     setOffSingle(WIFILED); 
//     setOffSingle(MQTTLED);             
// }
// -------------------------------------------------------------------
// Parpadeo LED MQTT Transmisión
// -------------------------------------------------------------------
// void mqttTX(){ 
//     for (int i = 0; i < 2; i++){
//         setOnSingle(MQTTLED);
//         delay(50);
//         setOffSingle(MQTTLED);
//         delay(10);
//     }  
// }
// -------------------------------------------------------------------
// Parpadeo LED MQTT Recepción
// -------------------------------------------------------------------
// void mqttRX(){
//     for (int i = 0; i < 1; i++){
//         blinkRandomSingle(5,50,MQTTLED);
//         delay(5);
//     }
// }
// -------------------------------------------------------------------
// Parpadeo de los LEDs WIFI & MQTT
// -------------------------------------------------------------------
// void leds(){
//     for(int i=0; i < 4; i++) { 
//         setOnSingle(MQTTLED);
//         setOnSingle(WIFILED);
//         delay(100);
//         setOffSingle(MQTTLED);
//         setOffSingle(WIFILED);
//         delay(100);
//     }
// }
// -------------------------------------------------------------------
// Retorna la calidad de señal WIFI en %
// -------------------------------------------------------------------
int getRSSIasQuality(int RSSI){
    int quality = 0;
    if(RSSI <= -100){
        quality = 0;
    } else if(RSSI >= -50){
        quality = 100;
    } else{
       quality = 2 * (RSSI + 100); 
    }
    return quality;
}
// -------------------------------------------------------------------
// Retorna la temperatura del CPU
// -------------------------------------------------------------------
float TempCPUValue (){
    return temp_cpu = (temprature_sens_read() - 32) / 1.8;
}
// -------------------------------------------------------------------
// Retorna el listado de todos los archivos en el SPIFFS /
// -------------------------------------------------------------------
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        log("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        log(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            log("  DIR : " + String(file.name()));
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            log("  FILE : " + String(file.name()));
            log("\tSIZE : " + String(file.size()));
        }
        file = root.openNextFile();
    }
    log("  Total space: " + String(SPIFFS.totalBytes()));
    log("  Total space used:  " + String(SPIFFS.usedBytes()));
}
// -------------------------------------------------------------------
// Convierte un char a IP
// -------------------------------------------------------------------
IPAddress StrIP(const char *str){
    sscanf(str, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
    return IPAddress(ip[0], ip[1], ip[2], ip[3]);
}
// -------------------------------------------------------------------
// Nos Devuelve un String con el SweetAlert2
// -------------------------------------------------------------------
String SweetAlert(String TitleWeb, String SweetTitle, String SweetText, String SweetIcon, String type){
    String SweetAlert;
    if(type == "aviso"){
        SweetAlert = "<head>"
                        "<html><meta charset='UTF-8'>"
                        "<title>AdminTools | "+ TitleWeb +"</title>"
                        "<meta content='width=device-width, initial-scale=1.0' name='viewport' />"
                        "<link rel='icon' href='img/esp32.png' type='image/png'>"
                        "<link rel='stylesheet' href='css/bootstrap.css'>"
                        "<link rel='stylesheet' href='css/font-aws.css'>"
                        "<link rel='stylesheet' href='css/vivify.css'>"
                        "<link rel='stylesheet' href='css/sweetalert.css'>"
                        "<script src='js/libscripts.js'></script>"
                        "<script src='js/vendorscripts.js'></script>"
                        "<script src='js/sweetalert.js'></script>"
                    "</head>"
                    "<body>"
                    "<script>"
                        "Swal.fire({title: '"+SweetTitle+"!',"
                                        " text: '"+SweetText+"',"
                                        " icon: '"+SweetIcon+"',"
                                        " confirmButtonText: 'Cerrar'}).then((result) => {"
                                        "if (result.isConfirmed){"
                                            "history.go(-1);"
                                        "};"
                                    "})"
                    "</script>"
                    "<body>"
                    "</html>";
    }else if(type == "accion"){
        SweetAlert = "<head>"
                        "<html><meta charset='UTF-8'>"
                        "<title>AdminTools | "+ TitleWeb +"</title>"
                        "<meta content='width=device-width, initial-scale=1.0' name='viewport' />"
                        "<link rel='icon' href='img/esp32.png' type='image/png'>"
                        "<link rel='stylesheet' href='css/bootstrap.css'>"
                        "<link rel='stylesheet' href='css/font-aws.css'>"
                        "<link rel='stylesheet' href='css/vivify.css'>"
                        "<link rel='stylesheet' href='css/sweetalert.css'>"
                        "<script src='js/libscripts.js'></script>"
                        "<script src='js/vendorscripts.js'></script>"
                        "<script src='js/sweetalert.js'></script>"
                    "</head>"
                    "<body>"
                    "<script>"
                        "Swal.fire({title: '"+SweetTitle+"!',"
                        " text: '"+SweetText+"',"
                        " icon: '"+SweetIcon+"',"
                        " showCancelButton: true,"
                        " confirmButtonColor: 'rgb(65, 184, 130)',"
                        " cancelButtonColor: 'rgb(255, 118, 116)',"
                        " confirmButtonText: 'Si, reiniciar',"
                        " cancelButtonText: 'Cancelar',"
                        " reverseButtons: true"
                        " }).then((result) => {"
                                    "if (result.isConfirmed){"
                                        "window.location = 'esp-restart';"
                                        "}else if ("
                                        "result.dismiss === Swal.DismissReason.cancel"
                                        "){"
                                        "history.go(-1);"
                                        "}"
                                    "})"
                    "</script>"
                    "<body>"
                    "</html>";
    }else{
        SweetAlert = "<head>"
                        "<html><meta charset='UTF-8'>"
                        "<title>AdminTools | "+ TitleWeb +"</title>"
                        "<meta content='width=device-width, initial-scale=1.0' name='viewport' />"
                        "<link rel='icon' href='img/esp32.png' type='image/png'>"
                        "<link rel='stylesheet' href='css/bootstrap.css'>"
                        "<link rel='stylesheet' href='css/font-aws.css'>"
                        "<link rel='stylesheet' href='css/vivify.css'>"
                        "<link rel='stylesheet' href='css/sweetalert.css'>"
                        "<script src='js/libscripts.js'></script>"
                        "<script src='js/vendorscripts.js'></script>"
                        "<script src='js/sweetalert.js'></script>"
                    "</head>"
                    "<body>"
                    "<script>"
                        "Swal.fire({title: '"+SweetTitle+"!',"
                                        " text: '"+SweetText+"',"
                                        " icon: '"+SweetIcon+"',"
                                        " confirmButtonText: 'Cerrar'}).then((result) => {"
                                        "if (result.isConfirmed){"
                                            "window.location = '/';"
                                        "};"
                                    "})"
                    "</script>"
                    "<body>"
                    "</html>";
    }
    return SweetAlert;
}

// -------------------------------------------------------------------
// Control de los Relay desde MQTT & WS
// -------------------------------------------------------------------

boolean settingsSaveRelays();

// void OnOffRelays(String command){

//     DynamicJsonDocument JsonDoc(1024);

//     deserializeJson(JsonDoc, command);
    
//     if(JsonDoc["protocol"] == "WS"){
//         log("Info: Commando por WS => " + command);
//     }else{
//         log("Info: Commando por MQTT => " + command);
//     }
    	
//     // serializeJsonPretty(JsonDoc, Serial);

//     // if (JsonDoc["value"]){
//     //     digitalWrite(JsonDoc["output"] == "RELAY1" ? RELAY1 : RELAY2, HIGH);
//     //     JsonDoc["output"] == "RELAY1" ? Relay01_status = HIGH : Relay02_status = HIGH ;
//     // }else {
//     //     digitalWrite(JsonDoc["output"] == "RELAY1" ? RELAY1 : RELAY2, LOW);
//     //     JsonDoc["output"] == "RELAY1" ? Relay01_status = LOW : Relay02_status = LOW ;
//     // }

//     // settingsSaveRelays();    

// }
// -------------------------------------------------------------------
// Retorna el Tipo de Encriptacion segun el codigo (0-1-2-3-4-5)
// -------------------------------------------------------------------
String EncryptionType(int encryptionType) {
  switch (encryptionType) {
    case (0):
      return "Open";
    case (1):
      return "WEP";
    case (2):
      return "WPA_PSK";
    case (3):
      return "WPA2_PSK";
    case (4):
      return "WPA_WPA2_PSK";
    case (5):
      return "WPA2_ENTERPRISE";
    default:
      return "UNKOWN";
    }
}

// -------------------------------------------------------------------
// Empaquetar el JSON para enviar por WS ( progress en % y Actividad MQTT )
// -------------------------------------------------------------------
// String getSendJson(String msg, String type){
//     String response = "";
//     StaticJsonDocument<300> doc;
//     doc["type"] = type;
//     doc["msg"] = msg;
//     serializeJson(doc, response);
//     return response;
// }

// -------------------------------------------------------------------
// Print Progress Firmware or SPIFFS Update
// -------------------------------------------------------------------
int c = 0;
void printProgress(size_t prog, size_t sz){
    int progress = (prog * 100) / content_len; 
    switch (progress){
        case 10:
            c ++;
            if(c>=2) c=1;
            if(c==1){
                // WsMessage(getSendJson(String(progress), "update"), "", "");
            }
            break;
        case 20:
            c ++;
            if(c>=2) c=1;
            if(c==1){
                // WsMessage(getSendJson(String(progress), "update"), "", "");
            }
            break;
        case 30:
            c ++;
            if(c>=2) c=1;
            if(c==1){
                // WsMessage(getSendJson(String(progress), "update"), "", "");
            }
            break;
        case 40:
            c ++;
            if(c>=2) c=1;
            if(c==1){
                // WsMessage(getSendJson(String(progress), "update"), "", "");
            }
            break;
        case 50:
            c ++;
            if(c>=2) c=1;
            if(c==1){
                // WsMessage(getSendJson(String(progress), "update"), "", "");
            }
            break;
        case 60:
            c ++;
            if(c>=2) c=1;
            if(c==1){
                // WsMessage(getSendJson(String(progress), "update"), "", "");
            }
            break;
        case 70:
            c ++;
            if(c>=2) c=1;
            if(c==1){
                // WsMessage(getSendJson(String(progress), "update"), "", "");
            }
            break;
        case 80:
            c ++;
            if(c>=2) c=1;
            if(c==1){
                // WsMessage(getSendJson(String(progress), "update"), "", "");
            }
            break;
        case 90:
            c ++;
            if(c>=2) c=1;
            if(c==1){
                // WsMessage(getSendJson(String(progress), "update"), "", "");
            }
            break;
        case 98:
            c ++;
            if(c>=2) c=1;
            if(c==1){
                // WsMessage(getSendJson(String(100), "update"), "", "");
            }
            break;        
        } 
    Serial.printf("Info: Progreso de la Actualización al : %d%%\n", progress);
}


// void printColors(){
//     Serial.print("Black R = "); Serial.print (String(black_R));
//     Serial.print("\tBlack G = "); Serial.print (String(black_G));
//     Serial.print("\tBlack B = "); Serial.print (String(black_B));
//     Serial.print("\tBlack T = "); Serial.println (String(black_T));

//     Serial.print("Blue R = "); Serial.print (String(blue_R));
//     Serial.print("\tBlue G = "); Serial.print (String(blue_G));
//     Serial.print("\tBlue B = "); Serial.print (String(blue_B));
//     Serial.print("\tBlue T = "); Serial.println (String(blue_T));

//     Serial.print("Green R = "); Serial.print (String(green_R));
//     Serial.print("\tGreen G = "); Serial.print (String(green_G));
//     Serial.print("\tGreen B = "); Serial.print (String(green_B));
//     Serial.print("\tGreen T = "); Serial.println (String(green_T));

//     Serial.print("Red R = "); Serial.print (String(red_R));
//     Serial.print("\tRed G = "); Serial.print (String(red_G));
//     Serial.print("\tRed B = "); Serial.print (String(red_B));
//     Serial.print("\tRed T = "); Serial.println (String(red_T));
// }

// -------------------------------------------------------------------
// Funcion similar a delay, pero sin detener procesos
// -------------------------------------------------------------------
// bool wait(unsigned long time){
//     bool ok = false;
//     currentTime=millis();
//     if((currentTime-previousTime) > time){
//         previousTime=currentTime;
//         ok = true;
//         }
//     return ok;
// }

// String processor(const String& var){
// //   getSensorReadings();
//   //Serial.println(var);
//   if(var == "sC_lectura"){
//     return String(sC_lectura);
//   }
//   return String();
// }