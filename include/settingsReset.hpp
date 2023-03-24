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
// Valores por defecto Parametros WIFI
// -------------------------------------------------------------------
// void settingsResetWiFi(){
//     /* ------------------- GENERAL -------------------- */
//     strlcpy(id, "adminesp32", sizeof(id));
//     bootCount = 0;
//     /* ------------------- CLIENTE -------------------- */
//     wifi_staticIP = true;
//     strlcpy(wifi_ssid, "INFINITUM3604_2.4", sizeof(wifi_ssid));
//     strlcpy(wifi_pass, "Rnus4s4pWr", sizeof(wifi_pass));
//     strlcpy(wifi_ip_static, "192.168.1.16", sizeof(wifi_ip_static));
//     strlcpy(wifi_gateway, "192.168.1.1", sizeof(wifi_gateway));
//     strlcpy(wifi_subnet, "255.255.255.0", sizeof(wifi_subnet));
//     strlcpy(wifi_primaryDNS, "8.8.8.8", sizeof(wifi_primaryDNS));
//     strlcpy(wifi_secondaryDNS, "8.8.4.4", sizeof(wifi_secondaryDNS));
//     /* ------------------- AP ------------------------- */
//     ap_AccessPoint = false;
//     strlcpy(ap_nameAP, deviceID().c_str(), sizeof(ap_nameAP));
//     strlcpy(ap_pass, "adminesp32", sizeof(ap_pass));
//     ap_canalAP = 9;         
//     ap_hiddenAP = false;        
//     ap_connAP = 4;
// }
// -------------------------------------------------------------------
// Valores por defecto Parametros MQTT
// -------------------------------------------------------------------
// void settingsResetMQTT(){
//     // Define configuración por defecto del equipo conexión MQTT
//     strlcpy(mqtt_user, "cuba_iot", sizeof(mqtt_user));
//     strlcpy(mqtt_passw, "cuba_iot", sizeof(mqtt_passw));
//     strlcpy(mqtt_server, "cubaelectronica.com", sizeof(mqtt_server));
//     strlcpy(mqtt_id, device_id.c_str(), sizeof(mqtt_id));
//     mqtt_time = 60000;  // Un Minuto
//     mqtt_port = 1883;
//     mqtt_enable = false;
// }
// -------------------------------------------------------------------
// Valores por defecto Relays
// -------------------------------------------------------------------

// -------------------------------------------------------------------
// Valores por defecto www_username/password
// -------------------------------------------------------------------
void settingsResetAdmin(){
    // Define configuración por defecto Usuario y Contraseña WWW
    strlcpy(www_username, "admin", sizeof(www_username));
    strlcpy(www_password, "admin", sizeof(www_password));
}
// -------------------------------------------------------------------
// Valores por defecto de los colores
// -------------------------------------------------------------------
// void settingsResetColors(){
//     // Define configuración por defecto de los valores de los colores
//     black_R = 0;    black_G = 0;    black_B = 0;    black_T = 10;
//     blue_R = 0;     blue_G = 0;     blue_B = 0;     blue_T = 10;
//     green_R = 0;    green_G = 0;    green_B = 0;    green_T = 10;
//     red_R = 0;      red_G = 0;      red_B = 0;      red_T = 10;
//     sC_time =250;   sC_samples=30;
// }