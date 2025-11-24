# Implementation Plan

- [ ] 1. Actualizar la estructura CoinType y crear función de inicialización
  - Modificar la estructura `CoinType` para usar un solo campo `referenceRadius` en lugar de `radiusPxNew` y `radiusPxOld`
  - Crear función `initializeCoinTypes()` que retorne un vector con las 5 denominaciones de monedas colombianas (50, 100, 200, 500, 1000 COP) con sus radios de referencia calculados como promedios
  - Definir constantes de configuración: `TOLERANCE_PERCENTAGE`, colores para etiquetas y círculos, y parámetros de fuente
  - _Requirements: 1.2, 4.1, 4.3_

- [x] 2. Implementar función de búsqueda y clasificación de monedas
  - [x] 2.1 Crear estructura `ClassificationResult` con campos para imagen etiquetada, monto total y conteos de monedas
    - Definir la estructura con `cv::Mat labeledImage`, `double totalAmount`, y `std::vector<CoinType> coinCounts`
    - _Requirements: 3.1, 3.2_
  
  - [x] 2.2 Implementar función `findMatchingCoin()` para encontrar la denominación que coincide con un radio detectado
    - Recibir radio detectado, vector de tipos de moneda, y tolerancia como parámetros
    - Iterar sobre los tipos de moneda y calcular la diferencia absoluta entre el radio detectado y el radio de referencia
    - Retornar puntero al tipo de moneda si la diferencia está dentro del margen de tolerancia (radio_referencia * tolerancia)
    - Retornar nullptr si no hay coincidencia
    - _Requirements: 1.1, 1.3, 4.2_
  
  - [x] 2.3 Implementar función `classifyAndLabelCoins()` para clasificar y etiquetar todas las monedas detectadas
    - Recibir `DetectionResult`, vector de tipos de moneda, y tolerancia como parámetros
    - Clonar la imagen anotada del `DetectionResult` para trabajar sobre ella
    - Iterar sobre cada círculo detectado y usar `findMatchingCoin()` para clasificar
    - Para cada moneda clasificada: dibujar círculo verde, añadir etiqueta con denominación en azul, incrementar contador y sumar al total
    - Para monedas no clasificadas: dibujar círculo rojo y etiquetar con "?"
    - Retornar `ClassificationResult` con la imagen etiquetada, monto total y conteos actualizados
    - _Requirements: 1.1, 1.3, 1.4, 2.1, 2.2, 2.3, 3.1, 3.2_

- [x] 3. Implementar funciones de salida y reporte
  - [x] 3.1 Crear función `printSummary()` para mostrar resumen en consola
    - Recibir `ClassificationResult` como parámetro
    - Imprimir conteo de monedas por denominación (formato: "50 COP = 3x")
    - Imprimir monto total en pesos colombianos
    - Manejar caso especial cuando no se detectan monedas (mostrar "Total: 0 COP")
    - _Requirements: 3.3, 3.4_
  
  - [x] 3.2 Crear función `saveResults()` para guardar la imagen etiquetada
    - Recibir `ClassificationResult` y directorio de salida como parámetros
    - Crear directorio de salida si no existe
    - Guardar imagen etiquetada como "imagen_monedas_valuadas.jpg"
    - Verificar que `cv::imwrite()` retorne true, lanzar excepción si falla
    - _Requirements: 2.4_

- [x] 4. Integrar componentes en la función main
  - Actualizar `main()` para llamar a `detectCoins()` con la ruta de entrada y directorio de salida
  - Inicializar vector de tipos de moneda con `initializeCoinTypes()`
  - Llamar a `classifyAndLabelCoins()` con los resultados de detección, tipos de moneda y tolerancia
  - Llamar a `saveResults()` para guardar la imagen etiquetada
  - Llamar a `printSummary()` para mostrar el resumen en consola
  - Mantener manejo de excepciones existente
  - _Requirements: 1.1, 2.4, 3.3, 3.4_

- [x] 5. Limpiar código comentado y validar funcionamiento
  - Eliminar la función `calculateAmount()` comentada que ya no se necesita
  - Compilar el proyecto y verificar que no hay errores de sintaxis
  - Ejecutar el programa con la imagen de prueba "data/coins_colombia.jpeg"
  - Verificar visualmente que la imagen de salida tenga las monedas correctamente etiquetadas
  - Verificar que el resumen en consola muestre conteos y total correcto
  - _Requirements: 1.1, 1.3, 2.1, 2.2, 2.3, 3.3, 3.4_
