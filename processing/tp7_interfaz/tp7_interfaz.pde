/**
 * TP7 - Entorno gráfico (Informática II, UTN FRM)
 *
 * Interfaz en Processing para monitorear y controlar las entradas/salidas
 * de un Arduino por puerto serie.
 *
 *   - "Entradas" (E1, E2): se pintan de verde mientras el pulsador está apretado.
 *   - "Salidas"  (L1, L2): círculos que se pintan de amarillo cuando el LED está
 *     encendido. Un clic del mouse sobre el círculo enciende/apaga el LED.
 *   - La pantalla solo refleja lo que el Arduino confirma por serie, por lo que
 *     siempre muestra el estado real del hardware.
 *
 * Protocolo (texto ASCII, un mensaje por línea terminado en '\n'):
 *   Arduino -> PC   "E<n>:<v>"  estado de la entrada n
 *                   "L<n>:<v>"  estado de la salida n
 *   PC -> Arduino   "L<n>:<v>"  ordena encender (1) o apagar (0) el LED n
 *                   "?"         pide el estado completo
 *
 * Modo simulación (sin placa): se activa con SIMULAR_POR_DEFECTO = true, o solo
 * si no se puede abrir el puerto. Las teclas 1 y 2 y el clic sobre E1/E2 imitan
 * a los pulsadores; el programa reproduce el mismo protocolo que el Arduino real.
 */

import processing.serial.*;

// ---------------------------------------------------------------- Configuración
final boolean SIMULAR_POR_DEFECTO = true;  // false para usar el Arduino real
final int INDICE_PUERTO = 0;               // posición del Arduino en Serial.list()
final int VELOCIDAD_SERIE = 9600;          // debe coincidir con el firmware
final int N_CANALES = 2;

// Geometría de la interfaz
final int ANCHO = 700;
final int ALTO = 300;
final int Y_ENTRADAS = 60;
final int Y_SALIDAS = 215;
final int ANCHO_CASILLERO = 80;
final int ALTO_CASILLERO = 50;
final int DIAMETRO_LED = 70;
final int[] X_ENTRADA = {400, 520};
final int[] X_SALIDA = {440, 560};

// Colores
final color COLOR_FONDO = color(255);
final color COLOR_APAGADO = color(255);
final color COLOR_ENTRADA_ALTA = color(0, 200, 0);
final color COLOR_LED_ENCENDIDO = color(255, 220, 0);
final color COLOR_ETIQUETA_ENTRADAS = color(150, 180, 220);
final color COLOR_ETIQUETA_SALIDAS = color(250, 225, 210);

// ---------------------------------------------------------------------- Estado
Serial puerto;
boolean simulado = SIMULAR_POR_DEFECTO;
String mensajeEstado = "";

boolean[] entradas = new boolean[N_CANALES];  // true = pulsador apretado
boolean[] salidas = new boolean[N_CANALES];   // true = LED encendido

// ---------------------------------------------------------------------- Ciclo
void setup() {
  size(ANCHO, ALTO);
  textAlign(CENTER, CENTER);
  if (!simulado) {
    abrirPuerto();
  }
  actualizarMensajeEstado();
}

void draw() {
  background(COLOR_FONDO);
  dibujarEtiquetas();
  for (int i = 0; i < N_CANALES; i++) {
    dibujarEntrada(i);
    dibujarSalida(i);
  }
  fill(120);
  text(mensajeEstado, ANCHO / 2, 20);
}

// ---------------------------------------------------------------------- Puerto

/** Abre el puerto serie; si falla, pasa a modo simulación con un aviso. */
void abrirPuerto() {
  String[] disponibles = Serial.list();
  printArray(disponibles);
  try {
    puerto = new Serial(this, disponibles[INDICE_PUERTO], VELOCIDAD_SERIE);
    puerto.bufferUntil('\n');
    puerto.write("?\n");                 // pide el estado inicial
  }
  catch (Exception e) {
    println("No se pudo abrir el puerto (" + e.getMessage() + "). Modo simulación.");
    simulado = true;
  }
}

void actualizarMensajeEstado() {
  mensajeEstado = simulado
    ? "MODO SIMULACION - teclas 1/2 o clic en E1/E2 = pulsadores"
    : "Conectado a " + Serial.list()[INDICE_PUERTO];
}

/** Evento de Processing: llega una línea completa desde el Arduino. */
void serialEvent(Serial p) {
  String linea = p.readStringUntil('\n');
  if (linea != null) {
    procesarMensaje(linea);
  }
}

/** Envía un comando al Arduino (o a la simulación si no hay placa). */
void enviarComando(String comando) {
  println("TX -> " + comando);
  if (simulado) {
    simularArduino(comando);
  } else {
    puerto.write(comando + "\n");
  }
}

