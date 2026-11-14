# Firmware Arduino

Código: [`tp7_arduino/tp7_arduino.ino`](tp7_arduino/tp7_arduino.ino)

## Qué hace

- Lee dos pulsadores con **antirrebote no bloqueante** (`millis()`, 25 ms).
- Al apretar el pulsador `Ei`, **alterna** el LED `Li`.
- Recibe comandos de la PC para encender/apagar cada LED.
- Informa a la PC cada cambio de entrada o salida, y su estado completo al arrancar o ante el comando `?`.

## Conexiones

| Elemento | Pin Arduino | Notas |
|---|---|---|
| Pulsador E1 | D2 | El otro terminal a GND (usa `INPUT_PULLUP`) |
| Pulsador E2 | D3 | El otro terminal a GND (usa `INPUT_PULLUP`) |
| LED L1 | D8 | Ánodo → resistencia 220 Ω → D8, cátodo a GND |
| LED L2 | D9 | Ánodo → resistencia 220 Ω → D9, cátodo a GND |

Los pines se cambian en las constantes `PIN_ENTRADA` y `PIN_LED` del inicio del `.ino`. Para agregar canales, aumentar `N_CANALES` y completar ambos arreglos (en Processing también hay que ajustar `N_CANALES`, `X_ENTRADA` y `X_SALIDA`).

## Cargar el programa

1. Abrir `tp7_arduino/tp7_arduino.ino` en el Arduino IDE.
2. Seleccionar la placa (*Herramientas → Placa → Arduino UNO*) y el puerto.
3. Subir el programa.

## Probarlo sin Processing

Abrir el Monitor Serie a **9600 baudios** con terminación **Nueva línea**:

| Acción | Resultado esperado |
|---|---|
| Apretar pulsador 1 | `E1:1`, `L1:1` (el LED cambia); al soltar, `E1:0` |
| Enviar `L2:1` | Se enciende el LED 2 y responde `L2:1` |
| Enviar `L2:0` | Se apaga el LED 2 y responde `L2:0` |
| Enviar `?` | Imprime `E1`, `L1`, `E2`, `L2` |
| Enviar algo inválido (`L9:1`, `hola`) | Se ignora |

Recordar **cerrar el Monitor Serie** antes de ejecutar Processing.

## Protocolo

Ver la tabla en el [README principal](../README.md#protocolo-de-comunicación).

## Decisiones de diseño

- **Antirrebote con `millis()`** en lugar de `delay()`, para no bloquear la lectura serie.
- **Buffer `char[]` de tamaño fijo** en lugar de `String`, para evitar fragmentación de memoria en AVR; las líneas demasiado largas se descartan.
- **Validación de comandos:** se verifican formato, canal y valor antes de actuar.
- **Una sola función (`fijarLed`)** cambia el LED y avisa a la PC, así ambos orígenes (pulsador y PC) se comportan igual.
