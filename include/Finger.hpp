
// //---------------------------------------------------
// // Variables Finger Print
// //---------------------------------------------------
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
// bool finger_check, finger_add, finger_clear,finger_ok;
// int finger_status, finger_state;
// bool equals, finger_led = false;

#include <Adafruit_Fingerprint.h>
// #include <HardwareSerial.h>
// #include <SoftwareSerial.h>
// #define RXD2 5
// #define TXD2 17
// SoftwareSerial mySerial(RXD2, TXD2);
HardwareSerial mySerial(2);
// ser();
// mySerial.begin(57600, SWSERIAL_8N1, RXD2, TXD2, false);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t finger_id;

bool parpadeo = false;

// Variables de tiempo
const long interval = 1000;
const long interval2 = 500;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

//---------------------------------------------------
// Parpadeo de led del sensor de huella
//---------------------------------------------------
void fingerParpadeo(int times, int _delay)
{
    bool fLed = false;
    for (int x = 0; x < times; x++)
    {
        finger.LEDcontrol(fLed);
        delay(_delay);
        fLed = !fLed;
    }
}

//---------------------------------------------------
// Configuracion del sensor de huella
//---------------------------------------------------
bool fingerConfig()
{
    // if (!mySerial) { // If the object did not initialize, then its configuration is invalid
    // Serial.println("Invalid SoftwareSerial pin configuration, check config"); 
    // }
    finger.begin(57600);

    if (finger.verifyPassword()){
        log("Finger Print: Sensor de huella detectado");
        finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 50, FINGERPRINT_LED_BLUE, 3);
        finger.LEDcontrol(FINGERPRINT_LED_ON, 50, FINGERPRINT_LED_BLUE, 1);
        digitalWrite(led_finger,1);
        // finger.LEDcontrol(FINGERPRINT_LED_ON, 50, FINGERPRINT_LED_RED, 1);
        // delay(1000);
        // fingerParpadeo(6, 150);
        return true;
    }else {
        log("Finger Print: ERROR No se encontro sensor de huella");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 50, FINGERPRINT_LED_RED, 1);
        digitalWrite(led_finger,0);
        // finger.LEDcontrol(false);
        return false;
    }
}

//---------------------------------------------------
// Borrar base de datos del sensor de huella
//---------------------------------------------------
void fingerEmptyDatabase()
{
    finger.emptyDatabase();
    finger.getParameters();
    finger.getTemplateCount();
    log("Finger Print: Base de datos eliminada");
}

//---------------------------------------------------
// Leer parametros del sensor de huella
//---------------------------------------------------
void fingerGetParameters()
{
    log(F("Finger Print: Leyendo parametros del sensor de huella"));
    finger.getParameters();
    finger_status_reg = finger.status_reg;
    finger_system_id = finger.system_id;
    finger_capacity = finger.capacity;
    finger_security_level = finger.security_level;
    finger_device_addr = finger.device_addr;
    finger_packet_len = finger.packet_len;
    finger_baud_rate = finger.baud_rate;
}

