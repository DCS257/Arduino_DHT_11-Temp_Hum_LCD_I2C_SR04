/* MEDIR DISTANCIA CON ARDUINO Y UN SENSOR DE ULTRASONIDOS HC-SR04 Y OTRO DE TEMPERATURA DHT-11
 * Ref: https://www.makerguides.com/hc-sr04-arduino-tutorial/
 * 
 * Cómo usar un sensor de temperatura y humedad DHT11 para calibrar la velocidad del sonido y 
 * obtener una lectura de distancia más precisa con el HC-SR04
*/

#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Definimos los pines para el sensor de proximidad
#define trigPin 2
#define echoPin 3

// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 7

// Definimos el pin digital para el pulsador
#define PULSEPIN 6

// Definimos variables
long duration;
int distance;
float speedofsound;

int pulseStatus;
int previousPulseStatus = HIGH;

//Creamos el objeto lcd 
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Dependiendo del tipo de sensor (DHT11 / DHT22)
#define DHTTYPE DHT11
 
// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  // Inicializando comunicacion puerto serie 9600 baud
  Serial.begin(9600);
 
  // Inicializamos el sensor DHT
  dht.begin(); 

  //Definimos entradas y salidas del sensor de proximidad
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(PULSEPIN, INPUT);
  
  // Inicializamos el LCD de 16x2
  lcd.begin(16, 2);  
  
  // Pintamos las cabeceras para temperatura, humedad y distancia en el LCD
  lcd.setCursor(0, 0);
  lcd.print("Dist:");  

  lcd.setCursor(10, 0);
  lcd.print("T:");  

  lcd.setCursor(10, 1);
  lcd.print("H:");  
}

void loop() {

  // Controlamos el estado del pulsador
  pulseStatus = digitalRead(PULSEPIN);

  Serial.print("Lectura pulsador: ");
  Serial.print(pulseStatus);
  Serial.print(" Lectura anterior: ");
  Serial.println(previousPulseStatus);

  // Si se presiona el pulsador
  if (pulseStatus == LOW) {  

    // Si la pantalla está apagada -> se encendera
    if (previousPulseStatus == LOW) {
      lcd.backlight();
      previousPulseStatus = HIGH;
    } else {
      // Si la pantalla esta encendida -> se apagará
      lcd.noBacklight();
      previousPulseStatus = LOW;
    }

  }

  // Limpiamos el pin del sensor estableciendolo a LOW
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  
  // Activación del sensor cada 10 microsegundos:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Lectura del eco del sensor (devuelve la duración en microsegundos)
  duration = pulseIn(echoPin, HIGH);
    
  // Leemos la humedad relativa
  float h = dht.readHumidity();
  
  // Leemos la temperatura en grados centígrados (por defecto)
  float t = dht.readTemperature();
  
  // Leemos la temperatura en grados Fahrenheit
  float f = dht.readTemperature(true);
 
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }

  // Calcular el índice de calor en Fahrenheit
  float hif = dht.computeHeatIndex(f, h);
  
  // Calcular el índice de calor en grados centígrados
  float hic = dht.computeHeatIndex(t, h, false);
 
  // Calculamos la velocidad del sonido en m/s
  // Ref: https://en.wikipedia.org/wiki/Speed_of_sound 
  // En el aire, a 0 °C, el sonido viaja a una velocidad de 331,3 m/s 
  // (por cada grado Celsius que sube la temperatura, la velocidad del sonido aumenta en 0,6 m/s).
  speedofsound = 331.3 + (0.606 * t);

  // Calculamos la distancia en cm:
  distance = duration * (speedofsound / 10000) /2;

  // Pintamos la Temperatura y Humedad en el monitor serie
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Índice de calor: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  // Pintamos la velocidad y distancia en el monitor serie
  Serial.print("Velocidad sonido = ");
  Serial.print(speedofsound);
  Serial.print(" m/s "); 

  Serial.print("Distancia = ");
  Serial.print(distance);
  Serial.println(" cm"); 

  // Mostramos la temperatura en Celsius en el LCD
  lcd.setCursor(12, 0);  
  lcd.print(t);

  // Mostramos la humedad en el LCD
  lcd.setCursor(12, 1);  
  lcd.print(h);

  // Limpiamos lectura anterior
  lcd.setCursor(1, 1);  
  lcd.print("   ");

  // Mostramos nueva lectura
  lcd.setCursor(1, 1);  
  lcd.print(distance);
  lcd.setCursor(4, 1);  
  lcd.print("cm");

  // Retardo cada segundo
  delay(1000);
}