// ------------------------------------------------------------------- Protocolo

/**
 * Interpreta un mensaje "E<n>:<v>" o "L<n>:<v>" y actualiza el estado.
 * Los mensajes mal formados se descartan.
 */
void procesarMensaje(String mensaje) {
  String m = trim(mensaje);
  println("RX <- " + m);
  if (m.length() != 4 || m.charAt(2) != ':') {
    return;
  }
  int canal = m.charAt(1) - '1';
  char valor = m.charAt(3);
  if (canal < 0 || canal >= N_CANALES || (valor != '0' && valor != '1')) {
    return;
  }
  boolean alto = (valor == '1');
  if (m.charAt(0) == 'E') {
    entradas[canal] = alto;
  } else if (m.charAt(0) == 'L') {
    salidas[canal] = alto;
  }
}

// ------------------------------------------------------------------ Simulación

/** Imita al firmware: responde a "L<n>:<v>" confirmando el nuevo estado. */
void simularArduino(String comando) {
  if (comando.length() == 4 && comando.charAt(0) == 'L' && comando.charAt(2) == ':') {
    procesarMensaje(comando);
  }
}

/** Imita un pulsador físico: informa la entrada y, al apretar, alterna el LED. */
void simularPulsador(int canal, boolean apretado) {
  if (entradas[canal] == apretado) {
    return;                              // sin cambio (p. ej. tecla repetida)
  }
  procesarMensaje("E" + (canal + 1) + ":" + (apretado ? 1 : 0));
  if (apretado) {
    procesarMensaje("L" + (canal + 1) + ":" + (salidas[canal] ? 0 : 1));
  }
}

// --------------------------------------------------------------------- Eventos

void mousePressed() {
  for (int i = 0; i < N_CANALES; i++) {
    if (clicSobreSalida(i)) {
      // Se pide el cambio; la pantalla se actualiza al recibir la confirmación.
      enviarComando("L" + (i + 1) + ":" + (salidas[i] ? 0 : 1));
    } else if (simulado && clicSobreEntrada(i)) {
      simularPulsador(i, true);
    }
  }
}

void mouseReleased() {
  if (simulado) {
    for (int i = 0; i < N_CANALES; i++) {
      simularPulsador(i, false);
    }
  }
}

void keyPressed() {
  if (simulado && key >= '1' && key < '1' + N_CANALES) {
    simularPulsador(key - '1', true);
  }
}

void keyReleased() {
  if (simulado && key >= '1' && key < '1' + N_CANALES) {
    simularPulsador(key - '1', false);
  }
}

// ----------------------------------------------------------------------- Dibujo

boolean clicSobreSalida(int i) {
  return dist(mouseX, mouseY, X_SALIDA[i], Y_SALIDAS) < DIAMETRO_LED / 2.0;
}

boolean clicSobreEntrada(int i) {
  return mouseX > X_ENTRADA[i] - ANCHO_CASILLERO / 2
      && mouseX < X_ENTRADA[i] + ANCHO_CASILLERO / 2
      && mouseY > Y_ENTRADAS - ALTO_CASILLERO / 2
      && mouseY < Y_ENTRADAS + ALTO_CASILLERO / 2;
}

void dibujarEtiquetas() {
  stroke(0);
  fill(COLOR_ETIQUETA_ENTRADAS);
  rect(60, Y_ENTRADAS - ALTO_CASILLERO / 2, 150, ALTO_CASILLERO);
  fill(COLOR_ETIQUETA_SALIDAS);
  rect(60, Y_SALIDAS - ALTO_CASILLERO / 2, 150, ALTO_CASILLERO);
  fill(0);
  text("Entradas", 135, Y_ENTRADAS);
  text("Salidas", 135, Y_SALIDAS);
}

void dibujarEntrada(int i) {
  stroke(0);
  fill(entradas[i] ? COLOR_ENTRADA_ALTA : COLOR_APAGADO);
  rectMode(CENTER);
  rect(X_ENTRADA[i], Y_ENTRADAS, ANCHO_CASILLERO, ALTO_CASILLERO);
  rectMode(CORNER);
  fill(0);
  text("E" + (i + 1), X_ENTRADA[i], Y_ENTRADAS);
}

void dibujarSalida(int i) {
  stroke(0);
  fill(salidas[i] ? COLOR_LED_ENCENDIDO : COLOR_APAGADO);
  ellipse(X_SALIDA[i], Y_SALIDAS, DIAMETRO_LED, DIAMETRO_LED);
  fill(0);
  text("L" + (i + 1), X_SALIDA[i], Y_SALIDAS);
}
