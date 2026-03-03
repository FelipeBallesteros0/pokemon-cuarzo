const struct AbilityInfo gAbilitiesInfo[ABILITIES_COUNT] =
{
    [ABILITY_NONE] =
    {
        .name = _("-------"),
        .description = COMPOUND_STRING("No special ability."),
        .aiRating = 0,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_STENCH] =
    {
        .name = _("Hedor"),
        .description = COMPOUND_STRING(
        "Puede amedrentar\n"
        "al rival al\n"
        "atacarlo debido al\n"
        "mal olor que\n"
        "emana.\n"),
        .aiRating = 1,
    },

    [ABILITY_DRIZZLE] =
    {
        .name = _("Llovizna"),
        .description = COMPOUND_STRING(
        "Hace que llueva al\n"
        "entrar en combate.\n"),
        .aiRating = 9,
    },

    [ABILITY_SPEED_BOOST] =
    {
        .name = _("Impulso"),
        .description = COMPOUND_STRING(
        "Aumenta su\n"
        "Velocidad en cada\n"
        "turno.\n"),
        .aiRating = 9,
    },

    [ABILITY_BATTLE_ARMOR] =
    {
        .name = _("Armadura Batalla"),
        .description = COMPOUND_STRING(
        "La robusta coraza\n"
        "que lo protege\n"
        "bloquea los golpes\n"
        "críticos.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_STURDY] =
    {
        .name = _("Robustez"),
        .description = COMPOUND_STRING(
        "Evita que el rival\n"
        "pueda debilitarlo\n"
        "de un solo golpe\n"
        "cuando tiene los\n"
        "PS al máximo.\n"
        "También evita los\n"
        "movimientos\n"
        "fulminantes.\n"),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_DAMP] =
    {
        .name = _("Humedad"),
        .description = COMPOUND_STRING(
        "Aumenta la humedad\n"
        "del entorno y\n"
        "evita que se\n"
        "puedan utilizar\n"
        "movimientos\n"
        "explosivos, tales\n"
        "como\n"
        "Autodestrucción.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_LIMBER] =
    {
        .name = _("Flexibilidad"),
        .description = COMPOUND_STRING(
        "Evita ser\n"
        "paralizado gracias\n"
        "a la flexibilidad\n"
        "de su cuerpo.\n"),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_SAND_VEIL] =
    {
        .name = _("Velo Arena"),
        .description = COMPOUND_STRING(
        "Aumenta su Evasión\n"
        "durante las\n"
        "tormentas de\n"
        "arena.\n"),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_STATIC] =
    {
        .name = _("Elec. Estática"),
        .description = COMPOUND_STRING(
        "La electricidad\n"
        "estática que lo\n"
        "envuelve puede\n"
        "paralizar al\n"
        "mínimo contacto.\n"),
        .aiRating = 4,
    },

    [ABILITY_VOLT_ABSORB] =
    {
        .name = _("Absorbe Elec"),
        .description = COMPOUND_STRING(
        "Si le alcanza un\n"
        "movimiento de tipo\n"
        "Eléctrico,\n"
        "recupera PS en vez\n"
        "de sufrir daño.\n"),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_WATER_ABSORB] =
    {
        .name = _("Absorbe Agua"),
        .description = COMPOUND_STRING(
        "Si le alcanza un\n"
        "movimiento de tipo\n"
        "Agua, recupera PS\n"
        "en vez de sufrir\n"
        "daño.\n"),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_OBLIVIOUS] =
    {
        .name = _("Despiste"),
        .description = COMPOUND_STRING(
        "Su indiferencia\n"
        "evita que sea\n"
        "provocado o caiga\n"
        "presa del\n"
        "enamoramiento.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_CLOUD_NINE] =
    {
        .name = _("Aclimatación"),
        .description = COMPOUND_STRING(
        "Anula todos los\n"
        "efectos del tiempo\n"
        "atmosférico.\n"),
        .aiRating = 5,
    },

    [ABILITY_COMPOUND_EYES] =
    {
        .name = _("Ojo Compuesto"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "precisión de sus\n"
        "movimientos.\n"),
        .aiRating = 7,
    },

    [ABILITY_INSOMNIA] =
    {
        .name = _("Insomnio"),
        .description = COMPOUND_STRING(
        "Su resistencia al\n"
        "sueño le impide\n"
        "quedarse dormido.\n"),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_COLOR_CHANGE] =
    {
        .name = _("Cambio Color"),
        .description = COMPOUND_STRING(
        "Adopta el tipo del\n"
        "último movimiento\n"
        "del que es blanco.\n"),
        .aiRating = 2,
    },

    [ABILITY_IMMUNITY] =
    {
        .name = _("Inmunidad"),
        .description = COMPOUND_STRING(
        "Su sistema\n"
        "inmunitario evita\n"
        "el envenenamiento.\n"),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_FLASH_FIRE] =
    {
        .name = _("Absorbe Fuego"),
        .description = COMPOUND_STRING(
        "Si le alcanza\n"
        "algún movimiento\n"
        "de tipo Fuego,\n"
        "potencia sus\n"
        "propios\n"
        "movimientos de\n"
        "dicho tipo.\n"),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_SHIELD_DUST] =
    {
        .name = _("Polvo Escudo"),
        .description = COMPOUND_STRING(
        "El polvo de\n"
        "escamas que lo\n"
        "envuelve lo\n"
        "protege de los\n"
        "efectos\n"
        "secundarios de los\n"
        "ataques recibidos.\n"),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_OWN_TEMPO] =
    {
        .name = _("Ritmo Propio"),
        .description = COMPOUND_STRING(
        "Como le gusta\n"
        "hacer las cosas a\n"
        "su manera, los\n"
        "rivales no logran\n"
        "confundirlo.\n"),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_SUCTION_CUPS] =
    {
        .name = _("Ventosas"),
        .description = COMPOUND_STRING(
        "Sus ventosas se\n"
        "aferran al suelo,\n"
        "con lo cual anula\n"
        "movimientos y\n"
        "objetos que\n"
        "fuerzan el relevo.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_INTIMIDATE] =
    {
        .name = _("Intimidación"),
        .description = COMPOUND_STRING(
        "Al entrar en\n"
        "combate amilana al\n"
        "rival de tal\n"
        "manera que su\n"
        "Ataque disminuye.\n"),
        .aiRating = 7,
    },

    [ABILITY_SHADOW_TAG] =
    {
        .name = _("Sombra Trampa"),
        .description = COMPOUND_STRING(
        "Impide que el\n"
        "enemigo huya o sea\n"
        "cambiado por otro.\n"),
        .aiRating = 10,
    },

    [ABILITY_ROUGH_SKIN] =
    {
        .name = _("Piel Tosca"),
        .description = COMPOUND_STRING(
        "Hiere con su piel\n"
        "áspera al rival\n"
        "que lo ataque con\n"
        "un movimiento de\n"
        "contacto.\n"),
        .aiRating = 6,
    },

    [ABILITY_WONDER_GUARD] =
    {
        .name = _("Superguarda"),
        .description = COMPOUND_STRING(
        "Gracias a un poder\n"
        "misterioso, solo\n"
        "le hacen daño los\n"
        "movimientos\n"
        "supereficaces.\n"),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .breakable = TRUE,
    },

    [ABILITY_LEVITATE] =
    {
        .name = _("Levitación"),
        .description = COMPOUND_STRING(
        "Su capacidad de\n"
        "flotar sobre el\n"
        "suelo le\n"
        "proporciona\n"
        "inmunidad frente a\n"
        "los movimientos de\n"
        "tipo Tierra.\n"),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_EFFECT_SPORE] =
    {
        .name = _("Efecto Espora"),
        .description = COMPOUND_STRING(
        "Puede dormir,\n"
        "envenenar o\n"
        "paralizar al\n"
        "Pokémon con el que\n"
        "entre en contacto\n"
        "al recibir un\n"
        "ataque.\n"),
        .aiRating = 4,
    },

    [ABILITY_SYNCHRONIZE] =
    {
        .name = _("Sincronía"),
        .description = COMPOUND_STRING(
        "Contagia el\n"
        "envenenamiento,\n"
        "las quemaduras o\n"
        "la parálisis al\n"
        "Pokémon que le\n"
        "cause ese estado.\n"),
        .aiRating = 4,
    },

    [ABILITY_CLEAR_BODY] =
    {
        .name = _("Cuerpo Puro"),
        .description = COMPOUND_STRING(
        "Evita que bajen\n"
        "sus\n"
        "características a\n"
        "causa de\n"
        "movimientos o\n"
        "habilidades de\n"
        "otros Pokémon.\n"),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_NATURAL_CURE] =
    {
        .name = _("Cura Natural"),
        .description = COMPOUND_STRING(
        "Cura sus problemas\n"
        "de estado al\n"
        "cambiarse con otro\n"
        "Pokémon.\n"),
        .aiRating = 7,
    },

    [ABILITY_LIGHTNING_ROD] =
    {
        .name = _("Pararrayos"),
        .description = COMPOUND_STRING(
        "Atrae y neutraliza\n"
        "los movimientos de\n"
        "tipo Eléctrico,\n"
        "que además le\n"
        "suben el Ataque\n"
        "Especial.\n"),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_SERENE_GRACE] =
    {
        .name = _("Dicha"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "probabilidad de\n"
        "que los\n"
        "movimientos causen\n"
        "efectos\n"
        "secundarios.\n"),
        .aiRating = 8,
    },

    [ABILITY_SWIFT_SWIM] =
    {
        .name = _("Nado Rápido"),
        .description = COMPOUND_STRING(
        "Sube su Velocidad\n"
        "cuando llueve.\n"),
        .aiRating = 6,
    },

    [ABILITY_CHLOROPHYLL] =
    {
        .name = _("Clorofila"),
        .description = COMPOUND_STRING(
        "Sube su Velocidad\n"
        "cuando hace sol.\n"),
        .aiRating = 6,
    },

    [ABILITY_ILLUMINATE] =
    {
        .name = _("Iluminación"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "probabilidad de\n"
        "encontrar Pokémon\n"
        "al iluminar el\n"
        "entorno.\n"),
        .aiRating = 0,
        .breakable = TRUE,
    },

    [ABILITY_TRACE] =
    {
        .name = _("Rastro"),
        .description = COMPOUND_STRING(
        "Al entrar en\n"
        "combate copia la\n"
        "habilidad del\n"
        "rival.\n"),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_HUGE_POWER] =
    {
        .name = _("Potencia"),
        .description = COMPOUND_STRING(
        "Duplica la\n"
        "potencia de sus\n"
        "ataques físicos.\n"),
        .aiRating = 10,
    },

    [ABILITY_POISON_POINT] =
    {
        .name = _("Punto Tóxico"),
        .description = COMPOUND_STRING(
        "Puede envenenar al\n"
        "mínimo contacto.\n"),
        .aiRating = 4,
    },

    [ABILITY_INNER_FOCUS] =
    {
        .name = _("Foco Interno"),
        .description = COMPOUND_STRING(
        "Gracias a su\n"
        "profunda\n"
        "concentración, no\n"
        "se amedrenta ante\n"
        "los ataques del\n"
        "rival.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_MAGMA_ARMOR] =
    {
        .name = _("Escudo Magma"),
        .description = COMPOUND_STRING(
        "Gracias al magma\n"
        "candente que lo\n"
        "envuelve, no puede\n"
        "ser congelado.\n"),
        .aiRating = 1,
        .breakable = TRUE,
    },

    [ABILITY_WATER_VEIL] =
    {
        .name = _("Velo Agua"),
        .description = COMPOUND_STRING(
        "Evita las\n"
        "quemaduras gracias\n"
        "a la capa de agua\n"
        "que lo envuelve.\n"),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_MAGNET_PULL] =
    {
        .name = _("Imán"),
        .description = COMPOUND_STRING(
        "Su magnetismo\n"
        "atrae a los\n"
        "Pokémon de tipo\n"
        "Acero y les impide\n"
        "huir.\n"),
        .aiRating = 9,
    },

    [ABILITY_SOUNDPROOF] =
    {
        .name = _("Insonorizar"),
        .description = COMPOUND_STRING(
        "Su aislamiento\n"
        "acústico lo\n"
        "protege de\n"
        "movimientos que\n"
        "usan sonido.\n"),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_RAIN_DISH] =
    {
        .name = _("Cura Lluvia"),
        .description = COMPOUND_STRING(
        "Recupera PS de\n"
        "forma gradual\n"
        "cuando llueve.\n"),
        .aiRating = 3,
    },

    [ABILITY_SAND_STREAM] =
    {
        .name = _("Chorro Arena"),
        .description = COMPOUND_STRING(
        "Crea una tormenta\n"
        "de arena al entrar\n"
        "en combate.\n"),
        .aiRating = 9,
    },

    [ABILITY_PRESSURE] =
    {
        .name = _("Presión"),
        .description = COMPOUND_STRING(
        "Presiona al rival\n"
        "para que sus PP se\n"
        "acaben antes.\n"),
        .aiRating = 5,
    },

    [ABILITY_THICK_FAT] =
    {
        .name = _("Sebo"),
        .description = COMPOUND_STRING(
        "Gracias a la\n"
        "gruesa capa de\n"
        "grasa que lo\n"
        "protege, reduce a\n"
        "la mitad el daño\n"
        "que recibe de\n"
        "ataques de tipo\n"
        "Fuego o Hielo.\n"),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_EARLY_BIRD] =
    {
        .name = _("Madrugar"),
        .description = COMPOUND_STRING(
        "Si se duerme,\n"
        "tardará la mitad\n"
        "de tiempo en\n"
        "despertarse.\n"),
        .aiRating = 4,
    },

    [ABILITY_FLAME_BODY] =
    {
        .name = _("Cuerpo Llama"),
        .description = COMPOUND_STRING(
        "Puede quemar al\n"
        "mínimo contacto.\n"),
        .aiRating = 4,
    },

    [ABILITY_RUN_AWAY] =
    {
        .name = _("Fuga"),
        .description = COMPOUND_STRING(
        "Puede escaparse de\n"
        "todos los Pokémon\n"
        "salvajes.\n"),
        .aiRating = 0,
    },

    [ABILITY_KEEN_EYE] =
    {
        .name = _("Vista Lince"),
        .description = COMPOUND_STRING(
        "Su aguda vista\n"
        "evita que le\n"
        "disminuya la\n"
        "Precisión.\n"),
        .aiRating = 1,
        .breakable = TRUE,
    },

    [ABILITY_HYPER_CUTTER] =
    {
        .name = _("Corte Fuerte"),
        .description = COMPOUND_STRING(
        "Evita que el rival\n"
        "le baje el Ataque.\n"),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_PICKUP] =
    {
        .name = _("Recogida"),
        .description = COMPOUND_STRING(
        "Puede recoger\n"
        "objetos que el\n"
        "rival haya usado,\n"
        "o bien otros que\n"
        "encuentre en plena\n"
        "aventura.\n"),
        .aiRating = 1,
    },

    [ABILITY_TRUANT] =
    {
        .name = _("Ausente"),
        .description = COMPOUND_STRING(
        "Al ejecutar un\n"
        "movimiento,\n"
        "descansará en el\n"
        "turno siguiente.\n"),
        .aiRating = -2,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_HUSTLE] =
    {
        .name = _("Entusiasmo"),
        .description = COMPOUND_STRING(
        "Aumenta su Ataque,\n"
        "pero reduce su\n"
        "Precisión.\n"),
        .aiRating = 7,
    },

    [ABILITY_CUTE_CHARM] =
    {
        .name = _("Gran Encanto"),
        .description = COMPOUND_STRING(
        "Puede causar\n"
        "enamoramiento al\n"
        "rival que lo\n"
        "toque.\n"),
        .aiRating = 2,
    },

    [ABILITY_PLUS] =
    {
        .name = _("Más"),
        .description = COMPOUND_STRING(
        "Potencia su Ataque\n"
        "Especial si un\n"
        "Pokémon aliado\n"
        "tiene la habilidad\n"
        "Más o Menos.\n"),
        .aiRating = 0,
    },

    [ABILITY_MINUS] =
    {
        .name = _("Menos"),
        .description = COMPOUND_STRING(
        "Potencia su Ataque\n"
        "Especial si un\n"
        "Pokémon aliado\n"
        "tiene la habilidad\n"
        "Más o Menos.\n"),
        .aiRating = 0,
    },

    [ABILITY_FORECAST] =
    {
        .name = _("Predicción"),
        .description = COMPOUND_STRING(
        "Cambia a tipo\n"
        "Agua, Fuego o\n"
        "Hielo en función\n"
        "del tiempo\n"
        "atmosférico.\n"),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_STICKY_HOLD] =
    {
        .name = _("Viscosidad"),
        .description = COMPOUND_STRING(
        "Los objetos se\n"
        "quedan pegados a\n"
        "su cuerpo, por lo\n"
        "que no pueden\n"
        "robárselos.\n"),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_SHED_SKIN] =
    {
        .name = _("Mudar"),
        .description = COMPOUND_STRING(
        "Puede curar sus\n"
        "problemas de\n"
        "estado al mudar la\n"
        "piel.\n"),
        .aiRating = 7,
    },

    [ABILITY_GUTS] =
    {
        .name = _("Agallas"),
        .description = COMPOUND_STRING(
        "Si sufre un\n"
        "problema de\n"
        "estado, se viene\n"
        "arriba y aumenta\n"
        "su Ataque.\n"),
        .aiRating = 6,
    },

    [ABILITY_MARVEL_SCALE] =
    {
        .name = _("Escama Especial"),
        .description = COMPOUND_STRING(
        "Sube su Defensa si\n"
        "sufre un problema\n"
        "de estado.\n"),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_LIQUID_OOZE] =
    {
        .name = _("Lodo Líquido"),
        .description = COMPOUND_STRING(
        "Exuda una\n"
        "sustancia tóxica\n"
        "de intenso hedor\n"
        "que hiere a\n"
        "quienes intentan\n"
        "drenarle PS.\n"),
        .aiRating = 3,
    },

    [ABILITY_OVERGROW] =
    {
        .name = _("Espesura"),
        .description = COMPOUND_STRING(
        "Potencia sus\n"
        "movimientos de\n"
        "tipo Planta cuando\n"
        "le quedan pocos\n"
        "PS.\n"),
        .aiRating = 5,
    },

    [ABILITY_BLAZE] =
    {
        .name = _("Mar Llamas"),
        .description = COMPOUND_STRING(
        "Potencia sus\n"
        "movimientos de\n"
        "tipo Fuego cuando\n"
        "le quedan pocos\n"
        "PS.\n"),
        .aiRating = 5,
    },

    [ABILITY_TORRENT] =
    {
        .name = _("Torrente"),
        .description = COMPOUND_STRING(
        "Potencia sus\n"
        "movimientos de\n"
        "tipo Agua cuando\n"
        "le quedan pocos\n"
        "PS.\n"),
        .aiRating = 5,
    },

    [ABILITY_SWARM] =
    {
        .name = _("Enjambre"),
        .description = COMPOUND_STRING(
        "Potencia sus\n"
        "movimientos de\n"
        "tipo Bicho cuando\n"
        "le quedan pocos\n"
        "PS.\n"),
        .aiRating = 5,
    },

    [ABILITY_ROCK_HEAD] =
    {
        .name = _("Cabeza Roca"),
        .description = COMPOUND_STRING(
        "No puede dañarse\n"
        "con sus propios\n"
        "movimientos.\n"),
        .aiRating = 5,
    },

    [ABILITY_DROUGHT] =
    {
        .name = _("Sequía"),
        .description = COMPOUND_STRING(
        "Cuando entra en\n"
        "combate, el tiempo\n"
        "pasa a ser\n"
        "soleado.\n"),
        .aiRating = 9,
    },

    [ABILITY_ARENA_TRAP] =
    {
        .name = _("Trampa Arena"),
        .description = COMPOUND_STRING(
        "Evita que el rival\n"
        "huya.\n"),
        .aiRating = 9,
    },

    [ABILITY_VITAL_SPIRIT] =
    {
        .name = _("Espíritu Vital"),
        .description = COMPOUND_STRING(
        "Su determinación\n"
        "le impide quedarse\n"
        "dormido.\n"),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_WHITE_SMOKE] =
    {
        .name = _("Humo Blanco"),
        .description = COMPOUND_STRING(
        "El humo blanco que\n"
        "lo protege evita\n"
        "que otros Pokémon\n"
        "le bajen las\n"
        "características.\n"),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_PURE_POWER] =
    {
        .name = _("Energía Pura"),
        .description = COMPOUND_STRING(
        "Duplica la\n"
        "potencia de sus\n"
        "ataques físicos\n"
        "gracias al yoga.\n"),
        .aiRating = 10,
    },

    [ABILITY_SHELL_ARMOR] =
    {
        .name = _("Caparazón"),
        .description = COMPOUND_STRING(
        "La robusta coraza\n"
        "que lo protege\n"
        "bloquea los golpes\n"
        "críticos.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_AIR_LOCK] =
    {
        .name = _("Bucle Aire"),
        .description = COMPOUND_STRING(
        "Neutraliza todos\n"
        "los efectos del\n"
        "tiempo\n"
        "atmosférico.\n"),
        .aiRating = 5,
    },

    [ABILITY_TANGLED_FEET] =
    {
        .name = _("Tumbos"),
        .description = COMPOUND_STRING(
        "Sube su Evasión si\n"
        "está confuso.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_MOTOR_DRIVE] =
    {
        .name = _("Electromotor"),
        .description = COMPOUND_STRING(
        "Si le alcanza un\n"
        "movimiento de tipo\n"
        "Eléctrico, le sube\n"
        "la Velocidad en\n"
        "vez de sufrir\n"
        "daño.\n"),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_RIVALRY] =
    {
        .name = _("Rivalidad"),
        .description = COMPOUND_STRING(
        "Si el objetivo es\n"
        "del mismo sexo, su\n"
        "competitividad le\n"
        "lleva a infligir\n"
        "más daño. Si es\n"
        "del sexo\n"
        "contrario, en\n"
        "cambio, el daño\n"
        "será menor.\n"),
        .aiRating = 1,
    },

    [ABILITY_STEADFAST] =
    {
        .name = _("Impasible"),
        .description = COMPOUND_STRING(
        "Cada vez que se\n"
        "amedrenta sube su\n"
        "Velocidad, debido\n"
        "a su voluntad\n"
        "inquebrantable.\n"),
        .aiRating = 2,
    },

    [ABILITY_SNOW_CLOAK] =
    {
        .name = _("Manto Níveo"),
        .description = COMPOUND_STRING(
        "Sube la Evasión\n"
        "cuando graniza.\n"),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_GLUTTONY] =
    {
        .name = _("Gula"),
        .description = COMPOUND_STRING(
        "Cuando sus PS se\n"
        "ven reducidos a la\n"
        "mitad, engulle la\n"
        "baya que\n"
        "normalmente solo\n"
        "se comería cuando\n"
        "le quedasen pocos\n"
        "PS.\n"),
        .aiRating = 3,
    },

    [ABILITY_ANGER_POINT] =
    {
        .name = _("Irascible"),
        .description = COMPOUND_STRING(
        "Si recibe un golpe\n"
        "crítico, monta en\n"
        "cólera y sube su\n"
        "Ataque hasta el\n"
        "máximo.\n"),
        .aiRating = 4,
    },

    [ABILITY_UNBURDEN] =
    {
        .name = _("Liviano"),
        .description = COMPOUND_STRING(
        "Sube su Velocidad\n"
        "si usa o pierde el\n"
        "objeto que lleva.\n"),
        .aiRating = 7,
    },

    [ABILITY_HEATPROOF] =
    {
        .name = _("Ignífugo"),
        .description = COMPOUND_STRING(
        "Su cuerpo,\n"
        "resistente al\n"
        "calor, reduce a la\n"
        "mitad el daño\n"
        "recibido por\n"
        "movimientos de\n"
        "tipo Fuego.\n"),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_SIMPLE] =
    {
        .name = _("Simple"),
        .description = COMPOUND_STRING(
        "Duplica los\n"
        "cambios en las\n"
        "características.\n"),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_DRY_SKIN] =
    {
        .name = _("Piel Seca"),
        .description = COMPOUND_STRING(
        "Pierde PS si hace\n"
        "sol y los recupera\n"
        "si llueve o recibe\n"
        "un movimiento de\n"
        "tipo Agua. Los\n"
        "movimientos de\n"
        "tipo Fuego, por su\n"
        "parte, le hacen\n"
        "más daño de lo\n"
        "normal.\n"),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_DOWNLOAD] =
    {
        .name = _("Descarga"),
        .description = COMPOUND_STRING(
        "Compara la Defensa\n"
        "y la Defensa\n"
        "Especial del rival\n"
        "para ver cuál es\n"
        "inferior y aumenta\n"
        "su propio Ataque o\n"
        "Ataque Especial\n"
        "según sea lo más\n"
        "eficaz.\n"),
        .aiRating = 7,
    },

    [ABILITY_IRON_FIST] =
    {
        .name = _("Puño Férreo"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "potencia de los\n"
        "puñetazos.\n"),
        .aiRating = 6,
    },

    [ABILITY_POISON_HEAL] =
    {
        .name = _("Antídoto"),
        .description = COMPOUND_STRING(
        "Si resulta\n"
        "envenenado,\n"
        "recupera PS en vez\n"
        "de perderlos.\n"),
        .aiRating = 8,
    },

    [ABILITY_ADAPTABILITY] =
    {
        .name = _("Adaptable"),
        .description = COMPOUND_STRING(
        "Potencia aún más\n"
        "los movimientos\n"
        "cuyo tipo coincida\n"
        "con el suyo.\n"),
        .aiRating = 8,
    },

    [ABILITY_SKILL_LINK] =
    {
        .name = _("Encadenado"),
        .description = COMPOUND_STRING(
        "Ejecuta siempre\n"
        "los movimientos\n"
        "múltiples con el\n"
        "número máximo de\n"
        "golpes.\n"),
        .aiRating = 7,
    },

    [ABILITY_HYDRATION] =
    {
        .name = _("Hidratación"),
        .description = COMPOUND_STRING(
        "Cura los problemas\n"
        "de estado si está\n"
        "lloviendo.\n"),
        .aiRating = 4,
    },

    [ABILITY_SOLAR_POWER] =
    {
        .name = _("Poder Solar"),
        .description = COMPOUND_STRING(
        "Si hace sol,\n"
        "aumenta su Ataque\n"
        "Especial, pero\n"
        "pierde PS en cada\n"
        "turno.\n"),
        .aiRating = 3,
    },

    [ABILITY_QUICK_FEET] =
    {
        .name = _("Pies Rápidos"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "Velocidad si sufre\n"
        "problemas de\n"
        "estado.\n"),
        .aiRating = 5,
    },

    [ABILITY_NORMALIZE] =
    {
        .name = _("Normalidad"),
        .description = COMPOUND_STRING(
        "Hace que todos sus\n"
        "movimientos se\n"
        "vuelvan de tipo\n"
        "Normal y aumenten\n"
        "ligeramente su\n"
        "potencia.\n"),
        .aiRating = -1,
    },

    [ABILITY_SNIPER] =
    {
        .name = _("Francotirador"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "golpes críticos\n"
        "que asesta aún más\n"
        "de lo normal.\n"),
        .aiRating = 3,
    },

    [ABILITY_MAGIC_GUARD] =
    {
        .name = _("Muro Mágico"),
        .description = COMPOUND_STRING(
        "Solo recibe daño\n"
        "de ataques.\n"),
        .aiRating = 9,
    },

    [ABILITY_NO_GUARD] =
    {
        .name = _("Indefenso"),
        .description = COMPOUND_STRING(
        "Al quedar ambos\n"
        "expuestos, tanto\n"
        "sus movimientos\n"
        "como los del rival\n"
        "acertarán siempre.\n"),
        .aiRating = 8,
    },

    [ABILITY_STALL] =
    {
        .name = _("Rezagado"),
        .description = COMPOUND_STRING(
        "Ejecuta su\n"
        "movimiento tras\n"
        "todos los demás.\n"),
        .aiRating = -1,
    },

    [ABILITY_TECHNICIAN] =
    {
        .name = _("Experto"),
        .description = COMPOUND_STRING(
        "Potencia sus\n"
        "movimientos más\n"
        "débiles.\n"),
        .aiRating = 8,
    },

    [ABILITY_LEAF_GUARD] =
    {
        .name = _("Defensa Hoja"),
        .description = COMPOUND_STRING(
        "Evita los\n"
        "problemas de\n"
        "estado si hace\n"
        "sol.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_KLUTZ] =
    {
        .name = _("Zoquete"),
        .description = COMPOUND_STRING(
        "No puede usar\n"
        "objetos equipados.\n"),
        .aiRating = -1,
    },

    [ABILITY_MOLD_BREAKER] =
    {
        .name = _("Rompemoldes"),
        .description = COMPOUND_STRING(
        "Las habilidades\n"
        "del objetivo no\n"
        "afectan a los\n"
        "movimientos que\n"
        "emplea.\n"),
        .aiRating = 7,
    },

    [ABILITY_SUPER_LUCK] =
    {
        .name = _("Afortunado"),
        .description = COMPOUND_STRING(
        "Su buena suerte\n"
        "aumenta la\n"
        "probabilidad de\n"
        "asestar golpes\n"
        "críticos.\n"),
        .aiRating = 3,
    },

    [ABILITY_AFTERMATH] =
    {
        .name = _("Detonación"),
        .description = COMPOUND_STRING(
        "Daña al agresor\n"
        "que le ha dado el\n"
        "golpe de gracia\n"
        "con un movimiento\n"
        "de contacto.\n"),
        .aiRating = 5,
    },

    [ABILITY_ANTICIPATION] =
    {
        .name = _("Anticipación"),
        .description = COMPOUND_STRING(
        "Prevé los\n"
        "movimientos\n"
        "peligrosos del\n"
        "rival.\n"),
        .aiRating = 2,
    },

    [ABILITY_FOREWARN] =
    {
        .name = _("Alerta"),
        .description = COMPOUND_STRING(
        "Indica el\n"
        "movimiento más\n"
        "potente del rival\n"
        "al entrar en\n"
        "combate.\n"),
        .aiRating = 2,
    },

    [ABILITY_UNAWARE] =
    {
        .name = _("Ignorante"),
        .description = COMPOUND_STRING(
        "Pasa por alto las\n"
        "mejoras en las\n"
        "características\n"
        "del rival al\n"
        "atacar.\n"),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_TINTED_LENS] =
    {
        .name = _("Cromolente"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "movimientos que no\n"
        "son muy eficaces,\n"
        "que infligen ahora\n"
        "un daño normal.\n"),
        .aiRating = 7,
    },

    [ABILITY_FILTER] =
    {
        .name = _("Filtro"),
        .description = COMPOUND_STRING(
        "Mitiga el daño que\n"
        "le infligen los\n"
        "movimientos\n"
        "supereficaces.\n"),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_SLOW_START] =
    {
        .name = _("Inicio Lento"),
        .description = COMPOUND_STRING(
        "Baja a la mitad su\n"
        "Ataque y su\n"
        "Velocidad durante\n"
        "cinco turnos.\n"),
        .aiRating = -2,
    },

    [ABILITY_SCRAPPY] =
    {
        .name = _("Intrépido"),
        .description = COMPOUND_STRING(
        "Puede alcanzar a\n"
        "Pokémon de tipo\n"
        "Fantasma con\n"
        "movimientos de\n"
        "tipo Normal o\n"
        "Lucha.\n"),
        .aiRating = 6,
    },

    [ABILITY_STORM_DRAIN] =
    {
        .name = _("Colector"),
        .description = COMPOUND_STRING(
        "Atrae y neutraliza\n"
        "los movimientos de\n"
        "tipo Agua, que\n"
        "además le suben el\n"
        "Ataque Especial.\n"),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_ICE_BODY] =
    {
        .name = _("Gélido"),
        .description = COMPOUND_STRING(
        "Recupera PS de\n"
        "forma gradual\n"
        "cuando hay\n"
        "tormentas de\n"
        "granizo.\n"),
        .aiRating = 3,
    },

    [ABILITY_SOLID_ROCK] =
    {
        .name = _("Roca Sólida"),
        .description = COMPOUND_STRING(
        "Mitiga el daño que\n"
        "le infligen los\n"
        "movimientos\n"
        "supereficaces.\n"),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_SNOW_WARNING] =
    {
        .name = _("Nevada"),
    #if B_SNOW_WARNING >= GEN_9
        .description = COMPOUND_STRING(
        "Invoca una\n"
        "tormenta de\n"
        "granizo al entrar\n"
        "en combate.\n"),
    #else
        .description = COMPOUND_STRING(
        "Invoca una\n"
        "tormenta de\n"
        "granizo al entrar\n"
        "en combate.\n"),
    #endif
        .aiRating = 8,
    },

    [ABILITY_HONEY_GATHER] =
    {
        .name = _("Recogemiel"),
        .description = COMPOUND_STRING(
        "Puede que\n"
        "encuentre Miel una\n"
        "vez concluido el\n"
        "combate.\n"),
        .aiRating = 0,
    },

    [ABILITY_FRISK] =
    {
        .name = _("Cacheo"),
        .description = COMPOUND_STRING(
        "Puede ver el\n"
        "objeto que lleva\n"
        "el rival al entrar\n"
        "en combate.\n"),
        .aiRating = 3,
    },

    [ABILITY_RECKLESS] =
    {
        .name = _("Audaz"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "movimientos que\n"
        "también dañan al\n"
        "usuario.\n"),
        .aiRating = 6,
    },

    [ABILITY_MULTITYPE] =
    {
        .name = _("Multitipo"),
        .description = COMPOUND_STRING(
        "Cambia su tipo al\n"
        "de la tabla o al\n"
        "del Cristal Z que\n"
        "lleve.\n"),
        .aiRating = 8,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_FLOWER_GIFT] =
    {
        .name = _("Don Floral"),
        .description = COMPOUND_STRING(
        "Si hace sol,\n"
        "aumenta su Ataque\n"
        "y su Defensa\n"
        "Especial, así como\n"
        "los de sus\n"
        "aliados.\n"),
        .aiRating = 4,
        .cantBeCopied = TRUE,
        .cantBeTraced = TRUE,
        .breakable = TRUE,
    },

    [ABILITY_BAD_DREAMS] =
    {
        .name = _("Mal Sueño"),
        .description = COMPOUND_STRING(
        "Inflige daño a\n"
        "cualquier rival\n"
        "que esté dormido.\n"),
        .aiRating = 4,
    },

    [ABILITY_PICKPOCKET] =
    {
        .name = _("Hurto"),
        .description = COMPOUND_STRING(
        "Si el rival usa un\n"
        "movimiento de\n"
        "contacto al\n"
        "atacar, le roba el\n"
        "objeto.\n"),
        .aiRating = 3,
    },

    [ABILITY_SHEER_FORCE] =
    {
        .name = _("Potencia Bruta"),
        .description = COMPOUND_STRING(
        "Sube la potencia\n"
        "de sus movimientos\n"
        "en detrimento de\n"
        "los efectos\n"
        "secundarios, que\n"
        "se ven anulados.\n"),
        .aiRating = 8,
    },

    [ABILITY_CONTRARY] =
    {
        .name = _("Respondón"),
        .description = COMPOUND_STRING(
        "Invierte los\n"
        "cambios en las\n"
        "características:\n"
        "bajan cuando les\n"
        "toca subir y suben\n"
        "cuando les toca\n"
        "bajar.\n"),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_UNNERVE] =
    {
        .name = _("Nerviosismo"),
        .description = COMPOUND_STRING(
        "Pone nervioso al\n"
        "rival y le impide\n"
        "usar bayas.\n"),
        .aiRating = 3,
    },

    [ABILITY_DEFIANT] =
    {
        .name = _("Competitivo"),
        .description = COMPOUND_STRING(
        "Sube mucho su\n"
        "Ataque cuando el\n"
        "rival le baja las\n"
        "características.\n"),
        .aiRating = 5,
    },

    [ABILITY_DEFEATIST] =
    {
        .name = _("Flaqueza"),
        .description = COMPOUND_STRING(
        "Se debilita tanto\n"
        "cuando sus PS se\n"
        "ven reducidos a la\n"
        "mitad que su\n"
        "Ataque y su Ataque\n"
        "Especial bajan.\n"),
        .aiRating = -1,
    },

    [ABILITY_CURSED_BODY] =
    {
        .name = _("Cuerpo Maldito"),
        .description = COMPOUND_STRING(
        "Puede anular el\n"
        "movimiento usado\n"
        "en su contra.\n"),
        .aiRating = 4,
    },

    [ABILITY_HEALER] =
    {
        .name = _("Alma Cura"),
        .description = COMPOUND_STRING(
        "A veces cura los\n"
        "problemas de\n"
        "estado de un\n"
        "aliado.\n"),
        .aiRating = 0,
    },

    [ABILITY_FRIEND_GUARD] =
    {
        .name = _("Compiescolta"),
        .description = COMPOUND_STRING(
        "Reduce el daño que\n"
        "sufren los\n"
        "aliados.\n"),
        .aiRating = 0,
        .breakable = TRUE,
    },

    [ABILITY_WEAK_ARMOR] =
    {
        .name = _("Armadura Frágil"),
        .description = COMPOUND_STRING(
        "Al recibir daño de\n"
        "un ataque físico,\n"
        "le baja la\n"
        "Defensa, pero le\n"
        "sube mucho la\n"
        "Velocidad.\n"),
        .aiRating = 2,
    },

    [ABILITY_HEAVY_METAL] =
    {
        .name = _("Metal Pesado"),
        .description = COMPOUND_STRING("Duplica su peso."),
        .aiRating = -1,
        .breakable = TRUE,
    },

    [ABILITY_LIGHT_METAL] =
    {
        .name = _("Metal Liviano"),
        .description = COMPOUND_STRING(
        "Reduce a la mitad\n"
        "su peso.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_MULTISCALE] =
    {
        .name = _("Multiescamas"),
        .description = COMPOUND_STRING(
        "Reduce el daño que\n"
        "sufre si sus PS\n"
        "están al máximo.\n"),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_TOXIC_BOOST] =
    {
        .name = _("Ímpetu Tóxico"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "potencia de sus\n"
        "ataques físicos\n"
        "cuando está\n"
        "envenenado.\n"),
        .aiRating = 6,
    },

    [ABILITY_FLARE_BOOST] =
    {
        .name = _("Ímpetu Ardiente"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "potencia de sus\n"
        "ataques especiales\n"
        "cuando sufre\n"
        "quemaduras.\n"),
        .aiRating = 5,
    },

    [ABILITY_HARVEST] =
    {
        .name = _("Cosecha"),
        .description = COMPOUND_STRING(
        "Puede reutilizar\n"
        "varias veces una\n"
        "misma baya.\n"),
        .aiRating = 5,
    },

    [ABILITY_TELEPATHY] =
    {
        .name = _("Telepatía"),
        .description = COMPOUND_STRING(
        "Elude los ataques\n"
        "de los aliados\n"
        "durante el\n"
        "combate.\n"),
        .aiRating = 0,
        .breakable = TRUE,
    },

    [ABILITY_MOODY] =
    {
        .name = _("Veleta"),
        .description = COMPOUND_STRING(
        "Una característica\n"
        "le sube mucho en\n"
        "cada turno, pero\n"
        "le baja otra.\n"),
        .aiRating = 10,
    },

    [ABILITY_OVERCOAT] =
    {
        .name = _("Funda"),
        .description = COMPOUND_STRING(
        "No le afectan las\n"
        "tormentas de\n"
        "arena, el granizo\n"
        "y los movimientos\n"
        "con polvos.\n"),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_POISON_TOUCH] =
    {
        .name = _("Toque Tóxico"),
        .description = COMPOUND_STRING(
        "Puede envenenar al\n"
        "objetivo con solo\n"
        "tocarlo.\n"),
        .aiRating = 4,
    },

    [ABILITY_REGENERATOR] =
    {
        .name = _("Regeneración"),
        .description = COMPOUND_STRING(
        "Recupera unos\n"
        "pocos PS al ser\n"
        "cambiado por otro\n"
        "Pokémon.\n"),
        .aiRating = 8,
    },

    [ABILITY_BIG_PECKS] =
    {
        .name = _("Sacapecho"),
        .description = COMPOUND_STRING(
        "Impide que el\n"
        "rival le baje la\n"
        "Defensa.\n"),
        .aiRating = 1,
        .breakable = TRUE,
    },

    [ABILITY_SAND_RUSH] =
    {
        .name = _("Ímpetu Arena"),
        .description = COMPOUND_STRING(
        "Aumenta su\n"
        "Velocidad durante\n"
        "las tormentas de\n"
        "arena.\n"),
        .aiRating = 6,
    },

    [ABILITY_WONDER_SKIN] =
    {
        .name = _("Piel Milagro"),
        .description = COMPOUND_STRING(
        "Presenta una mayor\n"
        "resistencia ante\n"
        "los movimientos de\n"
        "estado.\n"),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_ANALYTIC] =
    {
        .name = _("Cálculo Final"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "potencia de su\n"
        "movimiento si es\n"
        "el último en\n"
        "atacar.\n"),
        .aiRating = 5,
    },

    [ABILITY_ILLUSION] =
    {
        .name = _("Ilusión"),
        .description = COMPOUND_STRING(
        "Adopta el aspecto\n"
        "del último Pokémon\n"
        "del equipo al\n"
        "entrar en combate\n"
        "para desconcertar\n"
        "al rival.\n"),
        .aiRating = 8,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_IMPOSTER] =
    {
        .name = _("Impostor"),
        .description = COMPOUND_STRING(
        "Se transforma en\n"
        "el Pokémon que\n"
        "tiene enfrente.\n"),
        .aiRating = 9,
        .cantBeCopied = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_INFILTRATOR] =
    {
        .name = _("Allanamiento"),
        .description = COMPOUND_STRING(
        "Ataca sorteando la\n"
        "barrera o el\n"
        "sustituto del\n"
        "rival.\n"),
        .aiRating = 6,
    },

    [ABILITY_MUMMY] =
    {
        .name = _("Momia"),
        .description = COMPOUND_STRING(
        "Contagia la\n"
        "habilidad Momia al\n"
        "rival que entre en\n"
        "contacto con él.\n"),
        .aiRating = 5,
    },

    [ABILITY_MOXIE] =
    {
        .name = _("Autoestima"),
        .description = COMPOUND_STRING(
        "Al debilitar a un\n"
        "objetivo, su\n"
        "confianza se\n"
        "refuerza de tal\n"
        "manera que aumenta\n"
        "su Ataque.\n"),
        .aiRating = 7,
    },

    [ABILITY_JUSTIFIED] =
    {
        .name = _("Justiciero"),
        .description = COMPOUND_STRING(
        "Si le alcanza un\n"
        "movimiento de tipo\n"
        "Siniestro, aumenta\n"
        "el Ataque debido a\n"
        "su integridad.\n"),
        .aiRating = 4,
    },

    [ABILITY_RATTLED] =
    {
        .name = _("Cobardía"),
        .description = COMPOUND_STRING(
        "Si le alcanza un\n"
        "movimiento de tipo\n"
        "Siniestro, Bicho o\n"
        "Fantasma, el miedo\n"
        "hace que le suba\n"
        "la Velocidad.\n"),
        .aiRating = 3,
    },

    [ABILITY_MAGIC_BOUNCE] =
    {
        .name = _("Espejo Mágico"),
        .description = COMPOUND_STRING(
        "Puede devolver los\n"
        "movimientos de\n"
        "estado lanzados\n"
        "por el rival, sin\n"
        "verse afectado por\n"
        "ellos.\n"),
        .aiRating = 9,
        .breakable = TRUE,
    },

    [ABILITY_SAP_SIPPER] =
    {
        .name = _("Herbívoro"),
        .description = COMPOUND_STRING(
        "Neutraliza los\n"
        "movimientos de\n"
        "tipo Planta y sube\n"
        "su Ataque.\n"),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_PRANKSTER] =
    {
        .name = _("Bromista"),
        .description = COMPOUND_STRING(
        "Sus movimientos de\n"
        "estado tienen\n"
        "prioridad alta.\n"),
        .aiRating = 8,
    },

    [ABILITY_SAND_FORCE] =
    {
        .name = _("Poder Arena"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "movimientos de\n"
        "tipo Tierra, Acero\n"
        "y Roca durante las\n"
        "tormentas de\n"
        "arena.\n"),
        .aiRating = 4,
    },

    [ABILITY_IRON_BARBS] =
    {
        .name = _("Punta Acero"),
        .description = COMPOUND_STRING(
        "Inflige daño al\n"
        "rival si este le\n"
        "golpea con un\n"
        "movimiento de\n"
        "contacto.\n"),
        .aiRating = 6,
    },

    [ABILITY_ZEN_MODE] =
    {
        .name = _("Modo Daruma"),
        .description = COMPOUND_STRING(
        "Cambia de forma si\n"
        "sus PS se ven\n"
        "reducidos a la\n"
        "mitad.\n"),
        .aiRating = -1,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = B_UPDATED_ABILITY_DATA >= GEN_7,
    },

    [ABILITY_VICTORY_STAR] =
    {
        .name = _("Tinovictoria"),
        .description = COMPOUND_STRING(
        "Sube su Precisión\n"
        "y la de sus\n"
        "aliados.\n"),
        .aiRating = 6,
    },

    [ABILITY_TURBOBLAZE] =
    {
        .name = _("Turbollama"),
        .description = COMPOUND_STRING(
        "Las habilidades\n"
        "del objetivo no\n"
        "afectan a los\n"
        "movimientos que\n"
        "emplea.\n"),
        .aiRating = 7,
    },

    [ABILITY_TERAVOLT] =
    {
        .name = _("Terravoltaje"),
        .description = COMPOUND_STRING(
        "Las habilidades\n"
        "del objetivo no\n"
        "afectan a los\n"
        "movimientos que\n"
        "emplea.\n"),
        .aiRating = 7,
    },

    [ABILITY_AROMA_VEIL] =
    {
        .name = _("Velo Aroma"),
        .description = COMPOUND_STRING(
        "Se protege a sí\n"
        "mismo y a sus\n"
        "aliados de ataques\n"
        "que impiden elegir\n"
        "movimientos.\n"),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_FLOWER_VEIL] =
    {
        .name = _("Velo Flor"),
        .description = COMPOUND_STRING(
        "Evita que los\n"
        "Pokémon de tipo\n"
        "Planta aliados\n"
        "sufran problemas\n"
        "de estado o que\n"
        "les bajen sus\n"
        "características.\n"),
        .aiRating = 0,
        .breakable = TRUE,
    },

    [ABILITY_CHEEK_POUCH] =
    {
        .name = _("Carrillo"),
        .description = COMPOUND_STRING(
        "Recupera PS al\n"
        "comer cualquier\n"
        "baya.\n"),
        .aiRating = 4,
    },

    [ABILITY_PROTEAN] =
    {
        .name = _("Mutatipo"),
        .description = COMPOUND_STRING(
        "Cambia su tipo al\n"
        "del movimiento que\n"
        "va a usar.\n"),
        .aiRating = 8,
    },

    [ABILITY_FUR_COAT] =
    {
        .name = _("Pelaje Recio"),
        .description = COMPOUND_STRING(
        "Reduce a la mitad\n"
        "el daño que recibe\n"
        "de ataques físicos\n"
        "del rival.\n"),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_MAGICIAN] =
    {
        .name = _("Prestidigitador"),
        .description = COMPOUND_STRING(
        "Roba el objeto del\n"
        "Pokémon al que\n"
        "alcance con un\n"
        "movimiento.\n"),
        .aiRating = 3,
    },

    [ABILITY_BULLETPROOF] =
    {
        .name = _("Antibalas"),
        .description = COMPOUND_STRING(
        "No le afectan las\n"
        "bombas ni algunos\n"
        "proyectiles.\n"),
        .breakable = TRUE,
        .aiRating = 7,
    },

    [ABILITY_COMPETITIVE] =
    {
        .name = _("Tenacidad"),
        .description = COMPOUND_STRING(
        "Aumenta mucho su\n"
        "Ataque Especial\n"
        "cuando el rival le\n"
        "baja cualquiera de\n"
        "sus\n"
        "características.\n"),
        .aiRating = 5,
    },

    [ABILITY_STRONG_JAW] =
    {
        .name = _("Mandíbula Fuerte"),
        .description = COMPOUND_STRING(
        "Su robusta\n"
        "mandíbula le\n"
        "confiere una\n"
        "mordedura potente.\n"),
        .aiRating = 6,
    },

    [ABILITY_REFRIGERATE] =
    {
        .name = _("Piel Helada"),
        .description = COMPOUND_STRING(
        "Convierte los\n"
        "movimientos de\n"
        "tipo Normal en\n"
        "tipo Hielo y\n"
        "aumenta\n"
        "ligeramente su\n"
        "potencia.\n"),
        .aiRating = 8,
    },

    [ABILITY_SWEET_VEIL] =
    {
        .name = _("Velo Dulce"),
        .description = COMPOUND_STRING(
        "No cae dormido y\n"
        "evita también que\n"
        "sus aliados se\n"
        "duerman.\n"),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_STANCE_CHANGE] =
    {
        .name = _("Cambio Táctico"),
        .description = COMPOUND_STRING(
        "Adopta la Forma\n"
        "Filo al lanzar un\n"
        "ataque, o bien la\n"
        "Forma Escudo si\n"
        "usa el movimiento\n"
        "Escudo Real.\n"),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_GALE_WINGS] =
    {
        .name = _("Alas Vendaval"),
        .description = COMPOUND_STRING(
        "Da prioridad a los\n"
        "movimientos de\n"
        "tipo Volador si\n"
        "sus PS están al\n"
        "máximo.\n"),
        .aiRating = 6,
    },

    [ABILITY_MEGA_LAUNCHER] =
    {
        .name = _("Megadisparador"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "potencia de\n"
        "algunos\n"
        "movimientos de\n"
        "pulsos y auras.\n"),
        .aiRating = 7,
    },

    [ABILITY_GRASS_PELT] =
    {
        .name = _("Manto Frondoso"),
        .description = COMPOUND_STRING(
        "Aumenta su Defensa\n"
        "si hay un campo de\n"
        "hierba en el\n"
        "terreno de\n"
        "combate.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_SYMBIOSIS] =
    {
        .name = _("Simbiosis"),
        .description = COMPOUND_STRING(
        "Pasa su objeto a\n"
        "un aliado que ya\n"
        "haya utilizado el\n"
        "suyo.\n"),
        .aiRating = 0,
    },

    [ABILITY_TOUGH_CLAWS] =
    {
        .name = _("Garra Dura"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "potencia de los\n"
        "movimientos de\n"
        "contacto.\n"),
        .aiRating = 7,
    },

    [ABILITY_PIXILATE] =
    {
        .name = _("Piel Feérica"),
        .description = COMPOUND_STRING(
        "Convierte los\n"
        "movimientos de\n"
        "tipo Normal en\n"
        "tipo Hada y\n"
        "aumenta\n"
        "ligeramente su\n"
        "potencia.\n"),
        .aiRating = 8,
    },

    [ABILITY_GOOEY] =
    {
        .name = _("Baba"),
        .description = COMPOUND_STRING(
        "Baja la Velocidad\n"
        "del rival cuando\n"
        "este le ataca con\n"
        "un movimiento de\n"
        "contacto.\n"),
        .aiRating = 5,
    },

    [ABILITY_AERILATE] =
    {
        .name = _("Piel Celeste"),
        .description = COMPOUND_STRING(
        "Convierte los\n"
        "movimientos de\n"
        "tipo Normal en\n"
        "tipo Volador y\n"
        "aumenta\n"
        "ligeramente su\n"
        "potencia.\n"),
        .aiRating = 8,
    },

    [ABILITY_PARENTAL_BOND] =
    {
        .name = _("Amor Filial"),
        .description = COMPOUND_STRING(
        "Une fuerzas con su\n"
        "cría y ataca dos\n"
        "veces.\n"),
        .aiRating = 10,
    },

    [ABILITY_DARK_AURA] =
    {
        .name = _("Aura Oscura"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "potencia de todos\n"
        "los movimientos de\n"
        "tipo Siniestro.\n"),
        .aiRating = 6,
    },

    [ABILITY_FAIRY_AURA] =
    {
        .name = _("Aura Feérica"),
        .description = COMPOUND_STRING(
        "Aumenta la\n"
        "potencia de todos\n"
        "los movimientos de\n"
        "tipo Hada.\n"),
        .aiRating = 6,
    },

    [ABILITY_AURA_BREAK] =
    {
        .name = _("Rompeaura"),
        .description = COMPOUND_STRING(
        "Invierte los\n"
        "efectos de las\n"
        "auras, por lo que\n"
        "baja la potencia\n"
        "de ciertos\n"
        "movimientos en vez\n"
        "de subirla.\n"),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_PRIMORDIAL_SEA] =
    {
        .name = _("Mar del Albor"),
        .description = COMPOUND_STRING(
        "Altera el clima\n"
        "para anular los\n"
        "ataques de tipo\n"
        "Fuego.\n"),
        .aiRating = 10,
    },

    [ABILITY_DESOLATE_LAND] =
    {
        .name = _("Tierra del Ocaso"),
        .description = COMPOUND_STRING(
        "Altera el clima\n"
        "para anular los\n"
        "ataques de tipo\n"
        "Agua.\n"),
        .aiRating = 10,
    },

    [ABILITY_DELTA_STREAM] =
    {
        .name = _("Ráfaga Delta"),
        .description = COMPOUND_STRING(
        "Altera el clima\n"
        "para anular las\n"
        "vulnerabilidades\n"
        "del tipo Volador.\n"),
        .aiRating = 10,
    },

    [ABILITY_STAMINA] =
    {
        .name = _("Firmeza"),
        .description = COMPOUND_STRING(
        "Aumenta su Defensa\n"
        "al recibir un\n"
        "ataque.\n"),
        .aiRating = 6,
    },

    [ABILITY_WIMP_OUT] =
    {
        .name = _("Huida"),
        .description = COMPOUND_STRING(
        "Se asusta y\n"
        "abandona el\n"
        "terreno de combate\n"
        "cuando sus PS se\n"
        "ven reducidos a la\n"
        "mitad.\n"),
        .aiRating = 3,
    },

    [ABILITY_EMERGENCY_EXIT] =
    {
        .name = _("Retirada"),
        .description = COMPOUND_STRING(
        "Abandona el\n"
        "terreno de combate\n"
        "cuando sus PS se\n"
        "ven reducidos a la\n"
        "mitad para evitar\n"
        "males mayores.\n"),
        .aiRating = 3,
    },

    [ABILITY_WATER_COMPACTION] =
    {
        .name = _("Hidrorrefuerzo"),
        .description = COMPOUND_STRING(
        "Aumenta mucho su\n"
        "Defensa si le\n"
        "alcanza un\n"
        "movimiento de tipo\n"
        "Agua.\n"),
        .aiRating = 4,
    },

    [ABILITY_MERCILESS] =
    {
        .name = _("Ensañamiento"),
        .description = COMPOUND_STRING(
        "Hace que sus\n"
        "movimientos\n"
        "asesten siempre un\n"
        "golpe crítico si\n"
        "el rival está\n"
        "envenenado.\n"),
        .aiRating = 4,
    },

    [ABILITY_SHIELDS_DOWN] =
    {
        .name = _("Escudo Limitado"),
        .description = COMPOUND_STRING(
        "Rompe su coraza\n"
        "cuando sus PS se\n"
        "ven reducidos a la\n"
        "mitad y adopta una\n"
        "forma ofensiva.\n"),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_STAKEOUT] =
    {
        .name = _("Vigilante"),
        .description = COMPOUND_STRING(
        "Si el objetivo de\n"
        "su ataque es\n"
        "sustituido por\n"
        "otro, duplica el\n"
        "daño que\n"
        "infligirá.\n"),
        .aiRating = 6,
    },

    [ABILITY_WATER_BUBBLE] =
    {
        .name = _("Pompa"),
        .description = COMPOUND_STRING(
        "Reduce el daño que\n"
        "le provocan los\n"
        "movimientos de\n"
        "tipo Fuego y es\n"
        "inmune a las\n"
        "quemaduras.\n"),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_STEELWORKER] =
    {
        .name = _("Acero Templado"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "movimientos de\n"
        "tipo Acero.\n"),
        .aiRating = 6,
    },

    [ABILITY_BERSERK] =
    {
        .name = _("Cólera"),
        .description = COMPOUND_STRING(
        "Aumenta su Ataque\n"
        "Especial si sus PS\n"
        "se ven reducidos a\n"
        "la mitad debido a\n"
        "algún ataque.\n"),
        .aiRating = 5,
    },

    [ABILITY_SLUSH_RUSH] =
    {
        .name = _("Quitanieves"),
        .description = COMPOUND_STRING(
        "Aumenta su\n"
        "Velocidad si está\n"
        "granizando.\n"),
        .aiRating = 5,
    },

    [ABILITY_LONG_REACH] =
    {
        .name = _("Remoto"),
        .description = COMPOUND_STRING(
        "Puede usar\n"
        "cualquier\n"
        "movimiento sin\n"
        "entrar en contacto\n"
        "con el rival.\n"),
        .aiRating = 3,
    },

    [ABILITY_LIQUID_VOICE] =
    {
        .name = _("Voz Fluida"),
        .description = COMPOUND_STRING(
        "Hace que todos sus\n"
        "movimientos que\n"
        "usan sonido pasen\n"
        "a ser de tipo\n"
        "Agua.\n"),
        .aiRating = 5,
    },

    [ABILITY_TRIAGE] =
    {
        .name = _("Primer Auxilio"),
        .description = COMPOUND_STRING(
        "Da prioridad a los\n"
        "movimientos que\n"
        "restauran PS.\n"),
        .aiRating = 7,
    },

    [ABILITY_GALVANIZE] =
    {
        .name = _("Piel Eléctrica"),
        .description = COMPOUND_STRING(
        "Convierte los\n"
        "movimientos de\n"
        "tipo Normal en\n"
        "tipo Eléctrico y\n"
        "aumenta\n"
        "ligeramente su\n"
        "potencia.\n"),
        .aiRating = 8,
    },

    [ABILITY_SURGE_SURFER] =
    {
        .name = _("Cola Surf"),
        .description = COMPOUND_STRING(
        "Duplica su\n"
        "Velocidad si hay\n"
        "un campo eléctrico\n"
        "en el terreno de\n"
        "combate.\n"),
        .aiRating = 4,
    },

    [ABILITY_SCHOOLING] =
    {
        .name = _("Banco"),
        .description = COMPOUND_STRING(
        "Forma bancos con\n"
        "sus congéneres\n"
        "cuando tiene\n"
        "muchos PS, lo cual\n"
        "le otorga más\n"
        "fuerza. Cuando le\n"
        "quedan pocos PS,\n"
        "el banco se\n"
        "dispersa.\n"),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_DISGUISE] =
    {
        .name = _("Disfraz"),
        .description = COMPOUND_STRING(
        "Puede eludir un\n"
        "ataque valiéndose\n"
        "de la tela que le\n"
        "cubre el cuerpo\n"
        "una vez por\n"
        "combate.\n"),
        .aiRating = 8,
        .breakable = TRUE,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_BATTLE_BOND] =
    {
        .name = _("Fuerte Afecto"),
        .description = COMPOUND_STRING(
        "Al derrotar a un\n"
        "rival, los\n"
        "vínculos con su\n"
        "Entrenador se\n"
        "refuerzan y se\n"
        "convierte en\n"
        "Greninja Ash. Su\n"
        "Shuriken de Agua\n"
        "también se ve\n"
        "potenciado.\n"),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_POWER_CONSTRUCT] =
    {
        .name = _("Agrupamiento"),
        .description = COMPOUND_STRING(
        "Cuando sus PS se\n"
        "ven reducidos a la\n"
        "mitad, las células\n"
        "se reagrupan y\n"
        "adopta su Forma\n"
        "Completa.\n"),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_CORROSION] =
    {
        .name = _("Corrosión"),
        .description = COMPOUND_STRING(
        "Puede envenenar\n"
        "incluso a rivales\n"
        "de tipo Acero o\n"
        "Veneno.\n"),
        .aiRating = 5,
    },

    [ABILITY_COMATOSE] =
    {
        .name = _("Letargo Perenne"),
        .description = COMPOUND_STRING(
        "No despierta jamás\n"
        "de su profundo\n"
        "letargo e incluso\n"
        "ataca dormido.\n"),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_QUEENLY_MAJESTY] =
    {
        .name = _("Regia Presencia"),
        .description = COMPOUND_STRING(
        "Intimida al\n"
        "objetivo y le\n"
        "impide usar\n"
        "movimientos con\n"
        "prioridad.\n"),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_INNARDS_OUT] =
    {
        .name = _("Revés"),
        .description = COMPOUND_STRING(
        "Al caer\n"
        "debilitado,\n"
        "inflige al rival\n"
        "un daño\n"
        "equivalente a los\n"
        "PS que le\n"
        "quedaran.\n"),
        .aiRating = 5,
    },

    [ABILITY_DANCER] =
    {
        .name = _("Pareja de Baile"),
        .description = COMPOUND_STRING(
        "Puede copiar\n"
        "inmediatamente\n"
        "cualquier\n"
        "movimiento de\n"
        "baile que haya\n"
        "usado otro Pokémon\n"
        "presente en el\n"
        "combate.\n"),
        .aiRating = 5,
    },

    [ABILITY_BATTERY] =
    {
        .name = _("Batería"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "ataques especiales\n"
        "de los aliados.\n"),
        .aiRating = 0,
    },

    [ABILITY_FLUFFY] =
    {
        .name = _("Peluche"),
        .description = COMPOUND_STRING(
        "Reduce a la mitad\n"
        "el daño provocado\n"
        "por los\n"
        "movimientos de\n"
        "contacto, pero\n"
        "duplica el\n"
        "infligido por los\n"
        "de tipo Fuego.\n"),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_DAZZLING] =
    {
        .name = _("Cuerpo Vívido"),
        .description = COMPOUND_STRING(
        "Desconcierta al\n"
        "rival y le impide\n"
        "utilizar\n"
        "movimientos con\n"
        "prioridad en su\n"
        "contra.\n"),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_SOUL_HEART] =
    {
        .name = _("Coránima"),
        .description = COMPOUND_STRING(
        "Aumenta su Ataque\n"
        "Especial cada vez\n"
        "que un Pokémon cae\n"
        "debilitado.\n"),
        .aiRating = 7,
    },

    [ABILITY_TANGLING_HAIR] =
    {
        .name = _("Rizos Rebeldes"),
        .description = COMPOUND_STRING(
        "Baja la Velocidad\n"
        "del rival cuando\n"
        "este le ataca con\n"
        "un movimiento de\n"
        "contacto.\n"),
        .aiRating = 5,
    },

    [ABILITY_RECEIVER] =
    {
        .name = _("Receptor"),
        .description = COMPOUND_STRING(
        "Adquiere la\n"
        "habilidad de un\n"
        "aliado debilitado.\n"),
        .aiRating = 0,
        .cantBeCopied = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_POWER_OF_ALCHEMY] =
    {
        .name = _("Reacción Química"),
        .description = COMPOUND_STRING(
        "Reacciona copiando\n"
        "la habilidad de un\n"
        "aliado debilitado.\n"),
        .aiRating = 0,
        .cantBeCopied = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_BEAST_BOOST] =
    {
        .name = _("Ultraimpulso"),
        .description = COMPOUND_STRING(
        "Si derrota a un\n"
        "rival en ese\n"
        "turno, aumenta su\n"
        "característica más\n"
        "fuerte.\n"),
        .aiRating = 7,
    },

    [ABILITY_RKS_SYSTEM] =
    {
        .name = _("Sistema Alfa"),
        .description = COMPOUND_STRING(
        "Cambia su tipo\n"
        "según el disco que\n"
        "lleve instalado.\n"),
        .aiRating = 8,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_ELECTRIC_SURGE] =
    {
        .name = _("Electrogénesis"),
        .description = COMPOUND_STRING(
        "Crea un campo\n"
        "eléctrico al\n"
        "entrar en combate.\n"),
        .aiRating = 8,
    },

    [ABILITY_PSYCHIC_SURGE] =
    {
        .name = _("Psicogénesis"),
        .description = COMPOUND_STRING(
        "Crea un campo\n"
        "psíquico al entrar\n"
        "en combate.\n"),
        .aiRating = 8,
    },

    [ABILITY_MISTY_SURGE] =
    {
        .name = _("Nebulogénesis"),
        .description = COMPOUND_STRING(
        "Crea un campo de\n"
        "niebla al entrar\n"
        "en combate.\n"),
        .aiRating = 8,
    },

    [ABILITY_GRASSY_SURGE] =
    {
        .name = _("Herbogénesis"),
        .description = COMPOUND_STRING(
        "Crea un campo de\n"
        "hierba al entrar\n"
        "en combate.\n"),
        .aiRating = 8,
    },

    [ABILITY_FULL_METAL_BODY] =
    {
        .name = _("Guardia Metálica"),
        .description = COMPOUND_STRING(
        "Evita que bajen\n"
        "sus\n"
        "características a\n"
        "causa de\n"
        "movimientos o\n"
        "habilidades de\n"
        "otros Pokémon.\n"),
        .aiRating = 4,
    },

    [ABILITY_SHADOW_SHIELD] =
    {
        .name = _("Guardia Espectro"),
        .description = COMPOUND_STRING(
        "Reduce el daño que\n"
        "sufre si sus PS\n"
        "están al máximo.\n"),
        .aiRating = 8,
    },

    [ABILITY_PRISM_ARMOR] =
    {
        .name = _("Armadura Prisma"),
        .description = COMPOUND_STRING(
        "Mitiga el daño que\n"
        "le infligen los\n"
        "movimientos\n"
        "supereficaces.\n"),
        .aiRating = 6,
    },

    [ABILITY_NEUROFORCE] =
    {
        .name = _("Fuerza Cerebral"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "ataques\n"
        "supereficaces.\n"),
        .aiRating = 6,
    },

    [ABILITY_INTREPID_SWORD] =
    {
        .name = _("Espada Indómita"),
        .description = COMPOUND_STRING(
        "Aumenta su Ataque\n"
        "al entrar en\n"
        "combate.\n"),
        .aiRating = 3,
    },

    [ABILITY_DAUNTLESS_SHIELD] =
    {
        .name = _("Escudo Recio"),
        .description = COMPOUND_STRING(
        "Aumenta su Defensa\n"
        "al entrar en\n"
        "combate.\n"),
        .aiRating = 3,
    },

    [ABILITY_LIBERO] =
    {
        .name = _("Líbero"),
        .description = COMPOUND_STRING(
        "Cambia su tipo al\n"
        "del movimiento que\n"
        "va a usar.\n"),
    },

    [ABILITY_BALL_FETCH] =
    {
        .name = _("Recogebolas"),
        .description = COMPOUND_STRING(
        "Si no lleva\n"
        "equipado ningún\n"
        "objeto, recupera\n"
        "la Poké Ball del\n"
        "primer intento de\n"
        "captura fallido.\n"),
        .aiRating = 0,
    },

    [ABILITY_COTTON_DOWN] =
    {
        .name = _("Pelusa"),
        .description = COMPOUND_STRING(
        "Al ser alcanzado\n"
        "por un ataque,\n"
        "suelta una pelusa\n"
        "de algodón que\n"
        "reduce la\n"
        "Velocidad de todos\n"
        "los demás Pokémon.\n"),
        .aiRating = 3,
    },

    [ABILITY_PROPELLER_TAIL] =
    {
        .name = _("Hélice Caudal"),
        .description = COMPOUND_STRING(
        "Impide que el\n"
        "rival pueda atraer\n"
        "ataques hacia sí\n"
        "mismo por efecto\n"
        "de su habilidad o\n"
        "sus movimientos.\n"),
        .aiRating = 2,
    },

    [ABILITY_MIRROR_ARMOR] =
    {
        .name = _("Coraza Reflejo"),
        .description = COMPOUND_STRING(
        "Refleja los\n"
        "efectos que\n"
        "reducen las\n"
        "características.\n"),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_GULP_MISSILE] =
    {
        .name = _("Tragamisil"),
        .description = COMPOUND_STRING(
        "Tras usar Surf o\n"
        "Buceo, emerge con\n"
        "una presa en la\n"
        "boca. Al recibir\n"
        "daño, ataca\n"
        "escupiéndola al\n"
        "rival.\n"),
        .aiRating = 3,
        .cantBeCopied = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_STALWART] =
    {
        .name = _("Acérrimo"),
        .description = COMPOUND_STRING(
        "Impide que el\n"
        "rival pueda atraer\n"
        "ataques hacia sí\n"
        "mismo por efecto\n"
        "de su habilidad o\n"
        "sus movimientos.\n"),
        .aiRating = 2,
    },

    [ABILITY_STEAM_ENGINE] =
    {
        .name = _("Combustible"),
        .description = COMPOUND_STRING(
        "Si le alcanza un\n"
        "movimiento de tipo\n"
        "Fuego o Agua, le\n"
        "sube muchísimo la\n"
        "Velocidad.\n"),
        .aiRating = 3,
    },

    [ABILITY_PUNK_ROCK] =
    {
        .name = _("Punk Rock"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "movimientos que\n"
        "usan sonido y\n"
        "reduce a la mitad\n"
        "el daño que le\n"
        "infligen dichos\n"
        "movimientos.\n"),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_SAND_SPIT] =
    {
        .name = _("Expulsarena"),
        .description = COMPOUND_STRING(
        "Provoca una\n"
        "tormenta de arena\n"
        "al recibir un\n"
        "ataque.\n"),
        .aiRating = 5,
    },

    [ABILITY_ICE_SCALES] =
    {
        .name = _("Escama de Hielo"),
        .description = COMPOUND_STRING(
        "Las gélidas\n"
        "escamas que\n"
        "protegen su cuerpo\n"
        "reducen a la mitad\n"
        "el daño que le\n"
        "infligen los\n"
        "ataques\n"
        "especiales.\n"),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_RIPEN] =
    {
        .name = _("Maduración"),
        .description = COMPOUND_STRING(
        "Hace madurar las\n"
        "bayas, por lo que\n"
        "duplica sus\n"
        "efectos.\n"),
        .aiRating = 4,
    },

    [ABILITY_ICE_FACE] =
    {
        .name = _("Cara de Hielo"),
        .description = COMPOUND_STRING(
        "Absorbe el daño de\n"
        "un ataque físico\n"
        "con el hielo de la\n"
        "cabeza, tras lo\n"
        "cual cambia de\n"
        "forma. El hielo se\n"
        "regenerará la\n"
        "próxima vez que\n"
        "granice.\n"),
        .aiRating = 4,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .breakable = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_POWER_SPOT] =
    {
        .name = _("Fuente Energía"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "movimientos de los\n"
        "Pokémon\n"
        "adyacentes.\n"),
        .aiRating = 2,
    },

    [ABILITY_MIMICRY] =
    {
        .name = _("Mimetismo"),
        .description = COMPOUND_STRING(
        "Cambia su tipo\n"
        "según el campo que\n"
        "haya en el terreno\n"
        "de combate.\n"),
        .aiRating = 2,
    },

    [ABILITY_SCREEN_CLEANER] =
    {
        .name = _("Antibarrera"),
        .description = COMPOUND_STRING(
        "Anula los efectos\n"
        "de Pantalla de\n"
        "Luz, Reflejo y\n"
        "Velo Aurora tanto\n"
        "de rivales como de\n"
        "aliados al entrar\n"
        "en combate.\n"),
        .aiRating = 3,
    },

    [ABILITY_STEELY_SPIRIT] =
    {
        .name = _("Alma Acerada"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "movimientos de\n"
        "tipo Acero de los\n"
        "aliados.\n"),
        .aiRating = 2,
    },

    [ABILITY_PERISH_BODY] =
    {
        .name = _("Cuerpo Mortal"),
        .description = COMPOUND_STRING(
        "Si le alcanza un\n"
        "movimiento de\n"
        "contacto, se\n"
        "debilitará al cabo\n"
        "de 3 turnos, así\n"
        "como su agresor, a\n"
        "menos que\n"
        "abandonen el\n"
        "terreno de\n"
        "combate.\n"),
        .aiRating = -1,
    },

    [ABILITY_WANDERING_SPIRIT] =
    {
        .name = _("Alma Errante"),
        .description = COMPOUND_STRING(
        "Si le alcanza un\n"
        "movimiento de\n"
        "contacto,\n"
        "intercambia su\n"
        "habilidad con la\n"
        "del agresor.\n"),
        .aiRating = 2,
    },

    [ABILITY_GORILLA_TACTICS] =
    {
        .name = _("Monotema"),
        .description = COMPOUND_STRING(
        "Potencia su\n"
        "Ataque, pero solo\n"
        "puede usar el\n"
        "primer movimiento\n"
        "escogido.\n"),
        .aiRating = 4,
    },

    [ABILITY_NEUTRALIZING_GAS] =
    {
        .name = _("Gas Reactivo"),
        .description = COMPOUND_STRING(
        "Anula los efectos\n"
        "de las habilidades\n"
        "de los demás\n"
        "Pokémon presentes\n"
        "mientras esté en\n"
        "el terreno de\n"
        "combate.\n"),
        .aiRating = 5,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_PASTEL_VEIL] =
    {
        .name = _("Velo Pastel"),
        .description = COMPOUND_STRING(
        "Se protege a sí\n"
        "mismo y a sus\n"
        "aliados del\n"
        "envenenamiento.\n"),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_HUNGER_SWITCH] =
    {
        .name = _("Mutapetito"),
        .description = COMPOUND_STRING(
        "Alterna entre su\n"
        "Forma Saciada y\n"
        "Forma Voraz al\n"
        "final de cada\n"
        "turno.\n"),
        .aiRating = 2,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_QUICK_DRAW] =
    {
        .name = _("Mano Rápida"),
        .description = COMPOUND_STRING(
        "A veces, puede\n"
        "atacar el primero.\n"),
        .aiRating = 4,
    },

    [ABILITY_UNSEEN_FIST] =
    {
        .name = _("Puño Invisible"),
        .description = COMPOUND_STRING(
        "Si usa un\n"
        "movimiento de\n"
        "contacto, puede\n"
        "infligir daño al\n"
        "objetivo aunque\n"
        "este se proteja.\n"),
        .aiRating = 6,
    },

    [ABILITY_CURIOUS_MEDICINE] =
    {
        .name = _("Medicina Extraña"),
        .description = COMPOUND_STRING(
        "Al entrar en\n"
        "combate, rezuma\n"
        "una substancia\n"
        "medicinal por la\n"
        "caracola que\n"
        "revierte los\n"
        "cambios en las\n"
        "características de\n"
        "los aliados.\n"),
        .aiRating = 3,
    },

    [ABILITY_TRANSISTOR] =
    {
        .name = _("Transistor"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "movimientos de\n"
        "tipo Eléctrico.\n"),
        .aiRating = 6,
    },

    [ABILITY_DRAGONS_MAW] =
    {
        .name = _("Mandíbula Dragón"),
        .description = COMPOUND_STRING(
        "Potencia los\n"
        "movimientos de\n"
        "tipo Dragón.\n"),
        .aiRating = 6,
    },

    [ABILITY_CHILLING_NEIGH] =
    {
        .name = _("Relincho Blanco"),
        .description = COMPOUND_STRING(
        "Al derrotar a un\n"
        "objetivo, emite un\n"
        "relincho gélido y\n"
        "aumenta su Ataque.\n"),
        .aiRating = 7,
    },

    [ABILITY_GRIM_NEIGH] =
    {
        .name = _("Relincho Negro"),
        .description = COMPOUND_STRING(
        "Al derrotar a un\n"
        "objetivo, emite un\n"
        "relincho aterrador\n"
        "y aumenta su\n"
        "Ataque Especial.\n"),
        .aiRating = 7,
    },

    [ABILITY_AS_ONE_ICE_RIDER] =
    {
        .name = _("Unidad Ecuestre"),
        .description = COMPOUND_STRING(
        "El Pokémon tiene\n"
        "dos habilidades:\n"
        "Relincho Blanco de\n"
        "Glastrier y\n"
        "Nerviosismo de\n"
        "Calyrex.\n"),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_AS_ONE_SHADOW_RIDER] =
    {
        .name = _("Unidad Ecuestre"),
        .description = COMPOUND_STRING(
        "El Pokémon tiene\n"
        "dos habilidades:\n"
        "Relincho Negro de\n"
        "Spectrier y\n"
        "Nerviosismo de\n"
        "Calyrex.\n"),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_LINGERING_AROMA] =
    {
        .name = _("Olor Persistente"),
        .description = COMPOUND_STRING("Spreads with contact."),
        .aiRating = 5,
    },

    [ABILITY_SEED_SOWER] =
    {
        .name = _("Disemillar"),
        .description = COMPOUND_STRING("Affects terrain when hit."),
        .aiRating = 5,
    },

    [ABILITY_THERMAL_EXCHANGE] =
    {
        .name = _("Termoscambio"),
        .description = COMPOUND_STRING("Fire hits up Attack."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_ANGER_SHELL] =
    {
        .name = _("Coraza Ira"),
        .description = COMPOUND_STRING("Gets angry at half HP."),
        .aiRating = 3,
    },

    [ABILITY_PURIFYING_SALT] =
    {
        .name = _("Sal Purificadora"),
        .description = COMPOUND_STRING("Protected by pure salts."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_WELL_BAKED_BODY] =
    {
        .name = _("Cuerpo Horneado"),
        .description = COMPOUND_STRING("Strengthened by Fire."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_WIND_RIDER] =
    {
        .name = _("Surcavientos"),
        .description = COMPOUND_STRING("Ups Attack if hit by wind."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_GUARD_DOG] =
    {
        .name = _("Perro Guardián"),
        .description = COMPOUND_STRING("Cannot be intimidated."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_ROCKY_PAYLOAD] =
    {
        .name = _("Transportarrocas"),
        .description = COMPOUND_STRING("Powers up Rock moves."),
        .aiRating = 6,
    },

    [ABILITY_WIND_POWER] =
    {
        .name = _("Energía Eólica"),
        .description = COMPOUND_STRING("Gets charged by wind."),
        .aiRating = 4,
    },

    [ABILITY_ZERO_TO_HERO] =
    {
        .name = _("Cambio Heroico"),
        .description = COMPOUND_STRING("Changes form on switch out."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_COMMANDER] =
    {
        .name = _("Comandar"),
        .description = COMPOUND_STRING("Commands from Dondozo."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_ELECTROMORPHOSIS] =
    {
        .name = _("Dinamo"),
        .description = COMPOUND_STRING("Gets Charged when hit."),
        .aiRating = 5,
    },

    [ABILITY_PROTOSYNTHESIS] =
    {
        .name = _("Paleosíntesis"),
        .description = COMPOUND_STRING("Sun boosts best stat."),
        .aiRating = 7,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_QUARK_DRIVE] =
    {
        .name = _("Carga Cuark"),
        .description = COMPOUND_STRING("Elec. field ups best stat."),
        .aiRating = 7,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_GOOD_AS_GOLD] =
    {
        .name = _("Cuerpo Áureo"),
        .description = COMPOUND_STRING("Avoids status moves."),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_VESSEL_OF_RUIN] =
    {
        .name = _("Caldero Debacle"),
        .description = COMPOUND_STRING("Lowers foes' sp. damage."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_SWORD_OF_RUIN] =
    {
        .name = _("Espada Debacle"),
        .description = COMPOUND_STRING("Lowers foes' Defense."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_TABLETS_OF_RUIN] =
    {
        .name = _("Tablilla Debacle"),
        .description = COMPOUND_STRING("Lowers foes' damage."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_BEADS_OF_RUIN] =
    {
        .name = _("Abalorio Debacle"),
        .description = COMPOUND_STRING("Lowers foes' Sp. Defense."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_ORICHALCUM_PULSE] =
    {
        .name = _("Latido Oricalco"),
        .description = COMPOUND_STRING("Summons sunlight in battle."),
        .aiRating = 8,
    },

    [ABILITY_HADRON_ENGINE] =
    {
        .name = _("Motor Hadrónico"),
        .description = COMPOUND_STRING("Field becomes Electric."),
        .aiRating = 8,
    },

    [ABILITY_OPPORTUNIST] =
    {
        .name = _("Oportunista"),
        .description = COMPOUND_STRING("Copies foe's stat change."),
        .aiRating = 5,
    },

    [ABILITY_CUD_CHEW] =
    {
        .name = _("Rumia"),
        .description = COMPOUND_STRING("Eats a used berry again."),
        .aiRating = 4,
    },

    [ABILITY_SHARPNESS] =
    {
        .name = _("Cortante"),
        .description = COMPOUND_STRING("Strengthens slicing moves."),
        .aiRating = 7,
    },

    [ABILITY_SUPREME_OVERLORD] =
    {
        .name = _("General Supremo"),
        .description = COMPOUND_STRING("Inherits fallen's strength."),
        .aiRating = 6,
    },

    [ABILITY_COSTAR] =
    {
        .name = _("Unísono"),
        .description = COMPOUND_STRING("Copies ally's stat changes."),
        .aiRating = 5,
    },

    [ABILITY_TOXIC_DEBRIS] =
    {
        .name = _("Capa Tóxica"),
        .description = COMPOUND_STRING("Throws poison spikes if hit."),
        .aiRating = 4,
    },

    [ABILITY_ARMOR_TAIL] =
    {
        .name = _("Cola Armadura"),
        .description = COMPOUND_STRING("Protects from priority."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_EARTH_EATER] =
    {
        .name = _("Geofagia"),
        .description = COMPOUND_STRING("Eats ground to heal HP."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_MYCELIUM_MIGHT] =
    {
        .name = _("Poder Fúngico"),
        .description = COMPOUND_STRING("Status moves never fail."),
        .aiRating = 2,
    },

    [ABILITY_HOSPITALITY] =
    {
        .name = _("Ojo Mental "),
        .description = COMPOUND_STRING("Restores ally's HP."),
        .aiRating = 5,
    },

    [ABILITY_MINDS_EYE] =
    {
        .name = _("Néctar Dulce"),
        .description = COMPOUND_STRING("Keen Eye and Scrappy."),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_EMBODY_ASPECT_TEAL_MASK] =
    {
        .name = _("Hospitalidad"),
        .description = COMPOUND_STRING("Raises Speed."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_EMBODY_ASPECT_HEARTHFLAME_MASK] =
    {
        .name = _("Cadena Tóxica"),
        .description = COMPOUND_STRING("Raises Attack."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_EMBODY_ASPECT_WELLSPRING_MASK] =
    {
        .name = _("Evocarrecuerdos"),
        .description = COMPOUND_STRING("Raises Sp. Def."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_EMBODY_ASPECT_CORNERSTONE_MASK] =
    {
        .name = _("Teracambio"),
        .description = COMPOUND_STRING("Raises Defense."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_TOXIC_CHAIN] =
    {
        .name = _("Teracaparazón"),
        .description = COMPOUND_STRING("Moves can poison."),
        .aiRating = 8,
    },

    [ABILITY_SUPERSWEET_SYRUP] =
    {
        .name = _("Teraformación 0"),
        .description = COMPOUND_STRING("Lowers the foe's Evasion."),
        .aiRating = 5,
    },

    [ABILITY_TERA_SHIFT] =
    {
        .name = _("Títere Tóxico"),
        .description = COMPOUND_STRING("Terastallizes upon entry."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_TERA_SHELL] =
    {
        .name = _("Tera Shell"),
        .description = COMPOUND_STRING("Resists all at full HP."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .breakable = TRUE,
    },

    [ABILITY_TERAFORM_ZERO] =
    {
        .name = _("Teraform Zero"),
        .description = COMPOUND_STRING("Zeroes weather and terrain."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_POISON_PUPPETEER] =
    {
        .name = _("Poison Puppeteer"),
        .description = COMPOUND_STRING("Confuses poisoned foes."),
        .aiRating = 8,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
    },
};
