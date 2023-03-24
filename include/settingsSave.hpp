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

// -------------------------------------------------------------------
// Guardar los estados de los Relays
// -------------------------------------------------------------------
// boolean settingsSaveRelays(){
//     // Guarda configuración de los Relays
//     StaticJsonDocument<capacityrelays> jsonConfig;
//     File file = SPIFFS.open(F("/settingrelays.json"), "w+");
//     if (file){
//         jsonConfig["Relay01_status"] = Relay01_status;
//         jsonConfig["Relay02_status"] = Relay02_status;
//         serializeJsonPretty(jsonConfig, file);
//         file.close();
//         log(F("Info: Estado de los Relay guardado"));
//         //serializeJsonPretty(jsonConfig, Serial);
//         return true;
//     }else{
//         log(F("Error: Falló el guardado del estado de los Relay"));
//         return false;
//     }
// }
// -------------------------------------------------------------------
// Guardar las configuraciones del WIFI y Generales
// -------------------------------------------------------------------
// boolean settingsSaveWiFi(){
//     // Graba configuración del WiFi y General
//     StaticJsonDocument<capacitywifi> jsonConfig;
//     File file = SPIFFS.open("/settingwifi.json", "w+");
//     if (file){
//         /* ------------------- GENERAL -------------------- */
//         jsonConfig["id"] = id;
//         jsonConfig["boot"] = bootCount;
//         /* ------------------- CLIENTE -------------------- */
//         jsonConfig["wifi_staticIP"] = wifi_staticIP;
//         jsonConfig["wifi_ssid"] = wifi_ssid;
//         jsonConfig["wifi_passw"] = wifi_pass;
//         jsonConfig["wifi_ip_static"] = wifi_ip_static;
//         jsonConfig["wifi_gateway"] = wifi_gateway;
//         jsonConfig["wifi_subnet"] = wifi_subnet;
//         jsonConfig["wifi_primaryDNS"] = wifi_primaryDNS;
//         jsonConfig["wifi_secondaryDNS"] = wifi_secondaryDNS;
//         /* ------------------- AP ------------------------- */
//         jsonConfig["ap_accessPoint"] = ap_AccessPoint;
//         jsonConfig["ap_nameap"] = ap_nameAP;
//         jsonConfig["ap_passwordap"] = ap_pass;
//         jsonConfig["ap_canalap"] = ap_canalAP;
//         jsonConfig["ap_hiddenap"] = ap_hiddenAP;
//         jsonConfig["ap_connetap"] = ap_connAP;
//         serializeJsonPretty(jsonConfig, file);
//         file.close();
//         log("Info: Configuración WiFi Guardada");
//         //serializeJsonPretty(jsonConfig, Serial);   
//         return true;
//     }else{
//         log("Error: Falló el guardado de la configuración WiFI");
//         return false;
//     }    
// }
// -------------------------------------------------------------------
// Guardar las configuraciones www_username/password
// -------------------------------------------------------------------
boolean settingsSaveAdmin(){
    // Guarda configuración Usuario y Contraseña
    StaticJsonDocument<capacityadmin> jsonConfig;
    File file = SPIFFS.open(F("/settingadmin.json"), "w+");
    if (file){
        jsonConfig["www_username"] = www_username;
        jsonConfig["www_password"] = www_password;
        serializeJsonPretty(jsonConfig, file);
        file.close();
        log(F("Info: Usuario y Contraseña guardados"));
        //serializeJsonPretty(jsonConfig, Serial);
        return true;
    }else{
        log(F("Error: Falló el guardado del Usuario y Contraseña"));
        return false;
    }
}
// -------------------------------------------------------------------
// Guardar las configuraciones del Broker MQTT
// -------------------------------------------------------------------
// boolean settingsSaveMQTT(){
//   // Guarda configuración MQTT
//   StaticJsonDocument<capacitymqtt> jsonConfig;
//   File file = SPIFFS.open(F("/settingmqtt.json"), "w+");
//   if (file){
//     jsonConfig["mqtt_user"] = mqtt_user;
//     jsonConfig["mqtt_passw"] = mqtt_passw;
//     jsonConfig["mqtt_server"] = mqtt_server;
//     jsonConfig["mqtt_id"] = mqtt_id;
//     jsonConfig["mqtt_time"] = mqtt_time;
//     jsonConfig["mqtt_port"] = mqtt_port;
//     jsonConfig["mqtt_enable"] = mqtt_enable;
//     serializeJsonPretty(jsonConfig, file);
//     file.close();
//     log(F("Info: Configuración MQTT guardada"));
//     //serializeJsonPretty(jsonConfig, Serial);
//     return true;
//   }else{
//     log(F("Error: Falló el guardado de la configuración del MQTT"));
//     return false;
//   }    
// }
// -------------------------------------------------------------------
// Guardar las configuraciones de los valores de los colores
// -------------------------------------------------------------------
// boolean settingsSaveColors(){
//   // Guarda configuración de los colores a detectar
//   StaticJsonDocument<capacitycolors> jsonConfig;
//   File file = SPIFFS.open(F("/settingcolors.json"), "w+");
//   if (file){
//     jsonConfig["black_R"] = black_R;
//     jsonConfig["black_G"] = black_G;
//     jsonConfig["black_B"] = black_B;
//     jsonConfig["black_T"] = black_T;

//     jsonConfig["blue_R"] = blue_R;
//     jsonConfig["blue_G"] = blue_G;
//     jsonConfig["blue_B"] = blue_B;
//     jsonConfig["blue_T"] = blue_T;

//     jsonConfig["green_R"] = green_R;
//     jsonConfig["green_G"] = green_G;
//     jsonConfig["green_B"] = green_B;
//     jsonConfig["green_T"] = green_T;
    
//     jsonConfig["red_R"] = red_R;
//     jsonConfig["red_G"] = red_G;
//     jsonConfig["red_B"] = red_B;
//     jsonConfig["red_T"] = red_T;
    
//     jsonConfig["sC_time"] = sC_time;
//     jsonConfig["sC_samples"] = sC_samples;

//     serializeJsonPretty(jsonConfig, file);
//     file.close();
//     log(F("Info: Configuración de colores guardada"));
//     //printColors();
//     //serializeJsonPretty(jsonConfig, Serial);
//     return true;
//   }else{
//     log(F("Error: Falló el guardado de la configuración de los colores"));
//     return false;
//   }    
// }
