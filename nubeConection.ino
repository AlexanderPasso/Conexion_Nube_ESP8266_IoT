
/** Conexion 
  Sensor ---> Cable rojo -- VCC 3v -5v 
              Cable negro -- GND 
              Cable Amarillo ---   SCL 
              Cable blanco  ----   SDA 

  Pines Placa nodemcu ESP8266 wemos
              D2 --->  SDA
              D1 --->  SCL

**/



#include <Ubidots.h>
#include "SHT85.h"


#define motionSensor D5
#define pinLED D6 
#define pinPuerta D0
#define pinBuzzer D7
#define SHT85_ADDRESS   0x44


#define DEVICE  "84f3eb23bba9"      //NOmbre del dispositivo en la plataforma Ubidots
#define VARIABLE  "puerta"          //Nombre de la variable en Ubidots. De la cual quiero recibir datos

//*************** Variables para la conexion con ubidots ***********************
const char* UBIDOTS_TOKEN = "BBFF-0EF5M4m9PzlAhoOcGHvGSiFji8g33n"; 
const char* WIFI_SSID = "-----";        //Nombre de la Red Wifi
const char* WIFI_PASS = "******";     //Password de la Red Wifi

///******************************************************************//


Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);
SHT85 sht1;
////************************************************************////

//DEfinicion de variables

uint32_t start;
uint32_t stop;

int var_poten;
float Temperatura;
float Humedad;
int state_sensor;
int valor_buzer = 254;
int state_puerta;

void setup() {
  Serial.begin(115200);

  pinMode(motionSensor, INPUT);
  pinMode(pinPuerta, OUTPUT);
  pinMode(pinLED, OUTPUT);

  //Inicializo la conexion I2C
  Wire.begin();
  sht1.begin(0x44);
  Wire.setClock(100000);

  //uint16_t stat = sht1.readStatus();
  
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
  var_poten = 0;
}

 

void loop() {

  state_puerta = ubidots.get(DEVICE, VARIABLE);   //Leo el estado de la variable desde la plataforma Ubidots. 
  
  if(state_puerta == HIGH){
    digitalWrite(pinPuerta, HIGH);          //Si esta en alto enciendo el led que me simula la apertura de la puerta 
  }else{
    digitalWrite(pinPuerta, LOW );
  }
  

  sht1.read();                       //Lectura del sensor de temperatura - humedad 
  
  Temperatura = sht1.getTemperature();
  Humedad = sht1.getHumidity();
  state_sensor = digitalRead(motionSensor);     //Leo el estado del sensor de movimiento 

  if(state_sensor == HIGH){                     //Si se detecta movimiento
    digitalWrite(pinLED, HIGH);                  //Enciende el led azul y
    analogWrite(pinBuzzer, valor_buzer);          // Se enciende el buzzer
  }else{
    digitalWrite(pinLED, LOW);
    analogWrite(pinBuzzer, 0);
  }


  //Muestro los datos leidos en el Serial 
  Serial.print( Temperatura, 1);
  Serial.print("\t");
  Serial.print(Humedad, 1);
  Serial.print("\t");
  Serial.println(state_sensor);


  //Agrego las variables que quiero enviar a la nube 
  ubidots.add("temperatura", Temperatura);
  ubidots.add("humedad", Humedad);
  ubidots.add("motion_sensor", state_sensor);

  bool bufferSent = false;
  bufferSent = ubidots.send();        //Envio los datos por Wifi a la nube (Ubidots)

  delay(100);


  }
