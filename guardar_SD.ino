
#include <max6675.h>
#include <math.h>
#include <SPI.h>
#include <SD.h>

// ==================================================
// PINES
// ==================================================

// Bus compartido por los MAX6675 y la tarjeta SD
const int PIN_CLK = 13;
const int PIN_DO  = 12;

// Selección individual de cada MAX6675
const int CS_GASES_ESTUFA = 10;
const int CS_GASES_MURO   = 9;
const int CS_INTERIOR     = 8;
const int CS_SUPERFICIE   = 7;
const int CS_SUPLENTE     = 6;
const int CS_AMBIENTE     = 5;

// Selección de la tarjeta microSD
const int SD_CS = 4;

// ==================================================
// SENSORES
// ==================================================

MAX6675 sensorGasesEstufa(PIN_CLK, CS_GASES_ESTUFA, PIN_DO);
MAX6675 sensorGasesMuro(PIN_CLK, CS_GASES_MURO, PIN_DO);
MAX6675 sensorInterior(PIN_CLK, CS_INTERIOR, PIN_DO);
MAX6675 sensorSuperficie(PIN_CLK, CS_SUPERFICIE, PIN_DO);
MAX6675 sensorSuplente(PIN_CLK, CS_SUPLENTE, PIN_DO);
MAX6675 sensorAmbiente(PIN_CLK, CS_AMBIENTE, PIN_DO);

// ==================================================
// CONFIGURACIÓN DEL REGISTRO
// ==================================================

const unsigned long INTERVALO = 1000;
unsigned long ultimaLectura = 0;

const char NOMBRE_ARCHIVO[] = "DATOS.CSV";

// ==================================================
// FUNCIONES
// ==================================================

void imprimirTemperatura(File &archivo, double temperatura) {
  if (isnan(temperatura)) {
    archivo.print("nan");
  } else {
    archivo.print(temperatura, 2);
  }
}

void escribirCabecera() {
  File archivo = SD.open(NOMBRE_ARCHIVO, FILE_WRITE);

  if (archivo) {
    if (archivo.size() == 0) {
      archivo.println(
        "ms,gasEstufa_c,gasMuro_c,interior_c,"
        "superficie_c,suplente_c,ambiente_c"
      );
    }

    archivo.close();
  }
}

// ==================================================
// CONFIGURACIÓN INICIAL
// ==================================================

void setup() {

  // Mensajes de diagnóstico (no son necesarios para registrar)
  Serial.begin(115200);

  // Mantener todos los dispositivos deseleccionados
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  pinMode(CS_GASES_ESTUFA, OUTPUT);
  pinMode(CS_GASES_MURO, OUTPUT);
  pinMode(CS_INTERIOR, OUTPUT);
  pinMode(CS_SUPERFICIE, OUTPUT);
  pinMode(CS_SUPLENTE, OUTPUT);
  pinMode(CS_AMBIENTE, OUTPUT);

  digitalWrite(CS_GASES_ESTUFA, HIGH);
  digitalWrite(CS_GASES_MURO, HIGH);
  digitalWrite(CS_INTERIOR, HIGH);
  digitalWrite(CS_SUPERFICIE, HIGH);
  digitalWrite(CS_SUPLENTE, HIGH);
  digitalWrite(CS_AMBIENTE, HIGH);

  delay(500);

  // Inicializar tarjeta SD
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR: No se detecta la tarjeta SD.");

    while (true) {
      delay(1000);
    }
  }

  Serial.println("Tarjeta SD inicializada correctamente.");

  // Crear cabecera si el archivo está vacío
  escribirCabecera();

  Serial.println("Registro de temperaturas iniciado.");
}

// ==================================================
// BUCLE PRINCIPAL
// ==================================================

void loop() {

  unsigned long ahora = millis();

  if (ahora - ultimaLectura >= INTERVALO) {

    ultimaLectura = ahora;

    // Leer los seis sensores
    double gasesEstufa = sensorGasesEstufa.readCelsius();
    double gasesMuro = sensorGasesMuro.readCelsius();
    double interior = sensorInterior.readCelsius();
    double superficie = sensorSuperficie.readCelsius();
    double suplente = sensorSuplente.readCelsius();
    double ambiente = sensorAmbiente.readCelsius();

    // Abrir archivo en modo añadir
    File archivo = SD.open(NOMBRE_ARCHIVO, FILE_WRITE);

    if (archivo) {

      // Tiempo desde que se encendió el Arduino
      archivo.print(ahora);
      archivo.print(",");

      imprimirTemperatura(archivo, gasesEstufa);
      archivo.print(",");

      imprimirTemperatura(archivo, gasesMuro);
      archivo.print(",");

      imprimirTemperatura(archivo, interior);
      archivo.print(",");

      imprimirTemperatura(archivo, superficie);
      archivo.print(",");

      imprimirTemperatura(archivo, suplente);
      archivo.print(",");

      imprimirTemperatura(archivo, ambiente);
      archivo.println();

      // Cerrar para guardar los datos
      archivo.close();

    } else {
      Serial.println("ERROR: No se puede escribir en DATOS.CSV.");
    }
  }
}
