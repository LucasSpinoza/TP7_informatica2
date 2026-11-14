/**
 * TP7 - Entorno gráfico (Informática II, UTN FRM)
 *
 * Firmware de Arduino: lee pulsadores, maneja LEDs y se comunica con la PC
 * (programa de Processing) por el puerto serie.
 *
 * Comportamiento:
 *   - Al apretar el pulsador Ei, el LED Li cambia de estado (toggle).
 *   - La PC puede encender/apagar cualquier LED enviando un comando.
 *   - Cada cambio (de entrada o de salida) se informa a la PC.
 *
 * Protocolo (texto ASCII, un mensaje por línea, terminado en '\n', 9600 baudios):
 *
 *   Arduino -> PC   "E<n>:<v>"   estado de la entrada n (1 = pulsada, 0 = suelta)
 *   Arduino -> PC   "L<n>:<v>"   estado de la salida n  (1 = LED encendido)
 *   PC -> Arduino   "L<n>:<v>"   ordena encender (1) o apagar (0) el LED n
 *   PC -> Arduino   "?"          pide el estado completo de entradas y salidas
 *
 *   n = 1..N_CANALES, v = '0' o '1'. Ejemplo: "L2:1\n".
 *
 * Hardware (ver ../../esquematico):
 *   Pulsadores entre el pin y GND (se usa INPUT_PULLUP, no requieren resistencia).
 *   LEDs con resistencia de 220 ohm en serie, cátodo a GND.
 */

// ---------------------------------------------------------------- Configuración
const uint8_t N_CANALES = 2;                      // cantidad de pares entrada/LED
const uint8_t PIN_ENTRADA[N_CANALES] = {2, 3};    // pulsadores E1, E2
const uint8_t PIN_LED[N_CANALES]     = {8, 9};    // LEDs L1, L2

const unsigned long VELOCIDAD_SERIE = 9600;       // debe coincidir con Processing
const unsigned long TIEMPO_REBOTE_MS = 25;        // tiempo de estabilidad antirrebote
const uint8_t TAM_BUFFER = 16;                    // largo máximo de un comando

// ---------------------------------------------------------------------- Estado
bool estadoLed[N_CANALES];                        // true = LED encendido
bool pulsadorEstable[N_CANALES];                  // true = pulsado (ya filtrado)
bool lecturaPrevia[N_CANALES];                    // última lectura cruda
unsigned long instanteCambio[N_CANALES];          // cuándo cambió la lectura cruda

char bufferRx[TAM_BUFFER];                        // línea recibida en curso
uint8_t largoRx = 0;

// ------------------------------------------------------------------- Utilidades

/** Envía "<tipo><canal>:<valor>\n" a la PC. canal es el índice base 0. */
void enviarEstado(char tipo, uint8_t canal, bool valor) {
  Serial.print(tipo);
  Serial.print(canal + 1);
  Serial.print(':');
  Serial.println(valor ? 1 : 0);
}

/** Cambia el LED, actualiza el pin y avisa a la PC. */
void fijarLed(uint8_t canal, bool encendido) {
  estadoLed[canal] = encendido;
  digitalWrite(PIN_LED[canal], encendido ? HIGH : LOW);
  enviarEstado('L', canal, encendido);
}

/** Informa a la PC el estado actual de todas las entradas y salidas. */
void enviarEstadoCompleto() {
  for (uint8_t i = 0; i < N_CANALES; i++) {
    enviarEstado('E', i, pulsadorEstable[i]);
    enviarEstado('L', i, estadoLed[i]);
  }
}

// -------------------------------------------------------------------- Entradas

/**
 * Lee un pulsador con antirrebote sin bloquear el loop().
 * La lectura se acepta cuando se mantiene estable TIEMPO_REBOTE_MS.
 * Al detectar una pulsación (flanco de subida lógico) alterna el LED asociado.
 */
void actualizarPulsador(uint8_t canal) {
  // INPUT_PULLUP: pin en LOW significa pulsador apretado.
  bool lectura = (digitalRead(PIN_ENTRADA[canal]) == LOW);

  if (lectura != lecturaPrevia[canal]) {
    lecturaPrevia[canal] = lectura;
    instanteCambio[canal] = millis();
  }

  bool estable = (millis() - instanteCambio[canal]) >= TIEMPO_REBOTE_MS;
  if (estable && lectura != pulsadorEstable[canal]) {
    pulsadorEstable[canal] = lectura;
    enviarEstado('E', canal, lectura);
    if (lectura) {
      fijarLed(canal, !estadoLed[canal]);
    }
  }
}

// ------------------------------------------------------------------ Comunicación

/** Interpreta una línea completa recibida de la PC. Ignora lo que no entiende. */
void procesarComando(const char *linea) {
  if (linea[0] == '?') {
    enviarEstadoCompleto();
    return;
  }

  // Formato esperado: "L<n>:<v>"
  if (strlen(linea) == 4 && linea[0] == 'L' && linea[2] == ':') {
    int canal = linea[1] - '1';
    char valor = linea[3];
    bool canalValido = (canal >= 0 && canal < N_CANALES);
    bool valorValido = (valor == '0' || valor == '1');
    if (canalValido && valorValido) {
      fijarLed(canal, valor == '1');
    }
  }
}

/** Acumula los bytes recibidos y procesa cada línea al llegar '\n'. */
void leerSerie() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      bufferRx[largoRx] = '\0';
      procesarComando(bufferRx);
      largoRx = 0;
    } else if (c != '\r') {
      if (largoRx < TAM_BUFFER - 1) {
        bufferRx[largoRx++] = c;
      } else {
        largoRx = 0;                  // línea demasiado larga: se descarta
      }
    }
  }
}

// ------------------------------------------------------------------ Arduino core

void setup() {
  Serial.begin(VELOCIDAD_SERIE);
  for (uint8_t i = 0; i < N_CANALES; i++) {
    pinMode(PIN_ENTRADA[i], INPUT_PULLUP);
    pinMode(PIN_LED[i], OUTPUT);
    digitalWrite(PIN_LED[i], LOW);
    estadoLed[i] = false;
    pulsadorEstable[i] = false;
    lecturaPrevia[i] = false;
    instanteCambio[i] = millis();
  }
  enviarEstadoCompleto();             // la PC arranca sincronizada
}

void loop() {
  for (uint8_t i = 0; i < N_CANALES; i++) {
    actualizarPulsador(i);
  }
  leerSerie();
}
