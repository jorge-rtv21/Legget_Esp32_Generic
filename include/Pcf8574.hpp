// #define ARDUINO_UNO_INTERRUPTED_PIN 2
// void keyPressedOnPCF8574();
// Set i2c address
// PCF8574 pcf8574(0x38, ARDUINO_UNO_INTERRUPTED_PIN, keyPressedOnPCF8574);
PCF8574 io_ext_0(0x38,21,22);
PCF8574 io_ext_1(0x39,21,22);
bool keyPressed = false;

void exp0_init(){
    //Torreta
    io_ext_0.pinMode(P0, INPUT); 	//Encendido
	io_ext_0.pinMode(P1, INPUT);	//Sensor Huella
	io_ext_0.pinMode(P2, INPUT);	//WebServer
	io_ext_0.pinMode(P3, INPUT);	
	io_ext_0.pinMode(P4, INPUT);	//Cliente conectado
	io_ext_0.pinMode(P5, INPUT);		//Sensor Pieza
	io_ext_0.pinMode(P6, INPUT);
	io_ext_0.pinMode(P7, INPUT);

	Serial.print("Init pcf8574-0...");
	if (io_ext_0.begin()){
		Serial.println("OK");
	}else{
		Serial.println("KO");
	}
}

void exp1_init(){
    io_ext_1.pinMode(P0, OUTPUT, HIGH); 
	io_ext_1.pinMode(P1, OUTPUT, HIGH);
	io_ext_1.pinMode(P2, OUTPUT, HIGH);	
	io_ext_0.pinMode(P3, OUTPUT, HIGH); 
    io_ext_1.pinMode(P4, OUTPUT, HIGH);	
	io_ext_1.pinMode(P5, OUTPUT, HIGH);	
	io_ext_1.pinMode(P6, OUTPUT, HIGH);	
    io_ext_1.pinMode(P7, OUTPUT, HIGH);	

	Serial.print("Init pcf8574-1...");
	if (io_ext_1.begin()){
		Serial.println("OK");
	}else{
		Serial.println("KO");
	}
}
void keyPressedOnPCF8574(){
	// Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
	 keyPressed = true;
}

//Bloquea el motor con el piston
void block(){
    io_ext_1.digitalWrite(P7, LOW);
}
//Desbloquea el motor con el piston
void unblock(){
	io_ext_1.digitalWrite(P7, HIGH);
}

enum lamp{
	g,	//Green
	y,	//Yellow
	r,	//Red
	gy,	//Green and Yellow
	gr,	//Green and Red
	yr,	//Yellow and Red
	gyr,//Green and Yellow and Red
	off	//All Off
};


void lamp_color(lamp color){
	// log("Switch: " + String(color));
	switch (color){ //g->P4; y->P5; r->P6;
	case g:
		io_ext_1.digitalWrite(P4, !true);	//g
		io_ext_1.digitalWrite(P5, !false);	//y
		io_ext_1.digitalWrite(P6, !false);	//r
		break;
	case y:
		io_ext_1.digitalWrite(P4, !false);	//g
		io_ext_1.digitalWrite(P5, !true);	//y
		io_ext_1.digitalWrite(P6, !false);	//r
		break;
	case r:
		io_ext_1.digitalWrite(P4, !false);	//g
		io_ext_1.digitalWrite(P5, !false);	//y
		io_ext_1.digitalWrite(P6, !true);	//r
		break;
	case gy:
		io_ext_1.digitalWrite(P4, !true);	//g
		io_ext_1.digitalWrite(P5, !true);	//y
		io_ext_1.digitalWrite(P6, !false);	//r
		break;
	case gr:
		io_ext_1.digitalWrite(P4, !true);	//g
		io_ext_1.digitalWrite(P5, !false);	//y
		io_ext_1.digitalWrite(P6, !true);	//r
		break;
	case yr:
		io_ext_1.digitalWrite(P4, !false);	//g
		io_ext_1.digitalWrite(P5, !true);	//y
		io_ext_1.digitalWrite(P6, !true);	//r
		break;
	case gyr:
		io_ext_1.digitalWrite(P4, !true);	//g
		io_ext_1.digitalWrite(P5, !true);	//y
		io_ext_1.digitalWrite(P6, !true);	//r
		break;
	case off:
		io_ext_1.digitalWrite(P4, !false);	//g
		io_ext_1.digitalWrite(P5, !false);	//y
		io_ext_1.digitalWrite(P6, !false);	//r
		break;
	
	default:
		break;
	}
}

//Lee las entradas de la torreta para saber el estado de la prueba
int16_t get_state(){ 
    int16_t s;
	bool i0, i1, i2;
	i0 = !io_ext_0.digitalRead(P0);
	i1 = !io_ext_0.digitalRead(P1);
	i2 = !io_ext_0.digitalRead(P2);
	// log("P0: " + String(i0) + "\t P1: " + String(i1) + "\t P2: " + String(i2));
    if (i0 == LOW && i1 == LOW && i2 == LOW){
        s = 0;
    } else if  (i0 == HIGH && i1 == LOW && i2 == LOW){
        s = 1;
    } else if  (i0 == LOW && i1 == HIGH && i2 == LOW){
        s = 2;
    } else if  (i0 == HIGH && i1 == HIGH && i2 == LOW){
        s = 3;
    } else if  (i0 == LOW && i1 == LOW && i2 == HIGH){
        s = 4;
    } else if  (i0 == HIGH && i1 == LOW && i2 == HIGH){
        s = 5;
    }else if  (i0 == LOW && i1 == HIGH && i2 == HIGH){
        s = 6;
    } else if  (i0 == HIGH && i1 == HIGH && i2 == HIGH){
        s = 7;
    }
	// else {
	// 	s = 0;
	// }
	// log ("St " + String(s));
	// delay(50);
    return s;
}