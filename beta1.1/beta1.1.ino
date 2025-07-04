#include <Servo.h>

// Pines de sensores ultrasónicos
const int trigEntrada = 2;
const int echoEntrada = 3;
const int trigSalida = 4;
const int echoSalida = 5;

// Servo y pin
Servo barrera;
const int pinServo = 6;

// Variables de control
int espaciosDisponibles = 8;
const int capacidadMaxima = 8;

unsigned long bloqueoEntradaHasta = 0;
unsigned long bloqueoSalidaHasta = 0;
const unsigned long tiempoBloqueo = 20000; // 20 segundos

// Función para medir distancia con un sensor ultrasónico
long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duracion = pulseIn(echoPin, HIGH, 30000); // Timeout por seguridad
  long distancia = duracion * 0.034 / 2;
  return distancia;
}

// Función para abrir y cerrar la barrera
void abrirBarrera() {
  barrera.write(90);      // Abre la barrera
  delay(5000);            // Mantenerla abierta por 5 segundos
  barrera.write(0);       // Cierra la barrera
}

void setup() {
  Serial.begin(9600);
  pinMode(trigEntrada, OUTPUT);
  pinMode(echoEntrada, INPUT);
  pinMode(trigSalida, OUTPUT);
  pinMode(echoSalida, INPUT);
  barrera.attach(pinServo);
  barrera.write(0); // Inicia cerrada
}

void loop() {
  unsigned long ahora = millis();

  // SENSOR DE ENTRADA
  if (ahora > bloqueoEntradaHasta && espaciosDisponibles > 0) {
    long distanciaEntrada = medirDistancia(trigEntrada, echoEntrada);
    if (distanciaEntrada > 0 && distanciaEntrada < 15) {
      Serial.println("Vehículo entrando...");
      abrirBarrera();
      espaciosDisponibles--;
      Serial.println(String("ESPACIOS:") + espaciosDisponibles);

      // Bloquear la salida temporalmente
      bloqueoSalidaHasta = ahora + tiempoBloqueo;

      delay(2000); // Evitar múltiples lecturas
    }
  } else if (espaciosDisponibles == 0) {
    Serial.println("Estacionamiento lleno, entrada bloqueada.");
  }

  // SENSOR DE SALIDA
  if (ahora > bloqueoSalidaHasta && espaciosDisponibles < capacidadMaxima) {
    long distanciaSalida = medirDistancia(trigSalida, echoSalida);
    if (distanciaSalida > 0 && distanciaSalida < 15) {
      Serial.println("Vehículo saliendo...");
      abrirBarrera();
      espaciosDisponibles++;
      Serial.println(String("ESPACIOS:") + espaciosDisponibles);

      // Bloquear la entrada temporalmente
      bloqueoEntradaHasta = ahora + tiempoBloqueo;

      delay(2000); // Evitar múltiples lecturas
    }
  }

  delay(100); // Pequeña pausa para estabilidad
}
