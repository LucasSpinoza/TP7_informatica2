# Interfaz Processing

Código: [`tp7_interfaz/tp7_interfaz.pde`](tp7_interfaz/tp7_interfaz.pde)

> La carpeta del sketch y el archivo `.pde` deben llamarse igual (`tp7_interfaz`); no renombrar uno sin el otro.

## Qué muestra

| Elemento | Significado |
|---|---|
| Casillero **E1 / E2** | Blanco = pulsador suelto, **verde** = pulsado |
| Círculo **L1 / L2** | Blanco = LED apagado, **amarillo** = LED encendido |
| Clic sobre L1 / L2 | Pide al Arduino alternar ese LED |

La pantalla solo cambia cuando el Arduino confirma el estado por serie.

## Ejecutar con el Arduino real

1. Cargar el firmware de [`../arduino`](../arduino/README.md) y **cerrar el Monitor Serie** del Arduino IDE.
2. Abrir `tp7_interfaz.pde` en Processing 4.
3. En la parte superior del código:
   ```java
   final boolean SIMULAR_POR_DEFECTO = false;
   final int INDICE_PUERTO = 0;
   ```
4. Ejecutar (▶) y mirar la consola: imprime la lista de puertos (`printArray(Serial.list())`). Si el Arduino no es el índice 0, cambiar `INDICE_PUERTO` por su posición (por ejemplo `COM3` en Windows o `/dev/ttyACM0` en Linux) y volver a ejecutar.
5. La barra superior debe decir *"Conectado a …"*. Si no se puede abrir el puerto, el programa avisa en consola y pasa solo a modo simulación.

En **Linux**, si el puerto da "permission denied": `sudo usermod -aG dialout $USER` y reiniciar sesión.

## Ejecutar sin placa (modo simulación)

Con `SIMULAR_POR_DEFECTO = true` no se necesita hardware:

| Acción | Efecto |
|---|---|
| Mantener tecla **1** o **2**, o mantener clic sobre **E1** / **E2** | Simula el pulsador: el casillero se pone verde y alterna L1 / L2 |
| Clic sobre **L1** / **L2** | Alterna el LED |

La simulación usa las mismas funciones de protocolo que el modo real. En la consola se ven los mensajes `TX ->` (enviados) y `RX <-` (recibidos), útiles para comprobar el protocolo contra el Monitor Serie del Arduino.

> Hacer clic en la ventana antes de usar el teclado, para que tenga el foco.

## Estructura del código

| Sección | Contenido |
|---|---|
| Configuración | Constantes: modo, puerto, baudios, geometría y colores |
| Puerto | `abrirPuerto()`, `serialEvent()`, `enviarComando()` |
| Protocolo | `procesarMensaje()`: interpreta y valida `E<n>:<v>` / `L<n>:<v>` |
| Simulación | `simularArduino()`, `simularPulsador()` |
| Eventos | `mousePressed()`, `mouseReleased()`, `keyPressed()`, `keyReleased()` |
| Dibujo | `dibujarEntrada()`, `dibujarSalida()`, `dibujarEtiquetas()` |
