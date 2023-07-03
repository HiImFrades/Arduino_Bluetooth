#include <SoftwareSerial.h>
#include <Wire.h>
#include "SparkFun_VL53L1X.h"
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;

SoftwareSerial BT(10,11);
SFEVL53L1X distanceSensor;

//Optional interrupt and shutdown pins.
//#define SHUTDOWN_PIN 2
//#define INTERRUPT_PIN 3

//Uncomment the following line to use the optional shutdown and interrupt pins.
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

String orden = "";
uint8_t effect = 1;

//EL VECTOR DE PATRÓN CORRESPONDE A: [EFECTO, Nº VECES, INTERVALO VECES(ms), Nº REPETICIONES, INTERVALO REP(ms)]
//tal que, el patrón de notificaciones tendrá un efecto un nº de veces en un intervalo de tiempo que se repetirá un nº de repeticiones en otro intervalo de tiempo.
int patronAlarma[5] = {1, 2, 100, 2, 200};
int patronLlamada[5] = {47, 3, 200, 4, 200};
int patronBateria[5] = {12, 2, 200, 1, 0};
int patronPower[5] = {12, 4, 100, 1, 0};
int patronWhatsapp[5] = {90, 1, 0, 1, 0};
int patronGmail[5] = {76, 1, 0, 1, 0};
int patronSMS[5] = {86, 2, 400, 1, 0};

//FLAGS PARA CONFIGURAR LAS NOTIFICACIONES
bool flagSensor = false;

/**
 * Función que se ejecuta en el arranque. Inicializa las variables
 * y componentes necesarias en el código.
 */
void setup(){
  
  Wire.begin();
  Serial.begin(9600);
  
  BT.begin(9600);
  
  if (! drv.begin()) {
    Serial.println("Could not find DRV2605");
    while (1) delay(10);
  }
  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG); 

  if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }
  Serial.println("Sensor online!");
  Serial.println("Iniciando programa...");
}

/**
 * Función que se ejecuta en bucle después del setup. Forma el código
 * principal de la aplicación. Recoge las órdenes del dispositivo móvil
 * y maneja la lectura del sensor láser.
 */
void loop(){
  if(BT.available()){
    String orden = BT.readStringUntil('\n');
    Serial.print("Orden introducida: ");
    Serial.println(orden);

    //---------------------------------------------------------
    if (orden.equals("Encender")){    //ENCENDER LA EJECUCIÓN DEL BLOQUE DEL SENSOR
      flagSensor = true;
    }
    else if (orden.equals("Apagar")){   //APAGAR LA EJECUCIÓN DEL BLOQUE DEL SENSOR
      flagSensor = false;
    }
    else if (orden.equals("Alarma")){   //ORDEN DE ALARMA.
      funNotificacion(patronAlarma);
    }
    else if (orden.equals("LlamadaLlamada")){   //ORDEN DE LLAMADA.
      funNotificacion(patronLlamada);
    }
    else if (orden.equals("Bateria")){   //ORDEN DE BATERIA.
      funNotificacion(patronBateria);
    }
    else if (orden.equals("Power")){   //ORDEN DE POWER.
      funNotificacion(patronPower);
    }
    else if (orden.equals("WhatsappWhatsapp")){   //ORDEN DE WHATSAPP.
      funNotificacion(patronWhatsapp);
    }
    else if (orden.equals("GmailGmail")){   //ORDEN DE GMAIL.
      funNotificacion(patronGmail);
    }
    else if (orden.equals("SMSSMS")){   //ORDEN DE MENSAJE SMS.
      funNotificacion(patronSMS);
    }//---------------------------------------------------------
    
  }
  if (flagSensor == true){
    distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
    while (!distanceSensor.checkForDataReady())
    {
      delay(1);
    }
    int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
    distanceSensor.clearInterrupt();
    distanceSensor.stopRanging();
  
    Serial.print("Distance(mm): ");
    Serial.println(distance);

    if(distance < 500){
      effect = 15;
      drv.setWaveform(0, effect);
      drv.go();
      delay(50);
    }else if ((distance >= 500)&&(distance < 1500)){
      effect = 1;
      drv.setWaveform(0, effect);
      drv.go();
      delay(300);
    }
  }
}

/**
 * Función de reproducción de notificaciones. Se le pasa un vector por parámetro
 * correspondiente con el patrón de vibración que se va a reproducir.
 * 
 * @param patron - Vector de enteros que corresponde con el patrón de vibración
 * de la notificación.
 */
void funNotificacion(int patron[]){
  effect = patron[0];
  drv.setWaveform(0, effect);
  for(int j=0; j<patron[3]; j++){
    for(int i=0; i<patron[1]; i++){
      drv.go();
      delay(patron[2]);
    }
    delay(patron[4]);
  }
}
