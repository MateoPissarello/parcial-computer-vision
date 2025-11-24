# Design Document

## Overview

El sistema de etiquetado de denominaciones de monedas colombianas extiende la funcionalidad existente de detección de monedas. El diseño aprovecha la estructura actual del código que ya detecta círculos usando HoughCircles de OpenCV, y añade una capa de clasificación basada en el radio detectado para asignar denominaciones.

La solución se basa en comparar el radio en píxeles de cada moneda detectada con valores de referencia conocidos para las monedas colombianas, utilizando un margen de tolerancia para manejar variaciones en la distancia de captura y calidad de imagen.

## Architecture

El sistema mantiene la arquitectura existente y añade un nuevo componente de clasificación:

```
Input Image → detectCoins() → classifyAndLabelCoins() → Annotated Image + Summary
                    ↓                      ↓
              Circles (Vec3f)      Denomination Labels
```

### Flujo de Datos

1. **Detección**: La función `detectCoins()` existente identifica círculos y retorna `DetectionResult`
2. **Clasificación**: Nueva función `classifyAndLabelCoins()` procesa los círculos detectados
3. **Etiquetado**: Se anotan las denominaciones en la imagen
4. **Resumen**: Se genera un reporte en consola con conteos y total

## Components and Interfaces

### 1. Estructura CoinType (Modificada)

```cpp
struct CoinType
{
    std::string name;        // "50 COP", "100 COP", etc.
    double value;            // 50.0, 100.0, etc.
    double referenceRadius;  // Radio de referencia en píxeles
    int count;               // Contador de monedas detectadas
};
```

**Cambios respecto al código existente:**
- Simplificar a un solo campo `referenceRadius` en lugar de `radiusPxNew` y `radiusPxOld`
- Esto asume que usaremos un valor de referencia promedio o el más común

### 2. Función classifyAndLabelCoins()

```cpp
struct ClassificationResult
{
    cv::Mat labeledImage;
    double totalAmount;
    std::vector<CoinType> coinCounts;
};

ClassificationResult classifyAndLabelCoins(
    const DetectionResult& detection,
    const std::vector<CoinType>& coinTypes,
    double tolerance
);
```

**Responsabilidades:**
- Iterar sobre los círculos detectados
- Clasificar cada círculo comparando su radio con los valores de referencia
- Anotar la imagen con las denominaciones
- Calcular totales y conteos

### 3. Función auxiliar findMatchingCoin()

```cpp
const CoinType* findMatchingCoin(
    double detectedRadius,
    const std::vector<CoinType>& coinTypes,
    double tolerance
);
```

**Responsabilidades:**
- Comparar un radio detectado con todos los tipos de moneda
- Retornar el tipo de moneda que coincide dentro de la tolerancia
- Retornar nullptr si no hay coincidencia

### 4. Función main() actualizada

```cpp
int main()
{
    const std::string inputPath = "data/coins_colombia.jpeg";
    const fs::path outputDir = "output/coin_amount";
    
    // Detección
    DetectionResult detection = detectCoins(inputPath, outputDir);
    
    // Clasificación y etiquetado
    std::vector<CoinType> coinTypes = initializeCoinTypes();
    ClassificationResult result = classifyAndLabelCoins(
        detection, 
        coinTypes, 
        TOLERANCE_PERCENTAGE
    );
    
    // Guardar resultado
    saveResults(result, outputDir);
    
    // Mostrar resumen
    printSummary(result);
    
    return 0;
}
```

## Data Models

### Valores de Referencia para Monedas Colombianas

Basado en el código comentado existente, los valores de referencia son:

| Denominación | Radio de Referencia (px) | Notas |
|--------------|--------------------------|-------|
| 50 COP       | 73.5                     | Promedio de 64 y 83 |
| 100 COP      | 80.5                     | Promedio de 75 y 86 |
| 200 COP      | 88.5                     | Promedio de 85 y 92 |
| 500 COP      | 88.0                     | Promedio de 87 y 89 |
| 1000 COP     | 101.0                    | Mismo valor |

