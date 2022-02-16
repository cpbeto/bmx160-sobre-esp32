# bmx160-sobre-esp32 [![CI Badge](https://github.com/cpbeto/bmx160-sobre-esp32/actions/workflows/main.yml/badge.svg)](https://github.com/cpbeto/bmx160-sobre-esp32/actions/workflows/main.yml)
Implementación de BMX160 sobre ESP32.

Este proyecto contempla:
 - El desarrollo del driver del sensor Bosch BMX160 para la familia de microcontroladores ESP32 (`bmx160.h`).
 - La implementación de un AHRS por software que consuma datos de un sensor 9DOF genérico (`motion_sensor.h`).

El **objetivo** es que el sistema sea capaz de medir:
 - Aceleración, velocidad angular y campo magnético (9DOF de base).
 - Aceleración de la gravedad.
 - Aceleración lineal.
 - Compás (Norte magnético).
 - Orientación absoluta (Euler/Cuaternión).
 
 ## Compilación
 
El proyecto está desarrollado en VS Code + PlatformIO, importar y compilar.

## todo

- Completar el driver del sensor BMX160.
- Mejorar la medición de la gravedad con algún filtro de mayor complejidad.
- Implementar la medición de aceleración lineal.
- Implementar la medición de velocidad angular con corrección de offset.
- Implementar la medición de ángulo de giro.
- Implementar la medición de orientación absoluta.

## Referencias

![Bosch, "Absolute Orientation Sensor BMX160"](https://www.bosch-sensortec.com/products/motion-sensors/absolute-orientation-sensors/bmx160/)

![DFRobot, dfrobot_mx160 GitHub repository.](https://github.com/DFRobot/DFRobot_BMX160)

![W3C, "Motion Sensors Explainer"](https://www.w3.org/TR/motion-sensors/)

![Embedded Computing Design, "Basics of 6DOF and 9DOF sensor fusion".](https://www.embeddedcomputing.com/technology/analog-and-power/basics-of-6dof-and-9dof-sensor-fusion)

![NXP Semiconductors, "NXP Sensor Fusion"](https://www.nxp.com/design/sensor-developer-resources/nxp-sensor-fusion)

![Bartz, et al. razor-9dof-ahrs GitHub repository.](https://github.com/Razor-AHRS/razor-9dof-ahrs)

![Adafruit, "How to Fuse Motion Sensor Data into AHRS Orientation (Euler/Quaternions)"](https://learn.adafruit.com/how-to-fuse-motion-sensor-data-into-ahrs-orientation-euler-quaternions)