void fingerPrintParameters()
{
    Serial.print(F("Finger Print: Status: 0x"));
    Serial.println(finger_status_reg, HEX);
    Serial.print(F("Finger Print: Sys ID: 0x"));
    Serial.println(finger_system_id, HEX);
    Serial.print(F("Finger Print: Capacity: "));
    Serial.println(finger.capacity);
    Serial.print(F("Finger Print: Security level: "));
    Serial.println(finger_security_level);
    Serial.print(F("Finger Print: Device address: "));
    Serial.println(finger_device_addr, HEX);
    Serial.print(F("Finger Print: Packet len: "));
    Serial.println(finger_packet_len);
    Serial.print(F("Finger Print: Baud rate: "));
    Serial.println(finger_baud_rate);

    finger.getTemplateCount();
    finger_templateCount = finger.templateCount;
    finger_id = finger_templateCount + 1;
    if (finger_templateCount == 0)
    {
        log("Finger Print: Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    }
    else
    {
        log("Finger Print: Waiting for valid finger...");
        Serial.print("Finger Print: Sensor contains ");
        Serial.print(finger.templateCount);
        Serial.println(" templates");
    }
}
//---------------------------------------------------
// Obtiene el ID de una huella ya guardada
//---------------------------------------------------
uint8_t fingerGetID()
{
    finger_status = -1;
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
        // log("Finger Print: Image taken");
        s_p="Finger Print: Image taken";
        break;
    case FINGERPRINT_NOFINGER:
        // log("Finger Print: No finger detected");
        s_p="Finger Print: No finger detected";
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        // log("Finger Print: Communication error");
        s_p="Finger Print: Communication error";
        return p;
    case FINGERPRINT_IMAGEFAIL:
        // log("Finger Print: Imaging error");
        s_p="Finger Print: Imaging error";
        return p;
    default:
        // log("Finger Print: Unknown error");
        s_p="Finger Print: Unknown error";
        return p;
    }

    // OK success!

    p = finger.image2Tz();
    switch (p)
    {
    case FINGERPRINT_OK:
        // log("Finger Print: Image converted");
        s_p="Finger Print: Image converted";
        break;
    case FINGERPRINT_IMAGEMESS:
        // log("Finger Print: Image too messy");
        s_p="Finger Print: Image too messy";
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        // log("Finger Print: Communication error");
        s_p="Finger Print: Communication error";
        return p;
    case FINGERPRINT_FEATUREFAIL:
        // log("Finger Print: Could not find fingerprint features");
        s_p="Finger Print: Could not find fingerprint features";
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        // log("Finger Print: Could not find fingerprint features");
        s_p="Finger Print: Could not find fingerprint features";
        return p;
    default:
        // log("Finger Print: Unknown error");
        s_p="Finger Print: Unknown error";
        return p;
    }

    // OK converted!
    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK)
    {
        log("Finger Print: Found a print match!");
        s_p="Finger Print: Found a print match!";
        finger_status = 1;
        return p;
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        log("Finger Print: Communication error");
        s_p="Finger Print: Communication error";
        return p;
    }
    else if (p == FINGERPRINT_NOTFOUND)
    {
        log("Finger Print: Did not find a match");
        s_p="Finger Print: Did not find a match";
        finger_status = 0;
        return p;
    }
    else
    {
        log("Finger Print: Unknown error");
        s_p="Finger Print: Unknown error";
        return p;
    }

    // found a match!
    Serial.print("Finger Print: Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);

    return finger.fingerID;
}

//---------------------------------------------------
// Guarda huella en un ID
//---------------------------------------------------
uint8_t fingerEnroll()
{
    if(finger_state == 0)
    {   
        finger.getParameters();
        finger.getTemplateCount();
        finger_templateCount = finger.templateCount;
        finger_id = finger_templateCount + 1;

        s_p = ("State " + String(finger_state) + "- ");
        finger_status = -1;
        int p = -1;
        // log("Waiting for valid finger to enroll as #");
        // Serial.println(finger_id);
        s_p += "Waiting for valid finger to enroll as #" + finger_id;
        // while (p != FINGERPRINT_OK)
        finger_state++;
    }
    if(finger_state == 1 && wait(700))
    {   
        finger.LEDcontrol(FINGERPRINT_LED_ON, 1, FINGERPRINT_LED_PURPLE, 1);
        delay(300);
        // if (finger_led){
        //     finger.LEDcontrol(FINGERPRINT_LED_ON, 1, FINGERPRINT_LED_PURPLE, 1);
        //     // delay(20);
        // }else{
        //     finger.LEDcontrol(FINGERPRINT_LED_OFF, 1, FINGERPRINT_LED_PURPLE, 1);
        //     // delay(20);
        // }

        // finger.LEDcontrol(finger_led);
        finger_led != finger_led;
        
        s_p = ("State " + String(finger_state) + "- ");
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            // log("Finger Print: Image taken");
            s_p += "Imagen tomada";
            finger_state++;
            break;
        case FINGERPRINT_NOFINGER:
            // log("Finger Print: .");
            s_p += "Coloca el dedo.";
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            // log("Finger Print: Communication error");
            s_p += "Error de comunicacion.";
            break;
        case FINGERPRINT_IMAGEFAIL:
            // log("Finger Print: Imaging error");
            s_p += "Error en la imagen.";
            break;
        default:
            // log("Finger Print: Unknown error");
            s_p += "Error desconocido.";
            break;
        }
    }
    // OK success!
    if(finger_state == 2 && wait(200)){
        s_p = ("State " + String(finger_state) + "- ");
        p = finger.image2Tz(1);
        switch (p)
        {
        case FINGERPRINT_OK:
            // log("Finger Print: Image converted");
            s_p += "Imagen convertida.";
            finger_state++;
            break;
        case FINGERPRINT_IMAGEMESS:
            // log("Finger Print: Image too messy");
            s_p += "Imagen muy sucia.";
            finger_state++;
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            // log("Finger Print: Communication error");
            s_p += "Error de comunicación.";
            finger_state++;
            return p;
        case FINGERPRINT_FEATUREFAIL:
            // log("Finger Print: Could not find fingerprint features");
            s_p += "No se encontraron caracteristicas de la huella.";
            finger_state++;
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            // log("Finger Print: Invalid Image");
            s_p += "Imagen invalida.";
            finger_state++;
            return p;
        default:
            // log("Finger Print: Unknown error");
            s_p += "Error desconocido.";
            finger_state++;
            return p;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    if(finger_state == 3 && wait(200)){
        s_p = ("State " + String(finger_state) + "- ");
        // log("Finger Print: Remove finger");
        s_p += "Retira el dedo.";
        // finger.LEDcontrol(false);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 1, FINGERPRINT_LED_PURPLE, 1);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 1, FINGERPRINT_LED_RED, 1);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 1, FINGERPRINT_LED_BLUE, 1);

        p = 0;
        finger_state++;
    }
        // while (p != FINGERPRINT_NOFINGER)
    if(finger_state == 4 && wait(200))
    {
        s_p = ("State " + String(finger_state) + "- ");
        p = finger.getImage();
        log ("P: " + String(p));
        if(p == FINGERPRINT_NOFINGER){
            
            finger_state++;
        }else{
            log("----------------Retira el dedo");
            s_p +="Retira el dedo.";
        }
    }
    if(finger_state == 5 && wait(200)){
        s_p = ("State " + String(finger_state) + "- ");
        // finger.LEDcontrol(true);
        finger.LEDcontrol(FINGERPRINT_LED_ON, 25, FINGERPRINT_LED_PURPLE, 1);
        //  DEBUGPRINT("ID "); DEBUGPRINTLN(finger_id);
        p = -1;
        // log("Finger Print: Place same finger again");
        s_p += "Colola el dedo nuevamente.";
        finger_state++;
    }
    // while (p != FINGERPRINT_OK)
    if(finger_state == 6 && wait(200))
    {
        s_p = ("State " + String(finger_state) + "- ");
        p = finger.getImage();
        log("P: " + String(p));
        switch (p)
        {
        case FINGERPRINT_OK:
            // log("Finger Print: Image taken");
            s_p += "Imagen tomada.";
            finger_state++;
            break;
        case FINGERPRINT_NOFINGER:
            // log("Finger Print: .");
            s_p += "Coloca dedo.";
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            // log("Finger Print: Communication error");
            s_p += "Error de comunicación.";
            finger_state++;
            break;
        case FINGERPRINT_IMAGEFAIL:
            // log("Finger Print: Imaging error");
            s_p += "Error en la imagen.";
            finger_state++;
            break;
        default:
            // log("Finger Print: Unknown error");
            s_p += "Error desconocido.";
            finger_state++;
            break;
        }
        
    }

    // OK success!
    //  lamp(3);  //Amarillo
    if(finger_state == 7 && wait(200)){
        s_p = ("State " + String(finger_state) + "- ");
        p = finger.image2Tz(2);
        switch (p)
        {
        case FINGERPRINT_OK:
            // log("Finger Print: Image converted");
            s_p += "Imagen convertida.";
            finger_state++;
            break;
        case FINGERPRINT_IMAGEMESS:
            // log("Finger Print: Image too messy");
            s_p += "Imagen muy sucia.";
            finger_state++;
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            // log("Finger Print: Communication error");
            s_p += "Error de comunicación.";
            finger_state++;
            return p;
        case FINGERPRINT_FEATUREFAIL:
            // log("Finger Print: Could not find fingerprint features");
            s_p += "No se encontraron caracteristicas de la huella.";
            finger_state++;
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            // log("Finger Print: Could not find fingerprint features");
            s_p += "No se encontraron caracteristicas de la huella.";
            finger_state++;
            return p;
        default:
            // log("Finger Print: Unknown error");
            s_p += "Error desconocido.";
            finger_state++;
            return p;
        }
}

    // OK converted!
    //  DEBUGPRINT("Creating model for #");  DEBUGPRINTLN(finger_id);
    if(finger_state == 8 && wait(200)){
        s_p = ("State " + String(finger_state) + "- ");
        p = finger.createModel();
        if (p == FINGERPRINT_OK){
            // log("Finger Print: Prints matched!");
            s_p += "La huella SI coincide.";
            finger_status = 1;
            finger.LEDcontrol(FINGERPRINT_LED_ON, 10, FINGERPRINT_LED_BLUE, 1);
            finger.LEDcontrol(FINGERPRINT_LED_OFF, 10, FINGERPRINT_LED_RED, 1);
            finger.LEDcontrol(FINGERPRINT_LED_OFF, 10, FINGERPRINT_LED_PURPLE, 1);
            delay(200);
            finger_state++;
        } else if (p == FINGERPRINT_PACKETRECIEVEERR){
            // log("Finger Print: Communication error");
            s_p += "Error de comunicación.";
            finger_status = 0;
            finger_state++;
            return p;
        } else if (p == FINGERPRINT_ENROLLMISMATCH) {
            // log("Finger Print: Fingerprints did not match");
            s_p += "La huella NO coincide.";
            finger_status = 0;
            finger.LEDcontrol(FINGERPRINT_LED_OFF, 10, FINGERPRINT_LED_BLUE, 1);
            finger.LEDcontrol(FINGERPRINT_LED_ON, 10, FINGERPRINT_LED_RED, 1);
            finger.LEDcontrol(FINGERPRINT_LED_OFF, 10, FINGERPRINT_LED_PURPLE, 1);
            delay(200);
            finger_state++;
            return p;
        } else {
            // log("Finger Print: Unknown error");
            s_p += "Error desconocido.";
            finger_status = 0;
            finger_state++;
            return p;
        }
    }
    ////////////////////////////////////////////////////////
    if(finger_state == 9 && wait(200)){
        s_p = ("State " + String(finger_state) + "- ");
        //  Serial.print("Finger Print: ID "); Serial.println(id);
        p = finger.storeModel(finger_id);
        if (p == FINGERPRINT_OK){
            // log("Finger Print: Stored!");
            s_p += "Imagen almacenada.";
            // finger_status = 1;
            finger_state = 10;
        }else if (p == FINGERPRINT_PACKETRECIEVEERR){
            // log("Finger Print: Communication error");
            s_p += "Error de comunicación.";
            finger_state = 10;
            // finger_status = 0;
            return p;
        }else if (p == FINGERPRINT_BADLOCATION){
            // log("Finger Print: Could not store in that location");
            s_p += "No se almaceno huella.";
            // finger_status = 0;
            finger_state = 10;
            return p;
        }else if (p == FINGERPRINT_FLASHERR){
            // log("Finger Print: Error writing to flash");
            s_p += "Error guardando en la flash.";
            finger_state = 10;
            // finger_status = 0;
            return p;
        }else{
            // log("Finger Print: Unknown error");
            s_p += "Error desconocido.";
            finger_state = 10;
            // finger_status = 0;
            return p;
        }
        // log("State: " + String(finger_state));
    }
    if(finger_state == 10 && wait(200)){
        // log("State " + String(finger_state) + "- ");
        s_p = ("State " + String(finger_state) + "- ");
        // fingerParpadeo(4, 150);
        //// finger.getParameters();
        fingerGetParameters();
        fingerPrintParameters();
        //// finger.getTemplateCount();
        finger_templateCount = finger.templateCount;
        finger_id = finger_templateCount + 1;

        log("Numero de huellas " + String(finger_id));
        // finger_state = 0;
    }
    return finger_status;
}

//---------------------------------------------------
// Borrar un ID especifico
//---------------------------------------------------
uint8_t fingerDelete(uint8_t _id)
{
    uint8_t p = -1;

    p = finger.deleteModel(_id);

    if (p == FINGERPRINT_OK)
    {
        Serial.print("Finger Print: Deleted ID ");
        Serial.println(_id);
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        log("Finger Print: Communication error");
    }
    else if (p == FINGERPRINT_BADLOCATION)
    {
        log("Finger Print: Could not delete in that location");
    }
    else if (p == FINGERPRINT_FLASHERR)
    {
        log("Finger Print: Error writing to flash");
    }
    else
    {
        Serial.print("Unknown error: 0x");
        Serial.println(p, HEX);
    }

    return p;
}

