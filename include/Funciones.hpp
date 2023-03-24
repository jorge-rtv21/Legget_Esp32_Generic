// -------------------------------------------------------------------
// Genera un log en el puerto Serial
// -------------------------------------------------------------------
void log(String s){
    Serial.println(s);
}
// #include <WebServer_WT32_ETH01.h>
// De HEX a String
String hexStr(const unsigned long &h, const byte &l = 8){
    String s;
    s = String(h, HEX);
    s.toUpperCase();
    s = ("00000000" + s).substring(s.length() + 8 - l);
    return s;
}
// Funcion similar a delay, pero sin detener procesos

bool wait(unsigned long time){
    bool ok = false;
    currentTime=millis();
    if((currentTime-previousTime) > time){
        previousTime=currentTime;
        ok = true;
        }
    return ok;
}

//Recibe el comando del host
// bool incoming_cmd(WiFiClient *client){
//     // Serial.println("Incomming Data: ");
//     bool ok = false;
//     if (client->available()){
//         // Serial.println("Lectura Disponible:");
//         char c = client->read();
//         if (c == '\n' && currentLineIsBlank){
//             data_in = data_tmp;
//             data_tmp = "";
//             ok =  true;
//         // break;
//         }
//         else{
//             data_tmp += String(c);
//         }
//     }
//     return ok;
// }

int get_cmd(String &s){
    // log("String: " + s);
    int z;
    for (int x = 0; x < n_cmds; x++){
        if (s.equals(commands[x])){
            // log("F_cmd: " + commands[x]);
            z = x + 1;
            break;
        }
        else {z = 0;}
    }
    
    return z;
}


