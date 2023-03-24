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

#include <Update.h>
// -------------------------------------------------------------------
// CORS
// -------------------------------------------------------------------
bool enableCors = true;
// -------------------------------------------------------------------
// Función de ayuda para realizar autenticación en una solicitud http
// -------------------------------------------------------------------
bool requestPreProcess(AsyncWebServerRequest *request, AsyncResponseStream *&response, const char *contentType = "application/json"){  
  if(!request->authenticate(www_username, www_password)) {
    request->requestAuthentication();
    return false;
  }
  response = request->beginResponseStream(contentType);
  return true;
}

// -------------------------------------------------------------------
// Cargar página Index.html o Home
// url: /
// Metodo: GET
// -------------------------------------------------------------------
void handleHome(AsyncWebServerRequest *request) {
    AsyncResponseStream *response;
    if(false == requestPreProcess(request, response)) {
        return;
    }      
    File file = SPIFFS.open(F("/index.html"), "r");
    if (file){
        file.setTimeout(100);
        String s = file.readString();
        file.close();
        // -------------------------------------------------------------------
        // Actualiza el contenido del html al cargar
        // -------------------------------------------------------------------
        s.replace(F("#id#"), id);
        s.replace(F("#serie#"), device_id);
        /* Bloque Inalámbrico */
        // s.replace(F("#wifi_status#"), WiFi.status() == WL_CONNECTED ? F("<span class='label label-success'>CONECTADO</span>") : F("<span class='label label-danger'>DESCONECTADO</span>"));
        // s.replace(F("#wifi_ssid#"), WiFi.status() == WL_CONNECTED ? F(wifi_ssid) : F("WiFi not connected"));
        // s.replace(F("#wifi_ip#"), ipStr(WiFi.status() == WL_CONNECTED ? WiFi.localIP() : WiFi.softAPIP()));
        // s.replace(F("#wifi_mac#"), String(WiFi.macAddress()));
        // s.replace(F("#wifi_dbm#"), WiFi.status() == WL_CONNECTED ? String(WiFi.RSSI()) : F("0"));
        // s.replace(F("#wifi_percent#"), WiFi.status() == WL_CONNECTED ? String(getRSSIasQuality(WiFi.RSSI())) : F("0"));
        /* Bloque MQTT */
        // s.replace(F("#mqtt_status#"), mqttclient.connected() ? F("<span class='label label-success'>CONECTADO</span>") : F("<span class='label label-danger'>DESCONECTADO</span>"));
        // s.replace(F("#mqtt_server#"), mqttclient.connected() ? F(mqtt_server) : F("server not connected"));
        // s.replace(F("#mqtt_user#"), F(mqtt_user));
        // s.replace(F("#mqtt_id#"), F(mqtt_id));
        /* Bloque Información */
        s.replace(F("#clientIP#"), ipStr(request->client()->remoteIP()));
        s.replace(F("#userAgent#"), request->getHeader("User-Agent")->value().c_str());
        s.replace(F("#firmware#"), currentfirmware);
        s.replace(F("#sdk#"), String(ESP.getSdkVersion()));
        s.replace(F("#hardware#"), F(HW));
        s.replace(F("#manufacture#"), F(MF));
        s.replace(F("#cpu_freq#"), String(getCpuFrequencyMhz()));
        s.replace(F("#flash_size#"), String(ESP.getFlashChipSize() / (1024.0 * 1024), 2));
        s.replace(F("#ram_size#"), String(ESP.getHeapSize() / 1024.0, 2));
        s.replace(F("#time_active#"), longTimeStr(millis() / 1000));
        s.replace(F("#bootCount#"), String(bootCount));
        /* Bloque General */
        s.replace(F("#platform#"), platform());
        // s.replace(F("#mqtt_on#"), mqttclient.connected() ? F("<span class='label btn-metis-2'>Online</span>") : F("<span class='label label-danger'>Offline</span>"));
        s.replace(F("#temp_cpu#"), String(TempCPUValue()));
        /* Bloque Progressbar */
        s.replace(F("#spiffs_used#"), String(round(SPIFFS.usedBytes() * 100 / SPIFFS.totalBytes()), 0));
        s.replace(F("#ram_available#"), String(ESP.getFreeHeap() * 100 / ESP.getHeapSize()));
        /* Botones Relays */
        // s.replace(F("#relay1#"), Relay01_status ? "checked" : "");
        // s.replace(F("#relay2#"), Relay02_status ? "checked" : "");
        // s.replace(F("#label1#"), Relay01_status ? "label-primary" : "label-info");
        // s.replace(F("#label2#"), Relay02_status ? "label-primary" : "label-info");
        // s.replace(F("#Statustext1#"), Relay01_status ? "Activo" : "Apagado");
        // s.replace(F("#Statustext2#"), Relay02_status ? "Activo" : "Apagado");
        /* Bloque de sensor de huella*/
        s.replace(F("#nFingers#"), fingerOK ? String(finger_templateCount) : "Sin Conexion");
        // Send Data
        request->send(200, "text/html", s);    
    }else{
        request->send(500, "text/plain","/index.html not found, have you flashed the SPIFFS?");
    }

}

// -------------------------------------------------------------------
// Método POST carga del los archivos de configuración OK
// -------------------------------------------------------------------
// Variables para la carga del Archivo
File file;
bool opened = false;
void handleDoUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    const char* dataType = "application/json";
    if (!index) {
        Serial.printf("Info: Carga iniciada: %s\n", filename.c_str());
    }
    if (opened == false) {
        opened = true;
        file = SPIFFS.open(String("/") + filename, FILE_WRITE);
        if (!file) {
            log("Error: No se pudo abrir el archivo para escribirlo");
            request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo abrir el archivo para escribir!\"}");
            return;
        }
    } 
    if (file.write(data, len) != len) {
        log("Error: No se pudo escribir el Archivo");
        request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo escribir el Archivo: " + filename + " !\"}");
        return;
    }
    if (final) {
        AsyncWebServerResponse *response = request->beginResponse(201, dataType, "{ \"save\": true, \"msg\": \"¡Carga del Archivo: " + filename + " completada!\"}");
        response->addHeader("Cache-Control","no-cache");
        response->addHeader("Location", "/");
        request->send(response);
        file.close();
        opened = false;
        log("Info: Carga del Archivo " + filename + " completada");
        // Esperar la Transmisión de los datos seriales
        Serial.flush(); 
        ESP.restart();
    }
}

