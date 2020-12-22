//Receiver program
//Incluimos las diferentes librerías para el uso de la comunicación por radiofrecuencia, los botones,la memoria EEPROM,los servo motores, etc.
#include "GFButton.h"
#include <SPI.h>
#include <EEPROM.h>
#include "Mirf.h"
#include "nRF24L01.h"
#include "MirfHardwareSpiDriver.h"
#include <Servo.h>
Nrf24l Mirf = Nrf24l(10, 9);

//Llamamos a los servos
Servo pulgar;
Servo indice;
Servo corazon;
Servo anular;
Servo menique;
Servo ServoX, ServoY;

//Inicializamos las variables globales. En el receptor tenemos las posiciones de los siete servos, el vector "data" que recibimos del emisor
//y el botón de la función de guardado de la EEPROM.
int pos1=0;
int pos2=0;
int pos3=0;
int pos4=0;
int pos5=0;
int pos6=0;
int ServoRoll;
int ServoPitch;

GFButton b1(2);

int data[6];


void setup()
{
  Serial.begin(9600);
 //Usamos la memoria interna EEPROM para inicializar la posición de los servos a una preestablecida.
  EEPROM.get (0, pos1);
  EEPROM.get (10, pos2);
  EEPROM.get (20, pos3);
  EEPROM.get (30, pos4);
  EEPROM.get (40, ServoRoll);
  EEPROM.get (50, ServoPitch);
 //Le asignamos a cada servo su pin digital y le asociamos la posición inicial.
  pulgar.attach(4);
  pulgar.write(pos1);
  indice.attach(5);
  indice.write(pos2);
  corazon.attach(6);
  corazon.write(pos3);
  anular.attach(7);
  anular.write(pos4);
  ServoX.attach(9);
  ServoX.write(ServoRoll);
  ServoY.attach(8);
  ServoY.write(ServoPitch);
  //Para realizar correctamente la conexión por radiofrecuencia, utilizamos el siguiente código.
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"FGHIJ"); //Establecemos la dirección del receptor.
  Mirf.payload =6*sizeof(pos1); //Aquí estamos indicado el tamaño de los valores que estamos recibiendo. Como vamos a recibir 8 enteros, multiplicamos por 6 el tamaño de un entero.
  Mirf.channel = 90;             //Seleccionamos el canal que se va a utilizar.
  Mirf.config();
  //Esperamos a tener conexión e imprimimos la posición inicial.
  Serial.println("Esperando Conexión..."); 
  Serial.print("Posición 1 ");
  Serial.println(pos1);
  Serial.print("Posición 2 ");
  Serial.println(pos2);
  Serial.print("Posición 3 ");
  Serial.println(pos3);
  Serial.print("Posición 4 ");
  Serial.println(pos4);
  Serial.print("ServoX 5 ");
  Serial.println(ServoRoll);
  Serial.print("ServoY 6 ");
  Serial.println(ServoPitch);
  delay(5000);//Mantenemos la posición inicial durante un periodo de tiempo.
}

//Con esta función, cuando pulsamos el botón, la memoria EEPROM guarda la posición actual.
void memoria_EEPROM()
{
  if (b1.isPressed())
   {
    Serial.println("Guardando siguiente posición en la memoria EEPROM");
    EEPROM.put (0, pos1);
    EEPROM.put (10, pos2);
    EEPROM.put (20, pos3);
    EEPROM.put (30, pos4);
    EEPROM.put (40, ServoRoll);
    EEPROM.put (50, ServoPitch);
   }
}   
    
    
    void loop()
{
    //Recibimos el vector "data" del emisor con las medidas de los sensores.
    if (Mirf.dataReady()) { 
    Mirf.getData((byte *) &data);
    
    //Le damos a posición los valores de los sensores de los diferentes dedos
    pos1=data[0];
    pos2=data[1];
    pos3=data[2];
    pos4=data[3];
    pos5=-data[4];
    pos6=data[5];
  
    //Mapeamos los valores de la lectura de los sensores a los grados necesarios de cada servomotor.
    pos1 = map(pos1, 70, 140, 30, 180);//Número 12 (Los números son el número de sensor. Cada uno tiene medidas diferentes.)
    pulgar.write(pos1);
    pos2 = map(pos2, 65, 170, 10, 190);//Número 36
    indice.write(pos2);
    pos3 = map(pos3, 57, 165, 0, 270);//Número 41
    corazon.write(pos3);
    pos4 = map(pos4, 64, 172, 175, 0);//Número 2
    anular.write(pos4);
    
    //MPU-- Mapeamos los valores y ponemos los servos a posición.
    ServoRoll = map(pos5, -70, 70, 80, 180);
    ServoPitch = map(pos6, -85, 80, 35, 180);
    ServoX.write(ServoRoll);
    ServoY.write(ServoPitch);
    
    //Imprimimos dichos valores de las posiciones
    Serial.print("Posición 1 ");
    Serial.println(pos1);
    Serial.print("Posición 2 ");
    Serial.println(pos2);
    Serial.print("Posición 3 ");
    Serial.println(pos3);
    Serial.print("Posición 4 ");
    Serial.println(pos4);
    Serial.print("ServoX 5 ");
    Serial.println(pos5);
    Serial.print("ServoY 6 ");
    Serial.println(pos6);
    //Cuando pulsamos el botón, llamamos a la función memoria_EEPROM.
    memoria_EEPROM();
    
    delay(100);
  }
}
