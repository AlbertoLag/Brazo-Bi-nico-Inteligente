//Emisor
//Incluimos las diferentes librerías para el uso de la comunicación por radiofrecuencia, los botones, etc.
#include <Wire.h>
#include <SPI.h>
#include "Mirf.h"
#include "nRF24L01.h"
#include "MirfHardwareSpiDriver.h"
#include "GFButton.h"
Nrf24l Mirf = Nrf24l(10, 9);
//MPU
#define MPU 0x68
//Inicializamos las variables globales. En el emisor tenemos las lecturas de los diferentes dedos y el MPU. También añadimos el botón a utilizar.
//MPU
double AcX,AcY,AcZ;
int Pitch, Roll;
//Sensores Flexión
int lectura1=0;
int lectura2=0;
int lectura3=0;
int lectura4=0;

int data[6]; //El vector de datos que enviamos al receptor.
//Medias para reducir el ruido del movimiento.
int media_pulgar=0;
int media_indice=0;
int media_corazon=0;
int media_anular=0;
int media_Roll=0;
int media_Pitch=0;

GFButton b1(2); //Botón para llamar a la función "Jankenpon"

void setup()
{
  Serial.begin(9600);
  //Para realizar correctamente la conexión por radiofrecuencia, utilizamos el siguiente código.
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"ABCDE");  //Establecemos la dirección del emisor.
  Mirf.payload = 6*sizeof(lectura1); //Aquí señalamos el tamaño de los datos que vamos a enviar. Como estamos enviando "enteros", tenemos que multiplicar el tamaño por el número de enteros que queremos enviar. En nuestro caso 6
  Mirf.channel = 90;              //Seleccionamos el canal que se va a utilizar.
  Mirf.config();
  //Iniciamos el MPU
  init_MPU(); 
}
//En las siguientes funciones, realizo medias de los cinco últimos valores medidos para reducir el posible ruido en las lecturas y prevenir mal funcionamiento.
//Media del pulgar
void pulgar()
{
  int datos1[5];
  
  int i=0;
  for (i=0;i<5;i++)
    {
    int c=0;
    c=analogRead(A3);
    datos1[i]=c;
    }
    media_pulgar=(datos1[0]+datos1[1]+datos1[2]+datos1[3]+datos1[4])/5;
  return media_pulgar;
  }
//Media índice
void indice()
{
  int datos2[5];
  
  int i=0;
  for (i=0;i<5;i++)
    {
    int c=0;
    c=analogRead(A2);
    datos2[i]=c;
    }
    media_indice=(datos2[0]+datos2[1]+datos2[2]+datos2[3]+datos2[4])/5;
  return media_indice;
  }
//Media corazón.
  void corazon()
{
  int datos3[5];
  
  int i=0;
  for (i=0;i<5;i++)
    {
    int c=0;
    c=analogRead(A1);
    datos3[i]=c;
    }
    media_corazon=(datos3[0]+datos3[1]+datos3[2]+datos3[3]+datos3[4])/5;
  return media_corazon;
  }
//Media anular.
  void anular()
{
  int datos4[5];
  
  int i=0;
  for (i=0;i<5;i++)
    {
    int c=0;
    c=analogRead(A0);
    datos4[i]=c;
    }
    media_anular=(datos4[0]+datos4[1]+datos4[2]+datos4[3]+datos4[4])/5;
  return media_anular;
  }

//Aquí se desarrolla la función para jugar al "Piedra, papel o tijeras" (En japonés, Jankenpon).
void jankenpon()
{
  if (b1.isPressed()) //Si pulsamos el botón, jugamos al Jankenpon.
  {
    Serial.println("Piedra, papel o tijeras 1, 2....3!");
    //puño en alto abierto
    data[0] = 175; 
    data[1] = 184;
    data[2] = 192;
    data[3] = 187;
    data[4] = 1;
    data[5] = 77;
    //Mandamos esta posición inicial al receptor.
    int i=0;
    for (i=0;i<1000;i++)
    {
    Mirf.setTADDR((byte *)"FGHIJ");   //Seleccionamos la dirección del receptor.
    Mirf.send((byte *)&data); //Mandamos el vector "data".   
    while (Mirf.isSending()) delay(1);      //Esperamos hasta que envíe el dato correctamente.
    }  
    
    
    //Si saco piedra pongo papel.
    if(analogRead(A3)<110&&analogRead(A2)<110&&analogRead(A1)<110&&analogRead(A0)<110) //Lectura de Piedra en los sensores.
    {
    Serial.println("PAPEL!!");
    data[0] = 175; 
    data[1] = 184;
    data[2] = 192;
    data[3] = 187;
    data[4] = 21;
    data[5] = 11;      
    //Actualizo la posición papel en los servomotores.  
    }
    //Si saco papel pongo tijeras.
    if(analogRead(A3)>110&&analogRead(A2)>110&&analogRead(A1)>110&&analogRead(A0)>110)//Lectura de papel en los sensores.
    {
      Serial.println("TIJERAS!!");
    data[0] = 81; 
    data[1] = 184;
    data[2] = 192;
    data[3] = 81;
    data[4] = 21;
    data[5] = 11;      
    //Actualizo la posición tijeras en los servomotores.     
      }
    //Si saco tijeras pongo piedra.
    if(analogRead(A3)<110&&analogRead(A2)>110&&analogRead(A1)>110&&analogRead(A0)<110)//Lectura de tijeras en los sensores.
    {
      Serial.println("PIEDRA!!");
    data[0] = 90; 
    data[1] = 71;
    data[2] = 82;
    data[3] = 85;
    data[4] = 21;
    data[5] = 11;      
    //Actualizo la posición piedra en los servomotores.
    }

   //Envío los valores al receptor.
    int t=0;
    for (t=0;t<1000;t++)
    {
    Mirf.setTADDR((byte *)"FGHIJ");   //Seleccionamos la dirección del receptor.
    Mirf.send((byte *)&data); //Mandamos el vector "data"  
    while (Mirf.isSending()) delay(1);    //Esperamos hasta que envíe el dato correctamente.
    }
  }
}
  