// -------------------------------------------------------------------
// Método POST para la carga del nuevo Firmware OK o SPIFFS OK
// -------------------------------------------------------------------
void handleDoFirmware(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) { 
    const char* dataType = "application/json";
    // Si el nombre de archivo incluye ( spiffs ), actualiza la partición de spiffs
    int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
    String updateSystem = cmd == U_PART ? "FileSystem" : "Firmware";
    if (!index) {
        content_len = request->contentLength();    
        log("Info: Actualización del "+ updateSystem +" iniciada");        
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
            AsyncWebServerResponse *response = request->beginResponse(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo actualizar el "+ updateSystem +" Index: " + filename + " !\"}");
            request->send(response);
            Update.printError(Serial);
            log("Error: Update del "+ updateSystem +" ternimado");
        }
    }
    if (Update.write(data, len) != len) {
        Update.printError(Serial);
    }
    if (final) {        
        if (!Update.end(true)) {
            AsyncWebServerResponse *response = request->beginResponse(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo actualizar el "+ updateSystem +" Final: " + filename + " !\"}");
            request->send(response);
            Update.printError(Serial);
        } else {            
            AsyncWebServerResponse *response = request->beginResponse(201, dataType, "{ \"save\": true, \"type\": \""+updateSystem+"\"}");
            response->addHeader("Cache-Control","no-cache");
            response->addHeader("Location", "root@"+ updateSystem +"");
            request->send(response);
            log("Info: Update del " + updateSystem + " completado");
            // Esperar la Transmisión de los datos seriales
            Serial.flush(); 
            ESP.restart();
        }
    }
}
// -------------------------------------------------------------------
// Cargar Información de las paginas al Servidor peticiones GET/POST
// -------------------------------------------------------------------
void InitServer(){
    // -------------------------------------------------------------------
    // Cargar todos los Archivos estáticos del servidor
    // -------------------------------------------------------------------
    // JavaScripts
    server.serveStatic("/js/libscripts.js", SPIFFS, "/js/libscripts.js").setDefaultFile("js/libscripts.js").setCacheControl("max-age=600");
    server.serveStatic("/js/mainscripts.js", SPIFFS, "/js/mainscripts.js").setDefaultFile("js/mainscripts.js").setCacheControl("max-age=600");
    server.serveStatic("/js/scripts.js", SPIFFS, "/js/scripts.js").setDefaultFile("/js/scripts.js").setCacheControl("max-age=600");
    server.serveStatic("/js/sweetalert.js", SPIFFS, "/js/sweetalert.js").setDefaultFile("js/sweetalert.js").setCacheControl("max-age=600");
    server.serveStatic("/js/vendorscripts.js", SPIFFS, "/js/vendorscripts.js").setDefaultFile("js/vendorscripts.js").setCacheControl("max-age=600"); 
    // CSS
    server.serveStatic("/css/bootstrap.css", SPIFFS, "/css/bootstrap.css").setDefaultFile("css/bootstrap.css").setCacheControl("max-age=600");
    server.serveStatic("/css/font-aws.css", SPIFFS, "/css/font-aws.css").setDefaultFile("css/font-aws.css").setCacheControl("max-age=600");
    server.serveStatic("/css/font-aws.woff2", SPIFFS, "/css/font-aws.woff2").setDefaultFile("css/font-aws.woff2").setCacheControl("max-age=600"); 
    server.serveStatic("/css/mooli.css", SPIFFS, "/css/mooli.css").setDefaultFile("css/mooli.css").setCacheControl("max-age=600");
    server.serveStatic("/css/styles.css", SPIFFS, "/css/styles.css").setDefaultFile("css/styles.css").setCacheControl("max-age=600");
    server.serveStatic("/css/sweetalert.css", SPIFFS, "/css/sweetalert.css").setDefaultFile("css/sweetalert.css").setCacheControl("max-age=600");    
    server.serveStatic("/css/vivify.css", SPIFFS, "/css/vivify.css").setDefaultFile("css/vivify.css").setCacheControl("max-age=600"); 
    // img
    server.serveStatic("/img/esp32.png", SPIFFS, "/img/esp32.png").setDefaultFile("img/esp32.png").setCacheControl("max-age=600");
    server.serveStatic("/img/user.png", SPIFFS, "/img/user.png").setDefaultFile("img/user.png").setCacheControl("max-age=600");   
    // -------------------------------------------------------------------
    // Cargar página Index.html o Home
    // url: /
    // Metodo: GET
    // -------------------------------------------------------------------
    server.on("/",HTTP_GET,handleHome);
    // -------------------------------------------------------------------
    // Cargar página wifi.html
    // url: /esp-wifi
    // Metodo: GET
    // -------------------------------------------------------------------
    // server.on("/esp-wifi", HTTP_GET, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     } 
    //     // Cargar página html Configuración del WiFi
    //     File file = SPIFFS.open(F("/wifi.html"), "r");
    //     if(file){
    //         file.setTimeout(100);
    //         String s = file.readString();
    //         file.close();
    //         // Sección Cliente
    //         s.replace(F("#platform#"), platform());
    //         s.replace(F("#wifi_ssid#"), wifi_ssid);
    //         s.replace(F("#wifi_staticIP#"), wifi_staticIP ? "checked" : "");
    //         s.replace(F("#wifi_staticIPen#"), wifi_staticIP ? "1" : "0");
    //         s.replace(F("#wifi_ip_static#"), ipStr(StrIP(wifi_ip_static)));
    //         s.replace(F("#wifi_subnet#"), ipStr(StrIP(wifi_subnet)));
    //         s.replace(F("#wifi_gateway#"), ipStr(StrIP(wifi_gateway)));
    //         s.replace(F("#wifi_primaryDNS#"), ipStr(StrIP(wifi_primaryDNS)));
    //         s.replace(F("#wifi_secondaryDNS#"), ipStr(StrIP(wifi_secondaryDNS)));
    //         // Sección AP
    //         s.replace(F("#ap_accessPoint#"), ap_AccessPoint ? "checked" : "");
    //         s.replace(F("#ap_AP_en#"), ap_AccessPoint ? "1" : "0");
    //         s.replace(F("#ap_nameap#"), String(ap_nameAP));
    //         s.replace(F("#ap_canalap#"), String(ap_canalAP));
    //         s.replace(F("#ap_hiddenap#"), ap_hiddenAP ? "" : "checked");
    //         s.replace(F("#ap_visibility#"), ap_hiddenAP ? "0" : "1");
    //         s.replace(F("#ap_connetap#"), String(ap_connAP));
    //         // Send data
    //         request->send(200, "text/html", s);
    //     }else{
    //         request->send(500, "text/plain","/wifi.html not found, have you flashed the SPIFFS?");
    //     }
    // });
    // -------------------------------------------------------------------
    // Cargar página mqtt.html
    // url: /esp-mqtt
    // Metodo: GET
    // -------------------------------------------------------------------
    // server.on("/esp-mqtt", HTTP_GET, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     // Cargar página html Configuración del MQTT
    //     File file = SPIFFS.open(F("/mqtt.html"), "r");
    //     if (file){
    //         file.setTimeout(100);
    //         String s = file.readString();
    //         file.close();
    //         s.replace(F("#platform#"), platform());
    //         s.replace(F("#mqtt_enable#"), mqtt_enable ? "checked" : "");
    //         s.replace(F("#mqtten#"), mqtt_enable ? "1" : "0");
    //         s.replace(F("#mqtt_id#"), mqtt_id);
    //         s.replace(F("#mqtt_user#"), mqtt_user);
    //         s.replace(F("#mqtt_server#"), mqtt_server);
    //         s.replace(F("#mqtt_port#"), String(mqtt_port));
    //         s.replace(F("#mqtt_time#"), String(mqtt_time / 60000));
    //         // Send data
    //         request->send(200, "text/html", s);
    //     }else{
    //         request->send(500, "text/plain","/mqtt.html not found, have you flashed the SPIFFS?");
    //     }
    // });
    // -------------------------------------------------------------------
    // Cargar página device.html
    // url: /esp-device
    // Metodo: GET
    // -------------------------------------------------------------------
    server.on("/esp-device", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // Cargar página html Configuración del MQTT
        File file = SPIFFS.open(F("/device.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            s.replace(F("#platform#"), platform());
            s.replace(F("#id#"), id);
            s.replace(F("#serie#"), device_id);
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/device.html not found, have you flashed the SPIFFS?");
        }
    });
    // -------------------------------------------------------------------
    // Cargar página restore.html
    // url: /esp-restore
    // Metodo: GET
    // -------------------------------------------------------------------
    server.on("/esp-restore", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // Cargar página html Configuración del MQTT
        File file = SPIFFS.open(F("/restore.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            s.replace(F("#platform#"), platform());
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/restore.html not found, have you flashed the SPIFFS?");
        }
    });
    // -------------------------------------------------------------------
    // Cargar página restart.html
    // url: /esp-restart
    // Metodo: GET
    // -------------------------------------------------------------------
    server.on("/esp-restart", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // Cargar página html Configuración del MQTT
        File file = SPIFFS.open(F("/restart.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            s.replace(F("#platform#"), platform());
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/restart.html not found, have you flashed the SPIFFS?");
        }
    });
    // -------------------------------------------------------------------
    // Cargar página admin.html
    // url: /esp-admin
    // Metodo: GET
    // -------------------------------------------------------------------
    server.on("/esp-admin", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // Cargar página html Configuración del MQTT
        File file = SPIFFS.open(F("/admin.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            s.replace(F("#platform#"), platform());
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/admin.html not found, have you flashed the SPIFFS?");
        }
    });
    // -------------------------------------------------------------------
    // Guardar la Contraseña y el Usuario
    // url: /esp-admin
    // Metodo: POST
    // -------------------------------------------------------------------
    // server.on("/esp-admin", HTTP_POST, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     // Validar datos enviados
    //     if(request->params() == 5){
    //         String u, p, nu, np, cp; 
    //         if (request->hasArg("www_username"))
    //             u = request->arg("www_username");
    //         if (request->hasArg("www_password"))
    //             p = request->arg("www_password");
    //         if (request->hasArg("new_www_username"))
    //             nu = request->arg("new_www_username");
    //         if (request->hasArg("new_www_password"))
    //             np = request->arg("new_www_password");
    //         if (request->hasArg("c_new_www_password"))
    //             cp = request->arg("c_new_www_password");
    //         u.trim();
    //         p.trim();
    //         nu.trim();
    //         np.trim();
    //         cp.trim();
    //         // Validar que los datos del usario y contraseña anteriores no esten en blanco
    //         if(u != "" && p != ""){
    //             // validar que el usuario y contraseña coincidan con los anteriores
    //             if(u == www_username && p == www_password){
    //                 // Guardamos el Nuevo Usuario
    //                 if(nu != "" && np == "" && cp == ""){
    //                     // Validar que el usuario nuevo sea diferente al antiguo
    //                     if(nu == www_username){
    //                         request->send(200, "text/html", SweetAlert("Usuario", "Warning", "El usuario no puede ser igual al anterior", "warning", "aviso"));
    //                         return;
    //                     }
    //                     strlcpy(www_username, nu.c_str(), sizeof(www_username));
    //                     // Parpadeo de los LEDS
    //                     leds();
    //                     // Guardar el nuevo usuario
    //                     if(settingsSaveAdmin()){
    //                         request->send(200, "text/html", SweetAlert("Usuario", "Hecho", "Usuario actualizado", "success", "aviso"));
    //                         return;
    //                     }else{
    //                         request->send(200, "text/html", SweetAlert("Usuario", "Error", "Usuario no Actualizado", "error", "aviso"));
    //                         return;
    //                     }
    //                     // Solucionar bug cuando no se envia datos
    //                 }else if(nu == "" && np == "" && cp == ""){
    //                     request->send(200, "text/html", SweetAlert("Usuario", "Error", "¡Error, No se permite nuevo usuario, nueva contraseña y confirmación de nueva contraseña vacíos!", "error", "aviso"));
    //                     return;
    //                     // Guardar solo la contraseña nueva 
    //                 }else if(np != "" && cp != "" && np == cp && nu == ""){
    //                     // Validar que el usuario nuevo sea diferente al antiguo
    //                     if(np == www_password){
    //                         request->send(200, "text/html", SweetAlert("Contraseña", "Warning", "La contraseña no puede ser igual a la anterior", "warning", "aviso"));
    //                         return;
    //                     }
    //                     strlcpy(www_password, np.c_str(), sizeof(www_password));
    //                     // Leds
    //                     leds();
    //                     // Guardar el nuevo usuario
    //                     if(settingsSaveAdmin()){
    //                         request->send(200, "text/html", SweetAlert("Contraseña", "Hecho", "Contraseña actualizada", "success", "aviso"));
    //                         return;
    //                     }else{
    //                         request->send(200, "text/html", SweetAlert("Contraseña", "Error", "Contraseña no Actualizada", "error", "aviso"));
    //                         return;
    //                     } 
    //                 }else if(np != cp){
    //                     request->send(200, "text/html", SweetAlert("Contraseña", "Warning", "La contraseña y confirmación no coinciden", "warning", "aviso"));
    //                     return;
    //                     // Guardo Usuario y Contraseña
    //                 }else if(nu != "" && np != "" && cp != "" && np == cp){
    //                     // Validar que el usuario nuevo y la contraseña nueva sea diferente al antiguos
    //                     if(np == www_password && nu == www_username){
    //                         request->send(200, "text/html", SweetAlert("Usuario & Contraseña", "Warning", "La contraseña y el usuario no puede ser iguales a los anteriores", "warning", "aviso"));
    //                         return;
    //                     }
    //                     strlcpy(www_username, nu.c_str(), sizeof(www_username));
    //                     strlcpy(www_password, np.c_str(), sizeof(www_password));
    //                     // Leds
    //                     leds();
    //                     // Guardar el nuevo usuario
    //                     if(settingsSaveAdmin()){
    //                         request->send(200, "text/html", SweetAlert("Usuario & Contraseña", "Hecho", "Usuario & Contraseña actualizados", "success", "aviso"));
    //                         return;
    //                     }else{
    //                         request->send(200, "text/html", SweetAlert("Usuario & Contraseña", "Error", "Usuario & Contraseña no Actualizados", "error", "aviso"));
    //                         return;
    //                     }
    //                 }
    //             }else{
    //                 request->send(200, "text/html", SweetAlert("Información", "Información", "No se pudo Guardar, usuario y contraseña anterior no conciden", "info", "aviso"));
    //             }
    //         }else{
    //             request->send(200, "text/html", SweetAlert("Información", "Información", "No se pudo Guardar, no se permiten usuario y contraseña anterior en blanco", "info", "aviso"));
    //         }            
    //     }else{
    //         request->send(200, "text/html", SweetAlert("Error", "Error", "Error de Parámetros", "error", "aviso"));
    //     }
    // });
    // -------------------------------------------------------------------
    // Guardar el ID del Dispositivo
    // url: /esp-device
    // Metodo: POST
    // -------------------------------------------------------------------
    server.on("/esp-device", HTTP_POST, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        if (request->params() == 1){
            String s;
            if(request->hasArg("id"))
            s = request->arg("id");
            s.trim();
            if(s != "" && s != id){
                strlcpy(id, s.c_str(), sizeof(id));  
                // leds
                // leds();
                // Guardamos la informacion
                // if(settingsSaveWiFi()){
                //     request->send(200, "text/html", SweetAlert("Identificador", "Hecho", "El Identificador se Guardó correctamente", "success", "aviso"));
                //     return;
                // }else{
                //     request->send(200, "text/html", SweetAlert("Identificador", "Error", "El Identificador no se pudo Guardar", "error", "aviso")); 
                //     return;
                // }              
            }else{
               request->send(200, "text/html", SweetAlert("Warning", "Warning", "El Identificador no puede estar en Blanco ni ser igual al anterior", "warning", "aviso"));               
            }            
        }else{
            request->send(200, "text/html", SweetAlert("Error", "Error", "Error de Parámetros", "error", "aviso"));            
        }
    });
    // -------------------------------------------------------------------
    // Guardar la configuración del MQTT
    // url: /esp-mqtt
    // Metodo: POST
    // -------------------------------------------------------------------
    // server.on("/esp-mqtt", HTTP_POST, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     if(request->hasArg("mqtten"))
    //     mqtt_enable = request->arg("mqtten").toInt();
    //     if(mqtt_enable == 1){
    //         if (request->params() == 7){
    //             String s;
    //             // ID MQTT Broker
    //             if(request->hasArg("mqtt_id"))
    //             s = request->arg("mqtt_id");
    //             s.trim();
    //             if (s != ""){
    //                 strlcpy(mqtt_id, s.c_str(), sizeof(mqtt_id));
    //             }
    //             // Usuario MQTT Broker
    //             if(request->hasArg("mqtt_user"))
    //             s = request->arg("mqtt_user");
    //             s.trim();
    //             if (s != ""){
    //                 strlcpy(mqtt_user, s.c_str(), sizeof(mqtt_user));
    //             }
    //             // Contraseña MQTT Broker
    //             if(request->hasArg("mqtt_passw"))
    //             s = request->arg("mqtt_passw");
    //             s.trim();
    //             if (s != ""){
    //                 strlcpy(mqtt_passw, s.c_str(), sizeof(mqtt_passw));
    //             }
    //             // Servidor MQTT
    //             if(request->hasArg("mqtt_server"))
    //             s = request->arg("mqtt_server");
    //             s.trim();
    //             if (s != ""){
    //                 strlcpy(mqtt_server, s.c_str(), sizeof(mqtt_server));
    //             }
    //             // Puerto Servidor MQTT
    //             if(request->hasArg("mqtt_port"))
    //             mqtt_port = request->arg("mqtt_port").toInt();
    //             // Publish cada (min)
    //             if(request->hasArg("mqtt_time"))
    //             mqtt_time = request->arg("mqtt_time").toInt()*60000;
    //             // leds
    //             leds();
    //             // Guardar la configuracion del mqtt
    //             if(settingsSaveMQTT()){
    //                 request->send(200, "text/html", SweetAlert("MQTT", "Hecho", "La configuración del MQTT se Guardó correctamente", "success", "accion"));
    //                 return;
    //             }else{
    //                 request->send(200, "text/html", SweetAlert("MQTT", "Error", "La configuración del MQTT no se pudo Guardar", "error", "aviso")); 
    //                 return;
    //             }
    //         }else{
    //             request->send(200, "text/html", SweetAlert("Error", "Error", "Error de Parámetros", "error", "aviso"));
    //         }
    //     }else{
    //         // leds
    //         leds();
    //         // Guardar la configuracion del mqtt
    //         if(settingsSaveMQTT()){
    //             request->send(200, "text/html", SweetAlert("MQTT", "Hecho", "La configuración del MQTT se Guardó correctamente", "success", "accion"));
    //             return;
    //         }else{
    //             request->send(200, "text/html", SweetAlert("MQTT", "Error", "La configuración del MQTT no se pudo Guardar", "error", "aviso")); 
    //             return;
    //         }
    //     }
    // });
    // -------------------------------------------------------------------
    // Guardar la configuración del WIFI
    // url: /esp-wifi
    // Metodo: POST
    // -------------------------------------------------------------------
    // server.on("/esp-wifi", HTTP_POST, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     // IP Estática
    //     if(request->hasArg("wifi_staticIPen"))
    //     wifi_staticIP = request->arg("wifi_staticIPen").toInt();
    //     // AP Habilitado
    //     if(request->hasArg("ap_AP_en"))
    //     ap_AccessPoint = request->arg("ap_AP_en").toInt();
        
    //     String s; 
    //     // ssid Wifi
    //     if(request->hasArg("wifi_ssid"))
    //     s = request->arg("wifi_ssid");
    //     s.trim();
    //     if (s != ""){
    //         strlcpy(wifi_ssid, s.c_str(), sizeof(wifi_ssid));
    //     }
    //     s="";
    //     // password wifi
    //     if(request->hasArg("wifi_passw"))
    //     s = request->arg("wifi_passw");
    //     s.trim();
    //     if (s != ""){
    //         strlcpy(wifi_pass, s.c_str(), sizeof(wifi_pass));
    //     }
    //     s="";
    //     if(wifi_staticIP == 1){
    //         // Ip estatico
    //         if(request->hasArg("wifi_ip_static"))
    //         s = request->arg("wifi_ip_static");
    //         s.trim();
    //         if (s != ""){
    //             strlcpy(wifi_ip_static, s.c_str(), sizeof(wifi_ip_static));
    //         }
    //         s="";
    //         // Mascara de subred
    //         if(request->hasArg("wifi_subnet"))
    //         s = request->arg("wifi_subnet");
    //         s.trim();
    //         if (s != ""){
    //             strlcpy(wifi_subnet, s.c_str(), sizeof(wifi_subnet));
    //         }
    //         s="";
    //         // Gateway
    //         if(request->hasArg("wifi_gateway"))
    //         s = request->arg("wifi_gateway");
    //         s.trim();
    //         if (s != ""){
    //             strlcpy(wifi_gateway, s.c_str(), sizeof(wifi_gateway));
    //         }
    //         s="";
    //         // Primary DNS
    //         if(request->hasArg("wifi_primaryDNS"))
    //         s = request->arg("wifi_primaryDNS");
    //         s.trim();
    //         if (s != ""){
    //             strlcpy(wifi_primaryDNS, s.c_str(), sizeof(wifi_primaryDNS));
    //         }
    //         s="";
    //         // Secondary DNS
    //         if(request->hasArg("wifi_secondaryDNS"))
    //         s = request->arg("wifi_secondaryDNS");
    //         s.trim();
    //         if (s != ""){
    //             strlcpy(wifi_secondaryDNS, s.c_str(), sizeof(wifi_secondaryDNS));
    //         }
    //         s="";
    //     }else if (ap_AccessPoint == 1){
    //         // Nombre del AP
    //         if(request->hasArg("ap_nameap"))
    //         s = request->arg("ap_nameap");
    //         s.trim();
    //         if (s != ""){
    //             strlcpy(ap_nameAP, s.c_str(), sizeof(ap_nameAP));
    //         }
    //         s="";
    //         // password del AP
    //         if(request->hasArg("ap_passwordap"))
    //         s = request->arg("ap_passwordap");
    //         s.trim();
    //         if (s != ""){
    //             strlcpy(ap_pass, s.c_str(), sizeof(ap_pass));
    //         }
    //         // AP visibility ON 0 - OFF 1
    //         if(request->hasArg("ap_visibility"))
    //         ap_hiddenAP = request->arg("ap_visibility").toInt();
    //         // Canal del AP - 1- 13
    //         if(request->hasArg("ap_canalap"))
    //         ap_canalAP = request->arg("ap_canalap").toInt();    
    //         // Conexiones del AP - 0- 8
    //         if(request->hasArg("ap_connetap"))
    //         ap_connAP = request->arg("ap_connetap").toInt();
    //     }
    //     // Parpadea los LED
    //     leds();
    //     // Guarda configuracion del WiFi
    //     if (settingsSaveWiFi()){
    //         request->send(200, "text/html", SweetAlert("Inalámbrico", "Hecho", "La configuración Inálambrica se Guardó correctamente", "success", "accion"));
    //         return;
    //     }else{
    //         request->send(200, "text/html", SweetAlert("Inalámbrico", "Error", "La configuración Inálambrica no se pudo Guardar", "error", "aviso")); 
    //         return;
    //     }
    // });
    // -------------------------------------------------------------------
    // Escanear todas las redes WIFI al alcance de la señal
    // url: /scan
    // Metodo: GET
    // Notas: La primera solicitud devolverá 0 resultados a menos que comience
    // a escanear desde otro lugar (ciclo / configuración).
    // No solicite más de 3-5 segundos. \ ALT + 92 
    // -------------------------------------------------------------------
    // server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    // /* AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     } */
    //     String json = "";
    //     int n = WiFi.scanComplete();
    //     if(n == -2){
    //     json = "{";
    //     json += "\"meta\": { \"serial\": \""+ device_id +"\", \"count\": 0},";
    //     json += "\"data\": [";
    //     json += "],";   
    //     json += "\"code\": 0 ";
    //     json += "}";
    //     WiFi.scanNetworks(true, true); 
    //     } else if(n){
    //         json = "{";
    //         json += "\"meta\": { \"serial\": \""+ device_id +"\", \"count\":"+String(n)+"},";
    //         json += "\"data\": [";
    //         for (int i = 0; i < n; ++i){
    //             if(i) json += ",";
    //             json += "{";
    //             json += "\"n\":"+String(i+1);
    //             json += ",\"rssi\":"+String(WiFi.RSSI(i));
    //             json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
    //             json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
    //             json += ",\"channel\":"+String(WiFi.channel(i));
    //             json += ",\"secure\":\""+ EncryptionType(WiFi.encryptionType(i))+"\"";
    //             json += "}";
    //         }
    //         json += "],";   
    //         json += "\"code\": 1 ";
    //         json += "}";
    //         WiFi.scanDelete();
    //         if(WiFi.scanComplete() == -2){
    //             WiFi.scanNetworks(true, true);
    //         }
    //     }
    //     //response->addHeader("Server","ESP32 Admin Tools");
    //     request->send(200, "application/json", json);
    //     json = String();

    // });
    // -------------------------------------------------------------------
    // Manejo de la descarga del Archivo settingwifi.json
    // url: "/esp-settingwifi"
    // Método: GET
    // -------------------------------------------------------------------
    // server.on("/esp-settingwifi", HTTP_GET, [](AsyncWebServerRequest *request){
    //    /*  AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     } */
    //     const char* dataType = "application/json"; 
    //     log("Info: Descarga del archivo settingwifi.json");
    //     AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/settingwifi.json", dataType, true);
    //     request->send(response);
    // });
    // -------------------------------------------------------------------
    // Manejo de la descarga del Archivo settingmqtt.json
    // url: "/esp-settingmqtt"
    // Método: GET
    // -------------------------------------------------------------------
    // server.on("/esp-settingmqtt", HTTP_GET, [](AsyncWebServerRequest *request){
    //    /*  AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     } */
    //     const char* dataType = "application/json"; 
    //     log("Info: Descarga del archivo settingmqtt.json");
    //     AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/settingmqtt.json", dataType, true);
    //     request->send(response);
    // });
    // -------------------------------------------------------------------
    // Manejo de la descarga del Archivo settingrelays.json
    // url: "/esp-settingrelays"
    // Método: GET
    // -------------------------------------------------------------------
    // server.on("/esp-settingrelays", HTTP_GET, [](AsyncWebServerRequest *request){
    //    /*  AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     } */
    //     const char* dataType = "application/json"; 
    //     log("Info: Descarga del archivo settingrelays.json");
    //     AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/settingrelays.json", dataType, true);
    //     request->send(response);
    // });
    // -------------------------------------------------------------------
    // Manejo de la descarga del Archivo settingadmin.json
    // url: "/esp-settingadmin.json"
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/esp-settingadmin", HTTP_GET, [](AsyncWebServerRequest *request){
       /*  AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        } */
        const char* dataType = "application/json"; 
        log("Info: Descarga del archivo settingadmin.json");
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/settingadmin.json", dataType, true);
        request->send(response);
    });
    // -------------------------------------------------------------------
    // Manejo de la descarga del Archivo settingadmin.json
    // url: "/esp-settingcolor.json"
    // Método: GET
    // -------------------------------------------------------------------
    // server.on("/esp-settingcolors", HTTP_GET, [](AsyncWebServerRequest *request){
    //    /*  AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     } */
    //     const char* dataType = "application/json"; 
    //     log("Info: Descarga del archivo settingcolors.json");
    //     AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/settingcolors.json", dataType, true);
    //     request->send(response);
    // });
    // -------------------------------------------------------------------
    // Manejo de la carga del archivo Settings.json
    // url: "/esp-upload"
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/esp-upload", HTTP_POST, [](AsyncWebServerRequest * request) {
        opened = false;
    },
    [](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
        handleDoUpload(request, filename, index, data, len, final);
    });
    // -------------------------------------------------------------------
    // Manejo de la Actualización del Firmware a FileSystem
    // url: /esp-firmware
    // Método: POST 
    // -------------------------------------------------------------------
    server.on("/esp-firmware", HTTP_POST, [](AsyncWebServerRequest * request) {},
    [](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
        handleDoFirmware(request, filename, index, data, len, final);
    });
    // -------------------------------------------------------------------
    // Error 404 página no encontrada
    // url: "desconocido"
    // -------------------------------------------------------------------
    server.onNotFound([](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        log("Info: Página NOT_FOUND: ");
        if(request->method() == HTTP_GET) {
            log("Info: Método -> GET");
        } else if(request->method() == HTTP_POST) {
            log("Info: Método -> POST");
        } else if(request->method() == HTTP_DELETE) {
            log("Info: Método -> DELETE");
        } else if(request->method() == HTTP_PUT) {
            log("Info: Método -> PUT");
        } else if(request->method() == HTTP_PATCH) {
            log("Info: Método -> PATCH");
        } else if(request->method() == HTTP_HEAD) {
            log("Info: Método -> HEAD");
        } else if(request->method() == HTTP_OPTIONS) {
            log("Info: Método -> OPTIONS");
        } else {
            log("Info: Método -> UNKNOWN");
        }
        log("Info: http://"+ request->host()+ request->url());
        if(request->contentLength()){
            log("Info: _CONTENT_TYPE: "+ request->contentType());
            log("Info: _CONTENT_LENGTH: "+ request->contentLength());
        }

        int headers = request->headers();
        int i;
        for(i=0; i<headers; i++) {
            AsyncWebHeader* h = request->getHeader(i);
            log("Info: _HEADER[]: "+ h->name()+ h->value());
        }

        int params = request->params();
        for(i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);
            if(p->isFile()){
                log("Info: _FILE[]: , size: " + p->name() + p->value() + p->size());
            } else if(p->isPost()){
                log("Info: _POST[]: " + p->name() + p->value());
            } else {
                log("Info: _GET[]: " + p->name() + p->value());
            }
        }
        // Error 404 página no encontrada
        File file = SPIFFS.open(F("/error_404.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            // Send data
            request->send(404, "text/html", s);
        }
    });
    // -------------------------------------------------------------------
    // Petición de salir del Servidor
    // url: "esp-logout"
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/esp-logout", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        request->send(401, "text/html", SweetAlert("LogOut", "Salir", "Logout", "success", "logout"));
    });
    // -------------------------------------------------------------------
    // Cargar página sfinger_info.html
    // url: /esp-finger
    // Metodo: GET
    // -------------------------------------------------------------------
    server.on("/esp-finger-info", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        log("Finger OK=" + String(fingerOK));
        fingerGetParameters();
        fingerPrintParameters();
        if(false == requestPreProcess(request, response)) {
            return;
        }
        File file = SPIFFS.open(F("/sfinger_info.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            s.replace(F("#sFinger_ok#"), fingerOK ? "Conectado" : "Sin Conexion");
            s.replace(F("#sFinger_status#"), fingerOK ? String(finger_status_reg) : "Sin Conexion");
            s.replace(F("#sFinger_id#"), fingerOK ? String(finger_system_id) : "Sin Conexion");
            s.replace(F("#sFinger_cap#"), fingerOK ? String(finger_capacity) : "Sin Conexion");
            s.replace(F("#sFinger_sec#"), fingerOK ? String(finger_security_level) : "Sin Conexion");
            s.replace(F("#sFinger_add#"), fingerOK ? String(finger_device_addr) : "Sin Conexion");
            s.replace(F("#sFinger_len#"), fingerOK ? String(finger_packet_len) : "Sin Conexion");
            s.replace(F("#sFinger_baud#"), fingerOK ? String(finger_baud_rate) : "Sin Conexion");
            s.replace(F("#sFinger_fing#"), fingerOK ? String(finger_templateCount) : "Sin Conexion");
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/sfinger.html not found, have you flashed the SPIFFS?");
        }
    });
    // -------------------------------------------------------------------
    // Cargar página sfinger_clr.html
    // url: /esp-finger
    // Metodo: POST
    // -------------------------------------------------------------------
    server.on("/esp-finger-clr", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        //fingerGetParameters();
        finger_clear = true;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        File file = SPIFFS.open(F("/sfinger_clr.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/sfinger_clr.html not found, have you flashed the SPIFFS?");
        }
    });
    // -------------------------------------------------------------------
    // Cargar página sfinger_add.html
    // url: /esp-finger
    // Metodo: POST
    // -------------------------------------------------------------------
    server.on("/esp-finger-add", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        //fingerGetParameters();
        finger_add = true;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        File file = SPIFFS.open(F("/sfinger_add.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/sfinger_add.html not found, have you flashed the SPIFFS?");
        }
    });
    // -------------------------------------------------------------------
    // Cargar página sfinger_cal.html
    // url: /esp-finger
    // Metodo: POST
    // -------------------------------------------------------------------
    // server.on("/esp-finger-cal", HTTP_POST, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     //fingerGetParameters();
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     File file = SPIFFS.open(F("/sfinger_cal.html"), "r");
    //     if (file){
    //         file.setTimeout(100);
    //         String s = file.readString();
    //         file.close();
    //         // Send data
    //         request->send(200, "text/html", s);
    //     }else{
    //         request->send(500, "text/plain","/sfinger.html not found, have you flashed the SPIFFS?");
    //     }
    // });
    // -------------------------------------------------------------------
    // Cargar página scolor_info.html
    // url: /esp-color-info
    // Metodo: GET
    // -------------------------------------------------------------------
    // server.on("/esp-color-info", HTTP_GET, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     s_colorOK = s_color_init();
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     File file = SPIFFS.open(F("/scolor_info.html"), "r");
    //     if (file){
    //         file.setTimeout(100);
    //         String s = file.readString();
    //         file.close();
    //         s.replace(F("#sColor_ok#"), s_colorOK ? "Conectado" : "Sin Conexion");
    //         s.replace(F("#SColor_intTime#"), s_colorOK ? String(sC_Time) : "Sin Conexion");
    //         s.replace(F("#SColor_gain#"), s_colorOK ? String(sC_Gain) : "Sin Conexion");
           
    //         // Send data
    //         request->send(200, "text/html", s);
    //     }else{
    //         request->send(500, "text/plain","/scolor.html not found, have you flashed the SPIFFS?");
    //     }
    // });
    // -------------------------------------------------------------------
    // Cargar página scolor_cal.html
    // url: /esp-color-cal
    // Metodo: GET
    // -------------------------------------------------------------------
    // server.on("/esp-color-cal", HTTP_GET, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     s_colorOK = s_color_init();
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     File file = SPIFFS.open(F("/scolor_cal.html"), "r");
    //     if (file){
    //         file.setTimeout(100);
    //         String s = file.readString();
    //         file.close();
    //         s.replace(F("#black_R#"), s_colorOK ? String(black_R) : "0");
    //         s.replace(F("#black_G#"), s_colorOK ? String(black_G) : "0");
    //         s.replace(F("#black_B#"), s_colorOK ? String(black_B) : "0");
    //         s.replace(F("#black_T#"), s_colorOK ? String(black_T) : "0");

    //         s.replace(F("#blue_R#"), s_colorOK ? String(blue_R) : "0");
    //         s.replace(F("#blue_G#"), s_colorOK ? String(blue_G) : "0");
    //         s.replace(F("#blue_B#"), s_colorOK ? String(blue_B) : "0");
    //         s.replace(F("#blue_T#"), s_colorOK ? String(blue_T) : "0");

    //         s.replace(F("#green_R#"), s_colorOK ? String(green_R) : "0");
    //         s.replace(F("#green_G#"), s_colorOK ? String(green_G) : "0");
    //         s.replace(F("#green_B#"), s_colorOK ? String(green_B) : "0");
    //         s.replace(F("#green_T#"), s_colorOK ? String(green_T) : "0");

    //         s.replace(F("#red_R#"), s_colorOK ? String(red_R) : "0");
    //         s.replace(F("#red_G#"), s_colorOK ? String(red_G) : "0");
    //         s.replace(F("#red_B#"), s_colorOK ? String(red_B) : "0");
    //         s.replace(F("#red_T#"), s_colorOK ? String(red_T) : "0");

    //         s.replace(F("#color_samples#"), s_colorOK ? String(sC_samples) : "0");
    //         s.replace(F("#color_time#"), s_colorOK ? String(sC_time) : "0");

    //         // Send data
    //         request->send(200, "text/html", s);
    //     }else{
    //         request->send(500, "text/plain","/scolor_cal.html not found, have you flashed the SPIFFS?");
    //     }
    // });
    // -------------------------------------------------------------------
    // Guardar página scolor_cal.html
    // url: /esp-color-cal
    // Metodo: POST
    // -------------------------------------------------------------------
    // server.on("/esp-color-cal", HTTP_POST, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     log("Guardando colores");
    //     // Valores del color negro
    //     if(request->hasArg("black_R"))
    //     black_R = request->arg("black_R").toInt();
    //     if(request->hasArg("black_G"))
    //     black_G = request->arg("black_G").toInt();
    //     if(request->hasArg("black_B"))
    //     black_B = request->arg("black_B").toInt();
    //     if(request->hasArg("black_T"))
    //     black_T = request->arg("black_T").toInt();

    //     if(request->hasArg("blue_R"))
    //     blue_R = request->arg("blue_R").toInt();
    //     if(request->hasArg("blue_G"))
    //     blue_G = request->arg("blue_G").toInt();
    //     if(request->hasArg("blue_B"))
    //     blue_B = request->arg("blue_B").toInt();
    //     if(request->hasArg("blue_T"))
    //     blue_T = request->arg("blue_T").toInt();

    //     if(request->hasArg("green_R"))
    //     green_R = request->arg("green_R").toInt();
    //     if(request->hasArg("green_G"))
    //     green_G = request->arg("green_G").toInt();
    //     if(request->hasArg("green_B"))
    //     green_B = request->arg("green_B").toInt();
    //     if(request->hasArg("green_T"))
    //     green_T = request->arg("green_T").toInt();

    //     if(request->hasArg("red_R"))
    //     red_R = request->arg("red_R").toInt();
    //     if(request->hasArg("red_G"))
    //     red_G = request->arg("red_G").toInt();
    //     if(request->hasArg("red_B"))
    //     red_B = request->arg("red_B").toInt();
    //     if(request->hasArg("red_T"))
    //     red_T = request->arg("red_T").toInt();

    //     if(request->hasArg("color_samples"))
    //     sC_samples = request->arg("color_samples").toInt();
    //     if(request->hasArg("color_time"))
    //     sC_time = request->arg("color_time").toInt();

    //     if (settingsSaveColors()){
    //         request->send(200, "text/html", SweetAlert("Inalámbrico", "Hecho", "La configuración de los colores se guardó correctamente", "success", "accion"));
    //         return;
    //     }else{
    //         request->send(200, "text/html", SweetAlert("Inalámbrico", "Error", "La configuración de los colores no se pudo Guardar", "error", "aviso")); 
    //         return;
    //     }
    // });   
    // -------------------------------------------------------------------
    // Cargar página rtc_info.html
    // url: /esp-rtc-info
    // Metodo: GET
    // -------------------------------------------------------------------
    // server.on("/esp-rtc-info", HTTP_GET, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     s_colorOK = s_color_init();
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     File file = SPIFFS.open(F("/rtc_info.html"), "r");
    //     if (file){
    //         file.setTimeout(100);
    //         String s = file.readString();
    //         file.close();
    //         s.replace(F("#rtc_ok#"), rtc_ok ? "Conectado" : "Sin Conexion");
    //         s.replace(F("#rtc_runnin#"), rtc_runnin ? "Corriendo" : "Sin Conexion");
    //         s.replace(F("#rtc_stamp#"), rtc_stamp ? String(rtc_stamp) : "Sin Conexion");
           
    //         // Send data
    //         request->send(200, "text/html", s);
    //     }else{
    //         request->send(500, "text/plain","/rtc_info.html not found, have you flashed the SPIFFS?");
    //     }
    // });
    // -------------------------------------------------------------------
    // Cargar página rtc_cal.html
    // url: /esp-rtc-cal
    // Metodo: GET
    // -------------------------------------------------------------------
    // server.on("/esp-rtc-cal", HTTP_GET, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     s_colorOK = s_color_init();
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     File file = SPIFFS.open(F("/rtc_cal.html"), "r");
    //     if (file){
    //         file.setTimeout(100);
    //         String s = file.readString();
    //         file.close();
    //         s.replace(F("#rtc_year#"), rtc_year ? String(rtc_year) : "0");
    //         s.replace(F("#rtc_month#"), rtc_month ? String(rtc_month) : "0");
    //         s.replace(F("#rtc_day#"), rtc_day ? String(rtc_day) : "0");

    //         s.replace(F("#rtc_hour#"), rtc_hour ? String(rtc_hour) : "0");
    //         s.replace(F("#rtc_minute#"), rtc_minute ? String(rtc_minute) : "0");
    //         s.replace(F("#rtc_second#"), rtc_second ? String(rtc_second) : "0");

    //         // Send data
    //         request->send(200, "text/html", s);
    //     }else{
    //         request->send(500, "text/plain","/rtc_cal.html not found, have you flashed the SPIFFS?");
    //     }
    // });
    // -------------------------------------------------------------------
    // Guardar página rtc_cal.html
    // url: /esp-rtc-cal
    // Metodo: POST
    // -------------------------------------------------------------------
    // server.on("/esp-rtc-cal", HTTP_POST, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     log("Guardando colores");

    //     if(request->hasArg("rtc_year"))
    //     rtc_year = request->arg("rtc_year").toInt();
    //     if(request->hasArg("rtc_month"))
    //     rtc_month = request->arg("rtc_month").toInt();
    //     if(request->hasArg("rtc_day"))
    //     rtc_day = request->arg("rtc_day").toInt();

    //     if(request->hasArg("rtc_hour"))
    //     rtc_hour = request->arg("rtc_hour").toInt();
    //     if(request->hasArg("rtc_minute"))
    //     rtc_minute = request->arg("rtc_minute").toInt();
    //     if(request->hasArg("rtc_second"))
    //     rtc_second = request->arg("rtc_second").toInt();

    //     rtc_adjust(rtc_year,rtc_month,rtc_day,rtc_hour,rtc_minute,rtc_second);

    //     if (settingsSaveColors()){
    //         request->send(200, "text/html", SweetAlert("Inalámbrico", "Hecho", "La configuración de los colores se guardó correctamente", "success", "accion"));
    //         return;
    //     }else{
    //         request->send(200, "text/html", SweetAlert("Inalámbrico", "Error", "La configuración de los colores no se pudo Guardar", "error", "aviso")); 
    //         return;
    //     }
    // });
    // -------------------------------------------------------------------
    // Cargar página espnow.html
    // url: /esp-mesh
    // Metodo: GET
    // -------------------------------------------------------------------
    // server.on("/esp-mesh", HTTP_GET, [](AsyncWebServerRequest *request){
    //     AsyncResponseStream *response;
    //     s_colorOK = s_color_init();
    //     if(false == requestPreProcess(request, response)) {
    //         return;
    //     }
    //     File file = SPIFFS.open(F("/espnow.html"), "r");
    //     if (file){
    //         file.setTimeout(100);
    //         String s = file.readString();
    //         file.close();
    //         s.replace(F("#mesh_status#"), mesh_status ? String(mesh_status) : "0");
    //         s.replace(F("#mac_local#"), mac_local ? String(mac_local) : "0");
    //         s.replace(F("#mach_linea#"), mach_linea ? String(mach_linea) : "0");
    //         s.replace(F("#mach_op#"), mach_op ? String(mach_op) : "0");
    //         s.replace(F("#mach_desc#"), mach_desc ? String(mach_desc) : "0");
    //         s.replace(F("#mac_remote#"), s_mac_remote ? String(s_mac_remote) : "0");

    //         // s.replace(F("#rtc_year#"), rtc_year ? String(rtc_year) : "0");
            
    //         // Send data
    //         request->send(200, "text/html", s);
    //     }else{
    //         request->send(500, "text/plain","/rtc_cal.html not found, have you flashed the SPIFFS?");
    //     }
    // });
    // -------------------------------------------------------------------
    // Iniciar el Servidor WEB
    // -------------------------------------------------------------------
    if(enableCors) {
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    }
    // server.addHandler(&events);
    server.begin();
    // Update.onProgress(printProgress);
    log("Info: Servidor HTTP iniciado");

}
