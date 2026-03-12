# Rebuild Checklist - Pokemon Cuarzo

Objetivo: reconstruir gradualmente sobre base limpia (`rebuild/from-upstream-clean`) con compilacion y commit por bloque.

## Bloque 0 - Base limpia
- [x] Base upstream limpia creada
- [x] Pueblo Ciendra base integrado
- [x] Tilesets pool registrados y compilando

## Bloque 1 - Mapas de prueba
- [ ] Isla de Pruebas completa (mapa/layout/scripts/eventos)
- [ ] NPC utilitario de transporte y pruebas
- [ ] Warps y spawns de testing estables

## Bloque 2 - Main Menu (Archie/Mudskip)
- [ ] Menu principal custom con mugshot
- [ ] Navegacion y opciones bloqueadas/desbloqueadas
- [ ] Textos/offsets/paletas del main menu estabilizados

## Bloque 3 - Start Menu Full Screen
- [ ] Fondo scroll + botones 3x3
- [ ] Cursor y seleccion estable sin corrupcion
- [ ] Integracion funcional reemplazando start menu vanilla
- [ ] Ordenamiento dinamico de botones sin huecos
- [ ] Reloj en barra superior

## Bloque 4 - Followers
- [ ] Follower NPC activable/desactivable hablando
- [ ] Follower NPC con combates dobles junto al jugador
- [ ] Surf montado jugador + seguidor (sin blob)
- [ ] Animaciones de surf/field move completas y estables
- [ ] OW_FOLLOWERS_WEATHER_FORMS

## Bloque 5 - Party Menu
- [ ] Port gen5ish party menu
- [ ] Navegacion D-pad completa (arriba/abajo/izq/der)
- [ ] Comportamiento correcto por columnas en reordenamiento

## Bloque 6 - Menus extra
- [ ] Port `original_tx_registered_items_menu`
- [ ] Verificacion completa de flujo de objetos registrados

## Bloque 7 - Pesca y minijuego
- [ ] Integracion Fishing Minigame (Stardew style)
- [ ] Flujo completo: pique -> minijuego -> combate
- [ ] Sin cuelgues/ventanas en blanco
- [ ] Isla de Pruebas sin pesca forzada a Magikarp

## Bloque 8 - Clima y field moves
- [ ] Niebla custom + capas + transiciones
- [ ] Defog fuera de combate
- [ ] Haze fuera de combate
- [ ] Sunny Day / Rain Dance / Hail fuera de combate
- [ ] Rebote fuera de combate (seleccion y salto estable)

## Bloque 9 - Sistemas de objetos/UI
- [ ] Repelente recargable
- [ ] Berry Pouch y check_berry UI
- [ ] Ajustes de paleta/indices de bag UI

## Bloque 10 - DNS / Pokedex
- [ ] Battle DNS integrado y estable
- [ ] Selector de hora para testing DNS (si aplica)
- [ ] HGSS Pokedex Plus habilitada y validada

## Bloque 11 - Battle Factory custom
- [ ] Fabrica batalla custom integrada
- [ ] NPCs custom en lobby de Battle Factory
- [ ] Scripts/warps del lobby verificados

## Bloque 12 - Graficos y pipeline
- [ ] png2tileset (GUI/EXE) deterministico
- [ ] Validacion frame a frame para animaciones
- [ ] Integraciones de tilesets custom (pool/flor/etc.)
- [ ] Paridad visual Porymap vs emulador

## Bloque 13 - Personalizacion jugador/seguidor
- [ ] Toggle color de pelo (backsprite) jugador/seguidor
- [ ] Toggle color mochila/bolso (overworld/backsprite/bag)
- [ ] Recarga inmediata de paletas sin efectos secundarios

## Bloque 14 - Overworld visual / capas
- [ ] Reflejos en agua: solo en metatiles reflectantes
- [ ] Sin reflejos sobre pasto/tierra adyacente
- [ ] Sin glitches de transicion en abrir/cerrar menus

## Bloque 15 - QA final
- [ ] Sin crasheos en start menu, submenus, surf, pesca, bicicletas
- [ ] Sin corrupcion de tiles/paletas en flujos criticos
- [ ] Build final y copia a Windows
- [ ] Documento de cambios final actualizado

## Reglas de trabajo
- Cada bloque debe cerrar con:
  1. Compilacion exitosa.
  2. Prueba rapida en emulador.
  3. Commit de respaldo.
  4. Push.
