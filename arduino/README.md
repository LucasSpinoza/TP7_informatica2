# Firmware Arduino

Código: [`tp7_arduino/tp7_arduino.ino`](tp7_arduino/tp7_arduino.ino)

## Qué hace

- Lee dos pulsadores con antirrebote.
- Al apretar el pulsador `Ei`, **alterna** el LED `Li`.
- Recibe comandos de la PC para encender o apagar cada LED.
- Informa a la PC cada cambio de entrada o de salida.

## Conexiones

| Elemento | Pin Arduino | Notas |
|---|---|---|
| Pulsador E1 | D2 | El otro terminal a GND (usa `INPUT_PULLUP`) |
| Pulsador E2 | D3 | El otro terminal a GND (usa `INPUT_PULLUP`) |
| LED L1 | D8 | Ánodo → resistencia 220 Ω → D8, cátodo a GND |
| LED L2 | D9 | Ánodo → resistencia 220 Ω → D9, cátodo a GND |

Los pines se cambian en los arreglos `PIN_E` y `PIN_L` del inicio del `.ino`.

## Cargar el programa

1. Abrir `tp7_arduino/tp7_arduino.ino` en el Arduino IDE.
2. Seleccionar la placa (*Herramientas → Placa → Arduino UNO*) y el puerto.
3. Subir el programa con el botón **→** (el botón ✔ solo verifica, no carga).

## Probarlo sin Processing

Abrir el Monitor Serie a **9600 baudios** con terminación **Nueva línea**:

| Acción | Resultado esperado |
|---|---|
| Apretar pulsador 1 | `E1:1`, `L1:1` (el LED cambia); al soltar, `E1:0` |
| Enviar `L2:1` | Se enciende el LED 2 y responde `L2:1` |
| Enviar `L2:0` | Se apaga el LED 2 y responde `L2:0` |
| Enviar algo inválido (`L9:1`, `hola`) | Se ignora |

Recordar **cerrar el Monitor Serie** antes de ejecutar Processing: si queda abierto, ocupa el puerto.

## Protocolo

Ver la tabla en el [README principal](../README.md#protocolo-de-comunicación).

## Estructura del código

| Parte | Función |
|---|---|
| `enviarL(i)` | Informa a la PC el estado del LED `i` |
| `leerPulsadores()` | Lee los pulsadores con antirrebote, informa `E<n>` y alterna el LED |
| `leerComandos()` | Arma el comando recibido por serie y lo ejecuta al llegar `\n` |
| `setup()` / `loop()` | Configuración de pines y ciclo principal |
