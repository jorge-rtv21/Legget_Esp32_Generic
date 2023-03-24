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
// Leer configuraciones de los Parametros WIFI
// -------------------------------------------------------------------
// boolean settingsReadWiFi(){
    
//     //StaticJsonDocument<768> jsonConfig;    
//     StaticJsonDocument<capacitywifi> jsonConfig;

//     File file = SPIFFS.open("/settingwifi.json", "r");
//     if(deserializeJson(jsonConfig, file)){
//         // Si falla la lectura inicia valores por defecto
//         settingsResetWiFi();
//         log("Error: Falló la lectura de la configuración WiFi, tomando valores por defecto");
//         return false;
//     }else{
//         /* ------------------- GENERAL -------------------- */
//         strlcpy(id, jsonConfig["id"], sizeof(id)); 
//         bootCount = jsonConfig["boot"];
//         /* ------------------- CLIENTE -------------------- */
//         wifi_staticIP = jsonConfig["wifi_staticIP"];
//         strlcpy(wifi_ssid, jsonConfig["wifi_ssid"], sizeof(wifi_ssid));
//         strlcpy(wifi_pass, jsonConfig["wifi_passw"], sizeof(wifi_pass));        
//         strlcpy(wifi_ip_static, jsonConfig["wifi_ip_static"], sizeof(wifi_ip_static));
//         strlcpy(wifi_gateway, jsonConfig["wifi_gateway"], sizeof(wifi_gateway));
//         strlcpy(wifi_subnet, jsonConfig["wifi_subnet"], sizeof(wifi_subnet));
//         strlcpy(wifi_primaryDNS, jsonConfig["wifi_primaryDNS"], sizeof(wifi_primaryDNS));
//         strlcpy(wifi_secondaryDNS, jsonConfig["wifi_secondaryDNS"], sizeof(wifi_secondaryDNS));
//         /* ------------------- AP ------------------------ */
//         ap_AccessPoint = jsonConfig["ap_accessPoint"];
//         strlcpy(ap_nameAP, jsonConfig["ap_nameap"], sizeof(ap_nameAP));
//         strlcpy(ap_pass, jsonConfig["ap_passwordap"], sizeof(ap_pass));
//         ap_canalAP = jsonConfig["azp_canalap"];
//         ap_hiddenAP = jsonConfig["ap_hiddenap"];
//         ap_connAP = jsonConfig["ap_connetap"];
//         file.close();
//         log("Info: Lectura configuración WiFi correcta");
//         return true;
//     }
// }
// -------------------------------------------------------------------
// Leer configuraciones de los Parametros MQTT
// -------------------------------------------------------------------
// boolean settingsReadMQTT(){

//     // StaticJsonDocument<384> jsonConfig;
//     StaticJsonDocument<capacitymqtt> jsonConfig;

//     File file = SPIFFS.open(F("/settingmqtt.json"), "r");
//     if (deserializeJson(jsonConfig, file)){
//         // Si falla la lectura asume valores por defecto
//         settingsResetMQTT();
//         log(F("Error: Falló la lectura de la configuración MQTT, tomando valores por defecto"));
//         return false;
//     }else{
//         // Si lee el archivo
//         strlcpy(mqtt_user, jsonConfig["mqtt_user"], sizeof(mqtt_user));
//         strlcpy(mqtt_passw, jsonConfig["mqtt_passw"], sizeof(mqtt_passw));
//         strlcpy(mqtt_server, jsonConfig["mqtt_server"], sizeof(mqtt_server));
//         strlcpy(mqtt_id, jsonConfig["mqtt_id"], sizeof(mqtt_id));
//         mqtt_time = jsonConfig["mqtt_time"];
//         mqtt_port = jsonConfig["mqtt_port"];
//         mqtt_enable = jsonConfig["mqtt_enable"];
//         file.close();
//         log(F("Info: Lectura de configuración MQTT correcta"));
//         return true;
//     }
// }
// -------------------------------------------------------------------
// Leer estados de los Relays
// -------------------------------------------------------------------
// boolean settingsReadRelays(){

//     //StaticJsonDocument<96> jsonConfig;
//     StaticJsonDocument<capacityrelays> jsonConfig;
    
//     File file = SPIFFS.open("/settingrelays.json", "r");
//     if (deserializeJson(jsonConfig, file)){
//         // Si falla la lectura inicia valores por defecto
//         settingsResetRelays();
//         log("Error: Falló la lectura del estados de los Relays, tomando valores por defecto");
//         return false;
//     }else{        
//         Relay01_status = jsonConfig["Relay01_status"];
//         Relay02_status = jsonConfig["Relay02_status"];
//         file.close();
//         log("Info: Lectura de los Relay correcta");
//         return true;
//     } 
// }
// -------------------------------------------------------------------
// Leer www_username/password
// -------------------------------------------------------------------
boolean settingsReadAdmin(){
    // Lee el Usuario y Contraseña
    StaticJsonDocument<capacityadmin> jsonConfig;
    File file = SPIFFS.open("/settingadmin.json", "r");
    if (deserializeJson(jsonConfig, file)){
        // Si falla la lectura inicia valores por defecto
        settingsResetAdmin();
        log("Error: Falló la lectura del Usuario y Contraseña, tomando valores por defecto");
        return false;
    }else{
        /* ---------- Usuario y Contraseña ------------ */
        strlcpy(www_username, jsonConfig["www_username"], sizeof(www_username));
        strlcpy(www_password, jsonConfig["www_password"], sizeof(www_password));
        file.close();
        log("Info: Lectura del Usuario y Contraseña WWW correcta");
        return true;
    }
}
// -------------------------------------------------------------------
// Leer colores
// -------------------------------------------------------------------
// boolean settingsReadColors(){
//     StaticJsonDocument<capacitycolors> jsonConfig;

//     File file = SPIFFS.open("/settingcolors.json", "r");
//     if(deserializeJson(jsonConfig, file)){
//         // Si falla la lectura inicia valores por defecto
//         settingsResetColors();
//         log("Error: Falló la lectura de los colores, tomando valores por defecto");
//         return false;
//     }else{
//         /* ------------------- GENERAL -------------------- */
//         black_R = jsonConfig["black_R"];
//         black_G = jsonConfig["black_G"];
//         black_B = jsonConfig["black_B"];
//         black_T = jsonConfig["black_T"];

//         blue_R = jsonConfig["blue_R"];
//         blue_G = jsonConfig["blue_G"];
//         blue_B = jsonConfig["blue_B"];
//         blue_T = jsonConfig["blue_T"];

//         green_R = jsonConfig["green_R"];
//         green_G = jsonConfig["green_G"];
//         green_B = jsonConfig["green_B"];
//         green_T = jsonConfig["green_T"];

//         red_R = jsonConfig["red_R"];
//         red_G = jsonConfig["red_G"];
//         red_B = jsonConfig["red_B"];
//         red_T = jsonConfig["red_T"];

//         sC_time = jsonConfig["sC_time"];
//         sC_samples= jsonConfig["sC_samples"];

//         file.close();
//         log("Info: Lectura configuración de colores correcta");
//         //printColors();
//         return true;
//     }
// }