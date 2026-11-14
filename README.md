# TP7 – Entorno gráfico: control de E/S de Arduino con Processing

**Autor:** [Lucas Spinoza]
**Cátedra:** Informática II – Ingeniería Electrónica (UTN FRM) – 2026
**Video de demostración:** [ENLACE AL VIDEO] *(se ve mi rostro y el TP funcionando)*

## Descripción

Interfaz gráfica en **Processing** que muestra y controla las entradas y salidas de una placa **Arduino UNO** conectada por USB:

- **Entradas (E1, E2):** reflejan en color (verde = pulsado) el estado de dos pulsadores.
- **Salidas (L1, L2):** reflejan el estado de dos LEDs (amarillo = encendido).
- **Mouse:** un clic sobre L1 o L2 enciende/apaga el LED correspondiente.
- **Pulsadores físicos:** al apretar Ei, el Arduino cambia el LED Li y avisa a la PC.

El Arduino es el dueño del estado real de los LEDs: la PC envía órdenes y la pantalla se actualiza solo cuando el Arduino confirma, de modo que lo dibujado siempre coincide con el hardware.

```
 Pulsadores ──┐                              ┌── Processing (interfaz)
              ├── Arduino UNO ══ USB/serie ══╡
 LEDs ────────┘     (firmware)               └── PC (Windows / Linux)
```

## Estructura del repositorio

```
.
├── README.md
├── arduino/
│   ├── README.md              Hardware, pines y uso del firmware
│   └── tp7_arduino/
│       └── tp7_arduino.ino
├── processing/
│   ├── README.md              Cómo ejecutar la interfaz (con y sin placa)
│   └── tp7_interfaz/
│       └── tp7_interfaz.pde
└── esquematico/
    ├── README.md              Conexiones
    └── esquematico.png        (diagrama de conexiones; reemplazable por la captura de Tinkercad)
```

## Protocolo de comunicación

Texto ASCII a 9600 baudios, un mensaje por línea terminado en `\n`.

| Sentido | Mensaje | Significado |
|---|---|---|
| Arduino → PC | `E<n>:<v>` | Entrada `n` en estado `v` (1 = pulsada, 0 = suelta) |
| Arduino → PC | `L<n>:<v>` | LED `n` en estado `v` (1 = encendido) |
| PC → Arduino | `L<n>:<v>` | Ordena encender (1) o apagar (0) el LED `n` |
| PC → Arduino | `?` | Pide el estado completo |

Ejemplo: al apretar el pulsador 1 el Arduino envía `E1:1` y `L1:1`; al hacer clic sobre L2 la PC envía `L2:1` y el Arduino responde `L2:1`.

## Cómo usarlo (resumen)

1. Armar el circuito (ver [`esquematico/`](esquematico/README.md)) y cargar [`arduino/tp7_arduino/tp7_arduino.ino`](arduino/tp7_arduino/tp7_arduino.ino) con el Arduino IDE.
2. Cerrar el Monitor Serie del IDE (ocupa el puerto).
3. Abrir [`processing/tp7_interfaz/tp7_interfaz.pde`](processing/tp7_interfaz/tp7_interfaz.pde), poner `SIMULAR_POR_DEFECTO = false`, elegir `INDICE_PUERTO` y ejecutar.

Detalles en los README de cada carpeta. Sin placa, la interfaz también funciona en modo simulación.

## Requisitos

- Arduino IDE 2.x (o 1.8.x) y una placa Arduino UNO (o compatible).
- Processing 4.x (librería `Serial` incluida).
- 2 pulsadores, 2 LEDs, 2 resistencias de 220 Ω, cables.
