# Sistema de Control de Iluminación Descentralizado

LightControl es un sistema distribuido basado en microcontroladores ESP32, diseñado para controlar de manera inalámbrica luminarias de desinfección (u otros dispositivos) desde una unidad **master** hacia múltiples nodos **slave**. La comunicación se realiza utilizando el protocolo **ESP-NOW**, optimizado para redes sin infraestructura WiFi.

## 📌 Descripción general

El proyecto **LightControl** permite controlar salidas de manera remota desde un master hacia varios slaves, usando el protocolo de comunicación ESPNOW (por ejemplo, relés o LEDs) según horarios semanales definidos por el usuario.

- ### **Master ESP32 - Funciones básicas**
  - Se comunica con el usuario (pantalla Nextion y consola serial).
  - Controla el estado de encendido/apagado de:
    - LED principal (`GPIO2` de cada slave). (Luz ambiental)
    - Slot 1 (`GPIO33` de cada slave). (Luz de desinfección)
    - Slot 2 (`GPIO32` de cada slave). (Equipo de purificación)
  - Consulta estados programados de encendido/apagado diarios (por día de la semana), dos por dia, definidos por slots (slot0 y slot1). Contempla horarios "overnight" (ej On: 20:30 de un viernes Off 03:45 del sábado)
  - Envía comandos a todos los slaves.

- ### **Con la pantalla táctil Nextion desde el Master:**
  - Muestra la fecha y hora actual.
  - Permite controlar el encendido /apagado de la luz ambiental (GPIO2 en los slaves) con tres valores de iluminación a la luz ambiental (GPIO19): alta (100%) media (66%) y baja (33%).
  - Permite modificar fecha y hora.
  - Soporta programación de encendidos/apagados diarios (hasta 2 ciclos por día denominados slots).
  - Almacena los datos de los ciclos programados en una memoria EEPROM externa (24C32).
  - Mantiene la hora mediante un RTC DS3231, incluso sin energía.
  - Ejecuta los ciclos aún si se reinician los ESP32.
  - Permite el borrado de toda la programación de ciclos desde la interfaz.
  - Permite consultar los ciclos configurados por día.
  - Todo el sistema de programación de ciclos ON OFF está protegido por password, para evitar que cualquiera la modifique.

- ### **Slave ESP32 - Funciones básicas**
  - Escucha mensajes del master.
  - Ejecuta comandos para encender/apagar salidas.

## 📡 Comunicación — ESP-NOW

Se utiliza una estructura de mensajes optimizada para minimizar el uso de ancho de banda. Todos los mensajes entre dispositivos se encapsulan en estructuras definidas en el módulo`CommProtocol.h`.

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
         └── main.cpp
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
| **LED azul**  | GPIO02            | Salida Desinfección  simulada (led interno)     |
| **LED rojo**  | GPIO25            | Salida Purificación simulada (led externo)|
---

## 🔌 Conexiones SLAVE
| Componente          | Pin ESP32       | Detalles                                 |
|---------------------|------------------|-------------------------------------------|
| **LED azul**  | GPIO32            | Salida Desinfección simulada (led externo)     |
| **LED rojo**  | GPIO33            | Salida Purificación simulada (led externo) |
| **LED azul built in**  | GPIO02            | Salida simulada luz ambiental (led interno) |
| **Control intensidad**  | GPIO19            | Salida para controlar por PWM la luz ambiental |
---

## 🧱 Diseño modular

Los módulos del sistema ubicados dentro de la carpeta `lib/`, están desarrollados siguiendo el principio de responsabilidad única (*Single Responsibility Principle*).  
Cada uno encapsula una funcionalidad específica (como gestión del RTC, comunicación con el display, acceso a la EEPROM, etc.), lo que permite un código más limpio, mantenible y reutilizable.

## 📋 Requisitos adicionales

- Carga del `NextionClock.tft` en la pantalla Nextion mediante tarjeta microSD o mediante adaptador USB-TTL

## 👤 Autor

Es un ejercicio educativo desarrollado por José Faginas, usando el siguiente toolchain: **VsCode + PlatformIO en C++**. Para la interfaz de usuario: **Nextion Editor**. 