void loop()
{
  //Como la lectura de los valores del MPU son más complejas que los propios sensores de los dedos, he necesitado hacer la media en el void loop. 
  //He aumentado el número de valores en la media ya que son los que más errores generan en el comportamiento.
  int datos5[10];
  int datos6[10];
  int i=0;
  for (i=0;i<10;i++)
    {
    FunctionsMPU(); // Adquirimos los ejex AcX, AcY, AcZ.
    Roll = FunctionsPitchRoll(AcX, AcZ, AcY);   //Calculo del angulo del Roll(AcX, AcZ, AcY)
    Pitch = FunctionsPitchRoll(AcY, AcX, AcZ);  //Calculo del angulo del Pitch (AcY, AcX, AcZ)
    datos5[i]=Roll;
    datos6[i]=Pitch;
    }
    media_Roll=(datos5[0]+datos5[1]+datos5[2]+datos5[3]+datos5[4]+datos5[5]+datos5[6]+datos5[7]+datos5[8]+datos5[9])/10;
    media_Pitch=(datos6[0]+datos6[1]+datos6[2]+datos6[3]+datos6[4]+datos6[5]+datos6[6]+datos6[7]+datos6[8]+datos6[9])/10;
   
  //Calculamos las medias de las medidas de los sensores de flexión de cada uno de los dedos usando las funciones ya establecidas.
  pulgar();
  indice();
  corazon();
  anular();
  
  
  //Una vez que tenemos todas estas medias, actualizamos el vector "data" que enviamos al receptor.
  data[0] = media_pulgar;
  data[1] = media_indice;
  data[2] = media_corazon;
  data[3] = media_anular;
  data[4] = media_Pitch;
  data[5] = media_Roll;
  
  Mirf.setTADDR((byte *)"FGHIJ"); //Seleccionamos la dirección del receptor.
  Mirf.send((byte *)&data); //Mandamos el vector "data". 
  Serial.print("Esperando conexión..."); //Si la conexión falla, imprimimos este mensaje en el monitor serial.
  while (Mirf.isSending()) delay(1);    //Esperamos hasta que envíe el dato correctamente.
  
  //Imprimimos los datos dedos y los valores del MPU
  Serial.println("Send success:");
  Serial.println(data[0]);
  Serial.println(data[1]);
  Serial.println(data[2]);
  Serial.println(data[3]);
  Serial.print("Pitch: "); Serial.print(data[4]);
  Serial.print("\t");
  Serial.print("Roll: "); Serial.print(data[5]);
  Serial.print("\n");
  jankenpon(); //Función de juego "Piedra, papel o tijeras", en japonés, Jankenpon
  delay(100);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Código para el uso del sensor MPU6050.
void init_MPU(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // Seteamos a cero (wakes up - Despertamos el MPU-6050)
  Wire.endTransmission(true);
  delay(100);
}


//LECTURA MPU
//Funcion para el calculo del angulo Pitch y Roll
double FunctionsPitchRoll(double A, double B, double C){
  double DatoA, DatoB, Value;
  DatoA = A;
  DatoB = (B*B) + (C*C);
  DatoB = sqrt(DatoB);
  
  Value = atan2(DatoA, DatoB);
  Value = Value * 180/3.14;
  
  return (int)Value;
}

//Función para adquirir los ejes X, Y, Z del MPU6050
void FunctionsMPU(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // Empezamos con el registro 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,6,true);  // requerimos un total de 6 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
}
