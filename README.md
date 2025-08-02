# Sistema de Control de Iluminación Descentralizado

LightControl es un sistema distribuido basado en microcontroladores ESP32, diseñado para controlar de manera inalámbrica luminarias de desinfección (u otros dispositivos) desde una unidad **master** hacia múltiples nodos **slave**. La comunicación se realiza utilizando el protocolo **ESP-NOW**, optimizado para redes sin infraestructura WiFi.

## 🧱 Arquitectura General

- **Master ESP32**
  - Se comunica con el usuario (pantalla Nextion y consola serial).
  - Controla el estado de encendido/apagado de:
    - LED principal (`GPIO2` de cada slave). (Luz ambiental)
    - Slot 1 (`GPIO33` de cada slave). (Luz de desinfección)
    - Slot 2 (`GPIO32` de cada slave). (Equipo de purificación)
  - Consulta estados programados de encendido/apagado diarios (por día de la semana), dos por dia, definidos por slots (slot0 y slot1)
  - Envía comandos a todos los slaves.

- **Slave ESP32**
  - Escucha mensajes del master.
  - Ejecuta comandos para encender/apagar salidas.


## 📌 Descripción general

El proyecto **LightControl** permite controlar salidas de manera remota desde un master hacia varios slaves, usando el protocolo de comunicación ESPNOW (por ejemplo, relés o LEDs) según horarios semanales definidos por el usuario.

El sistema desde el Master:

- Muestra la fecha y hora actual en una pantalla Nextion.
- Permite modificar fecha y hora desde la interfaz táctil.
- Soporta programación de encendidos/apagados diarios (hasta 2 ciclos por día).
- Almacena los datos en una memoria EEPROM externa (24C32).
- Mantiene la hora mediante un RTC DS3231, incluso sin energía.
- Ejecuta los ciclos aún si se reinicia el ESP32.
- Permite borrar toda la programación desde la interfaz.
- Permite consultar los ciclos configurados por día.


## 📡 Comunicación — ESP-NOW

Se utiliza una estructura de mensajes binarios optimizada para minimizar el uso de ancho de banda. Todos los mensajes entre dispositivos se encapsulan en estructuras definidas en `CommProtocol.h`.

## 📁 Estructura del proyecto
```text
LightControl
│ 
├── Master
│   ├── lib
│   │   ├── CommProtocol
│   │   │   └── CommProtocol.h
│   │   ├── DateTimeUtils
│   │   │   ├── DateTimeUtils.cpp
│   │   │   └── DateTimeUtils.h
│   │   ├── DS3231Manager
│   │   │   ├── DS3231Manager.cpp
│   │   │   └── DS3231Manager.h
│   │   ├── EEPROMManager
│   │   │   ├── EEPROMManager.cpp
│   │   │   └── EEPROMManager.h
│   │   ├── EspNowInterface
│   │   │   ├── EspNowInterface.cpp
│   │   │   └── EspNowInterface.h
│   │   ├── MasterLogic
│   │   │   ├── MasterLogic.cpp
│   │   │   └── MasterLogic.h
│   │   ├── NextionManager
│   │   │   ├── NextionManager.cpp
│   │   │   └── NextionManager.h
│   │   ├── SchedulerManager
│   │   │   ├── SchedulerManager.cpp
│   │   │   └── SchedulerManager.h
│   │   └── SystemManager
│   │       ├── SystemManager.cpp
│   │       └── SystemManager.h
│   ├── platformio.ini
│   └── src
│       └── main.cpp
├── NEXTION
│   └── README.md
├── README.md
└── Slave
    ├── lib
    │   ├── CommProtocol
    │   │   └── CommProtocol.h
    │   ├── EspNowInterface
    │   │   ├── EspNowInterface.cpp
    │   │   └── EspNowInterface.h
    │   └── SlaveApp
    │       ├── SlaveApp.cpp
    │       └── SlaveApp.h
    ├── platformio.ini
    └── src
```

## 🔌 Conexiones MASTER

| Componente          | Pin ESP32       | Detalles                                 |
|---------------------|------------------|-------------------------------------------|
| **DS3231 (RTC)**    | GPIO21 (SDA)     | Bus I2C compartido                        |
|                     | GPIO22 (SCL)     | Bus I2C compartido                        |
| **EEPROM 24C32**    | GPIO21 (SDA)     | Misma línea I2C que el DS3231             |
|                     | GPIO22 (SCL)     | Misma línea I2C que el DS3231             |
|                     | VCC              | 3.3V o 5V según módulo                    |
|                     | GND              | GND común                                |
| **Nextion Display** | GPIO17 (TX2)     | TX del ESP32 → RX del Nextion             |
|                     | GPIO16 (RX2)     | RX del ESP32 ← TX del Nextion             |
|                     | VCC              | 5V (o 3.3V si el modelo lo soporta)       |
|                     | GND              | GND común                                |
| **LED azul**  | GPIO02            | Salida Desinfección simulada     |
| **LED rojo**  | GPIO25            | Salida Purificación simulada|
---

## 🔌 Conexiones SLAVE
| Componente          | Pin ESP32       | Detalles                                 |
|---------------------|------------------|-------------------------------------------|
| **LED azul**  | GPIO32            | Salida Desinfección simulada     |
| **LED rojo**  | GPIO33            | Salida Purificación simulada |
---

## 🧱 Diseño modular

Los módulos del sistema ubicados dentro de la carpeta `lib/` están desarrollados siguiendo el principio de responsabilidad única (*Single Responsibility Principle*).  
Cada uno encapsula una funcionalidad específica (como gestión del RTC, comunicación con el display, acceso a la EEPROM, etc.), lo que permite un código más limpio, mantenible y reutilizable.

## 📋 Requisitos

- Carga del `NextionClock.tft` en la pantalla Nextion mediante tarjeta microSD o USB-TTL

## 👤 Autor

Es un ejercicio educativo desarrollado por José Faginas, usando el siguiente toolchain: VsCode + PlatformIO en C++ y para la interfaz de usuario: el Nextion Editor. 
