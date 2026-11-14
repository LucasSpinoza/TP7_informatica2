/**
 * TP7 - Entorno gráfico (Informática II, UTN FRM)
 *
 * Firmware de Arduino: lee dos pulsadores, maneja dos LEDs y se comunica
 * con la PC (programa de Processing) por el puerto serie.
 *
 * Comportamiento:
 *   - Al apretar el pulsador Ei, el LED Li cambia de estado (toggle).
 *   - La PC puede encender o apagar cada LED enviando un comando.
 *   - Cada cambio de entrada o de salida se informa a la PC.
 *
 * Protocolo (texto ASCII, un mensaje por línea terminado en '\n', 9600 baudios):
 *
 *   Arduino -> PC   "E<n>:<v>"   estado de la entrada n (1 = pulsada, 0 = suelta)
 *   Arduino -> PC   "L<n>:<v>"   estado del LED n       (1 = encendido, 0 = apagado)
 *   PC -> Arduino   "L<n>:<v>"   ordena encender (1) o apagar (0) el LED n
 *
 *   n = 1 o 2, v = 0 o 1.  Ejemplo: "L2:1\n" enciende el LED 2.
 *
 * Conexiones (ver carpeta esquematico/):
 *   Pulsador 1: D2 y GND      Pulsador 2: D3 y GND
 *   (usan INPUT_PULLUP: no necesitan resistencia externa)
 *   LED 1: D8 -> resistencia 220 ohm -> ánodo, cátodo a GND
 *   LED 2: D9 -> resistencia 220 ohm -> ánodo, cátodo a GND
 */

// ----------------------------------------------------------------- Pines
const int PIN_E[2] = {2, 3};     // entradas: pulsadores E1 y E2
const int PIN_L[2] = {8, 9};     // salidas:  LEDs L1 y L2

// ---------------------------------------------------------------- Estado
int ultimoE[2] = {HIGH, HIGH};   // última lectura estable de cada pulsador
                                 // (con INPUT_PULLUP: HIGH = suelto, LOW = pulsado)
bool estadoL[2] = {false, false};  // estado actual de cada LED (true = encendido)
String buffer = "";              // acumula los caracteres del comando que llega de la PC

// ------------------------------------------------------------ Comunicación

// Envía a la PC el estado del LED i con el formato "L<n>:<v>".
void enviarL(int i) {
  Serial.print("L"); Serial.print(i + 1);
  Serial.print(":"); Serial.println(estadoL[i] ? 1 : 0);
}

// ---------------------------------------------------------------- Entradas

// Lee los pulsadores. Cuando uno cambia, informa a la PC y, si fue una
// pulsación, cambia el LED correspondiente.
void leerPulsadores() {
  for (int i = 0; i < 2; i++) {
    int lec = digitalRead(PIN_E[i]);
    if (lec != ultimoE[i]) {
      delay(20);                          // antirrebote: espera a que se estabilice
      lec = digitalRead(PIN_E[i]);        // y vuelve a leer para confirmar
      if (lec != ultimoE[i]) {
        ultimoE[i] = lec;
        // Informa la entrada: 1 = pulsado (LOW), 0 = suelto (HIGH)
        Serial.print("E"); Serial.print(i + 1);
        Serial.print(":"); Serial.println(lec == LOW ? 1 : 0);
        if (lec == LOW) {                 // al apretar, cambia el LED
          estadoL[i] = !estadoL[i];
          digitalWrite(PIN_L[i], estadoL[i]);
          enviarL(i);
        }
      }
    }
  }
}

// ---------------------------------------------------------- Comandos de la PC

// Lee los caracteres que llegan por serie. Cuando recibe '\n', el comando
// está completo: si tiene el formato "L<n>:<v>" actualiza el LED y confirma.
void leerComandos() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      if (buffer.length() >= 4 && buffer[0] == 'L') {
        int i = buffer[1] - '1';          // '1' -> 0, '2' -> 1
        if (i >= 0 && i < 2) {
          estadoL[i] = (buffer[3] == '1');
          digitalWrite(PIN_L[i], estadoL[i]);
          enviarL(i);                     // confirma a la PC el nuevo estado
        }
      }
      buffer = "";                        // listo para el próximo comando
    } else if (c != '\r') {               // ignora el retorno de carro
      buffer += c;
    }
  }
}

// ----------------------------------------------------------- Programa principal

void setup() {
  Serial.begin(9600);                     // debe coincidir con Processing
  for (int i = 0; i < 2; i++) {
    pinMode(PIN_E[i], INPUT_PULLUP);      // resistencia interna: sin resistencia externa
    pinMode(PIN_L[i], OUTPUT);
  }
}

void loop() {
  leerPulsadores();   // 1) pulsadores físicos
  leerComandos();     // 2) comandos que llegan desde la PC
}