**Nota de diseño:** Los valores originales tenían `radiusPxNew` y `radiusPxOld`, posiblemente para diferentes generaciones de monedas. Para simplificar, usaremos el promedio. Si se necesita mayor precisión, podemos mantener ambos valores y verificar contra ambos.

### Algoritmo de Clasificación

```
Para cada círculo detectado con radio R:
  Para cada tipo de moneda con radio de referencia Rref:
    diferencia = |R - Rref|
    margen = Rref * tolerance
    
    Si diferencia <= margen:
      Asignar denominación
      Incrementar contador
      Romper bucle
  
  Si no se encontró coincidencia:
    Marcar como "Desconocida"
```

### Constantes de Configuración

```cpp
constexpr double TOLERANCE_PERCENTAGE = 0.10;  // 10% de tolerancia
const cv::Scalar LABEL_COLOR = cv::Scalar(255, 0, 0);  // Azul para etiquetas
const cv::Scalar CIRCLE_COLOR = cv::Scalar(0, 255, 0);  // Verde para círculos
constexpr double FONT_SCALE = 1.2;
constexpr int FONT_THICKNESS = 3;
```

## Error Handling

### Casos de Error

1. **No se detectan círculos**
   - Retornar resultado con totalAmount = 0
   - Mensaje: "No se detectaron monedas. Total: 0 COP"

2. **Moneda no clasificada**
   - No incrementar ningún contador
   - Etiquetar como "?" en la imagen
   - Continuar con las demás monedas

3. **Imagen de entrada inválida**
   - Ya manejado por `detectCoins()` con excepción
   - Propagar la excepción

4. **Error al guardar imagen de salida**
   - Verificar retorno de `cv::imwrite()`
   - Lanzar excepción si falla

### Validaciones

- Verificar que `tolerance` esté en rango razonable (0.0 - 0.5)
- Verificar que `coinTypes` no esté vacío
- Verificar que los radios de referencia sean positivos

## Testing Strategy

### Pruebas Funcionales

1. **Clasificación correcta**
   - Usar imagen de prueba con monedas conocidas
   - Verificar que cada moneda se clasifique correctamente
   - Verificar que el total calculado sea correcto

2. **Manejo de tolerancia**
   - Probar con diferentes valores de tolerancia
   - Verificar que monedas en el límite se clasifiquen correctamente

3. **Monedas no reconocidas**
   - Incluir objetos circulares que no sean monedas
   - Verificar que se marquen como desconocidas

4. **Casos extremos**
   - Imagen sin monedas
   - Imagen con muchas monedas del mismo tipo
   - Monedas parcialmente visibles

### Pruebas de Integración

1. **Pipeline completo**
   - Ejecutar desde imagen de entrada hasta salida anotada
   - Verificar que todos los archivos se generen correctamente

2. **Formato de salida**
   - Verificar que la imagen anotada sea legible
   - Verificar que el resumen en consola sea correcto

### Validación Visual

- Revisar manualmente las imágenes anotadas
- Comparar con conteo manual de monedas
- Verificar que las etiquetas sean legibles y estén bien posicionadas

## Implementation Notes

### Optimizaciones

- Ordenar `coinTypes` por radio de referencia para búsqueda más eficiente
- Usar búsqueda binaria si el número de tipos de moneda crece
- Cachear cálculos de margen de tolerancia

### Consideraciones de Precisión

- La tolerancia del 10% puede necesitar ajuste según las imágenes reales
- Si hay solapamiento entre rangos de monedas, priorizar la coincidencia más cercana
- Considerar normalización de radios si las imágenes tienen diferentes resoluciones

### Extensibilidad

- Fácil añadir nuevas denominaciones al vector `coinTypes`
- Posibilidad de cargar valores de referencia desde archivo de configuración
- Estructura permite añadir más metadatos por moneda (color, año, etc.)
