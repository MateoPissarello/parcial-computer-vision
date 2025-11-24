# Requirements Document

## Introduction

Este documento especifica los requisitos para implementar un sistema de etiquetado automático de monedas colombianas basado en el diámetro detectado. El sistema debe identificar la denominación de cada moneda detectada (50, 100, 200, 500, 1000 COP) comparando su radio en píxeles con valores de referencia conocidos, y mostrar esta información visualmente en la imagen procesada.

## Glossary

- **Sistema de Detección**: El componente de software que procesa imágenes para identificar y clasificar monedas colombianas
- **Radio en Píxeles**: La medida del radio de una moneda detectada expresada en unidades de píxel de la imagen
- **Denominación**: El valor monetario de una moneda (50, 100, 200, 500, o 1000 pesos colombianos)
- **Tolerancia**: El margen de error permitido al comparar el radio detectado con los valores de referencia
- **Imagen Anotada**: La imagen de salida con círculos y etiquetas que muestran las monedas detectadas y sus denominaciones

## Requirements

### Requirement 1

**User Story:** Como usuario del sistema, quiero que las monedas detectadas sean clasificadas automáticamente por su denominación, para poder identificar el valor de cada moneda en la imagen.

#### Acceptance Criteria

1. WHEN el Sistema de Detección procesa una imagen con monedas, THE Sistema de Detección SHALL comparar el Radio en Píxeles de cada moneda detectada con los valores de referencia de las denominaciones colombianas
2. THE Sistema de Detección SHALL mantener una tabla de referencia con los radios en píxeles para cada Denominación de moneda colombiana (50, 100, 200, 500, 1000 COP)
3. WHEN el Radio en Píxeles de una moneda detectada está dentro de la Tolerancia definida para una Denominación, THE Sistema de Detección SHALL asignar esa Denominación a la moneda
4. IF el Radio en Píxeles de una moneda no coincide con ninguna Denominación dentro de la Tolerancia, THEN THE Sistema de Detección SHALL marcar la moneda como no clasificada

### Requirement 2

**User Story:** Como usuario del sistema, quiero ver las denominaciones etiquetadas visualmente en la imagen de salida, para poder verificar fácilmente qué monedas fueron detectadas y su valor.

#### Acceptance Criteria

1. THE Sistema de Detección SHALL dibujar un círculo alrededor de cada moneda detectada en la Imagen Anotada
2. WHEN una moneda es clasificada con una Denominación, THE Sistema de Detección SHALL mostrar el valor de la Denominación como texto cerca del centro de la moneda en la Imagen Anotada
3. THE Sistema de Detección SHALL usar un tamaño de fuente legible y un color que contraste con la imagen de fondo para las etiquetas de texto
4. THE Sistema de Detección SHALL guardar la Imagen Anotada en el directorio de salida especificado

### Requirement 3

**User Story:** Como usuario del sistema, quiero ver un resumen del conteo y valor total de las monedas detectadas, para conocer la cantidad total de dinero en la imagen.

#### Acceptance Criteria

1. THE Sistema de Detección SHALL contar cuántas monedas de cada Denominación fueron detectadas
2. THE Sistema de Detección SHALL calcular el valor total sumando todas las denominaciones detectadas
3. THE Sistema de Detección SHALL mostrar en la consola el conteo de monedas por Denominación
4. THE Sistema de Detección SHALL mostrar en la consola el valor total en pesos colombianos

### Requirement 4

**User Story:** Como desarrollador del sistema, quiero que la Tolerancia de clasificación sea configurable, para poder ajustar la precisión del sistema según las condiciones de las imágenes.

#### Acceptance Criteria

1. THE Sistema de Detección SHALL definir un valor de Tolerancia como porcentaje del Radio en Píxeles esperado
2. WHEN se compara un Radio en Píxeles detectado con un valor de referencia, THE Sistema de Detección SHALL considerar una coincidencia si la diferencia está dentro del rango de Tolerancia
3. THE Sistema de Detección SHALL permitir ajustar el valor de Tolerancia mediante una constante en el código
