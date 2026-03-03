# Cambios Propios - Pokemon Cuarzo

Este documento resume los cambios personalizados implementados en este fork sobre `pokeemerald-expansion`.

## Overworld y movilidad

- Animaciones de puertas 2x2 personalizadas para edificio Pokeball, con correcciones de metatiles, offsets y paletas.
- Apertura sincronizada de puertas dobles (izquierda/derecha) cuando corresponde.
- Implementacion de `Surf` mostrando el Pokemon surfista en overworld (en lugar del blob).
- Integracion de sprites surfables desde un pack externo para mejorar la experiencia visual.
- Ajustes de orden de capas durante surf (incluyendo direccion sur) para prioridad visual correcta.
- Sistema de salto tipo `Rebote` fuera de combate entre rocas altas vinculadas.
- Flujo de `Rebote` con seleccion del Pokemon que conoce el movimiento, ocultado del seguidor, animacion HM y salto largo.

## Clima, niebla y capas

- Clima de niebla personalizado con variantes visuales y ajuste de intensidad.
- Integracion de `Despejar` fuera de combate para limpiar la niebla personalizada.
- Integracion de `Niebla/Haze` fuera de combate para aplicar niebla en mapa.
- `Danza Lluvia` fuera de combate ajustado para aplicar `lluvia` (`WEATHER_RAIN`) en vez de `diluvio`.
- Integracion de `Granizo` fuera de combate aplicando `nieve` (`WEATHER_SNOW`) en overworld.
- Efectos de transicion (`fade`) al activar/desactivar niebla.
- Ajustes de capas y prioridad para jugador, seguidor, edificios y niebla.
- Correcciones para evitar que menus UI queden debajo/encima de capas climaticas incorrectas.
- Correcciones para que animaciones de puertas respeten la niebla.

## Movimientos fuera de combate

- `Despejar (Defog)` fuera de combate.
- `Niebla (Haze)` fuera de combate (efecto inverso de Defog).
- `Dia Soleado (Sunny Day)` fuera de combate para aplicar `sequia`.
- `Danza Lluvia (Rain Dance)` fuera de combate para aplicar `lluvia`.
- `Granizo (Hail)` fuera de combate para aplicar `nieve`.
- `Rebote (Bounce)` fuera de combate para salto entre puntos de mapa definidos por rocas altas.

## Sistema de objetos y UI de bayas

- `Repelente Recargable` (uso en campo y carga de pasos).
- Entrega automatizada de items de soporte desde NPC de clima (segun flujo de pruebas).
- Entrega temporal de 5 bayas para pruebas de Bolsa de Bayas.
- `Bolsa de Bayas` habilitada para abrir interfaz de consulta tipo `check_berry`.
- Ajustes de `graphics/bag/check_berry.png` para mantener compatibilidad de indices de paleta con la UI.

## Seguidor y comportamiento de NPC/party

- Correcciones de animacion del Pokemon seguidor (segun modo configurado para siempre animado o ligado al jugador).
- Correcciones para evitar desincronizaciones de movimiento del seguidor.
- Ajustes de ocultar/mostrar seguidor durante secuencias especiales (ej. Rebote).
- NPC de clima extendido para pruebas (entrega de Pokemon con movimientos de campo y utilidades de progreso).
- NPC de clima actualizado para entregar `Castform` de prueba con `Dia Soleado`, `Danza Lluvia` y `Granizo`.

## Calidad de vida y contenido adicional

- Ajustes de textos en espanol para interacciones y prompts de movimientos de campo (incluyendo surf/rebote).
- Ajustes de warps y comportamiento de entradas/salidas en mapas de prueba y lobby.
- Integracion funcional de DexNav y PokeNav desde flujo de NPC de soporte en entorno de testing.
- Correcciones de paletas y assets UI (tipos, mochila, paneles de batalla y otros recursos graficos modificados).

## Nota

- Algunos cambios de este fork fueron pensados para pruebas rapidas en mapa de desarrollo (NPC de clima, entregas y desbloqueos).  
- Si se desea una version "produccion", se recomienda separar estas ayudas de debug en scripts opcionales.
