#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_message.h"
#include "battle_setup.h"
#include "battle_tower.h"
#include "battle_z_move.h"
#include "data.h"
#include "event_data.h"
#include "frontier_util.h"
#include "graphics.h"
#include "international_string_util.h"
#include "item.h"
#include "link.h"
#include "menu.h"
#include "palette.h"
#include "recorded_battle.h"
#include "string_util.h"
#include "strings.h"
#include "test_runner.h"
#include "text.h"
#include "trainer_hill.h"
#include "trainer_slide.h"
#include "window.h"
#include "line_break.h"
#include "constants/abilities.h"
#include "constants/battle_dome.h"
#include "constants/battle_string_ids.h"
#include "constants/frontier_util.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "constants/trainer_hill.h"
#include "constants/weather.h"

struct BattleWindowText
{
    u8 fillValue;
    u8 fontId;
    u8 x;
    u8 y;
    u8 letterSpacing;
    u8 lineSpacing;
    u8 speed;
    u8 fgColor;
    u8 bgColor;
    u8 shadowColor;
};

#if TESTING
EWRAM_DATA u16 sBattlerAbilities[MAX_BATTLERS_COUNT] = {0};
#else
static EWRAM_DATA u16 sBattlerAbilities[MAX_BATTLERS_COUNT] = {0};
#endif
EWRAM_DATA struct BattleMsgData *gBattleMsgDataPtr = NULL;

// todo: make some of those names less vague: attacker/target vs pkmn, etc.

static const u8 sText_EmptyString4[] = _("");

const u8 gText_PkmnShroudedInMist[] = _("¡El equipo {B_ATK_TEAM1} se envolvió en niebla!");
const u8 gText_PkmnGettingPumped[] = _("¡{B_DEF_NAME_WITH_PREFIX} se viene arriba!");
const u8 gText_PkmnsXPreventsSwitching[] = _("¡{B_BUFF1} impide el cambio con su habilidad {B_LAST_ABILITY}!\p");
const u8 gText_StatSharply[] = _("mucho ");
const u8 gText_StatRose[] = _("subió!");
const u8 gText_StatFell[] = _("bajó!");
const u8 gText_DefendersStatRose[] = _("¡La {B_BUFF1} de {B_DEF_NAME_WITH_PREFIX} {B_BUFF2}subió!");
static const u8 sText_GotAwaySafely[] = _("{PLAY_SE SE_FLEE}¡Has escapado sin problemas!\p");
static const u8 sText_PlayerDefeatedLinkTrainer[] = _("¡Has vencido a {B_LINK_OPPONENT1_NAME}!");
static const u8 sText_TwoLinkTrainersDefeated[] = _("¡Has vencido a {B_LINK_OPPONENT1_NAME} y {B_LINK_OPPONENT2_NAME}!");
static const u8 sText_PlayerLostAgainstLinkTrainer[] = _("¡Has perdido contra {B_LINK_OPPONENT1_NAME}!");
static const u8 sText_PlayerLostToTwo[] = _("¡Has perdido contra {B_LINK_OPPONENT1_NAME} y {B_LINK_OPPONENT2_NAME}!");
static const u8 sText_PlayerBattledToDrawLinkTrainer[] = _("¡Empataste contra {B_LINK_OPPONENT1_NAME}!");
static const u8 sText_PlayerBattledToDrawVsTwo[] = _("¡Empataste contra {B_LINK_OPPONENT1_NAME} y {B_LINK_OPPONENT2_NAME}!");
static const u8 sText_WildFled[] = _("{PLAY_SE SE_FLEE}¡{B_LINK_OPPONENT1_NAME} huyó!"); //not in gen 5+, replaced with match was forfeited text
static const u8 sText_TwoWildFled[] = _("{PLAY_SE SE_FLEE}¡{B_LINK_OPPONENT1_NAME} y {B_LINK_OPPONENT2_NAME} huyeron!"); //not in gen 5+, replaced with match was forfeited text
static const u8 sText_PlayerDefeatedLinkTrainerTrainer1[] = _("¡Has vencido a {B_TRAINER1_NAME_WITH_CLASS}!\p");
static const u8 sText_OpponentMon1Appeared[] = _("¡Apareció {B_OPPONENT_MON1_NAME}!\p");
static const u8 sText_WildPkmnAppeared[] = _("¡Has encontrado un {B_OPPONENT_MON1_NAME} salvaje!\p");
static const u8 sText_LegendaryPkmnAppeared[] = _("¡Has encontrado un {B_OPPONENT_MON1_NAME} salvaje!\p");
static const u8 sText_WildPkmnAppearedPause[] = _("¡Has encontrado un {B_OPPONENT_MON1_NAME} salvaje!{PAUSE 127}");
static const u8 sText_TwoWildPkmnAppeared[] = _("¡Oh! ¡Aparecieron {B_OPPONENT_MON1_NAME} y {B_OPPONENT_MON2_NAME} salvajes!\p");
static const u8 sText_Trainer1WantsToBattle[] = _("¡{B_TRAINER1_NAME_WITH_CLASS} te desafía!\p");
static const u8 sText_LinkTrainerWantsToBattle[] = _("¡{B_LINK_OPPONENT1_NAME} te desafía!");
static const u8 sText_TwoLinkTrainersWantToBattle[] = _("¡{B_LINK_OPPONENT1_NAME} y {B_LINK_OPPONENT2_NAME} te desafían!");
static const u8 sText_Trainer1SentOutPkmn[] = _("¡{B_TRAINER1_NAME_WITH_CLASS} sacó a {B_OPPONENT_MON1_NAME}!");
static const u8 sText_Trainer1SentOutTwoPkmn[] = _("¡{B_TRAINER1_NAME_WITH_CLASS} sacó a {B_OPPONENT_MON1_NAME} y {B_OPPONENT_MON2_NAME}!");
static const u8 sText_Trainer1SentOutPkmn2[] = _("¡{B_TRAINER1_NAME_WITH_CLASS} sacó a {B_BUFF1}!");
static const u8 sText_LinkTrainerSentOutPkmn[] = _("¡{B_LINK_OPPONENT1_NAME} sacó a {B_OPPONENT_MON1_NAME}!");
static const u8 sText_LinkTrainerSentOutTwoPkmn[] = _("¡{B_LINK_OPPONENT1_NAME} sacó a {B_OPPONENT_MON1_NAME} y {B_OPPONENT_MON2_NAME}!");
static const u8 sText_TwoLinkTrainersSentOutPkmn[] = _("¡{B_LINK_OPPONENT1_NAME} sacó a {B_LINK_OPPONENT_MON1_NAME}! ¡{B_LINK_OPPONENT2_NAME} sacó a {B_LINK_OPPONENT_MON2_NAME}!");
static const u8 sText_LinkTrainerSentOutPkmn2[] = _("¡{B_LINK_OPPONENT1_NAME} sacó a {B_BUFF1}!");
static const u8 sText_LinkTrainerMultiSentOutPkmn[] = _("¡{B_LINK_SCR_TRAINER_NAME} sacó a {B_BUFF1}!");
static const u8 sText_GoPkmn[] = _("¡Adelante, {B_PLAYER_MON1_NAME}!");
static const u8 sText_GoTwoPkmn[] = _("¡Adelante, {B_PLAYER_MON1_NAME} y {B_PLAYER_MON2_NAME}!");
static const u8 sText_GoPkmn2[] = _("¡Adelante, {B_BUFF1}!");
static const u8 sText_DoItPkmn[] = _("¡Te toca, {B_BUFF1}!");
static const u8 sText_GoForItPkmn[] = _("¡Vamos, {B_BUFF1}!");
static const u8 sText_JustALittleMorePkmn[] = _("¡Solo un poco más! ¡Aguanta, {B_BUFF1}!"); //currently unused, will require code changes
static const u8 sText_YourFoesWeakGetEmPkmn[] = _("¡Tu rival está débil! ¡A por él, {B_BUFF1}!");
static const u8 sText_LinkPartnerSentOutPkmnGoPkmn[] = _("¡{B_LINK_PARTNER_NAME} sacó a {B_LINK_PLAYER_MON2_NAME}! ¡Adelante, {B_LINK_PLAYER_MON1_NAME}!");
static const u8 sText_PkmnSwitchOut[] = _("¡{B_BUFF1}, retírate! ¡Vuelve!"); //currently unused, I believe its used for when you switch on a pokemon in shift mode
static const u8 sText_PkmnThatsEnough[] = _("¡{B_BUFF1}, basta! ¡Vuelve!");
static const u8 sText_PkmnComeBack[] = _("¡{B_BUFF1}, vuelve!");
static const u8 sText_PkmnOkComeBack[] = _("¡Bien, {B_BUFF1}! ¡Vuelve!");
static const u8 sText_PkmnGoodComeBack[] = _("¡Buen trabajo, {B_BUFF1}! ¡Vuelve!");
static const u8 sText_Trainer1WithdrewPkmn[] = _("¡{B_TRAINER1_NAME_WITH_CLASS} retiró a {B_BUFF1}!");
static const u8 sText_Trainer2WithdrewPkmn[] = _("¡{B_TRAINER2_NAME_WITH_CLASS} retiró a {B_BUFF1}!");
static const u8 sText_LinkTrainer1WithdrewPkmn[] = _("¡{B_LINK_OPPONENT1_NAME} retiró a {B_BUFF1}!");
static const u8 sText_LinkTrainer2WithdrewPkmn[] = _("¡{B_LINK_SCR_TRAINER_NAME} retiró a {B_BUFF1}!");
static const u8 sText_WildPkmnPrefix[] = _("El salvaje ");
static const u8 sText_FoePkmnPrefix[] = _("El rival ");
static const u8 sText_WildPkmnPrefixLower[] = _("el salvaje ");
static const u8 sText_FoePkmnPrefixLower[] = _("el rival ");
static const u8 sText_EmptyString8[] = _("");
static const u8 sText_FoePkmnPrefix2[] = _("Rival");
static const u8 sText_AllyPkmnPrefix[] = _("Aliado");
static const u8 sText_FoePkmnPrefix3[] = _("Rival");
static const u8 sText_AllyPkmnPrefix2[] = _("Aliado");
static const u8 sText_FoePkmnPrefix4[] = _("Rival");
static const u8 sText_AllyPkmnPrefix3[] = _("Aliado");
static const u8 sText_AttackerUsedX[] = _("{B_ATK_NAME_WITH_PREFIX} usó {B_BUFF3}!");
static const u8 sText_ExclamationMark[] = _("!");
static const u8 sText_ExclamationMark2[] = _("!");
static const u8 sText_ExclamationMark3[] = _("!");
static const u8 sText_ExclamationMark4[] = _("!");
static const u8 sText_ExclamationMark5[] = _("!");
static const u8 sText_HP[] = _("PS");
static const u8 sText_Attack[] = _("Ataque");
static const u8 sText_Defense[] = _("Defensa");
static const u8 sText_Speed[] = _("Velocidad");
static const u8 sText_SpAttack[] = _("At. Esp.");
static const u8 sText_SpDefense[] = _("Def. Esp.");
static const u8 sText_Accuracy[] = _("precisión");
static const u8 sText_Evasiveness[] = _("evasión");

const u8 *const gStatNamesTable[NUM_BATTLE_STATS] =
{
    [STAT_HP]      = sText_HP,
    [STAT_ATK]     = sText_Attack,
    [STAT_DEF]     = sText_Defense,
    [STAT_SPEED]   = sText_Speed,
    [STAT_SPATK]   = sText_SpAttack,
    [STAT_SPDEF]   = sText_SpDefense,
    [STAT_ACC]     = sText_Accuracy,
    [STAT_EVASION] = sText_Evasiveness,
};
const u8 *const gPokeblockWasTooXStringTable[FLAVOR_COUNT] =
{
    [FLAVOR_SPICY]  = COMPOUND_STRING("¡era demasiado picante!"),
    [FLAVOR_DRY]    = COMPOUND_STRING("¡era demasiado seca!"),
    [FLAVOR_SWEET]  = COMPOUND_STRING("¡era demasiado dulce!"),
    [FLAVOR_BITTER] = COMPOUND_STRING("¡era demasiado amarga!"),
    [FLAVOR_SOUR]   = COMPOUND_STRING("¡era demasiado ácida!"),
};

static const u8 sText_Someones[] = _("de alguien");
static const u8 sText_Lanettes[] = _("de LANETTE"); //no decapitalize until it is everywhere
static const u8 sText_EnigmaBerry[] = _("BAYA ENIGMA"); //no decapitalize until it is everywhere
static const u8 sText_BerrySuffix[] = _(" BAYA"); //no decapitalize until it is everywhere
const u8 gText_EmptyString3[] = _("");

static const u8 sText_TwoInGameTrainersDefeated[] = _("¡Has vencido a {B_TRAINER1_NAME_WITH_CLASS} y {B_TRAINER2_NAME_WITH_CLASS}!\p");

// New battle strings.
const u8 gText_drastically[] = _("drásticamente ");
const u8 gText_severely[] = _("severamente ");
static const u8 sText_TerrainReturnedToNormal[] = _("¡El terreno volvió a la normalidad!"); // Unused

const u8 *const gBattleStringsTable[STRINGID_COUNT] =
{
    [STRINGID_TRAINER1LOSETEXT]                     = COMPOUND_STRING("{B_TRAINER1_LOSE_TEXT}"),
    [STRINGID_PKMNGAINEDEXP]                        = COMPOUND_STRING("{B_BUFF1} ganó{B_BUFF2} {B_BUFF3} Puntos Exp.!\p"),
    [STRINGID_PKMNGREWTOLV]                         = COMPOUND_STRING("{B_BUFF1} subió al Nv. {B_BUFF2}!{WAIT_SE}\p"),
    [STRINGID_PKMNLEARNEDMOVE]                      = COMPOUND_STRING("{B_BUFF1} aprendió {B_BUFF2}!{WAIT_SE}\p"),
    [STRINGID_TRYTOLEARNMOVE1]                      = COMPOUND_STRING("{B_BUFF1} quiere aprender {B_BUFF2}.\p"),
    [STRINGID_TRYTOLEARNMOVE2]                      = COMPOUND_STRING("Pero {B_BUFF1} ya conoce cuatro movimientos.\p"),
    [STRINGID_TRYTOLEARNMOVE3]                      = COMPOUND_STRING("¿Olvidar otro movimiento y aprender {B_BUFF2}?"),
    [STRINGID_PKMNFORGOTMOVE]                       = COMPOUND_STRING("{B_BUFF1} olvidó {B_BUFF2}…\p"),
    [STRINGID_STOPLEARNINGMOVE]                     = COMPOUND_STRING("{PAUSE 32}¿Quieres dejar de aprender {B_BUFF2}?"),
    [STRINGID_DIDNOTLEARNMOVE]                      = COMPOUND_STRING("{B_BUFF1} no aprendió {B_BUFF2}.\p"),
    [STRINGID_PKMNLEARNEDMOVE2]                     = COMPOUND_STRING("{B_ATK_NAME_WITH_PREFIX} aprendió {B_BUFF1}!"),
    [STRINGID_ATTACKMISSED]                         = COMPOUND_STRING("¡El ataque de {B_ATK_NAME_WITH_PREFIX} falló!"),
    [STRINGID_PKMNPROTECTEDITSELF]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se protegió!"),
    [STRINGID_STATSWONTINCREASE2]                   = COMPOUND_STRING("¡Las características de {B_ATK_NAME_WITH_PREFIX} no pueden subir más!"),
    [STRINGID_AVOIDEDDAMAGE]                        = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} evitó el daño con {B_DEF_ABILITY}!"), //not in gen 5+, ability popup
    [STRINGID_ITDOESNTAFFECT]                       = COMPOUND_STRING("No afecta a {B_DEF_NAME_WITH_PREFIX2}…"),
    [STRINGID_BATTLERFAINTED]                       = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} se debilitó!\p"),
    [STRINGID_PLAYERGOTMONEY]                       = COMPOUND_STRING("¡Has ganado ¥{B_BUFF1}!\p"),
    [STRINGID_PLAYERWHITEOUT]                       = COMPOUND_STRING("¡No te quedan POKéMON para luchar!\p"),
    [STRINGID_PLAYERWHITEOUT2_WILD]                 = COMPOUND_STRING("Entraste en pánico y dejaste caer ¥{B_BUFF1}…"),
    [STRINGID_PLAYERWHITEOUT2_TRAINER]              = COMPOUND_STRING("Diste ¥{B_BUFF1} al ganador…"),
    [STRINGID_PLAYERWHITEOUT3]                      = COMPOUND_STRING("¡Te ha podido la derrota!"),
    [STRINGID_PREVENTSESCAPE]                       = COMPOUND_STRING("{B_SCR_NAME_WITH_PREFIX} impide escapar con {B_SCR_ABILITY}!\p"),
    [STRINGID_HITXTIMES]                            = COMPOUND_STRING("¡Se golpeó {B_BUFF1} vez/veces!"), //SV has dynamic plural here
    [STRINGID_PKMNFELLASLEEP]                       = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} se durmió!"),
    [STRINGID_PKMNMADESLEEP]                        = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} con {B_BUFF1} durmió a {B_EFF_NAME_WITH_PREFIX2}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNALREADYASLEEP]                    = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} ya está dormido!"),
    [STRINGID_PKMNALREADYASLEEP2]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} ya está dormido!"),
    [STRINGID_PKMNWASPOISONED]                      = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} fue envenenado!"),
    [STRINGID_PKMNPOISONEDBY]                       = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} fue envenenado por {B_BUFF1} de {B_SCR_NAME_WITH_PREFIX2}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNHURTBYPOISON]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre por el veneno!"),
    [STRINGID_PKMNALREADYPOISONED]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} ya está envenenado!"),
    [STRINGID_PKMNBADLYPOISONED]                    = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} fue gravemente envenenado!"),
    [STRINGID_PKMNENERGYDRAINED]                    = COMPOUND_STRING("¡Le drenaron la energía a {B_DEF_NAME_WITH_PREFIX}!"),
    [STRINGID_PKMNWASBURNED]                        = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} se quemó!"),
    [STRINGID_PKMNBURNEDBY]                         = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} con {B_BUFF1} quemó a {B_EFF_NAME_WITH_PREFIX2}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNHURTBYBURN]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre por la quemadura!"),
    [STRINGID_PKMNWASFROZEN]                        = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} se congeló!"),
    [STRINGID_PKMNFROZENBY]                         = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} con {B_BUFF1} congeló a {B_EFF_NAME_WITH_PREFIX2}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNISFROZEN]                         = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está congelado!"),
    [STRINGID_PKMNWASDEFROSTED]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se descongeló!"),
    [STRINGID_PKMNWASDEFROSTED2]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se descongeló!"),
    [STRINGID_PKMNWASDEFROSTEDBY]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} derritió el hielo con {B_CURRENT_MOVE}!"),
    [STRINGID_PKMNWASPARALYZED]                     = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} está paralizado! ¡Quizá no pueda moverse!"),
    [STRINGID_PKMNWASPARALYZEDBY]                   = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} con {B_BUFF1} paralizó a {B_EFF_NAME_WITH_PREFIX2}! ¡Quizá no pueda moverse!"), //not in gen 5+, ability popup
    [STRINGID_PKMNISPARALYZED]                      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no pudo moverse por la parálisis!"),
    [STRINGID_PKMNISALREADYPARALYZED]               = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} ya está paralizado!"),
    [STRINGID_PKMNHEALEDPARALYSIS]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se curó de la parálisis!"),
    [STRINGID_STATSWONTINCREASE]                    = COMPOUND_STRING("¡Las {B_BUFF1} de {B_ATK_NAME_WITH_PREFIX} no pueden subir más!"),
    [STRINGID_STATSWONTDECREASE]                    = COMPOUND_STRING("¡Las {B_BUFF1} de {B_DEF_NAME_WITH_PREFIX} no pueden bajar más!"),
    [STRINGID_PKMNISCONFUSED]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está confuso!"),
    [STRINGID_PKMNHEALEDCONFUSION]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se curó de la confusión!"),
    [STRINGID_PKMNWASCONFUSED]                      = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} se confundió!"),
    [STRINGID_PKMNALREADYCONFUSED]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} ya está confuso!"),
    [STRINGID_PKMNFELLINLOVE]                       = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se enamoró!"),
    [STRINGID_PKMNINLOVE]                           = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está enamorado de {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNIMMOBILIZEDBYLOVE]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está paralizado de amor!"),
    [STRINGID_PKMNCHANGEDTYPE]                      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se transformó en el tipo {B_BUFF1}!"),
    [STRINGID_PKMNFLINCHED]                         = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} retrocedió y no pudo moverse!"),
    [STRINGID_PKMNREGAINEDHEALTH]                   = COMPOUND_STRING("Los PS de {B_DEF_NAME_WITH_PREFIX} se restauraron."),
    [STRINGID_PKMNHPFULL]                           = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} tiene los PS completos!"),
    [STRINGID_PKMNRAISEDSPDEF]                      = COMPOUND_STRING("¡Pantalla Luz reforzó al equipo {B_ATK_TEAM2} frente a ataques especiales!"),
    [STRINGID_PKMNRAISEDDEF]                        = COMPOUND_STRING("¡Reflejo reforzó al equipo {B_ATK_TEAM2} frente a ataques físicos!"),
    [STRINGID_PKMNCOVEREDBYVEIL]                    = COMPOUND_STRING("¡El equipo {B_ATK_TEAM1} se cubrió con un velo místico!"),
    [STRINGID_PKMNUSEDSAFEGUARD]                    = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} está protegido por Velo Sagrado!"),
    [STRINGID_PKMNSAFEGUARDEXPIRED]                 = COMPOUND_STRING("¡El equipo {B_ATK_TEAM1} ya no está protegido por Velo Sagrado!"),
    [STRINGID_PKMNWENTTOSLEEP]                      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se durmió!"), //not in gen 5+
    [STRINGID_PKMNSLEPTHEALTHY]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} durmió y restauró sus PS!"),
    [STRINGID_PKMNWHIPPEDWHIRLWIND]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} levantó un remolino!"),
    [STRINGID_PKMNTOOKSUNLIGHT]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} absorbió luz!"),
    [STRINGID_PKMNLOWEREDHEAD]                      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} agachó la cabeza!"),
    [STRINGID_PKMNISGLOWING]                        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se cubrió con una luz intensa!"),
    [STRINGID_PKMNFLEWHIGH]                         = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} voló alto!"),
    [STRINGID_PKMNDUGHOLE]                          = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se metió bajo tierra!"),
    [STRINGID_PKMNSQUEEZEDBYBIND]                   = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} fue atrapado por {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNTRAPPEDINVORTEX]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} quedó atrapado en el remolino!"),
    [STRINGID_PKMNWRAPPEDBY]                        = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} fue envuelto por {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNCLAMPED]                          = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} atrapó a {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNHURTBY]                           = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre por {B_BUFF1}!"),
    [STRINGID_PKMNFREEDFROM]                        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se liberó de {B_BUFF1}!"),
    [STRINGID_PKMNCRASHED]                          = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} siguió y se estrelló!"),
    [STRINGID_PKMNSHROUDEDINMIST]                   = gText_PkmnShroudedInMist,
    [STRINGID_PKMNPROTECTEDBYMIST]                  = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} está protegido por la niebla!"),
    [STRINGID_PKMNGETTINGPUMPED]                    = gText_PkmnGettingPumped,
    [STRINGID_PKMNHITWITHRECOIL]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} recibió daño por el retroceso!"),
    [STRINGID_PKMNPROTECTEDITSELF2]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se protegió!"),
    [STRINGID_PKMNBUFFETEDBYSANDSTORM]              = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre la tormenta de arena!"),
    [STRINGID_PKMNPELTEDBYHAIL]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre el granizo!"),
    [STRINGID_PKMNSEEDED]                           = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} fue sembrado!"),
    [STRINGID_PKMNEVADEDATTACK]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} evitó el ataque!"),
    [STRINGID_PKMNSAPPEDBYLEECHSEED]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre por Drenadoras!"),
    [STRINGID_PKMNFASTASLEEP]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está profundamente dormido!"),
    [STRINGID_PKMNWOKEUP]                           = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se despertó!"),
    [STRINGID_PKMNUPROARKEPTAWAKE]                  = COMPOUND_STRING("¡Pero el alboroto mantuvo despierto a {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNWOKEUPINUPROAR]                   = COMPOUND_STRING("¡El alboroto despertó a {B_EFF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNCAUSEDUPROAR]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} armó alboroto!"),
    [STRINGID_PKMNMAKINGUPROAR]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está armando alboroto!"),
    [STRINGID_PKMNCALMEDDOWN]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se calmó!"),
    [STRINGID_PKMNCANTSLEEPINUPROAR]                = COMPOUND_STRING("¡Pero {B_DEF_NAME_WITH_PREFIX2} no puede dormir con alboroto!"),
    [STRINGID_PKMNSTOCKPILED]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} acumuló {B_BUFF1}!"),
    [STRINGID_PKMNCANTSLEEPINUPROAR2]               = COMPOUND_STRING("¡Pero {B_DEF_NAME_WITH_PREFIX2} no puede dormir con alboroto!"),
    [STRINGID_UPROARKEPTPKMNAWAKE]                  = COMPOUND_STRING("¡Pero el alboroto mantuvo despierto a {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNSTAYEDAWAKEUSING]                 = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se mantuvo despierto gracias a {B_DEF_ABILITY}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNSTORINGENERGY]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está acumulando energía!"),
    [STRINGID_PKMNUNLEASHEDENERGY]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} liberó su energía!"),
    [STRINGID_PKMNFATIGUECONFUSION]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se confundió por el cansancio!"),
    [STRINGID_PLAYERPICKEDUPMONEY]                  = COMPOUND_STRING("¡Recogiste ¥{B_BUFF1}!\p"),
    [STRINGID_PKMNUNAFFECTED]                       = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} no se ve afectado!"),
    [STRINGID_PKMNTRANSFORMEDINTO]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se transformó en {B_BUFF1}!"),
    [STRINGID_PKMNMADESUBSTITUTE]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} creó un sustituto!"),
    [STRINGID_PKMNHASSUBSTITUTE]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} ya tiene un sustituto!"),
    [STRINGID_SUBSTITUTEDAMAGED]                    = COMPOUND_STRING("¡El sustituto recibió daño por {B_DEF_NAME_WITH_PREFIX2}!\p"),
    [STRINGID_PKMNSUBSTITUTEFADED]                  = COMPOUND_STRING("¡El sustituto de {B_DEF_NAME_WITH_PREFIX} se desvaneció!\p"),
    [STRINGID_PKMNMUSTRECHARGE]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} debe recargarse!"),
    [STRINGID_PKMNRAGEBUILDING]                     = COMPOUND_STRING("¡La ira de {B_DEF_NAME_WITH_PREFIX} aumenta!"),
    [STRINGID_PKMNMOVEWASDISABLED]                  = COMPOUND_STRING("¡El {B_BUFF1} de {B_DEF_NAME_WITH_PREFIX} fue deshabilitado!"),
    [STRINGID_PKMNMOVEISDISABLED]                   = COMPOUND_STRING("¡{B_CURRENT_MOVE} de {B_ATK_NAME_WITH_PREFIX} está deshabilitado!\p"),
    [STRINGID_PKMNMOVEDISABLEDNOMORE]               = COMPOUND_STRING("¡El movimiento de {B_SCR_NAME_WITH_PREFIX} ya no está deshabilitado!"),
    [STRINGID_PKMNGOTENCORE]                        = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} quedó bajo Repetición!"),
    [STRINGID_PKMNGOTENCOREDMOVE]                   = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} solo puede usar {B_CURRENT_MOVE}!\p"),
    [STRINGID_PKMNENCOREENDED]                      = COMPOUND_STRING("¡El efecto de Repetición de {B_SCR_NAME_WITH_PREFIX} terminó!"),
    [STRINGID_PKMNTOOKAIM]                          = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} apuntó a {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNSKETCHEDMOVE]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} calcó {B_BUFF1}!"),
    [STRINGID_PKMNTRYINGTOTAKEFOE]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} quiere llevarse a su atacante consigo!"),
    [STRINGID_PKMNTOOKFOE]                          = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se llevó a su atacante consigo!"),
    [STRINGID_PKMNREDUCEDPP]                        = COMPOUND_STRING("¡Los PP de {B_DEF_NAME_WITH_PREFIX} bajaron!"),
    [STRINGID_PKMNSTOLEITEM]                        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} robó el {B_LAST_ITEM} de {B_EFF_NAME_WITH_PREFIX2}!"),
    [STRINGID_TARGETCANTESCAPENOW]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} ya no puede escapar!"),
    [STRINGID_PKMNFELLINTONIGHTMARE]                = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} empezó a tener una pesadilla!"),
    [STRINGID_PKMNLOCKEDINNIGHTMARE]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está atrapado en una pesadilla!"),
    [STRINGID_PKMNLAIDCURSE]                        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} recortó sus PS y lanzó una maldición a {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNAFFLICTEDBYCURSE]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre la maldición!"),
    [STRINGID_SPIKESSCATTERED]                      = COMPOUND_STRING("¡Se dispersaron Púas alrededor del equipo {B_DEF_TEAM2}!"),
    [STRINGID_PKMNHURTBYSPIKES]                     = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} sufrió daño por las púas!"),
    [STRINGID_PKMNIDENTIFIED]                       = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} fue identificado!"),
    [STRINGID_PKMNPERISHCOUNTFELL]                  = COMPOUND_STRING("¡La cuenta de Perish de {B_ATK_NAME_WITH_PREFIX} bajó a {B_BUFF1}!"),
    [STRINGID_PKMNBRACEDITSELF]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se preparó!"),
    [STRINGID_PKMNENDUREDHIT]                       = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} resistió el golpe!"),
    [STRINGID_MAGNITUDESTRENGTH]                    = COMPOUND_STRING("¡Magnitud {B_BUFF1}!"),
    [STRINGID_PKMNCUTHPMAXEDATTACK]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} recortó sus PS y maximizó su Ataque!"),
    [STRINGID_PKMNCOPIEDSTATCHANGES]                = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} copió los cambios de características de {B_EFF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNGOTFREE]                          = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se liberó de {B_BUFF1} de {B_DEF_NAME_WITH_PREFIX2}!"), //not in gen 5+, generic rapid spin?
    [STRINGID_PKMNSHEDLEECHSEED]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se deshizo de Drenadoras!"), //not in gen 5+, generic rapid spin?
    [STRINGID_PKMNBLEWAWAYSPIKES]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} dispersó Púas!"), //not in gen 5+, generic rapid spin?
    [STRINGID_PKMNFLEDFROMBATTLE]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} huyó del combate!"),
    [STRINGID_PKMNFORESAWATTACK]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} previó un ataque!"),
    [STRINGID_PKMNTOOKATTACK]                       = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} recibió el ataque {B_BUFF1}!"),
    [STRINGID_PKMNATTACK]                           = COMPOUND_STRING("¡Ataque de {B_BUFF1}!"), //not in gen 5+
    [STRINGID_PKMNCENTERATTENTION]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se convirtió en el centro de atención!"),
    [STRINGID_PKMNCHARGINGPOWER]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} empezó a cargar energía!"),
    [STRINGID_NATUREPOWERTURNEDINTO]                = COMPOUND_STRING("¡Adaptación se convirtió en {B_CURRENT_MOVE}!"),
    [STRINGID_PKMNSTATUSNORMAL]                     = COMPOUND_STRING("¡El estado de {B_ATK_NAME_WITH_PREFIX} volvió a la normalidad!"),
    [STRINGID_PKMNHASNOMOVESLEFT]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no tiene movimientos que pueda usar!\p"),
    [STRINGID_PKMNSUBJECTEDTOTORMENT]               = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} fue torturado!"),
    [STRINGID_PKMNCANTUSEMOVETORMENT]               = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no puede usar el mismo movimiento dos veces seguidas por Tormento!\p"),
    [STRINGID_PKMNTIGHTENINGFOCUS]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está concentrándose!"),
    [STRINGID_PKMNFELLFORTAUNT]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} cayó en la provocación!"),
    [STRINGID_PKMNCANTUSEMOVETAUNT]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no puede usar {B_CURRENT_MOVE} tras la provocación!\p"),
    [STRINGID_PKMNREADYTOHELP]                      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está listo para ayudar a {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNSWITCHEDITEMS]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} intercambió objetos con su objetivo!"),
    [STRINGID_PKMNCOPIEDFOE]                        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} copió la habilidad de {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNWISHCAMETRUE]                     = COMPOUND_STRING("¡Se cumplió el deseo de {B_BUFF1}!"),
    [STRINGID_PKMNPLANTEDROOTS]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} echó raíces!"),
    [STRINGID_PKMNABSORBEDNUTRIENTS]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} absorbió nutrientes con sus raíces!"),
    [STRINGID_PKMNANCHOREDITSELF]                   = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se ancló con sus raíces!"),
    [STRINGID_PKMNWASMADEDROWSY]                    = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se adormeció!"),
    [STRINGID_PKMNKNOCKEDOFF]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} tiró el {B_LAST_ITEM} de {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNSWAPPEDABILITIES]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} intercambió habilidades con su objetivo!"),
    [STRINGID_PKMNSEALEDOPPONENTMOVE]               = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} selló los movimientos que comparte con su objetivo!"),
    [STRINGID_PKMNCANTUSEMOVESEALED]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no puede usar su {B_CURRENT_MOVE} sellado!\p"),
    [STRINGID_PKMNWANTSGRUDGE]                      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} quiere guardar rencor!"),
    [STRINGID_PKMNLOSTPPGRUDGE]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} perdió todos los PP de {B_BUFF1} por el rencor!"),
    [STRINGID_PKMNSHROUDEDITSELF]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se cubrió con Capa Mágica!"),
    [STRINGID_PKMNMOVEBOUNCED]                      = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} devolvió {B_CURRENT_MOVE}!"),
    [STRINGID_PKMNWAITSFORTARGET]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} espera a que el objetivo se mueva!"),
    [STRINGID_PKMNSNATCHEDMOVE]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} robó el movimiento de {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNMADEITRAIN]                       = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} hizo que lloviera!"), //not in gen 5+, ability popup
    [STRINGID_PKMNPROTECTEDBY]                      = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} fue protegido por {B_DEF_ABILITY}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNPREVENTSUSAGE]                    = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} impide que {B_ATK_NAME_WITH_PREFIX2} use {B_CURRENT_MOVE}!"), //I don't see this in SV text
    [STRINGID_PKMNRESTOREDHPUSING]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} recuperó PS con {B_DEF_ABILITY}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNCHANGEDTYPEWITH]                  = COMPOUND_STRING("¡{B_EFF_ABILITY} de {B_EFF_NAME_WITH_PREFIX} lo convirtió en tipo {B_BUFF1}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNPREVENTSROMANCEWITH]              = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} impide enamorarse!"), //not in gen 5+, ability popup
    [STRINGID_PKMNPREVENTSCONFUSIONWITH]            = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} evita la confusión!"), //not in gen 5+, ability popup
    [STRINGID_PKMNRAISEDFIREPOWERWITH]              = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} potenció los movimientos de tipo Fuego!"), //not in gen 5+, ability popup
    [STRINGID_PKMNANCHORSITSELFWITH]                = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se ancló con {B_DEF_ABILITY}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNCUTSATTACKWITH]                   = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} bajó el Ataque de {B_DEF_NAME_WITH_PREFIX2}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNPREVENTSSTATLOSSWITH]             = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} evita la pérdida de características!"), //not in gen 5+, ability popup
    [STRINGID_PKMNHURTSWITH]                        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufrió daño por {B_BUFF1} de {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNTRACED]                           = COMPOUND_STRING("¡Copió {B_BUFF2} de {B_BUFF1}!"),
    [STRINGID_STATSHARPLY]                          = gText_StatSharply,
    [STRINGID_STATROSE]                             = gText_StatRose,
    [STRINGID_STATHARSHLY]                          = COMPOUND_STRING("mucho "),
    [STRINGID_STATFELL]                             = gText_StatFell,
    [STRINGID_ATTACKERSSTATROSE]                    = COMPOUND_STRING("¡La {B_BUFF1} de {B_ATK_NAME_WITH_PREFIX} {B_BUFF2}subió!"),
    [STRINGID_DEFENDERSSTATROSE]                    = gText_DefendersStatRose,
    [STRINGID_ATTACKERSSTATFELL]                    = COMPOUND_STRING("¡Las {B_BUFF1} de {B_ATK_NAME_WITH_PREFIX} {B_BUFF2}on!"),
    [STRINGID_DEFENDERSSTATFELL]                    = COMPOUND_STRING("¡Las {B_BUFF1} de {B_DEF_NAME_WITH_PREFIX} {B_BUFF2}on!"),
    [STRINGID_CRITICALHIT]                          = COMPOUND_STRING("¡Un golpe crítico!"),
    [STRINGID_ONEHITKO]                             = COMPOUND_STRING("¡Es un KO de un golpe!"),
    [STRINGID_123POOF]                              = COMPOUND_STRING("Uno…{PAUSE 10}dos…{PAUSE 10}y…{PAUSE 10}{PAUSE 20}{PLAY_SE SE_BALL_BOUNCE_1}¡tachán!\p"),
    [STRINGID_ANDELLIPSIS]                          = COMPOUND_STRING("Y…\p"),
    [STRINGID_NOTVERYEFFECTIVE]                     = COMPOUND_STRING("No es muy eficaz…"),
    [STRINGID_SUPEREFFECTIVE]                       = COMPOUND_STRING("¡Es muy eficaz!"),
    [STRINGID_GOTAWAYSAFELY]                        = sText_GotAwaySafely,
    [STRINGID_WILDPKMNFLED]                         = COMPOUND_STRING("{PLAY_SE SE_FLEE}¡El {B_BUFF1} salvaje huyó!"),
    [STRINGID_NORUNNINGFROMTRAINERS]                = COMPOUND_STRING("¡No! ¡No puedes huir de un combate contra Entrenador!\p"),
    [STRINGID_CANTESCAPE]                           = COMPOUND_STRING("¡No puedes huir!\p"),
    [STRINGID_DONTLEAVEBIRCH]                       = COMPOUND_STRING("PROF. BIRCH: ¡No me dejes así!\p"), //no decapitalize until it is everywhere
    [STRINGID_BUTNOTHINGHAPPENED]                   = COMPOUND_STRING("¡Pero no ocurrió nada!"),
    [STRINGID_BUTITFAILED]                          = COMPOUND_STRING("¡Pero falló!"),
    [STRINGID_ITHURTCONFUSION]                      = COMPOUND_STRING("¡Se hirió a sí mismo en su confusión!"),
    [STRINGID_STARTEDTORAIN]                        = COMPOUND_STRING("¡Empezó a llover!"),
    [STRINGID_DOWNPOURSTARTED]                      = COMPOUND_STRING("¡Empezó un aguacero!"), // corresponds to DownpourText in pokegold and pokecrystal and is used by Rain Dance in GSC
    [STRINGID_RAINCONTINUES]                        = COMPOUND_STRING("La lluvia continúa."), //not in gen 5+
    [STRINGID_DOWNPOURCONTINUES]                    = COMPOUND_STRING("El aguacero continúa."), // unused
    [STRINGID_RAINSTOPPED]                          = COMPOUND_STRING("La lluvia cesó."),
    [STRINGID_SANDSTORMBREWED]                      = COMPOUND_STRING("¡Se levantó una tormenta de arena!"),
    [STRINGID_SANDSTORMRAGES]                       = COMPOUND_STRING("La tormenta de arena arreció."),
    [STRINGID_SANDSTORMSUBSIDED]                    = COMPOUND_STRING("La tormenta de arena amainó."),
    [STRINGID_SUNLIGHTGOTBRIGHT]                    = COMPOUND_STRING("¡El sol se volvió abrasador!"),
    [STRINGID_SUNLIGHTSTRONG]                       = COMPOUND_STRING("La luz solar es intensa."), //not in gen 5+
    [STRINGID_SUNLIGHTFADED]                        = COMPOUND_STRING("La luz solar se debilitó."),
    [STRINGID_STARTEDHAIL]                          = COMPOUND_STRING("¡Empezó a granizar!"),
    [STRINGID_HAILCONTINUES]                        = COMPOUND_STRING("El granizo cae con fuerza."),
    [STRINGID_HAILSTOPPED]                          = COMPOUND_STRING("Dejó de granizar."),
    [STRINGID_STATCHANGESGONE]                      = COMPOUND_STRING("¡Se eliminaron todos los cambios de características!"),
    [STRINGID_COINSSCATTERED]                       = COMPOUND_STRING("¡Las monedas se dispersaron por todas partes!"),
    [STRINGID_TOOWEAKFORSUBSTITUTE]                 = COMPOUND_STRING("¡Pero no tiene suficientes PS para crear un sustituto!"),
    [STRINGID_SHAREDPAIN]                           = COMPOUND_STRING("¡Los combatientes compartieron su dolor!"),
    [STRINGID_BELLCHIMED]                           = COMPOUND_STRING("¡Sonó una campana!"),
    [STRINGID_FAINTINTHREE]                         = COMPOUND_STRING("¡Todos los Pokémon que oyeron la canción se debilitarán en tres turnos!"),
    [STRINGID_NOPPLEFT]                             = COMPOUND_STRING("¡No quedan PP para este movimiento!\p"), //not in gen 5+
    [STRINGID_BUTNOPPLEFT]                          = COMPOUND_STRING("¡Pero no quedaban PP para el movimiento!"),
    [STRINGID_PLAYERUSEDITEM]                       = COMPOUND_STRING("¡Usaste {B_LAST_ITEM}!"),
    [STRINGID_WALLYUSEDITEM]                        = COMPOUND_STRING("WALLY usó {B_LAST_ITEM}!"), //no decapitalize until it is everywhere
    [STRINGID_TRAINERBLOCKEDBALL]                   = COMPOUND_STRING("¡El ENTRENADOR bloqueó tu Poké Ball!"),
    [STRINGID_DONTBEATHIEF]                         = COMPOUND_STRING("¡No seas ladrón!"),
    [STRINGID_ITDODGEDBALL]                         = COMPOUND_STRING("¡Esquivó tu Poké Ball! ¡Este POKéMON no se puede capturar!"),
    [STRINGID_YOUMISSEDPKMN]                        = COMPOUND_STRING("¡Fallaste al POKéMON!"),
    [STRINGID_PKMNBROKEFREE]                        = COMPOUND_STRING("¡Oh, no! ¡El POKéMON se liberó!"),
    [STRINGID_ITAPPEAREDCAUGHT]                     = COMPOUND_STRING("¡Uy! ¡Parecía que lo ibas a capturar!"),
    [STRINGID_AARGHALMOSTHADIT]                     = COMPOUND_STRING("¡Argh! ¡Casi lo tenía!"),
    [STRINGID_SHOOTSOCLOSE]                         = COMPOUND_STRING("¡Uf! ¡Casi lo consigues!"),
    [STRINGID_GOTCHAPKMNCAUGHTPLAYER]               = COMPOUND_STRING("¡Lo tienes! ¡{B_DEF_NAME} fue capturado!{WAIT_SE}{PLAY_BGM MUS_CAUGHT}\p"),
    [STRINGID_GOTCHAPKMNCAUGHTWALLY]                = COMPOUND_STRING("¡Lo tienes! ¡{B_DEF_NAME} fue capturado!{WAIT_SE}{PLAY_BGM MUS_CAUGHT}{PAUSE 127}"),
    [STRINGID_GIVENICKNAMECAPTURED]                 = COMPOUND_STRING("¿Quieres dar un mote a {B_DEF_NAME}?"),
    [STRINGID_PKMNSENTTOPC]                         = COMPOUND_STRING("¡{B_DEF_NAME} se envió al PC de {B_PC_CREATOR_NAME}!"), //Still used lanette's pc since terminology is different
    [STRINGID_PKMNDATAADDEDTODEX]                   = COMPOUND_STRING("¡Los datos de {B_DEF_NAME} se añadieron a la Pokédex!\p"),
    [STRINGID_ITISRAINING]                          = COMPOUND_STRING("¡Está lloviendo!"),
    [STRINGID_SANDSTORMISRAGING]                    = COMPOUND_STRING("¡La tormenta de arena arrecia!"),
    [STRINGID_CANTESCAPE2]                          = COMPOUND_STRING("¡No pudiste escapar!\p"),
    [STRINGID_PKMNIGNORESASLEEP]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} ignoró órdenes y siguió durmiendo!"),
    [STRINGID_PKMNIGNOREDORDERS]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} ignoró órdenes!"),
    [STRINGID_PKMNBEGANTONAP]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se puso a echar una siesta!"),
    [STRINGID_PKMNLOAFING]                          = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está holgazaneando!"),
    [STRINGID_PKMNWONTOBEY]                         = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no obedece!"),
    [STRINGID_PKMNTURNEDAWAY]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se dio la vuelta!"),
    [STRINGID_PKMNPRETENDNOTNOTICE]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} fingió no darse cuenta!"),
    [STRINGID_ENEMYABOUTTOSWITCHPKMN]               = COMPOUND_STRING("{B_TRAINER1_NAME_WITH_CLASS} va a sacar a {B_BUFF2}.\p¿Quieres cambiar de POKéMON?"),
    [STRINGID_CREPTCLOSER]                          = COMPOUND_STRING("¡{B_PLAYER_NAME} se acercó sigilosamente a {B_OPPONENT_MON1_NAME}!"), //safari
    [STRINGID_CANTGETCLOSER]                        = COMPOUND_STRING("¡{B_PLAYER_NAME} no puede acercarse más!"), //safari
    [STRINGID_PKMNWATCHINGCAREFULLY]                = COMPOUND_STRING("¡{B_OPPONENT_MON1_NAME} observa con atención!"), //safari
    [STRINGID_PKMNCURIOUSABOUTX]                    = COMPOUND_STRING("¡{B_OPPONENT_MON1_NAME} siente curiosidad por {B_BUFF1}!"), //safari
    [STRINGID_PKMNENTHRALLEDBYX]                    = COMPOUND_STRING("¡{B_OPPONENT_MON1_NAME} está fascinado por {B_BUFF1}!"), //safari
    [STRINGID_PKMNIGNOREDX]                         = COMPOUND_STRING("¡{B_OPPONENT_MON1_NAME} ignoró por completo el {B_BUFF1}!"), //safari
    [STRINGID_THREWPOKEBLOCKATPKMN]                 = COMPOUND_STRING("¡{B_PLAYER_NAME} lanzó un {POKEBLOCK} al {B_OPPONENT_MON1_NAME}!"), //safari
    [STRINGID_OUTOFSAFARIBALLS]                     = COMPOUND_STRING("{PLAY_SE SE_DING_DONG}LOCUTOR: ¡No te quedan Safari Balls! ¡Fin del juego!\p"), //safari
    [STRINGID_PKMNSITEMCUREDPARALYSIS]              = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_SCR_NAME_WITH_PREFIX} curó la parálisis!"),
    [STRINGID_PKMNSITEMCUREDPOISON]                 = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_SCR_NAME_WITH_PREFIX} curó el veneno!"),
    [STRINGID_PKMNSITEMHEALEDBURN]                  = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_SCR_NAME_WITH_PREFIX} curó la quemadura!"),
    [STRINGID_PKMNSITEMDEFROSTEDIT]                 = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_SCR_NAME_WITH_PREFIX} lo descongeló!"),
    [STRINGID_PKMNSITEMWOKEIT]                      = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_SCR_NAME_WITH_PREFIX} lo despertó!"),
    [STRINGID_PKMNSITEMSNAPPEDOUT]                  = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_SCR_NAME_WITH_PREFIX} lo curó de la confusión!"),
    [STRINGID_PKMNSITEMCUREDPROBLEM]                = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_SCR_NAME_WITH_PREFIX} curó su problema de {B_BUFF1}!"),
    [STRINGID_PKMNSITEMRESTOREDHEALTH]              = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} recuperó PS con {B_LAST_ITEM}!"),
    [STRINGID_PKMNSITEMRESTOREDPP]                  = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} recuperó PP de {B_BUFF1} con {B_LAST_ITEM}!"),
    [STRINGID_PKMNSITEMRESTOREDSTATUS]              = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} normalizó sus características con {B_LAST_ITEM}!"),
    [STRINGID_PKMNSITEMRESTOREDHPALITTLE]           = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} recuperó un poco de PS con {B_LAST_ITEM}!"),
    [STRINGID_ITEMALLOWSONLYYMOVE]                  = COMPOUND_STRING("¡{B_LAST_ITEM} solo permite usar {B_CURRENT_MOVE}!\p"),
    [STRINGID_PKMNHUNGONWITHX]                      = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} resistió gracias a {B_LAST_ITEM}!"),
    [STRINGID_EMPTYSTRING3]                         = gText_EmptyString3,
    [STRINGID_PKMNSXBLOCKSY]                        = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} bloquea {B_CURRENT_MOVE}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNSXRESTOREDHPALITTLE2]             = COMPOUND_STRING("¡{B_ATK_ABILITY} de {B_ATK_NAME_WITH_PREFIX} recuperó un poco de PS!"), //not in gen 5+, ability popup
    [STRINGID_PKMNSXWHIPPEDUPSANDSTORM]             = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} levantó una tormenta de arena!"), //not in gen 5+, ability popup
    [STRINGID_PKMNSXPREVENTSYLOSS]                  = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} evita la pérdida de {B_BUFF1}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNSXINFATUATEDY]                    = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} enamoró a {B_ATK_NAME_WITH_PREFIX2}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNSXMADEYINEFFECTIVE]               = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} hizo ineficaz {B_CURRENT_MOVE}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNSXCUREDYPROBLEM]                  = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} curó su problema de {B_BUFF1}!"), //not in gen 5+, ability popup
    [STRINGID_ITSUCKEDLIQUIDOOZE]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} absorbió el lodo líquido!"),
    [STRINGID_PKMNTRANSFORMED]                      = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} se transformó!"),
    [STRINGID_ELECTRICITYWEAKENED]                  = COMPOUND_STRING("¡El poder de la Electricidad se debilitó!"),
    [STRINGID_FIREWEAKENED]                         = COMPOUND_STRING("¡El poder del Fuego se debilitó!"),
    [STRINGID_PKMNHIDUNDERWATER]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se ocultó bajo el agua!"),
    [STRINGID_PKMNSPRANGUP]                         = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} saltó!"),
    [STRINGID_HMMOVESCANTBEFORGOTTEN]               = COMPOUND_STRING("Las MO no pueden olvidarse ahora.\p"),
    [STRINGID_XFOUNDONEY]                           = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} encontró un {B_LAST_ITEM}!"),
    [STRINGID_PLAYERDEFEATEDTRAINER1]               = sText_PlayerDefeatedLinkTrainerTrainer1,
    [STRINGID_SOOTHINGAROMA]                        = COMPOUND_STRING("¡Un aroma relajante envolvió la zona!"),
    [STRINGID_ITEMSCANTBEUSEDNOW]                   = COMPOUND_STRING("No se pueden usar objetos ahora.{PAUSE 64}"), //not in gen 5+, i think
    [STRINGID_USINGITEMSTATOFPKMNROSE]              = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX2} {B_BUFF2}subió su {B_BUFF1} con {B_LAST_ITEM}!"), //todo: update this, will require code changes
    [STRINGID_USINGITEMSTATOFPKMNFELL]              = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX2} {B_BUFF2}ó su {B_BUFF1} usando {B_LAST_ITEM}!"),
    [STRINGID_PKMNUSEDXTOGETPUMPED]                 = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} usó {B_LAST_ITEM} para venirse arriba!"),
    [STRINGID_PKMNSXMADEYUSELESS]                   = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} inutilizó {B_CURRENT_MOVE}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNTRAPPEDBYSANDTOMB]                = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} quedó atrapado por las arenas movedizas!"),
    [STRINGID_EMPTYSTRING4]                         = COMPOUND_STRING(""),
    [STRINGID_ABOOSTED]                             = COMPOUND_STRING(" un aumento de"),
    [STRINGID_PKMNSXINTENSIFIEDSUN]                 = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} intensificó los rayos del sol!"), //not in gen 5+, ability popup
    [STRINGID_PKMNMAKESGROUNDMISS]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} hace fallar los movimientos de tipo Tierra con {B_DEF_ABILITY}!"), //not in gen 5+, ability popup
    [STRINGID_YOUTHROWABALLNOWRIGHT]                = COMPOUND_STRING("¿Lanzarás una Ball ahora? Yo… ¡haré lo que pueda!"),
    [STRINGID_PKMNSXTOOKATTACK]                     = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} absorbió el ataque!"), //In gen 5+ but without naming the ability
    [STRINGID_PKMNCHOSEXASDESTINY]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} eligió Deseo Oculto como destino!"),
    [STRINGID_PKMNLOSTFOCUS]                        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} perdió la concentración y no pudo moverse!"),
    [STRINGID_USENEXTPKMN]                          = COMPOUND_STRING("¿Usar el siguiente Pokémon?"),
    [STRINGID_PKMNFLEDUSINGITS]                     = COMPOUND_STRING("{PLAY_SE SE_FLEE}¡{B_ATK_NAME_WITH_PREFIX} huyó usando {B_LAST_ITEM}!\p"),
    [STRINGID_PKMNFLEDUSING]                        = COMPOUND_STRING("{PLAY_SE SE_FLEE}¡{B_ATK_NAME_WITH_PREFIX} huyó usando {B_ATK_ABILITY}!\p"), //not in gen 5+
    [STRINGID_PKMNWASDRAGGEDOUT]                    = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} fue sacado!\p"),
    [STRINGID_PKMNSITEMNORMALIZEDSTATUS]            = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_SCR_NAME_WITH_PREFIX} normalizó su estado!"),
    [STRINGID_TRAINER1USEDITEM]                     = COMPOUND_STRING("¡{B_ATK_TRAINER_NAME_WITH_CLASS} usó {B_LAST_ITEM}!"),
    [STRINGID_BOXISFULL]                            = COMPOUND_STRING("¡La CAJA está llena! ¡No puedes capturar más!\p"),
    [STRINGID_PKMNAVOIDEDATTACK]                    = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} evitó el ataque!"),
    [STRINGID_PKMNSXMADEITINEFFECTIVE]              = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} lo hizo ineficaz!"), //not in gen 5+, ability popup
    [STRINGID_PKMNSXPREVENTSFLINCHING]              = COMPOUND_STRING("¡{B_EFF_ABILITY} de {B_EFF_NAME_WITH_PREFIX} evita el retroceso!"), //not in gen 5+, ability popup
    [STRINGID_PKMNALREADYHASBURN]                   = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} ya está quemado!"),
    [STRINGID_STATSWONTDECREASE2]                   = COMPOUND_STRING("¡Las características de {B_DEF_NAME_WITH_PREFIX} no pueden bajar más!"),
    [STRINGID_PKMNSXBLOCKSY2]                       = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} bloquea {B_CURRENT_MOVE}!"), //not in gen 5+, ability popup
    [STRINGID_PKMNSXWOREOFF]                        = COMPOUND_STRING("¡El efecto de {B_BUFF1} del equipo {B_ATK_TEAM1} se pasó!"),
    [STRINGID_THEWALLSHATTERED]                     = COMPOUND_STRING("¡La barrera se rompió!"), //not in gen5+, uses "your teams light screen wore off!" etc instead
    [STRINGID_PKMNSXCUREDITSYPROBLEM]               = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} curó su problema de {B_BUFF1}!"), //not in gen 5+, ability popup
    [STRINGID_ATTACKERCANTESCAPE]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no puede escapar!"),
    [STRINGID_PKMNOBTAINEDX]                        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} obtuvo {B_BUFF1}!"),
    [STRINGID_PKMNOBTAINEDX2]                       = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} obtuvo {B_BUFF2}!"),
    [STRINGID_PKMNOBTAINEDXYOBTAINEDZ]              = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} obtuvo {B_BUFF1}!\p¡{B_DEF_NAME_WITH_PREFIX} obtuvo {B_BUFF2}!"),
    [STRINGID_BUTNOEFFECT]                          = COMPOUND_STRING("¡Pero no tuvo efecto!"),
    [STRINGID_TWOENEMIESDEFEATED]                   = sText_TwoInGameTrainersDefeated,
    [STRINGID_TRAINER2LOSETEXT]                     = COMPOUND_STRING("{B_TRAINER2_LOSE_TEXT}"),
    [STRINGID_PKMNINCAPABLEOFPOWER]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} parece incapaz de usar su poder!"),
    [STRINGID_GLINTAPPEARSINEYE]                    = COMPOUND_STRING("¡Un brillo aparece en los ojos de {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNGETTINGINTOPOSITION]              = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} se está preparando!"),
    [STRINGID_PKMNBEGANGROWLINGDEEPLY]              = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} empezó a rugir profundamente!"),
    [STRINGID_PKMNEAGERFORMORE]                     = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} quiere más!"),
    [STRINGID_DEFEATEDOPPONENTBYREFEREE]            = COMPOUND_STRING("{B_PLAYER_MON1_NAME} venció al rival {B_OPPONENT_MON1_NAME} por decisión del ÁRBITRO."),
    [STRINGID_LOSTTOOPPONENTBYREFEREE]              = COMPOUND_STRING("{B_PLAYER_MON1_NAME} perdió contra {B_OPPONENT_MON1_NAME} por decisión del ÁRBITRO."),
    [STRINGID_TIEDOPPONENTBYREFEREE]                = COMPOUND_STRING("{B_PLAYER_MON1_NAME} empató con {B_OPPONENT_MON1_NAME} por decisión del ÁRBITRO."),
    [STRINGID_QUESTIONFORFEITMATCH]                 = COMPOUND_STRING("¿Quieres abandonar el combate y salir ahora?"),
    [STRINGID_FORFEITEDMATCH]                       = COMPOUND_STRING("El combate fue abandonado."),
    [STRINGID_PKMNTRANSFERREDSOMEONESPC]            = gText_PkmnTransferredSomeonesPC,
    [STRINGID_PKMNTRANSFERREDLANETTESPC]            = gText_PkmnTransferredLanettesPC,
    [STRINGID_PKMNBOXSOMEONESPCFULL]                = gText_PkmnTransferredSomeonesPCBoxFull,
    [STRINGID_PKMNBOXLANETTESPCFULL]                = gText_PkmnTransferredLanettesPCBoxFull,
    [STRINGID_TRAINER1WINTEXT]                      = COMPOUND_STRING("{B_TRAINER1_WIN_TEXT}"),
    [STRINGID_TRAINER2WINTEXT]                      = COMPOUND_STRING("{B_TRAINER2_WIN_TEXT}"),
    [STRINGID_ENDUREDSTURDY]                        = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} resistió el golpe gracias a {B_DEF_ABILITY}!"),
    [STRINGID_POWERHERB]                            = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} quedó totalmente cargado por {B_LAST_ITEM}!"),
    [STRINGID_HURTBYITEM]                           = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufrió daño por {B_LAST_ITEM}!"),
    [STRINGID_PSNBYITEM]                            = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} fue gravemente envenenado por {B_LAST_ITEM}!"),
    [STRINGID_BRNBYITEM]                            = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} fue quemado por {B_LAST_ITEM}!"),
    [STRINGID_DEFABILITYIN]                         = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} se activa!"),
    [STRINGID_GRAVITYINTENSIFIED]                   = COMPOUND_STRING("¡La gravedad se intensificó!"),
    [STRINGID_TARGETIDENTIFIED]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} fue identificado!"),
    [STRINGID_TARGETWOKEUP]                         = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se despertó!"),
    [STRINGID_PKMNSTOLEANDATEITEM]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} robó y se comió el {B_LAST_ITEM} de su objetivo!"),
    [STRINGID_TAILWINDBLEW]                         = COMPOUND_STRING("¡Viento Afín sopló detrás del equipo {B_ATK_TEAM2}!"),
    [STRINGID_PKMNWENTBACK]                         = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} volvió con {B_ATK_TRAINER_NAME}!"),
    [STRINGID_PKMNCANTUSEITEMSANYMORE]              = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} ya no puede usar objetos!"),
    [STRINGID_PKMNFLUNG]                            = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} lanzó su {B_LAST_ITEM}!"),
    [STRINGID_PKMNPREVENTEDFROMHEALING]             = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} no puede curarse!"),
    [STRINGID_PKMNSWITCHEDATKANDDEF]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} intercambió su Ataque y su Defensa!"),
    [STRINGID_PKMNSABILITYSUPPRESSED]               = COMPOUND_STRING("¡La habilidad de {B_DEF_NAME_WITH_PREFIX} fue anulada!"),
    [STRINGID_SHIELDEDFROMCRITICALHITS]             = COMPOUND_STRING("¡Conjuro protegió al equipo {B_ATK_TEAM2} de los golpes críticos!"),
    [STRINGID_SWITCHEDATKANDSPATK]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} intercambió sus cambios de Ataque y At. Esp.\pcon su objetivo!"),
    [STRINGID_SWITCHEDDEFANDSPDEF]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} intercambió sus cambios de Defensa y Def. Esp.\pcon su objetivo!"),
    [STRINGID_PKMNACQUIREDABILITY]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} obtuvo {B_DEF_ABILITY}!"),
    [STRINGID_POISONSPIKESSCATTERED]                = COMPOUND_STRING("¡Se dispersaron Púas Tóxicas alrededor del equipo {B_DEF_TEAM2}!"),
    [STRINGID_PKMNSWITCHEDSTATCHANGES]              = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} intercambió cambios de características con su objetivo!"),
    [STRINGID_PKMNSURROUNDEDWITHVEILOFWATER]        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se cubrió con un velo de agua!"),
    [STRINGID_PKMNLEVITATEDONELECTROMAGNETISM]      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} levitó con electromagnetismo!"),
    [STRINGID_PKMNTWISTEDDIMENSIONS]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} retorció las dimensiones!"),
    [STRINGID_POINTEDSTONESFLOAT]                   = COMPOUND_STRING("¡Piedras afiladas flotan alrededor del equipo {B_DEF_TEAM2}!"),
    [STRINGID_CLOAKEDINMYSTICALMOONLIGHT]           = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se cubrió con un brillo lunar místico!"),
    [STRINGID_TRAPPEDBYSWIRLINGMAGMA]               = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} quedó atrapado por magma arremolinado!"),
    [STRINGID_VANISHEDINSTANTLY]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} desapareció al instante!"),
    [STRINGID_PROTECTEDTEAM]                        = COMPOUND_STRING("¡{B_CURRENT_MOVE} protegió al equipo {B_ATK_TEAM2}!"),
    [STRINGID_SHAREDITSGUARD]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} compartió su guardia con el objetivo!"),
    [STRINGID_SHAREDITSPOWER]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} compartió su poder con el objetivo!"),
    [STRINGID_SWAPSDEFANDSPDEFOFALLPOKEMON]         = COMPOUND_STRING("¡Se creó un área extraña donde Defensa y Def. Esp. se intercambian!"),
    [STRINGID_BECAMENIMBLE]                         = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se volvió más ágil!"),
    [STRINGID_HURLEDINTOTHEAIR]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} fue lanzado por los aires!"),
    [STRINGID_HELDITEMSLOSEEFFECTS]                 = COMPOUND_STRING("¡Se creó un área extraña donde los objetos equipados pierden su efecto!"),
    [STRINGID_FELLSTRAIGHTDOWN]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} cayó en picado!"),
    [STRINGID_TARGETCHANGEDTYPE]                    = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se transformó en el tipo {B_BUFF1}!"),
    [STRINGID_PKMNACQUIREDSIMPLE]                   = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} obtuvo Simple!"), //shouldn't directly use the name
    [STRINGID_KINDOFFER]                            = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} aceptó la amable oferta!"),
    [STRINGID_RESETSTARGETSSTATLEVELS]              = COMPOUND_STRING("¡Los cambios de características de {B_DEF_NAME_WITH_PREFIX} se eliminaron!"),
    [STRINGID_ALLYSWITCHPOSITION]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} y {B_SCR_NAME_WITH_PREFIX2} intercambiaron posiciones!"),
    [STRINGID_RESTORETARGETSHEALTH]                 = COMPOUND_STRING("¡Los PS de {B_DEF_NAME_WITH_PREFIX} se restauraron!"),
    [STRINGID_TOOKPJMNINTOTHESKY]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se llevó a {B_DEF_NAME_WITH_PREFIX2} a los cielos!"),
    [STRINGID_FREEDFROMSKYDROP]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se liberó de Caída Libre!"),
    [STRINGID_POSTPONETARGETMOVE]                   = COMPOUND_STRING("¡El movimiento de {B_DEF_NAME_WITH_PREFIX} se pospuso!"),
    [STRINGID_REFLECTTARGETSTYPE]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se volvió del mismo tipo que {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_TRANSFERHELDITEM]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} recibió {B_LAST_ITEM} de {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_EMBARGOENDS]                          = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} ya puede usar objetos otra vez!"),
    [STRINGID_ELECTROMAGNETISM]                     = COMPOUND_STRING("electromagnetism"),
    [STRINGID_BUFFERENDS]                           = COMPOUND_STRING("¡El efecto de {B_BUFF1} de {B_SCR_NAME_WITH_PREFIX} se pasó!"),
    [STRINGID_TELEKINESISENDS]                      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se liberó de Telequinesis!"),
    [STRINGID_TAILWINDENDS]                         = COMPOUND_STRING("¡Viento Afín del equipo {B_ATK_TEAM1} se disipó!"),
    [STRINGID_LUCKYCHANTENDS]                       = COMPOUND_STRING("¡Conjuro del equipo {B_ATK_TEAM1} se desvaneció!"),
    [STRINGID_TRICKROOMENDS]                        = COMPOUND_STRING("¡Las dimensiones deformadas volvieron a la normalidad!"),
    [STRINGID_WONDERROOMENDS]                       = COMPOUND_STRING("¡Sala Extraña se desvaneció y Defensa y Def. Esp. volvieron a la normalidad!"),
    [STRINGID_MAGICROOMENDS]                        = COMPOUND_STRING("¡Zona Mágica se desvaneció y los efectos de los objetos equipados volvieron a la normalidad!"),
    [STRINGID_MUDSPORTENDS]                         = COMPOUND_STRING("El efecto de Chapoteolodo se desvaneció."),
    [STRINGID_WATERSPORTENDS]                       = COMPOUND_STRING("El efecto de Chapoteagua se desvaneció."),
    [STRINGID_GRAVITYENDS]                          = COMPOUND_STRING("¡La gravedad volvió a la normalidad!"),
    [STRINGID_AQUARINGHEAL]                         = COMPOUND_STRING("¡Un velo de agua restauró los PS de {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_ELECTRICTERRAINENDS]                  = COMPOUND_STRING("La electricidad desapareció del terreno."),
    [STRINGID_MISTYTERRAINENDS]                     = COMPOUND_STRING("La niebla desapareció del terreno."),
    [STRINGID_PSYCHICTERRAINENDS]                   = COMPOUND_STRING("¡La rareza desapareció del terreno!"),
    [STRINGID_GRASSYTERRAINENDS]                    = COMPOUND_STRING("La hierba desapareció del terreno."),
    [STRINGID_TARGETABILITYSTATRAISE]               = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} {B_BUFF2}subió su {B_BUFF1}!"),
    [STRINGID_TARGETSSTATWASMAXEDOUT]               = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} maximizó su {B_BUFF1}!"),
    [STRINGID_ATTACKERABILITYSTATRAISE]             = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} {B_BUFF2}subió su {B_BUFF1}!"),
    [STRINGID_POISONHEALHPUP]                       = COMPOUND_STRING("¡El envenenamiento curó un poco a {B_ATK_NAME_WITH_PREFIX2}!"), //don't think this message is displayed anymore
    [STRINGID_BADDREAMSDMG]                         = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} está atormentado!"),
    [STRINGID_MOLDBREAKERENTERS]                    = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} rompe el molde!"),
    [STRINGID_TERAVOLTENTERS]                       = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} irradia un aura explosiva!"),
    [STRINGID_TURBOBLAZEENTERS]                     = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} irradia un aura ardiente!"),
    [STRINGID_SLOWSTARTENTERS]                      = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} tarda en reaccionar!"),
    [STRINGID_SLOWSTARTEND]                         = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} por fin se puso en marcha!"),
    [STRINGID_SOLARPOWERHPDROP]                     = COMPOUND_STRING("¡{B_ATK_ABILITY} de {B_ATK_NAME_WITH_PREFIX} le pasa factura!"), //don't think this message is displayed anymore
    [STRINGID_AFTERMATHDMG]                         = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} resultó herido!"),
    [STRINGID_ANTICIPATIONACTIVATES]                = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} se estremeció!"),
    [STRINGID_FOREWARNACTIVATES]                    = COMPOUND_STRING("¡{B_SCR_ABILITY} alertó a {B_SCR_NAME_WITH_PREFIX2} sobre {B_BUFF1} de {B_EFF_NAME_WITH_PREFIX2}!"),
    [STRINGID_ICEBODYHPGAIN]                        = COMPOUND_STRING("¡{B_ATK_ABILITY} de {B_ATK_NAME_WITH_PREFIX} recuperó un poco de PS!"), //don't think this message is displayed anymore
    [STRINGID_SNOWWARNINGHAIL]                      = COMPOUND_STRING("¡Empezó a granizar!"),
    [STRINGID_FRISKACTIVATES]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} cacheó a {B_DEF_NAME_WITH_PREFIX2} y encontró su {B_LAST_ITEM}!"),
    [STRINGID_UNNERVEENTERS]                        = COMPOUND_STRING("¡El equipo {B_EFF_TEAM1} está demasiado nervioso para comer BAYAS!"),
    [STRINGID_HARVESTBERRY]                         = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} recuperó su {B_LAST_ITEM}!"),
    [STRINGID_MAGICBOUNCEACTIVATES]                 = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} devolvió {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_PROTEANTYPECHANGE]                    = COMPOUND_STRING("¡{B_ATK_ABILITY} de {B_ATK_NAME_WITH_PREFIX} lo convirtió en tipo {B_BUFF1}!"),
    [STRINGID_SYMBIOSISITEMPASS]                    = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} pasó su {B_LAST_ITEM} a {B_EFF_NAME_WITH_PREFIX2} mediante {B_LAST_ABILITY}!"),
    [STRINGID_STEALTHROCKDMG]                       = COMPOUND_STRING("¡Las piedras afiladas hirieron a {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_TOXICSPIKESABSORBED]                  = COMPOUND_STRING("¡Las Púas Tóxicas desaparecieron alrededor del equipo {B_EFF_TEAM2}!"),
    [STRINGID_TOXICSPIKESPOISONED]                  = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} fue envenenado!"),
    [STRINGID_TOXICSPIKESBADLYPOISONED]             = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} fue gravemente envenenado!"),
    [STRINGID_STICKYWEBSWITCHIN]                    = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} quedó atrapado en una telaraña pegajosa!"),
    [STRINGID_HEALINGWISHCAMETRUE]                  = COMPOUND_STRING("¡El deseo cura se cumplió para {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_HEALINGWISHHEALED]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} recuperó PS!"),
    [STRINGID_LUNARDANCECAMETRUE]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se cubrió con un brillo lunar místico!"),
    [STRINGID_CURSEDBODYDISABLED]                    = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX2} deshabilitó {B_BUFF1} de {B_ATK_NAME_WITH_PREFIX}!"),
    [STRINGID_ATTACKERACQUIREDABILITY]              = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} obtuvo {B_ATK_ABILITY}!"),
    [STRINGID_TARGETABILITYSTATLOWER]               = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX} {B_BUFF2}bajó su {B_BUFF1}!"),
    [STRINGID_TARGETSTATWONTGOHIGHER]               = COMPOUND_STRING("¡Las {B_BUFF1} de {B_DEF_NAME_WITH_PREFIX} no pueden subir más!"),
    [STRINGID_PKMNMOVEBOUNCEDABILITY]               = COMPOUND_STRING("¡{B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX2} devolvió {B_CURRENT_MOVE} de {B_ATK_NAME_WITH_PREFIX}!"),
    [STRINGID_IMPOSTERTRANSFORM]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se transformó en {B_DEF_NAME_WITH_PREFIX2} con {B_LAST_ABILITY}!"),
    [STRINGID_ASSAULTVESTDOESNTALLOW]               = COMPOUND_STRING("¡Los efectos de {B_LAST_ITEM} impiden usar movimientos de estado!\p"),
    [STRINGID_GRAVITYPREVENTSUSAGE]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no puede usar {B_CURRENT_MOVE} por la gravedad!\p"),
    [STRINGID_HEALBLOCKPREVENTSUSAGE]               = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no pudo curarse!\p"),
    [STRINGID_NOTDONEYET]                           = COMPOUND_STRING("¡El efecto del movimiento aún no ha terminado!\p"),
    [STRINGID_STICKYWEBUSED]                        = COMPOUND_STRING("¡Se extendió una telaraña pegajosa alrededor del equipo {B_DEF_TEAM2}!"),
    [STRINGID_QUASHSUCCESS]                         = COMPOUND_STRING("¡El movimiento de {B_DEF_NAME_WITH_PREFIX} se pospuso!"),
    [STRINGID_PKMNBLEWAWAYTOXICSPIKES]              = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} dispersó Púas Tóxicas!"),
    [STRINGID_PKMNBLEWAWAYSTICKYWEB]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} dispersó Telaraña Pegajosa!"),
    [STRINGID_PKMNBLEWAWAYSTEALTHROCK]              = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} dispersó Trampa Rocas!"),
    [STRINGID_IONDELUGEON]                          = COMPOUND_STRING("¡Un aluvión de iones recorre el campo de batalla!"),
    [STRINGID_TOPSYTURVYSWITCHEDSTATS]              = COMPOUND_STRING("¡Se invirtieron todos los cambios de características de {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_TERRAINBECOMESMISTY]                  = COMPOUND_STRING("¡La niebla envolvió el terreno!"),
    [STRINGID_TERRAINBECOMESGRASSY]                 = COMPOUND_STRING("¡La hierba cubrió el terreno!"),
    [STRINGID_TERRAINBECOMESELECTRIC]               = COMPOUND_STRING("¡Una corriente eléctrica recorrió el terreno!"),
    [STRINGID_TERRAINBECOMESPSYCHIC]                = COMPOUND_STRING("¡El campo se volvió extraño!"),
    [STRINGID_TARGETELECTRIFIED]                    = COMPOUND_STRING("¡Los movimientos de {B_DEF_NAME_WITH_PREFIX} se electrificaron!"),
    [STRINGID_MEGAEVOREACTING]                      = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_ATK_NAME_WITH_PREFIX} reacciona al Megaaro de {B_ATK_TRAINER_NAME}!"), //actually displays the type of mega ring in inventory, but we didnt implement them :(
    [STRINGID_MEGAEVOEVOLVED]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} megaevolucionó en Mega {B_BUFF1}!"),
    [STRINGID_DRASTICALLY]                          = gText_drastically,
    [STRINGID_SEVERELY]                             = gText_severely,
    [STRINGID_INFESTATION]                          = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} fue infestado por {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_NOEFFECTONTARGET]                     = COMPOUND_STRING("¡No tendrá ningún efecto en {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_BURSTINGFLAMESHIT]                    = COMPOUND_STRING("¡Las llamas estallaron sobre {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_BESTOWITEMGIVING]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} recibió {B_LAST_ITEM} de {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_THIRDTYPEADDED]                       = COMPOUND_STRING("¡Se añadió el tipo {B_BUFF1} a {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_FELLFORFEINT]                         = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} cayó en la finta!"),
    [STRINGID_POKEMONCANNOTUSEMOVE]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no puede usar {B_CURRENT_MOVE}!"),
    [STRINGID_COVEREDINPOWDER]                      = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} está cubierto de polvo!"),
    [STRINGID_POWDEREXPLODES]                       = COMPOUND_STRING("¡Al tocar las llamas el polvo del Pokémon, explotó!"),
    [STRINGID_BELCHCANTSELECT]                      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} no ha comido ninguna BAYA equipada, así que no puede eructar!\p"),
    [STRINGID_SPECTRALTHIEFSTEAL]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} robó los aumentos de características del objetivo!"),
    [STRINGID_GRAVITYGROUNDING]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} cayó del cielo por la gravedad!"),
    [STRINGID_MISTYTERRAINPREVENTS]                 = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se rodea de una niebla protectora!"),
    [STRINGID_GRASSYTERRAINHEALS]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se cura con el Campo de Hierba!"),
    [STRINGID_ELECTRICTERRAINPREVENTS]              = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se rodea de un terreno electrificado!"),
    [STRINGID_PSYCHICTERRAINPREVENTS]               = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} está protegido por el Campo Psíquico!"),
    [STRINGID_SAFETYGOGGLESPROTECTED]               = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} no se ve afectado gracias a {B_LAST_ITEM}!"),
    [STRINGID_FLOWERVEILPROTECTED]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se rodeó con un velo de pétalos!"),
    [STRINGID_SWEETVEILPROTECTED]                   = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} no puede dormirse por el Velo Dulce!"),
    [STRINGID_AROMAVEILPROTECTED]                   = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} está protegido por un Velo Aromático!"),
    [STRINGID_CELEBRATEMESSAGE]                     = COMPOUND_STRING("¡Enhorabuena, {B_PLAYER_NAME}!"),
    [STRINGID_USEDINSTRUCTEDMOVE]                   = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} siguió las instrucciones de {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_THROATCHOPENDS]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} ya puede usar movimientos de sonido!"),
    [STRINGID_PKMNCANTUSEMOVETHROATCHOP]            = COMPOUND_STRING("¡Efecto de Tajo Garganta impide a {B_ATK_NAME_WITH_PREFIX2} usar ciertos movimientos!\p"),
    [STRINGID_LASERFOCUS]                           = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se concentró intensamente!"),
    [STRINGID_GEMACTIVATES]                         = COMPOUND_STRING("¡{B_LAST_ITEM} fortaleció el poder de {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_BERRYDMGREDUCES]                      = COMPOUND_STRING("¡{B_LAST_ITEM} redujo el daño a {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_AIRBALLOONFLOAT]                      = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} flota en el aire con su Globo Helio!"),
    [STRINGID_AIRBALLOONPOP]                        = COMPOUND_STRING("¡El Globo Helio de {B_DEF_NAME_WITH_PREFIX} estalló!"),
    [STRINGID_INCINERATEBURN]                       = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_EFF_NAME_WITH_PREFIX} se consumió!"),
    [STRINGID_BUGBITE]                              = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} robó y se comió el {B_LAST_ITEM} de su objetivo!"),
    [STRINGID_ILLUSIONWOREOFF]                      = COMPOUND_STRING("¡La ilusión de {B_SCR_NAME_WITH_PREFIX} se desvaneció!"),
    [STRINGID_ATTACKERCUREDTARGETSTATUS]            = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} curó el problema de {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_ATTACKERLOSTFIRETYPE]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se consumió!"),
    [STRINGID_HEALERCURE]                           = COMPOUND_STRING("¡{B_LAST_ABILITY} de {B_ATK_NAME_WITH_PREFIX} curó el problema de {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_SCRIPTINGABILITYSTATRAISE]            = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} {B_BUFF2}subió su {B_BUFF1}!"),
    [STRINGID_RECEIVERABILITYTAKEOVER]              = COMPOUND_STRING("¡La habilidad {B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} fue suprimida!"),
    [STRINGID_PKNMABSORBINGPOWER]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} está absorbiendo poder!"),
    [STRINGID_NOONEWILLBEABLETORUNAWAY]             = COMPOUND_STRING("¡Nadie podrá huir durante el próximo turno!"),
    [STRINGID_DESTINYKNOTACTIVATES]                 = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se enamoró por {B_LAST_ITEM}!"),
    [STRINGID_CLOAKEDINAFREEZINGLIGHT]              = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se cubrió con una luz helada!"),
    [STRINGID_CLEARAMULETWONTLOWERSTATS]            = COMPOUND_STRING("¡Los efectos de {B_LAST_ITEM} de {B_SCR_NAME_WITH_PREFIX2} impiden que bajen sus características!"),
    [STRINGID_FERVENTWISHREACHED]                   = COMPOUND_STRING("¡El ferviente deseo de {B_ATK_TRAINER_NAME} llegó a {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_AIRLOCKACTIVATES]                     = COMPOUND_STRING("Los efectos del clima desaparecieron."),
    [STRINGID_PRESSUREENTERS]                       = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} ejerce su presión!"),
    [STRINGID_DARKAURAENTERS]                       = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} irradia un aura oscura!"),
    [STRINGID_FAIRYAURAENTERS]                      = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} irradia un aura feérica!"),
    [STRINGID_AURABREAKENTERS]                      = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} invirtió las auras de los demás Pokémon!"),
    [STRINGID_COMATOSEENTERS]                       = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} está adormilado!"),
    [STRINGID_SCREENCLEANERENTERS]                  = COMPOUND_STRING("¡Se eliminaron todas las pantallas del campo!"),
    [STRINGID_FETCHEDPOKEBALL]                      = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} encontró un {B_LAST_ITEM}!"),
    [STRINGID_ASANDSTORMKICKEDUP]                   = COMPOUND_STRING("¡Se levantó una tormenta de arena!"),
    [STRINGID_PKMNSWILLPERISHIN3TURNS]              = COMPOUND_STRING("¡Ambos Pokémon se debilitarán en tres turnos!"),  //don't think this message is displayed anymore
    [STRINGID_AURAFLAREDTOLIFE]                     = COMPOUND_STRING("¡El aura de {B_DEF_NAME_WITH_PREFIX} cobró vida!"),
    [STRINGID_ASONEENTERS]                          = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} tiene dos habilidades!"),
    [STRINGID_CURIOUSMEDICINEENTERS]                = COMPOUND_STRING("¡Se eliminaron los cambios de características de {B_EFF_NAME_WITH_PREFIX}!"),
    [STRINGID_CANACTFASTERTHANKSTO]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} puede actuar más rápido gracias a {B_BUFF1}!"),
    [STRINGID_MICLEBERRYACTIVATES]                  = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} aumentó la precisión de su próximo movimiento con {B_LAST_ITEM}!"),
    [STRINGID_PKMNSHOOKOFFTHETAUNT]                 = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} se libró de la provocación!"),
    [STRINGID_PKMNGOTOVERITSINFATUATION]            = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} superó su enamoramiento!"),
    [STRINGID_ITEMCANNOTBEREMOVED]                  = COMPOUND_STRING("¡El objeto de {B_ATK_NAME_WITH_PREFIX} no se puede quitar!"),
    [STRINGID_STICKYBARBTRANSFER]                   = COMPOUND_STRING("¡{B_LAST_ITEM} se pegó a {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNBURNHEALED]                       = COMPOUND_STRING("¡La quemadura de {B_DEF_NAME_WITH_PREFIX} se curó!"),
    [STRINGID_REDCARDACTIVATE]                      = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} mostró su Tarjeta Roja a {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_EJECTBUTTONACTIVATE]                  = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} fue cambiado por {B_LAST_ITEM}!"),
    [STRINGID_ATKGOTOVERINFATUATION]                = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} superó su enamoramiento!"),
    [STRINGID_TORMENTEDNOMORE]                      = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} ya no está atormentado!"),
    [STRINGID_HEALBLOCKEDNOMORE]                    = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} se liberó del bloqueo de curación!"),
    [STRINGID_ATTACKERBECAMEFULLYCHARGED]           = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se cargó por completo por su vínculo con su entrenador!\p"),
    [STRINGID_ATTACKERBECAMEASHSPECIES]             = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se transformó en Greninja Ash!\p"),
    [STRINGID_EXTREMELYHARSHSUNLIGHT]               = COMPOUND_STRING("¡El sol se volvió extremadamente abrasador!"),
    [STRINGID_EXTREMESUNLIGHTFADED]                 = COMPOUND_STRING("¡El sol extremadamente abrasador se debilitó!"),
    [STRINGID_MOVEEVAPORATEDINTHEHARSHSUNLIGHT]     = COMPOUND_STRING("¡El ataque de tipo Agua se evaporó por el sol abrasador!"),
    [STRINGID_EXTREMELYHARSHSUNLIGHTWASNOTLESSENED] = COMPOUND_STRING("¡El sol extremadamente abrasador no se debilitó en absoluto!"),
    [STRINGID_HEAVYRAIN]                            = COMPOUND_STRING("¡Empezó a caer una lluvia torrencial!"),
    [STRINGID_HEAVYRAINLIFTED]                      = COMPOUND_STRING("¡La lluvia torrencial se disipó!"),
    [STRINGID_MOVEFIZZLEDOUTINTHEHEAVYRAIN]         = COMPOUND_STRING("¡El ataque de tipo Fuego se apagó por la lluvia torrencial!"),
    [STRINGID_NORELIEFROMHEAVYRAIN]                 = COMPOUND_STRING("¡Esta lluvia torrencial no afloja!"),
    [STRINGID_MYSTERIOUSAIRCURRENT]                 = COMPOUND_STRING("¡Vientos misteriosos protegen a los POKéMON de tipo Volador!"),
    [STRINGID_STRONGWINDSDISSIPATED]                = COMPOUND_STRING("¡Los vientos misteriosos se disiparon!"),
    [STRINGID_MYSTERIOUSAIRCURRENTBLOWSON]          = COMPOUND_STRING("¡Los vientos misteriosos siguen soplando!"),
    [STRINGID_ATTACKWEAKENEDBSTRONGWINDS]           = COMPOUND_STRING("¡Los vientos misteriosos debilitaron el ataque!"),
    [STRINGID_STUFFCHEEKSCANTSELECT]                = COMPOUND_STRING("¡No puede usar el movimiento porque no tiene una BAYA!\p"),
    [STRINGID_PKMNREVERTEDTOPRIMAL]                 = COMPOUND_STRING("¡Reversión Primigenia de {B_SCR_NAME_WITH_PREFIX}! ¡Volvió a su estado primigenio!"),
    [STRINGID_BUTPOKEMONCANTUSETHEMOVE]             = COMPOUND_STRING("¡Pero {B_ATK_NAME_WITH_PREFIX2} no puede usar el movimiento!"),
    [STRINGID_BUTHOOPACANTUSEIT]                    = COMPOUND_STRING("¡Pero {B_ATK_NAME_WITH_PREFIX2} no puede usarlo así ahora!"),
    [STRINGID_BROKETHROUGHPROTECTION]               = COMPOUND_STRING("¡Atravesó la protección de {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_ABILITYALLOWSONLYMOVE]                = COMPOUND_STRING("¡{B_ATK_ABILITY} solo permite usar {B_CURRENT_MOVE}!\p"),
    [STRINGID_SWAPPEDABILITIES]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} intercambió habilidades con su objetivo!"),
    [STRINGID_PASTELVEILENTERS]                     = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se curó del envenenamiento!"),
    [STRINGID_BATTLERTYPECHANGEDTO]                 = COMPOUND_STRING("¡El tipo de {B_SCR_NAME_WITH_PREFIX} cambió a {B_BUFF1}!"),
    [STRINGID_BOTHCANNOLONGERESCAPE]                = COMPOUND_STRING("¡Ningún Pokémon puede huir!"),
    [STRINGID_CANTESCAPEDUETOUSEDMOVE]              = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} ya no puede escapar por usar Retirada!"),
    [STRINGID_PKMNBECAMEWEAKERTOFIRE]               = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} se volvió más débil al Fuego!"),
    [STRINGID_ABOUTTOUSEPOLTERGEIST]                = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} está a punto de ser atacado por su {B_BUFF1}!"),
    [STRINGID_CANTESCAPEBECAUSEOFCURRENTMOVE]       = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} ya no puede escapar por Octopresa!"),
    [STRINGID_NEUTRALIZINGGASENTERS]                = COMPOUND_STRING("¡El gas neutralizador llenó la zona!"),
    [STRINGID_NEUTRALIZINGGASOVER]                  = COMPOUND_STRING("¡El efecto del gas neutralizante se pasó!"),
    [STRINGID_TARGETTOOHEAVY]                       = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} es demasiado pesado para levantarlo!"),
    [STRINGID_PKMNTOOKTARGETHIGH]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se llevó a {B_DEF_NAME_WITH_PREFIX2} a los cielos!"),
    [STRINGID_PKMNINSNAPTRAP]                       = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} quedó atrapado por una trampa!"),
    [STRINGID_METEORBEAMCHARGING]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} rebosa poder espacial!"),
    [STRINGID_HEATUPBEAK]                           = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} empezó a calentar su pico!"),
    [STRINGID_COURTCHANGE]                          = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} intercambió los efectos de campo de cada lado!"),
    [STRINGID_ZPOWERSURROUNDS]                      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se rodeó de su Poder Z!"),
    [STRINGID_ZMOVEUNLEASHED]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} desató su Megamovimiento Z!"),
    [STRINGID_ZMOVERESETSSTATS]                     = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} normalizó sus cambios de características con su Poder Z!"),
    [STRINGID_ZMOVEALLSTATSUP]                      = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} aumentó sus características con su Poder Z!"),
    [STRINGID_ZMOVEZBOOSTCRIT]                      = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} aumentó su prob. de crítico con su Poder Z!"),
    [STRINGID_ZMOVERESTOREHP]                       = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} restauró sus PS con su Poder Z!"),
    [STRINGID_ZMOVESTATUP]                          = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} aumentó sus características con su Poder Z!"),
    [STRINGID_ZMOVEHPTRAP]                          = COMPOUND_STRING("¡Los PS de {B_SCR_NAME_WITH_PREFIX} se restauraron con el Poder Z!"),
    [STRINGID_ATTACKEREXPELLEDTHEPOISON]            = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} expulsó el veneno para que no te preocupes!"),
    [STRINGID_ATTACKERSHOOKITSELFAWAKE]             = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se despertó a la fuerza para que no te preocupes!"),
    [STRINGID_ATTACKERBROKETHROUGHPARALYSIS]        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} reunió energía para superar la parálisis y que no te preocupes!"),
    [STRINGID_ATTACKERHEALEDITSBURN]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} curó su quemadura con pura determinación para que no te preocupes!"),
    [STRINGID_ATTACKERMELTEDTHEICE]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} derritió el hielo con su ardiente determinación para que no te preocupes!"),
    [STRINGID_TARGETTOUGHEDITOUT]                   = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} aguantó para que no te entristezcas!"),
    [STRINGID_ATTACKERLOSTELECTRICTYPE]             = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} agotó toda su electricidad!"),
    [STRINGID_ATTACKERSWITCHEDSTATWITHTARGET]       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} intercambió {B_BUFF1} con su objetivo!"),
    [STRINGID_BEINGHITCHARGEDPKMNWITHPOWER]         = COMPOUND_STRING("¡Al recibir {B_CURRENT_MOVE}, {B_DEF_NAME_WITH_PREFIX2} se cargó de energía!"),
    [STRINGID_SUNLIGHTACTIVATEDABILITY]             = COMPOUND_STRING("¡El sol abrasador activó Protosíntesis de {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_STATWASHEIGHTENED]                    = COMPOUND_STRING("¡La {B_BUFF1} de {B_SCR_NAME_WITH_PREFIX} aumentó!"),
    [STRINGID_ELECTRICTERRAINACTIVATEDABILITY]      = COMPOUND_STRING("¡El Campo Eléctrico activó Motor Hadrónico de {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_ABILITYWEAKENEDSURROUNDINGMONSSTAT]   = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} bajó la {B_BUFF1} de todos los Pokémon cercanos!\p"),
    [STRINGID_ATTACKERGAINEDSTRENGTHFROMTHEFALLEN]  = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} ganó fuerza de los caídos!"),
    [STRINGID_PKMNSABILITYPREVENTSABILITY]          = COMPOUND_STRING("¡{B_SCR_ABILITY} de {B_SCR_NAME_WITH_PREFIX} impide que funcione {B_DEF_ABILITY} de {B_DEF_NAME_WITH_PREFIX2}!"), //not in gen 5+, ability popup
    [STRINGID_PREPARESHELLTRAP]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} colocó una Trampa Coraza!"),
    [STRINGID_SHELLTRAPDIDNTWORK]                   = COMPOUND_STRING("¡La Trampa Coraza de {B_ATK_NAME_WITH_PREFIX} no funcionó!"),
    [STRINGID_SPIKESDISAPPEAREDFROMTEAM]            = COMPOUND_STRING("¡Las Púas desaparecieron del terreno alrededor del equipo {B_ATK_TEAM2}!"),
    [STRINGID_TOXICSPIKESDISAPPEAREDFROMTEAM]       = COMPOUND_STRING("¡Las Púas Tóxicas desaparecieron del terreno alrededor del equipo {B_ATK_TEAM2}!"),
    [STRINGID_STICKYWEBDISAPPEAREDFROMTEAM]         = COMPOUND_STRING("¡La Telaraña Pegajosa desapareció del terreno alrededor del equipo {B_ATK_TEAM2}!"),
    [STRINGID_STEALTHROCKDISAPPEAREDFROMTEAM]       = COMPOUND_STRING("¡Las Trampas Rocas desaparecieron alrededor del equipo {B_ATK_TEAM2}!"),
    [STRINGID_COULDNTFULLYPROTECT]                  = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} no pudo protegerse del todo y sufrió daño!"),
    [STRINGID_STOCKPILEDEFFECTWOREOFF]              = COMPOUND_STRING("¡El efecto de Acumular de {B_ATK_NAME_WITH_PREFIX} se pasó!"),
    [STRINGID_PKMNREVIVEDREADYTOFIGHT]              = COMPOUND_STRING("¡{B_BUFF1} revivió y está listo para luchar otra vez!"),
    [STRINGID_ITEMRESTOREDSPECIESHEALTH]            = COMPOUND_STRING("¡{B_BUFF1} recuperó sus PS!"),
    [STRINGID_ITEMCUREDSPECIESSTATUS]               = COMPOUND_STRING("¡{B_BUFF1} curó su estado!"),
    [STRINGID_ITEMRESTOREDSPECIESPP]                = COMPOUND_STRING("¡{B_BUFF1} recuperó PP!"),
    [STRINGID_THUNDERCAGETRAPPED]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} atrapó a {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNHURTBYFROSTBITE]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre por la congelación!"),
    [STRINGID_PKMNGOTFROSTBITE]                     = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} sufrió congelación!"),
    [STRINGID_PKMNSITEMHEALEDFROSTBITE]             = COMPOUND_STRING("¡{B_LAST_ITEM} de {B_SCR_NAME_WITH_PREFIX} curó la congelación!"),
    [STRINGID_ATTACKERHEALEDITSFROSTBITE]           = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} curó su congelación con pura determinación para que no te preocupes!"),
    [STRINGID_PKMNFROSTBITEHEALED]                  = COMPOUND_STRING("¡La congelación de {B_DEF_NAME_WITH_PREFIX} se curó!"),
    [STRINGID_PKMNFROSTBITEHEALED2]                 = COMPOUND_STRING("¡La congelación de {B_ATK_NAME_WITH_PREFIX} se curó!"),
    [STRINGID_PKMNFROSTBITEHEALEDBY]                = COMPOUND_STRING("¡{B_CURRENT_MOVE} de {B_ATK_NAME_WITH_PREFIX} curó su congelación!"),
    [STRINGID_MIRRORHERBCOPIED]                     = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} usó su Hierba Copia para copiar los cambios de características del rival!"),
    [STRINGID_STARTEDSNOW]                          = COMPOUND_STRING("¡Empezó a nevar!"),
    [STRINGID_SNOWCONTINUES]                        = COMPOUND_STRING("La nieve sigue cayendo."), //not in gen 5+ (lol)
    [STRINGID_SNOWSTOPPED]                          = COMPOUND_STRING("Dejó de nevar."),
    [STRINGID_SNOWWARNINGSNOW]                      = COMPOUND_STRING("¡Empezó a nevar!"),
    [STRINGID_PKMNITEMMELTED]                       = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} corroyó el {B_LAST_ITEM} de {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_ULTRABURSTREACTING]                   = COMPOUND_STRING("¡Una luz intensa está a punto de estallar en {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_ULTRABURSTCOMPLETED]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} recuperó su verdadero poder con Ultraexplosión!"),
    [STRINGID_TEAMGAINEDEXP]                        = COMPOUND_STRING("¡El resto de tu equipo ganó Puntos Exp. gracias al Repartir Exp.!\p"),
    [STRINGID_CURRENTMOVECANTSELECT]                = COMPOUND_STRING("¡{B_BUFF1} no se puede usar!\p"),
    [STRINGID_TARGETISBEINGSALTCURED]               = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} está siendo curado con sal!"),
    [STRINGID_TARGETISHURTBYSALTCURE]               = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre por {B_BUFF1}!"),
    [STRINGID_TARGETCOVEREDINSTICKYCANDYSYRUP]      = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} quedó cubierto de sirope de caramelo pegajoso!"),
    [STRINGID_SHARPSTEELFLOATS]                     = COMPOUND_STRING("¡Trozos de acero afilado empezaron a flotar alrededor del equipo {B_DEF_TEAM2}!"),
    [STRINGID_SHARPSTEELDMG]                        = COMPOUND_STRING("¡El acero afilado hirió a {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_PKMNBLEWAWAYSHARPSTEEL]               = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} dispersó el acero afilado!"),
    [STRINGID_SHARPSTEELDISAPPEAREDFROMTEAM]        = COMPOUND_STRING("¡Los trozos de acero alrededor del equipo {B_ATK_TEAM2} desaparecieron!"),
    [STRINGID_TEAMTRAPPEDWITHVINES]                 = COMPOUND_STRING("¡El equipo {B_DEF_TEAM1} quedó atrapado con lianas!"),
    [STRINGID_PKMNHURTBYVINES]                      = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre por el duro latigazo de Látigo Cepa G-Max!"),
    [STRINGID_TEAMCAUGHTINVORTEX]                   = COMPOUND_STRING("¡El equipo {B_DEF_TEAM1} quedó atrapado en un vórtice de agua!"),
    [STRINGID_PKMNHURTBYVORTEX]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre por el vórtice de Cañón G-Max!"),
    [STRINGID_TEAMSURROUNDEDBYFIRE]                 = COMPOUND_STRING("¡El equipo {B_DEF_TEAM1} quedó rodeado por el fuego!"),
    [STRINGID_PKMNBURNINGUP]                        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se quema con las llamas de Fuego G-Max!"),
    [STRINGID_TEAMSURROUNDEDBYROCKS]                = COMPOUND_STRING("¡El equipo {B_DEF_TEAM1} quedó rodeado por rocas!"),
    [STRINGID_PKMNHURTBYROCKSTHROWN]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre por las rocas de Rocas G-Max!"),
    [STRINGID_MOVEBLOCKEDBYDYNAMAX]                 = COMPOUND_STRING("¡El movimiento fue bloqueado por el poder Dinamax!"),
    [STRINGID_ZEROTOHEROTRANSFORMATION]             = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} se transformó heroicamente!"),
    [STRINGID_THETWOMOVESBECOMEONE]                 = COMPOUND_STRING("¡Los dos movimientos se han unido! ¡Es un movimiento combinado!{PAUSE 16}"),
    [STRINGID_ARAINBOWAPPEAREDONSIDE]               = COMPOUND_STRING("¡Un arcoíris apareció en el cielo del lado del equipo {B_ATK_TEAM2}!"),
    [STRINGID_THERAINBOWDISAPPEARED]                = COMPOUND_STRING("¡El arcoíris del lado del equipo {B_ATK_TEAM2} desapareció!"),
    [STRINGID_WAITINGFORPARTNERSMOVE]               = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} espera el movimiento de {B_ATK_PARTNER_NAME}…{PAUSE 16}"),
    [STRINGID_SEAOFFIREENVELOPEDSIDE]               = COMPOUND_STRING("¡Un mar de fuego envolvió al equipo {B_DEF_TEAM2}!"),
    [STRINGID_HURTBYTHESEAOFFIRE]                   = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} sufre por el mar de fuego!"),
    [STRINGID_THESEAOFFIREDISAPPEARED]              = COMPOUND_STRING("¡El mar de fuego alrededor del equipo {B_ATK_TEAM2} desapareció!"),
    [STRINGID_SWAMPENVELOPEDSIDE]                   = COMPOUND_STRING("¡Un pantano envolvió al equipo {B_DEF_TEAM2}!"),
    [STRINGID_THESWAMPDISAPPEARED]                  = COMPOUND_STRING("¡El pantano alrededor del equipo {B_ATK_TEAM2} desapareció!"),
    [STRINGID_PKMNTELLCHILLINGRECEPTIONJOKE]        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se prepara para contar un chiste helador!"),
    [STRINGID_HOSPITALITYRESTORATION]               = COMPOUND_STRING("¡{B_EFF_NAME_WITH_PREFIX} se bebió todo el matcha que preparó {B_SCR_NAME_WITH_PREFIX2}!"),
    [STRINGID_ELECTROSHOTCHARGING]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} absorbió electricidad!"),
    [STRINGID_ITEMWASUSEDUP]                        = COMPOUND_STRING("¡{B_LAST_ITEM} se gastó…"),
    [STRINGID_ATTACKERLOSTITSTYPE]                  = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} perdió su tipo {B_BUFF1}!"),
    [STRINGID_SHEDITSTAIL]                          = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se desprendió de su cola para crear un señuelo!"),
    [STRINGID_CLOAKEDINAHARSHLIGHT]                 = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se cubrió con una luz intensa!"),
    [STRINGID_SUPERSWEETAROMAWAFTS]                 = COMPOUND_STRING("¡Un aroma superdulce emana del sirope que cubre a {B_ATK_NAME_WITH_PREFIX2}!"),
    [STRINGID_DIMENSIONSWERETWISTED]                = COMPOUND_STRING("¡Las dimensiones se retorcieron!"),
    [STRINGID_BIZARREARENACREATED]                  = COMPOUND_STRING("¡Se creó un área extraña donde los objetos equipados pierden su efecto!"),
    [STRINGID_BIZARREAREACREATED]                   = COMPOUND_STRING("¡Se creó un área extraña donde Defensa y Def. Esp. se intercambian!"),
    [STRINGID_TIDYINGUPCOMPLETE]                    = COMPOUND_STRING("¡Recogida completada!"),
    [STRINGID_PKMNTERASTALLIZEDINTO]                = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} teracristalizó en el tipo {B_BUFF1}!"),
    [STRINGID_BOOSTERENERGYACTIVATES]               = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} usó {B_LAST_ITEM} para activar {B_SCR_ABILITY}!"),
    [STRINGID_FOGCREPTUP]                           = COMPOUND_STRING("¡La niebla se extendió espesa como la sopa!"),
    [STRINGID_FOGISDEEP]                            = COMPOUND_STRING("La niebla es densa…"),
    [STRINGID_FOGLIFTED]                            = COMPOUND_STRING("La niebla se disipó."),
    [STRINGID_PKMNMADESHELLGLEAM]                   = COMPOUND_STRING("¡{B_DEF_NAME_WITH_PREFIX} hizo brillar su caparazón! ¡Distorsiona las relaciones de tipo!"),
    [STRINGID_FICKLEBEAMDOUBLED]                    = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} lo dio todo en este ataque!"),
    [STRINGID_COMMANDERACTIVATES]                   = COMPOUND_STRING("¡{B_SCR_NAME_WITH_PREFIX} fue tragado por Dondozo y se convirtió en su comandante!"),
    [STRINGID_POKEFLUTECATCHY]                      = COMPOUND_STRING("¡{B_PLAYER_NAME} tocó el {B_LAST_ITEM}.\p¡Qué melodía tan pegadiza!"),
    [STRINGID_POKEFLUTE]                            = COMPOUND_STRING("¡{B_PLAYER_NAME} tocó el {B_LAST_ITEM}."),
    [STRINGID_MONHEARINGFLUTEAWOKE]                 = COMPOUND_STRING("¡Los Pokémon que oyeron la flauta se despertaron!"),
    [STRINGID_SUNLIGHTISHARSH]                      = COMPOUND_STRING("¡El sol es abrasador!"),
    [STRINGID_ITISHAILING]                          = COMPOUND_STRING("¡Está granizando!"),
    [STRINGID_ITISSNOWING]                          = COMPOUND_STRING("¡Está nevando!"),
    [STRINGID_ISCOVEREDWITHGRASS]                   = COMPOUND_STRING("¡El terreno está cubierto de hierba!"),
    [STRINGID_MISTSWIRLSAROUND]                     = COMPOUND_STRING("¡La niebla envuelve el terreno!"),
    [STRINGID_ELECTRICCURRENTISRUNNING]             = COMPOUND_STRING("¡Una corriente eléctrica recorre el terreno!"),
    [STRINGID_SEEMSWEIRD]                           = COMPOUND_STRING("¡El terreno parece extraño!"),
    [STRINGID_WAGGLINGAFINGER]                      = COMPOUND_STRING("¡Mover el dedo permitió usar {B_CURRENT_MOVE}!"),
    [STRINGID_BLOCKEDBYSLEEPCLAUSE]                 = COMPOUND_STRING("¡La Cláusula Sueño mantuvo despierto a {B_DEF_NAME_WITH_PREFIX2}!"),
    [STRINGID_SUPEREFFECTIVETWOFOES]                = COMPOUND_STRING("¡Es muy eficaz contra {B_DEF_NAME_WITH_PREFIX2} y {B_DEF_PARTNER_NAME}!"),
    [STRINGID_NOTVERYEFFECTIVETWOFOES]              = COMPOUND_STRING("No es muy eficaz contra {B_DEF_NAME_WITH_PREFIX2} y {B_DEF_PARTNER_NAME}…"),
    [STRINGID_ITDOESNTAFFECTTWOFOES]                = COMPOUND_STRING("No afecta a {B_DEF_NAME_WITH_PREFIX2} ni a {B_DEF_PARTNER_NAME}…"),
    [STRINGID_SENDCAUGHTMONPARTYORBOX]              = COMPOUND_STRING("¿Añadir a {B_DEF_NAME} a tu equipo?"),
    [STRINGID_PKMNSENTTOPCAFTERCATCH]               = gText_PkmnSentToPCAfterCatch,
    [STRINGID_PKMNDYNAMAXED]                        = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se hizo enorme en su forma Dinamax!"),
    [STRINGID_PKMNGIGANTAMAXED]                     = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se hizo enorme en su forma Gigamax!"),
    [STRINGID_TIMETODYNAMAX]                        = COMPOUND_STRING("¡Hora de Dinamax!"),
    [STRINGID_TIMETOGIGANTAMAX]                     = COMPOUND_STRING("¡Hora de Gigamax!"),
    [STRINGID_QUESTIONFORFEITBATTLE]                = COMPOUND_STRING("¿Quieres abandonar el combate y salir ahora? Abandonar equivale a perder."),
    [STRINGID_POWERCONSTRUCTPRESENCEOFMANY]         = COMPOUND_STRING("¡Sientes la presencia de muchos!"),
    [STRINGID_POWERCONSTRUCTTRANSFORM]              = COMPOUND_STRING("¡{B_ATK_NAME_WITH_PREFIX} se transformó en su Forma Completa!"),
    [STRINGID_ABILITYSHIELDPROTECTS]                = COMPOUND_STRING("¡La habilidad de {B_ATK_NAME_WITH_PREFIX} está protegida por los efectos de {B_LAST_ITEM}!"),
};

const u16 gTrainerUsedItemStringIds[] =
{
    STRINGID_PLAYERUSEDITEM, STRINGID_TRAINER1USEDITEM
};

const u16 gZEffectStringIds[] =
{
    [B_MSG_Z_RESET_STATS] = STRINGID_ZMOVERESETSSTATS,
    [B_MSG_Z_ALL_STATS_UP]= STRINGID_ZMOVEALLSTATSUP,
    [B_MSG_Z_BOOST_CRITS] = STRINGID_ZMOVEZBOOSTCRIT,
    [B_MSG_Z_FOLLOW_ME]   = STRINGID_PKMNCENTERATTENTION,
    [B_MSG_Z_RECOVER_HP]  = STRINGID_ZMOVERESTOREHP,
    [B_MSG_Z_STAT_UP]     = STRINGID_ZMOVESTATUP,
    [B_MSG_Z_HP_TRAP]     = STRINGID_ZMOVEHPTRAP,
};

const u16 gMentalHerbCureStringIds[] =
{
    [B_MSG_MENTALHERBCURE_INFATUATION] = STRINGID_ATKGOTOVERINFATUATION,
    [B_MSG_MENTALHERBCURE_TAUNT]       = STRINGID_BUFFERENDS,
    [B_MSG_MENTALHERBCURE_ENCORE]      = STRINGID_PKMNENCOREENDED,
    [B_MSG_MENTALHERBCURE_TORMENT]     = STRINGID_TORMENTEDNOMORE,
    [B_MSG_MENTALHERBCURE_HEALBLOCK]   = STRINGID_HEALBLOCKEDNOMORE,
    [B_MSG_MENTALHERBCURE_DISABLE]     = STRINGID_PKMNMOVEDISABLEDNOMORE,
};

const u16 gStartingStatusStringIds[B_MSG_STARTING_STATUS_COUNT] =
{
    [B_MSG_TERRAIN_SET_MISTY]    = STRINGID_TERRAINBECOMESMISTY,
    [B_MSG_TERRAIN_SET_ELECTRIC] = STRINGID_TERRAINBECOMESELECTRIC,
    [B_MSG_TERRAIN_SET_PSYCHIC]  = STRINGID_TERRAINBECOMESPSYCHIC,
    [B_MSG_TERRAIN_SET_GRASSY]   = STRINGID_TERRAINBECOMESGRASSY,
    [B_MSG_SET_TRICK_ROOM]       = STRINGID_DIMENSIONSWERETWISTED,
    [B_MSG_SET_MAGIC_ROOM]       = STRINGID_BIZARREARENACREATED,
    [B_MSG_SET_WONDER_ROOM]      = STRINGID_BIZARREAREACREATED,
    [B_MSG_SET_TAILWIND]         = STRINGID_TAILWINDBLEW,
    [B_MSG_SET_RAINBOW]          = STRINGID_ARAINBOWAPPEAREDONSIDE,
    [B_MSG_SET_SEA_OF_FIRE]      = STRINGID_SEAOFFIREENVELOPEDSIDE,
    [B_MSG_SET_SWAMP]            = STRINGID_SWAMPENVELOPEDSIDE,
};

const u16 gTerrainStringIds[B_MSG_TERRAIN_COUNT] =
{
    [B_MSG_TERRAIN_SET_MISTY] = STRINGID_TERRAINBECOMESMISTY,
    [B_MSG_TERRAIN_SET_ELECTRIC] = STRINGID_TERRAINBECOMESELECTRIC,
    [B_MSG_TERRAIN_SET_PSYCHIC] = STRINGID_TERRAINBECOMESPSYCHIC,
    [B_MSG_TERRAIN_SET_GRASSY] = STRINGID_TERRAINBECOMESGRASSY,
    [B_MSG_TERRAIN_END_MISTY] = STRINGID_MISTYTERRAINENDS,
    [B_MSG_TERRAIN_END_ELECTRIC] = STRINGID_ELECTRICTERRAINENDS,
    [B_MSG_TERRAIN_END_PSYCHIC] = STRINGID_PSYCHICTERRAINENDS,
    [B_MSG_TERRAIN_END_GRASSY] = STRINGID_GRASSYTERRAINENDS,
};

const u16 gTerrainPreventsStringIds[] =
{
    [B_MSG_TERRAINPREVENTS_MISTY]    = STRINGID_MISTYTERRAINPREVENTS,
    [B_MSG_TERRAINPREVENTS_ELECTRIC] = STRINGID_ELECTRICTERRAINPREVENTS,
    [B_MSG_TERRAINPREVENTS_PSYCHIC]  = STRINGID_PSYCHICTERRAINPREVENTS
};

const u16 gHealingWishStringIds[] =
{
    STRINGID_HEALINGWISHCAMETRUE,
    STRINGID_LUNARDANCECAMETRUE
};

const u16 gDmgHazardsStringIds[] =
{
    [B_MSG_PKMNHURTBYSPIKES]   = STRINGID_PKMNHURTBYSPIKES,
    [B_MSG_STEALTHROCKDMG]     = STRINGID_STEALTHROCKDMG,
    [B_MSG_SHARPSTEELDMG]      = STRINGID_SHARPSTEELDMG,
    [B_MSG_POINTEDSTONESFLOAT] = STRINGID_POINTEDSTONESFLOAT,
    [B_MSG_SPIKESSCATTERED]    = STRINGID_SPIKESSCATTERED,
    [B_MSG_SHARPSTEELFLOATS]   = STRINGID_SHARPSTEELFLOATS,
};

const u16 gSwitchInAbilityStringIds[] =
{
    [B_MSG_SWITCHIN_MOLDBREAKER] = STRINGID_MOLDBREAKERENTERS,
    [B_MSG_SWITCHIN_TERAVOLT] = STRINGID_TERAVOLTENTERS,
    [B_MSG_SWITCHIN_TURBOBLAZE] = STRINGID_TURBOBLAZEENTERS,
    [B_MSG_SWITCHIN_SLOWSTART] = STRINGID_SLOWSTARTENTERS,
    [B_MSG_SWITCHIN_UNNERVE] = STRINGID_UNNERVEENTERS,
    [B_MSG_SWITCHIN_ANTICIPATION] = STRINGID_ANTICIPATIONACTIVATES,
    [B_MSG_SWITCHIN_FOREWARN] = STRINGID_FOREWARNACTIVATES,
    [B_MSG_SWITCHIN_PRESSURE] = STRINGID_PRESSUREENTERS,
    [B_MSG_SWITCHIN_DARKAURA] = STRINGID_DARKAURAENTERS,
    [B_MSG_SWITCHIN_FAIRYAURA] = STRINGID_FAIRYAURAENTERS,
    [B_MSG_SWITCHIN_AURABREAK] = STRINGID_AURABREAKENTERS,
    [B_MSG_SWITCHIN_COMATOSE] = STRINGID_COMATOSEENTERS,
    [B_MSG_SWITCHIN_SCREENCLEANER] = STRINGID_SCREENCLEANERENTERS,
    [B_MSG_SWITCHIN_ASONE] = STRINGID_ASONEENTERS,
    [B_MSG_SWITCHIN_CURIOUS_MEDICINE] = STRINGID_CURIOUSMEDICINEENTERS,
    [B_MSG_SWITCHIN_PASTEL_VEIL] = STRINGID_PASTELVEILENTERS,
    [B_MSG_SWITCHIN_NEUTRALIZING_GAS] = STRINGID_NEUTRALIZINGGASENTERS,
};

const u16 gMissStringIds[] =
{
    [B_MSG_MISSED]      = STRINGID_ATTACKMISSED,
    [B_MSG_PROTECTED]   = STRINGID_PKMNPROTECTEDITSELF,
    [B_MSG_AVOIDED_ATK] = STRINGID_PKMNAVOIDEDATTACK,
    [B_MSG_AVOIDED_DMG] = STRINGID_AVOIDEDDAMAGE,
    [B_MSG_GROUND_MISS] = STRINGID_PKMNMAKESGROUNDMISS
};

const u16 gNoEscapeStringIds[] =
{
    [B_MSG_CANT_ESCAPE]          = STRINGID_CANTESCAPE,
    [B_MSG_DONT_LEAVE_BIRCH]     = STRINGID_DONTLEAVEBIRCH,
    [B_MSG_PREVENTS_ESCAPE]      = STRINGID_PREVENTSESCAPE,
    [B_MSG_CANT_ESCAPE_2]        = STRINGID_CANTESCAPE2,
    [B_MSG_ATTACKER_CANT_ESCAPE] = STRINGID_ATTACKERCANTESCAPE
};

const u16 gMoveWeatherChangeStringIds[] =
{
    [B_MSG_STARTED_RAIN]      = STRINGID_STARTEDTORAIN,
    [B_MSG_STARTED_DOWNPOUR]  = STRINGID_DOWNPOURSTARTED, // Unused
    [B_MSG_WEATHER_FAILED]    = STRINGID_BUTITFAILED,
    [B_MSG_STARTED_SANDSTORM] = STRINGID_SANDSTORMBREWED,
    [B_MSG_STARTED_SUNLIGHT]  = STRINGID_SUNLIGHTGOTBRIGHT,
    [B_MSG_STARTED_HAIL]      = STRINGID_STARTEDHAIL,
    [B_MSG_STARTED_SNOW]      = STRINGID_STARTEDSNOW,
    [B_MSG_STARTED_FOG]       = STRINGID_FOGCREPTUP, // Unused, can use for custom moves that set fog
};

const u16 gWeatherEndsStringIds[B_MSG_WEATHER_END_COUNT] =
{
    [B_MSG_WEATHER_END_RAIN]         = STRINGID_RAINSTOPPED,
    [B_MSG_WEATHER_END_SUN]          = STRINGID_SUNLIGHTFADED,
    [B_MSG_WEATHER_END_SANDSTORM]    = STRINGID_SANDSTORMSUBSIDED,
    [B_MSG_WEATHER_END_HAIL]         = STRINGID_HAILSTOPPED,
    [B_MSG_WEATHER_END_SNOW]         = STRINGID_SNOWSTOPPED,
    [B_MSG_WEATHER_END_FOG]          = STRINGID_FOGLIFTED,
    [B_MSG_WEATHER_END_STRONG_WINDS] = STRINGID_STRONGWINDSDISSIPATED,
};

const u16 gWeatherTurnStringIds[] =
{
    [B_MSG_WEATHER_TURN_RAIN]         = STRINGID_RAINCONTINUES,
    [B_MSG_WEATHER_TURN_DOWNPOUR]     = STRINGID_DOWNPOURCONTINUES,
    [B_MSG_WEATHER_TURN_SUN]          = STRINGID_SUNLIGHTSTRONG,
    [B_MSG_WEATHER_TURN_SANDSTORM]    = STRINGID_SANDSTORMRAGES,
    [B_MSG_WEATHER_TURN_HAIL]         = STRINGID_HAILCONTINUES,
    [B_MSG_WEATHER_TURN_SNOW]         = STRINGID_SNOWCONTINUES,
    [B_MSG_WEATHER_TURN_FOG]          = STRINGID_FOGISDEEP,
    [B_MSG_WEATHER_TURN_STRONG_WINDS] = STRINGID_MYSTERIOUSAIRCURRENTBLOWSON,
};

const u16 gSandStormHailDmgStringIds[] =
{
    [B_MSG_SANDSTORM] = STRINGID_PKMNBUFFETEDBYSANDSTORM,
    [B_MSG_HAIL]      = STRINGID_PKMNPELTEDBYHAIL
};

const u16 gProtectLikeUsedStringIds[] =
{
    [B_MSG_PROTECTED_ITSELF] = STRINGID_PKMNPROTECTEDITSELF2,
    [B_MSG_BRACED_ITSELF]    = STRINGID_PKMNBRACEDITSELF,
    [B_MSG_PROTECT_FAILED]   = STRINGID_BUTITFAILED,
    [B_MSG_PROTECTED_TEAM]   = STRINGID_PROTECTEDTEAM,
};

const u16 gReflectLightScreenSafeguardStringIds[] =
{
    [B_MSG_SIDE_STATUS_FAILED]     = STRINGID_BUTITFAILED,
    [B_MSG_SET_REFLECT_SINGLE]     = STRINGID_PKMNRAISEDDEF,
    [B_MSG_SET_REFLECT_DOUBLE]     = STRINGID_PKMNRAISEDDEF,
    [B_MSG_SET_LIGHTSCREEN_SINGLE] = STRINGID_PKMNRAISEDSPDEF,
    [B_MSG_SET_LIGHTSCREEN_DOUBLE] = STRINGID_PKMNRAISEDSPDEF,
    [B_MSG_SET_SAFEGUARD]          = STRINGID_PKMNCOVEREDBYVEIL,
};

const u16 gLeechSeedStringIds[] =
{
    [B_MSG_LEECH_SEED_SET]   = STRINGID_PKMNSEEDED,
    [B_MSG_LEECH_SEED_MISS]  = STRINGID_PKMNEVADEDATTACK,
    [B_MSG_LEECH_SEED_FAIL]  = STRINGID_ITDOESNTAFFECT,
    [B_MSG_LEECH_SEED_DRAIN] = STRINGID_PKMNSAPPEDBYLEECHSEED,
    [B_MSG_LEECH_SEED_OOZE]  = STRINGID_ITSUCKEDLIQUIDOOZE,
};

const u16 gRestUsedStringIds[] =
{
    [B_MSG_REST]          = STRINGID_PKMNWENTTOSLEEP,
    [B_MSG_REST_STATUSED] = STRINGID_PKMNSLEPTHEALTHY
};

const u16 gUproarOverTurnStringIds[] =
{
    [B_MSG_UPROAR_CONTINUES] = STRINGID_PKMNMAKINGUPROAR,
    [B_MSG_UPROAR_ENDS]      = STRINGID_PKMNCALMEDDOWN
};

const u16 gWokeUpStringIds[] =
{
    [B_MSG_WOKE_UP]        = STRINGID_PKMNWOKEUP,
    [B_MSG_WOKE_UP_UPROAR] = STRINGID_PKMNWOKEUPINUPROAR
};

const u16 gUproarAwakeStringIds[] =
{
    [B_MSG_CANT_SLEEP_UPROAR]  = STRINGID_PKMNCANTSLEEPINUPROAR2,
    [B_MSG_UPROAR_KEPT_AWAKE]  = STRINGID_UPROARKEPTPKMNAWAKE,
};

const u16 gStatUpStringIds[] =
{
    [B_MSG_ATTACKER_STAT_CHANGED] = STRINGID_ATTACKERSSTATROSE,
    [B_MSG_DEFENDER_STAT_CHANGED] = STRINGID_DEFENDERSSTATROSE,
    [B_MSG_STAT_WONT_CHANGE]      = STRINGID_STATSWONTINCREASE,
    [B_MSG_STAT_CHANGE_EMPTY]     = STRINGID_EMPTYSTRING3,
    [B_MSG_STAT_CHANGED_ITEM]     = STRINGID_USINGITEMSTATOFPKMNROSE,
    [B_MSG_USED_DIRE_HIT]         = STRINGID_PKMNUSEDXTOGETPUMPED,
};

const u16 gStatDownStringIds[] =
{
    [B_MSG_ATTACKER_STAT_CHANGED] = STRINGID_ATTACKERSSTATFELL,
    [B_MSG_DEFENDER_STAT_CHANGED] = STRINGID_DEFENDERSSTATFELL,
    [B_MSG_STAT_WONT_CHANGE]      = STRINGID_STATSWONTDECREASE,
    [B_MSG_STAT_CHANGE_EMPTY]     = STRINGID_EMPTYSTRING3,
    [B_MSG_STAT_CHANGED_ITEM]     = STRINGID_USINGITEMSTATOFPKMNFELL,
};

// Index copied from move's index in sTrappingMoves
const u16 gWrappedStringIds[NUM_TRAPPING_MOVES] =
{
    [B_MSG_WRAPPED_BIND]        = STRINGID_PKMNSQUEEZEDBYBIND,     // MOVE_BIND
    [B_MSG_WRAPPED_WRAP]        = STRINGID_PKMNWRAPPEDBY,          // MOVE_WRAP
    [B_MSG_WRAPPED_FIRE_SPIN]   = STRINGID_PKMNTRAPPEDINVORTEX,    // MOVE_FIRE_SPIN
    [B_MSG_WRAPPED_CLAMP]       = STRINGID_PKMNCLAMPED,            // MOVE_CLAMP
    [B_MSG_WRAPPED_WHIRLPOOL]   = STRINGID_PKMNTRAPPEDINVORTEX,    // MOVE_WHIRLPOOL
    [B_MSG_WRAPPED_SAND_TOMB]   = STRINGID_PKMNTRAPPEDBYSANDTOMB,  // MOVE_SAND_TOMB
    [B_MSG_WRAPPED_MAGMA_STORM] = STRINGID_TRAPPEDBYSWIRLINGMAGMA, // MOVE_MAGMA_STORM
    [B_MSG_WRAPPED_INFESTATION] = STRINGID_INFESTATION,            // MOVE_INFESTATION
    [B_MSG_WRAPPED_SNAP_TRAP]   = STRINGID_PKMNINSNAPTRAP,         // MOVE_SNAP_TRAP
    [B_MSG_WRAPPED_THUNDER_CAGE]= STRINGID_THUNDERCAGETRAPPED,     // MOVE_THUNDER_CAGE
};

const u16 gMistUsedStringIds[] =
{
    [B_MSG_SET_MIST]    = STRINGID_PKMNSHROUDEDINMIST,
    [B_MSG_MIST_FAILED] = STRINGID_BUTITFAILED
};

const u16 gFocusEnergyUsedStringIds[] =
{
    [B_MSG_GETTING_PUMPED]      = STRINGID_PKMNGETTINGPUMPED,
    [B_MSG_FOCUS_ENERGY_FAILED] = STRINGID_BUTITFAILED
};

const u16 gTransformUsedStringIds[] =
{
    [B_MSG_TRANSFORMED]      = STRINGID_PKMNTRANSFORMEDINTO,
    [B_MSG_TRANSFORM_FAILED] = STRINGID_BUTITFAILED
};

const u16 gSubstituteUsedStringIds[] =
{
    [B_MSG_SET_SUBSTITUTE]    = STRINGID_PKMNMADESUBSTITUTE,
    [B_MSG_SUBSTITUTE_FAILED] = STRINGID_TOOWEAKFORSUBSTITUTE
};

const u16 gGotPoisonedStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNWASPOISONED,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNPOISONEDBY
};

const u16 gGotParalyzedStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNWASPARALYZED,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNWASPARALYZEDBY
};

const u16 gFellAsleepStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNFELLASLEEP,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNMADESLEEP,
};

const u16 gGotBurnedStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNWASBURNED,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNBURNEDBY
};

const u16 gGotFrostbiteStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNGOTFROSTBITE,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNGOTFROSTBITE,
};

const u16 gFrostbiteHealedStringIds[] =
{
    [B_MSG_FROSTBITE_HEALED]         = STRINGID_PKMNFROSTBITEHEALED2,
    [B_MSG_FROSTBITE_HEALED_BY_MOVE] = STRINGID_PKMNFROSTBITEHEALEDBY
};

const u16 gGotFrozenStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNWASFROZEN,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNFROZENBY
};

const u16 gGotDefrostedStringIds[] =
{
    [B_MSG_DEFROSTED]         = STRINGID_PKMNWASDEFROSTED2,
    [B_MSG_DEFROSTED_BY_MOVE] = STRINGID_PKMNWASDEFROSTEDBY
};

const u16 gKOFailedStringIds[] =
{
    [B_MSG_KO_MISS]       = STRINGID_ATTACKMISSED,
    [B_MSG_KO_UNAFFECTED] = STRINGID_PKMNUNAFFECTED
};

const u16 gAttractUsedStringIds[] =
{
    [B_MSG_STATUSED]            = STRINGID_PKMNFELLINLOVE,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNSXINFATUATEDY
};

const u16 gAbsorbDrainStringIds[] =
{
    [B_MSG_ABSORB]      = STRINGID_PKMNENERGYDRAINED,
    [B_MSG_ABSORB_OOZE] = STRINGID_ITSUCKEDLIQUIDOOZE
};

const u16 gSportsUsedStringIds[] =
{
    [B_MSG_WEAKEN_ELECTRIC] = STRINGID_ELECTRICITYWEAKENED,
    [B_MSG_WEAKEN_FIRE]     = STRINGID_FIREWEAKENED
};

const u16 gPartyStatusHealStringIds[] =
{
    [B_MSG_BELL]                     = STRINGID_BELLCHIMED,
    [B_MSG_BELL_SOUNDPROOF_ATTACKER] = STRINGID_BELLCHIMED,
    [B_MSG_BELL_SOUNDPROOF_PARTNER]  = STRINGID_BELLCHIMED,
    [B_MSG_BELL_BOTH_SOUNDPROOF]     = STRINGID_BELLCHIMED,
    [B_MSG_SOOTHING_AROMA]           = STRINGID_SOOTHINGAROMA
};

const u16 gFutureMoveUsedStringIds[] =
{
    [B_MSG_FUTURE_SIGHT] = STRINGID_PKMNFORESAWATTACK,
    [B_MSG_DOOM_DESIRE]  = STRINGID_PKMNCHOSEXASDESTINY
};

const u16 gBallEscapeStringIds[] =
{
    [BALL_NO_SHAKES]     = STRINGID_PKMNBROKEFREE,
    [BALL_1_SHAKE]       = STRINGID_ITAPPEAREDCAUGHT,
    [BALL_2_SHAKES]      = STRINGID_AARGHALMOSTHADIT,
    [BALL_3_SHAKES_FAIL] = STRINGID_SHOOTSOCLOSE
};

// Overworld weathers that don't have an associated battle weather default to "It is raining."
const u16 gWeatherStartsStringIds[] =
{
    [WEATHER_NONE]               = STRINGID_ITISRAINING,
    [WEATHER_SUNNY_CLOUDS]       = STRINGID_ITISRAINING,
    [WEATHER_SUNNY]              = STRINGID_ITISRAINING,
    [WEATHER_RAIN]               = STRINGID_ITISRAINING,
    [WEATHER_SNOW]               = (B_OVERWORLD_SNOW >= GEN_9 ? STRINGID_ITISSNOWING : STRINGID_ITISHAILING),
    [WEATHER_RAIN_THUNDERSTORM]  = STRINGID_ITISRAINING,
    [WEATHER_FOG_HORIZONTAL]     = STRINGID_FOGISDEEP,
    [WEATHER_VOLCANIC_ASH]       = STRINGID_ITISRAINING,
    [WEATHER_SANDSTORM]          = STRINGID_SANDSTORMISRAGING,
    [WEATHER_FOG_DIAGONAL]       = STRINGID_FOGISDEEP,
    [WEATHER_UNDERWATER]         = STRINGID_ITISRAINING,
    [WEATHER_SHADE]              = STRINGID_ITISRAINING,
    [WEATHER_DROUGHT]            = STRINGID_SUNLIGHTISHARSH,
    [WEATHER_DOWNPOUR]           = STRINGID_ITISRAINING,
    [WEATHER_UNDERWATER_BUBBLES] = STRINGID_ITISRAINING,
    [WEATHER_ABNORMAL]           = STRINGID_ITISRAINING
};

const u16 gTerrainStartsStringIds[] =
{
    [B_MSG_TERRAIN_SET_MISTY]    = STRINGID_MISTSWIRLSAROUND,
    [B_MSG_TERRAIN_SET_ELECTRIC] = STRINGID_ELECTRICCURRENTISRUNNING,
    [B_MSG_TERRAIN_SET_PSYCHIC]  = STRINGID_SEEMSWEIRD,
    [B_MSG_TERRAIN_SET_GRASSY]   = STRINGID_ISCOVEREDWITHGRASS,
};

const u16 gPrimalWeatherBlocksStringIds[] =
{
    [B_MSG_PRIMAL_WEATHER_FIZZLED_BY_RAIN]      = STRINGID_MOVEFIZZLEDOUTINTHEHEAVYRAIN,
    [B_MSG_PRIMAL_WEATHER_EVAPORATED_IN_SUN]    = STRINGID_MOVEEVAPORATEDINTHEHARSHSUNLIGHT,
};

const u16 gInobedientStringIds[] =
{
    [B_MSG_LOAFING]            = STRINGID_PKMNLOAFING,
    [B_MSG_WONT_OBEY]          = STRINGID_PKMNWONTOBEY,
    [B_MSG_TURNED_AWAY]        = STRINGID_PKMNTURNEDAWAY,
    [B_MSG_PRETEND_NOT_NOTICE] = STRINGID_PKMNPRETENDNOTNOTICE,
    [B_MSG_INCAPABLE_OF_POWER] = STRINGID_PKMNINCAPABLEOFPOWER
};

const u16 gSafariGetNearStringIds[] =
{
    [B_MSG_CREPT_CLOSER]    = STRINGID_CREPTCLOSER,
    [B_MSG_CANT_GET_CLOSER] = STRINGID_CANTGETCLOSER
};

const u16 gSafariPokeblockResultStringIds[] =
{
    [B_MSG_MON_CURIOUS]    = STRINGID_PKMNCURIOUSABOUTX,
    [B_MSG_MON_ENTHRALLED] = STRINGID_PKMNENTHRALLEDBYX,
    [B_MSG_MON_IGNORED]    = STRINGID_PKMNIGNOREDX
};

const u16 CureStatusBerryEffectStringID[] =
{
    [B_MSG_CURED_PARALYSIS] = STRINGID_PKMNSITEMCUREDPARALYSIS,
    [B_MSG_CURED_POISON] = STRINGID_PKMNSITEMCUREDPOISON,
    [B_MSG_CURED_BURN] = STRINGID_PKMNSITEMHEALEDBURN,
    [B_MSG_CURED_FREEEZE] = STRINGID_PKMNSITEMDEFROSTEDIT,
    [B_MSG_CURED_FROSTBITE] = STRINGID_PKMNSITEMHEALEDFROSTBITE,
    [B_MSG_CURED_SLEEP] = STRINGID_PKMNSITEMWOKEIT,
    [B_MSG_CURED_PROBLEM]     = STRINGID_PKMNSITEMCUREDPROBLEM,
    [B_MSG_NORMALIZED_STATUS] = STRINGID_PKMNSITEMNORMALIZEDSTATUS
};

const u16 gItemSwapStringIds[] =
{
    [B_MSG_ITEM_SWAP_TAKEN] = STRINGID_PKMNOBTAINEDX,
    [B_MSG_ITEM_SWAP_GIVEN] = STRINGID_PKMNOBTAINEDX2,
    [B_MSG_ITEM_SWAP_BOTH]  = STRINGID_PKMNOBTAINEDXYOBTAINEDZ
};

const u16 gFlashFireStringIds[] =
{
    [B_MSG_FLASH_FIRE_BOOST]    = STRINGID_PKMNRAISEDFIREPOWERWITH,
    [B_MSG_FLASH_FIRE_NO_BOOST] = STRINGID_PKMNSXMADEYINEFFECTIVE
};

const u16 gCaughtMonStringIds[] =
{
    [B_MSG_SENT_SOMEONES_PC]   = STRINGID_PKMNTRANSFERREDSOMEONESPC,
    [B_MSG_SENT_LANETTES_PC]   = STRINGID_PKMNTRANSFERREDLANETTESPC,
    [B_MSG_SOMEONES_BOX_FULL]  = STRINGID_PKMNBOXSOMEONESPCFULL,
    [B_MSG_LANETTES_BOX_FULL]  = STRINGID_PKMNBOXLANETTESPCFULL,
    [B_MSG_SWAPPED_INTO_PARTY] = STRINGID_PKMNSENTTOPCAFTERCATCH,
};

const u16 gRoomsStringIds[] =
{
    STRINGID_PKMNTWISTEDDIMENSIONS, STRINGID_TRICKROOMENDS,
    STRINGID_SWAPSDEFANDSPDEFOFALLPOKEMON, STRINGID_WONDERROOMENDS,
    STRINGID_HELDITEMSLOSEEFFECTS, STRINGID_MAGICROOMENDS,
    STRINGID_EMPTYSTRING3
};

const u16 gStatusConditionsStringIds[] =
{
    STRINGID_PKMNWASPOISONED, STRINGID_PKMNBADLYPOISONED, STRINGID_PKMNWASBURNED, STRINGID_PKMNWASPARALYZED, STRINGID_PKMNFELLASLEEP, STRINGID_PKMNGOTFROSTBITE
};

const u16 gDamageNonTypesStartStringIds[] =
{
    [B_MSG_TRAPPED_WITH_VINES]  = STRINGID_TEAMTRAPPEDWITHVINES,
    [B_MSG_CAUGHT_IN_VORTEX]    = STRINGID_TEAMCAUGHTINVORTEX,
    [B_MSG_SURROUNDED_BY_FIRE]  = STRINGID_TEAMSURROUNDEDBYFIRE,
    [B_MSG_SURROUNDED_BY_ROCKS] = STRINGID_TEAMSURROUNDEDBYROCKS,
};

const u16 gDamageNonTypesDmgStringIds[] =
{
    [B_MSG_HURT_BY_VINES]        = STRINGID_PKMNHURTBYVINES,
    [B_MSG_HURT_BY_VORTEX]       = STRINGID_PKMNHURTBYVORTEX,
    [B_MSG_BURNING_UP]           = STRINGID_PKMNBURNINGUP,
    [B_MSG_HURT_BY_ROCKS_THROWN] = STRINGID_PKMNHURTBYROCKSTHROWN,
};

const u16 gDefogHazardsStringIds[] =
{
    [HAZARDS_SPIKES] = STRINGID_SPIKESDISAPPEAREDFROMTEAM,
    [HAZARDS_STICKY_WEB] = STRINGID_STICKYWEBDISAPPEAREDFROMTEAM,
    [HAZARDS_TOXIC_SPIKES] = STRINGID_TOXICSPIKESDISAPPEAREDFROMTEAM,
    [HAZARDS_STEALTH_ROCK] = STRINGID_STEALTHROCKDISAPPEAREDFROMTEAM,
    [HAZARDS_STEELSURGE] = STRINGID_SHARPSTEELDISAPPEAREDFROMTEAM,
};

const u16 gSpinHazardsStringIds[] =
{
    [HAZARDS_SPIKES] = STRINGID_PKMNBLEWAWAYSPIKES,
    [HAZARDS_STICKY_WEB] = STRINGID_PKMNBLEWAWAYSTICKYWEB,
    [HAZARDS_TOXIC_SPIKES] = STRINGID_PKMNBLEWAWAYTOXICSPIKES,
    [HAZARDS_STEALTH_ROCK] = STRINGID_PKMNBLEWAWAYSTEALTHROCK,
    [HAZARDS_STEELSURGE] = STRINGID_PKMNBLEWAWAYSHARPSTEEL,
};

const u8 gText_PkmnIsEvolving[] = _("¿Eh?\n¡{STR_VAR_1} está evolucionando!");
const u8 gText_CongratsPkmnEvolved[] = _("¡Enhorabuena! Tu {STR_VAR_1}\nevolucionó a {STR_VAR_2}!{WAIT_SE}\p");
const u8 gText_PkmnStoppedEvolving[] = _("¿Eh? {STR_VAR_1}\nparó la evolución.\p");
const u8 gText_EllipsisQuestionMark[] = _("……?\p");
const u8 gText_WhatWillPkmnDo[] = _("¿Qué hará\n{B_BUFF1}?");
const u8 gText_WhatWillPkmnDo2[] = _("¿Qué hará\n{B_PLAYER_NAME}?");
const u8 gText_WhatWillWallyDo[] = _("¿Qué hará\nWALLY?");
const u8 gText_LinkStandby[] = _("{PAUSE 16}Enlace en espera…");
const u8 gText_BattleMenu[] = _("Luchar{CLEAR_TO 56}Mochila\nPokémon{CLEAR_TO 56}Huir");
const u8 gText_SafariZoneMenu[] = _("Ball{CLEAR_TO 56}{POKEBLOCK}\nAcercarse{CLEAR_TO 56}Huir");
const u8 gText_MoveInterfacePP[] = _("PP ");
const u8 gText_MoveInterfaceType[] = _("TIPO/");
const u8 gText_MoveInterfacePpType[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}PP\nTIPO/");
const u8 gText_MoveInterfaceDynamicColors[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}");
const u8 gText_WhichMoveToForget4[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}¿Qué movimiento\nse debe olvidar?");
const u8 gText_BattleYesNoChoice[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}Sí\nNo");
const u8 gText_BattleSwitchWhich[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}¿Cambiar\ncuál?");
const u8 gText_BattleSwitchWhich2[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}");
const u8 gText_BattleSwitchWhich3[] = _("{UP_ARROW}");
const u8 gText_BattleSwitchWhich4[] = _("{ESCAPE 4}");
const u8 gText_BattleSwitchWhich5[] = _("-");
const u8 gText_SafariBalls[] = _("{HIGHLIGHT DARK_GRAY}Safari Ball");
const u8 gText_SafariBallLeft[] = _("{HIGHLIGHT DARK_GRAY}Restan: $" "{HIGHLIGHT DARK_GRAY}");
const u8 gText_Sleep[] = _("sueño");
const u8 gText_Poison[] = _("veneno");
const u8 gText_Burn[] = _("quemadura");
const u8 gText_Paralysis[] = _("parálisis");
const u8 gText_Ice[] = _("hielo");
const u8 gText_Confusion[] = _("confusión");
const u8 gText_Love[] = _("amor");
const u8 gText_SpaceAndSpace[] = _(" y ");
const u8 gText_CommaSpace[] = _(", ");
const u8 gText_Space2[] = _(" ");
const u8 gText_LineBreak[] = _("\l");
const u8 gText_NewLine[] = _("\n");
const u8 gText_Are[] = _("son");
const u8 gText_Are2[] = _("son");
const u8 gText_BadEgg[] = _("Huevo Malo");
const u8 gText_BattleWallyName[] = _("WALLY");
const u8 gText_Win[] = _("{HIGHLIGHT TRANSPARENT}Gana");
const u8 gText_Loss[] = _("{HIGHLIGHT TRANSPARENT}Pierde");
const u8 gText_Draw[] = _("{HIGHLIGHT TRANSPARENT}Empate");
static const u8 sText_SpaceIs[] = _(" es");
static const u8 sText_ApostropheS[] = _(" de");
const u8 gText_BattleTourney[] = _("TORNEO BATALLA");

const u8 *const gRoundsStringTable[DOME_ROUNDS_COUNT] =
{
    [DOME_ROUND1]    = COMPOUND_STRING("Ronda 1"),
    [DOME_ROUND2]    = COMPOUND_STRING("Ronda 2"),
    [DOME_SEMIFINAL] = COMPOUND_STRING("Semifinal"),
    [DOME_FINAL]     = COMPOUND_STRING("Final"),
};

const u8 gText_TheGreatNewHope[] = _("¡La gran nueva esperanza!\p");
const u8 gText_WillChampionshipDreamComeTrue[] = _("¿Se hará realidad el sueño del campeonato?!\p");
const u8 gText_AFormerChampion[] = _("¡Un antiguo campeón!\p");
const u8 gText_ThePreviousChampion[] = _("¡El campeón anterior!\p");
const u8 gText_TheUnbeatenChampion[] = _("¡El campeón invicto!\p");
const u8 gText_PlayerMon1Name[] = _("{B_PLAYER_MON1_NAME}");
const u8 gText_Vs[] = _("VS");
const u8 gText_OpponentMon1Name[] = _("{B_OPPONENT_MON1_NAME}");
const u8 gText_Mind[] = _("Mente");
const u8 gText_Skill[] = _("Técnica");
const u8 gText_Body[] = _("Cuerpo");
const u8 gText_Judgment[] = _("{B_BUFF1}{CLEAR 13}Juicio{CLEAR 13}{B_BUFF2}");
static const u8 sText_TwoTrainersSentPkmn[] = _("¡{B_TRAINER1_NAME_WITH_CLASS} sacó a {B_OPPONENT_MON1_NAME}!\p¡{B_TRAINER2_NAME_WITH_CLASS} sacó a {B_OPPONENT_MON2_NAME}!");
static const u8 sText_Trainer2SentOutPkmn[] = _("¡{B_TRAINER2_NAME_WITH_CLASS} sacó a {B_BUFF1}!");
static const u8 sText_TwoTrainersWantToBattle[] = _("¡{B_TRAINER1_NAME_WITH_CLASS} y {B_TRAINER2_NAME_WITH_CLASS} te desafían!\p");
static const u8 sText_InGamePartnerSentOutZGoN[] = _("¡{B_PARTNER_NAME_WITH_CLASS} sacó a {B_PLAYER_MON2_NAME}! ¡Adelante, {B_PLAYER_MON1_NAME}!");
static const u8 sText_InGamePartnerSentOutPkmn2[] = _("¡{B_PARTNER_NAME_WITH_CLASS} sacó a {B_PLAYER_MON2_NAME}!");
static const u8 sText_InGamePartnerWithdrewPkmn2[] = _("¡{B_PARTNER_NAME_WITH_CLASS} retiró a {B_PLAYER_MON2_NAME}!");

const u16 gBattlePalaceFlavorTextTable[] =
{
    [B_MSG_GLINT_IN_EYE]   = STRINGID_GLINTAPPEARSINEYE,
    [B_MSG_GETTING_IN_POS] = STRINGID_PKMNGETTINGINTOPOSITION,
    [B_MSG_GROWL_DEEPLY]   = STRINGID_PKMNBEGANGROWLINGDEEPLY,
    [B_MSG_EAGER_FOR_MORE] = STRINGID_PKMNEAGERFORMORE,
};

const u8 *const gRefereeStringsTable[] =
{
    [B_MSG_REF_NOTHING_IS_DECIDED] = COMPOUND_STRING("ÁRBITRO: ¡Si en 3 turnos no se decide, iremos a juicio!"),
    [B_MSG_REF_THATS_IT]           = COMPOUND_STRING("ÁRBITRO: ¡Eso es todo! ¡Ahora iremos a juicio para decidir al ganador!"),
    [B_MSG_REF_JUDGE_MIND]         = COMPOUND_STRING("ÁRBITRO: Categoría 1, Mente. ¡El POKéMON con más agallas!\p"),
    [B_MSG_REF_JUDGE_SKILL]        = COMPOUND_STRING("ÁRBITRO: Categoría 2, Técnica. ¡El POKéMON que mejor usó sus movimientos!\p"),
    [B_MSG_REF_JUDGE_BODY]         = COMPOUND_STRING("ÁRBITRO: Categoría 3, Cuerpo. ¡El POKéMON con más vitalidad!\p"),
    [B_MSG_REF_PLAYER_WON]         = COMPOUND_STRING("ÁRBITRO: Juicio: {B_BUFF1} a {B_BUFF2}. ¡El ganador es {B_PLAYER_MON1_NAME} de {B_PLAYER_NAME}!\p"),
    [B_MSG_REF_OPPONENT_WON]       = COMPOUND_STRING("ÁRBITRO: Juicio: {B_BUFF1} a {B_BUFF2}. ¡El ganador es {B_OPPONENT_MON1_NAME} de {B_TRAINER1_NAME}!\p"),
    [B_MSG_REF_DRAW]               = COMPOUND_STRING("ÁRBITRO: Juicio: 3 a 3. ¡Empate!\p"),
    [B_MSG_REF_COMMENCE_BATTLE]    = COMPOUND_STRING("ÁRBITRO: ¡{B_PLAYER_MON1_NAME} VS {B_OPPONENT_MON1_NAME}! ¡Empieza el combate!"),
};

static const u8 sText_Trainer1Fled[] = _( "{PLAY_SE SE_FLEE}¡{B_TRAINER1_NAME_WITH_CLASS} huyó!");
static const u8 sText_PlayerLostAgainstTrainer1[] = _("¡Has perdido contra {B_TRAINER1_NAME_WITH_CLASS}!");
static const u8 sText_PlayerBattledToDrawTrainer1[] = _("¡Empataste contra {B_TRAINER1_NAME_WITH_CLASS}!");
const u8 gText_RecordBattleToPass[] = _("¿Quieres guardar tu combate\nen el Pase de la Frontera?");
const u8 gText_BattleRecordedOnPass[] = _("El resultado del combate de {B_PLAYER_NAME}\nse registró en el Pase de la Frontera.");
static const u8 sText_LinkTrainerWantsToBattlePause[] = _("¡{B_LINK_OPPONENT1_NAME} te desafía!\p");
static const u8 sText_TwoLinkTrainersWantToBattlePause[] = _("¡{B_LINK_OPPONENT1_NAME} y {B_LINK_OPPONENT2_NAME} te desafían!\p");
static const u8 sText_Your1[] = _("Tu");
static const u8 sText_Opposing1[] = _("El rival");
static const u8 sText_Your2[] = _("tu");
static const u8 sText_Opposing2[] = _("el rival");

// This is four lists of moves which use a different attack string in Japanese
// to the default. See the documentation for ChooseTypeOfMoveUsedString for more detail.
static const u16 sGrammarMoveUsedTable[] =
{
    MOVE_SWORDS_DANCE, MOVE_STRENGTH, MOVE_GROWTH,
    MOVE_HARDEN, MOVE_MINIMIZE, MOVE_SMOKESCREEN,
    MOVE_WITHDRAW, MOVE_DEFENSE_CURL, MOVE_EGG_BOMB,
    MOVE_SMOG, MOVE_BONE_CLUB, MOVE_FLASH, MOVE_SPLASH,
    MOVE_ACID_ARMOR, MOVE_BONEMERANG, MOVE_REST, MOVE_SHARPEN,
    MOVE_SUBSTITUTE, MOVE_MIND_READER, MOVE_SNORE,
    MOVE_PROTECT, MOVE_SPIKES, MOVE_ENDURE, MOVE_ROLLOUT,
    MOVE_SWAGGER, MOVE_SLEEP_TALK, MOVE_HIDDEN_POWER,
    MOVE_PSYCH_UP, MOVE_EXTREME_SPEED, MOVE_FOLLOW_ME,
    MOVE_TRICK, MOVE_ASSIST, MOVE_INGRAIN, MOVE_KNOCK_OFF,
    MOVE_CAMOUFLAGE, MOVE_ASTONISH, MOVE_ODOR_SLEUTH,
    MOVE_GRASS_WHISTLE, MOVE_SHEER_COLD, MOVE_MUDDY_WATER,
    MOVE_IRON_DEFENSE, MOVE_BOUNCE, 0,

    MOVE_TELEPORT, MOVE_RECOVER, MOVE_BIDE, MOVE_AMNESIA,
    MOVE_FLAIL, MOVE_TAUNT, MOVE_BULK_UP, 0,

    MOVE_MEDITATE, MOVE_AGILITY, MOVE_MIMIC, MOVE_DOUBLE_TEAM,
    MOVE_BARRAGE, MOVE_TRANSFORM, MOVE_STRUGGLE, MOVE_SCARY_FACE,
    MOVE_CHARGE, MOVE_WISH, MOVE_BRICK_BREAK, MOVE_YAWN,
    MOVE_FEATHER_DANCE, MOVE_TEETER_DANCE, MOVE_MUD_SPORT,
    MOVE_FAKE_TEARS, MOVE_WATER_SPORT, MOVE_CALM_MIND, 0,

    MOVE_POUND, MOVE_SCRATCH, MOVE_VISE_GRIP,
    MOVE_WING_ATTACK, MOVE_FLY, MOVE_BIND, MOVE_SLAM,
    MOVE_HORN_ATTACK, MOVE_WRAP, MOVE_THRASH, MOVE_TAIL_WHIP,
    MOVE_LEER, MOVE_BITE, MOVE_GROWL, MOVE_ROAR,
    MOVE_SING, MOVE_PECK, MOVE_ABSORB, MOVE_STRING_SHOT,
    MOVE_EARTHQUAKE, MOVE_FISSURE, MOVE_DIG, MOVE_TOXIC,
    MOVE_SCREECH, MOVE_METRONOME, MOVE_LICK, MOVE_CLAMP,
    MOVE_CONSTRICT, MOVE_POISON_GAS, MOVE_BUBBLE,
    MOVE_SLASH, MOVE_SPIDER_WEB, MOVE_NIGHTMARE, MOVE_CURSE,
    MOVE_FORESIGHT, MOVE_CHARM, MOVE_ATTRACT, MOVE_ROCK_SMASH,
    MOVE_UPROAR, MOVE_SPIT_UP, MOVE_SWALLOW, MOVE_TORMENT,
    MOVE_FLATTER, MOVE_ROLE_PLAY, MOVE_ENDEAVOR, MOVE_TICKLE,
    MOVE_COVET, 0
};

static const u8 sText_EmptyStatus[] = _("$$$$$$$");

static const struct BattleWindowText sTextOnWindowsInfo_Normal[] =
{
    [B_WIN_MSG] = {
        .fillValue = PIXEL_FILL(0xF),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 1,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 6,
    },
    [B_WIN_ACTION_PROMPT] = {
        .fillValue = PIXEL_FILL(0xF),
        .fontId = FONT_NORMAL,
        .x = 1,
        .y = 1,
        .speed = 0,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 6,
    },
    [B_WIN_ACTION_MENU] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_MOVE_NAME_1] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_MOVE_NAME_2] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_MOVE_NAME_3] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_MOVE_NAME_4] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_PP] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = B_SHOW_EFFECTIVENESS != SHOW_EFFECTIVENESS_NEVER ? 13 : 12,
        .bgColor = 14,
        .shadowColor = B_SHOW_EFFECTIVENESS != SHOW_EFFECTIVENESS_NEVER ? 15 : 11,
    },
    [B_WIN_DUMMY] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_PP_REMAINING] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 2,
        .y = 1,
        .speed = 0,
        .fgColor = 12,
        .bgColor = 14,
        .shadowColor = 11,
    },
    [B_WIN_MOVE_TYPE] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_SWITCH_PROMPT] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_YESNO] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_LEVEL_UP_BOX] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_LEVEL_UP_BANNER] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = 32,
        .y = 1,
        .speed = 0,
        .fgColor = 1,
        .shadowColor = 2,
    },
    [B_WIN_VS_PLAYER] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_VS_OPPONENT] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_1] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_2] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_3] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_VS_MULTI_PLAYER_4] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_VS_OUTCOME_DRAW] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 1,
        .shadowColor = 6,
    },
    [B_WIN_VS_OUTCOME_LEFT] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 1,
        .shadowColor = 6,
    },
    [B_WIN_VS_OUTCOME_RIGHT] = {
        .fillValue = PIXEL_FILL(0x0),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 1,
        .shadowColor = 6,
    },
    [B_WIN_MOVE_DESCRIPTION] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .speed = 0,
        .fgColor = TEXT_DYNAMIC_COLOR_4,
        .bgColor = TEXT_DYNAMIC_COLOR_5,
        .shadowColor = TEXT_DYNAMIC_COLOR_6,
    },
};

static const struct BattleWindowText sTextOnWindowsInfo_Arena[] =
{
    [B_WIN_MSG] = {
        .fillValue = PIXEL_FILL(0xF),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 1,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 6,
    },
    [B_WIN_ACTION_PROMPT] = {
        .fillValue = PIXEL_FILL(0xF),
        .fontId = FONT_NORMAL,
        .x = 1,
        .y = 1,
        .speed = 0,
        .fgColor = 1,
        .bgColor = 15,
        .shadowColor = 6,
    },
    [B_WIN_ACTION_MENU] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_MOVE_NAME_1] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_MOVE_NAME_2] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_MOVE_NAME_3] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_MOVE_NAME_4] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_PP] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = B_SHOW_EFFECTIVENESS != SHOW_EFFECTIVENESS_NEVER ? 13 : 12,
        .bgColor = 14,
        .shadowColor = B_SHOW_EFFECTIVENESS != SHOW_EFFECTIVENESS_NEVER ? 15 : 11,
    },
    [B_WIN_DUMMY] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_PP_REMAINING] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 2,
        .y = 1,
        .speed = 0,
        .fgColor = 12,
        .bgColor = 14,
        .shadowColor = 11,
    },
    [B_WIN_MOVE_TYPE] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_SWITCH_PROMPT] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_YESNO] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_LEVEL_UP_BOX] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [B_WIN_LEVEL_UP_BANNER] = {
        .fillValue = PIXEL_FILL(0),
        .fontId = FONT_NORMAL,
        .x = 32,
        .y = 1,
        .speed = 0,
        .fgColor = 1,
        .shadowColor = 2,
    },
    [ARENA_WIN_PLAYER_NAME] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 1,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [ARENA_WIN_VS] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [ARENA_WIN_OPPONENT_NAME] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [ARENA_WIN_MIND] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [ARENA_WIN_SKILL] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [ARENA_WIN_BODY] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [ARENA_WIN_JUDGMENT_TITLE] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NORMAL,
        .x = -1,
        .y = 1,
        .speed = 0,
        .fgColor = 13,
        .bgColor = 14,
        .shadowColor = 15,
    },
    [ARENA_WIN_JUDGMENT_TEXT] = {
        .fillValue = PIXEL_FILL(0x1),
        .fontId = FONT_NORMAL,
        .x = 0,
        .y = 1,
        .speed = 1,
        .fgColor = 2,
        .bgColor = 1,
        .shadowColor = 3,
    },
    [B_WIN_MOVE_DESCRIPTION] = {
        .fillValue = PIXEL_FILL(0xE),
        .fontId = FONT_NARROW,
        .x = 0,
        .y = 1,
        .letterSpacing = 0,
        .lineSpacing = 0,
        .speed = 0,
        .fgColor = TEXT_DYNAMIC_COLOR_4,
        .bgColor = TEXT_DYNAMIC_COLOR_5,
        .shadowColor = TEXT_DYNAMIC_COLOR_6,
    },
};

static const struct BattleWindowText *const sBattleTextOnWindowsInfo[] =
{
    [B_WIN_TYPE_NORMAL] = sTextOnWindowsInfo_Normal,
    [B_WIN_TYPE_ARENA]  = sTextOnWindowsInfo_Arena
};

static const u8 sRecordedBattleTextSpeeds[] = {8, 4, 1, 0};

void BufferStringBattle(enum StringID stringID, u32 battler)
{
    s32 i;
    const u8 *stringPtr = NULL;

    gBattleMsgDataPtr = (struct BattleMsgData *)(&gBattleResources->bufferA[battler][4]);
    gLastUsedItem = gBattleMsgDataPtr->lastItem;
    gLastUsedAbility = gBattleMsgDataPtr->lastAbility;
    gBattleScripting.battler = gBattleMsgDataPtr->scrActive;
    gBattleStruct->scriptPartyIdx = gBattleMsgDataPtr->bakScriptPartyIdx;
    gBattleStruct->hpScale = gBattleMsgDataPtr->hpScale;
    gPotentialItemEffectBattler = gBattleMsgDataPtr->itemEffectBattler;
    gBattleStruct->stringMoveType = gBattleMsgDataPtr->moveType;

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        sBattlerAbilities[i] = gBattleMsgDataPtr->abilities[i];
    }
    for (i = 0; i < TEXT_BUFF_ARRAY_COUNT; i++)
    {
        gBattleTextBuff1[i] = gBattleMsgDataPtr->textBuffs[0][i];
        gBattleTextBuff2[i] = gBattleMsgDataPtr->textBuffs[1][i];
        gBattleTextBuff3[i] = gBattleMsgDataPtr->textBuffs[2][i];
    }

    switch (stringID)
    {
    case STRINGID_INTROMSG: // first battle msg
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        {
            if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
            {
                if (gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI)
                {
                    stringPtr = sText_TwoTrainersWantToBattle;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                {
                    if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
                    {
                        if (TESTING && gBattleTypeFlags & BATTLE_TYPE_MULTI)
                        {
                            if (!(gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS))
                                stringPtr = sText_Trainer1WantsToBattle;
                            else
                                stringPtr = sText_TwoTrainersWantToBattle;
                        }
                        else if (TESTING && gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                        {
                            stringPtr = sText_TwoTrainersWantToBattle;
                        }
                        else if (!(gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS))
                        {
                            stringPtr = sText_LinkTrainerWantsToBattlePause;
                        }
                        else
                        {
                            stringPtr = sText_TwoLinkTrainersWantToBattlePause;
                        }
                    }
                    else
                    {
                        stringPtr = sText_TwoLinkTrainersWantToBattle;
                    }
                }
                else
                {
                    if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_UNION_ROOM)
                        stringPtr = sText_Trainer1WantsToBattle;
                    else if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
                        stringPtr = sText_LinkTrainerWantsToBattlePause;
                    else
                        stringPtr = sText_LinkTrainerWantsToBattle;
                }
            }
            else
            {
                if (BATTLE_TWO_VS_ONE_OPPONENT)
                    stringPtr = sText_Trainer1WantsToBattle;
                else if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER))
                    stringPtr = sText_TwoTrainersWantToBattle;
                else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                    stringPtr = sText_TwoTrainersWantToBattle;
                else
                    stringPtr = sText_Trainer1WantsToBattle;
            }
        }
        else
        {
            if (gBattleTypeFlags & BATTLE_TYPE_LEGENDARY)
                stringPtr = sText_LegendaryPkmnAppeared;
            else if (IsDoubleBattle() && IsValidForBattle(GetBattlerMon(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT))))
                stringPtr = sText_TwoWildPkmnAppeared;
            else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL)
                stringPtr = sText_WildPkmnAppearedPause;
            else
                stringPtr = sText_WildPkmnAppeared;
        }
        break;
    case STRINGID_INTROSENDOUT: // poke first send-out
        if (IsOnPlayerSide(battler))
        {
            if (IsDoubleBattle() && IsValidForBattle(GetBattlerMon(BATTLE_PARTNER(battler))))
            {
                if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
                    stringPtr = sText_InGamePartnerSentOutZGoN;
                else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                    stringPtr = sText_GoTwoPkmn;
                else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    stringPtr = sText_LinkPartnerSentOutPkmnGoPkmn;
                else
                    stringPtr = sText_GoTwoPkmn;
            }
            else
            {
                stringPtr = sText_GoPkmn;
            }
        }
        else
        {
            if (IsDoubleBattle() && IsValidForBattle(GetBattlerMon(BATTLE_PARTNER(battler))))
            {
                if (BATTLE_TWO_VS_ONE_OPPONENT)
                    stringPtr = sText_Trainer1SentOutTwoPkmn;
                else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                    stringPtr = sText_TwoTrainersSentPkmn;
                else if (gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI)
                    stringPtr = sText_TwoTrainersSentPkmn;
                else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    stringPtr = sText_TwoLinkTrainersSentOutPkmn;
                else if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
                    stringPtr = sText_LinkTrainerSentOutTwoPkmn;
                else
                    stringPtr = sText_Trainer1SentOutTwoPkmn;
            }
            else
            {
                if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK)))
                    stringPtr = sText_Trainer1SentOutPkmn;
                else if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_UNION_ROOM)
                    stringPtr = sText_Trainer1SentOutPkmn;
                else
                    stringPtr = sText_LinkTrainerSentOutPkmn;
            }
        }
        break;
    case STRINGID_RETURNMON: // sending poke to ball msg
        if (IsOnPlayerSide(battler))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER && GetBattlerPosition(battler) == B_POSITION_PLAYER_RIGHT)
                stringPtr = sText_InGamePartnerWithdrewPkmn2;
            else if (*(&gBattleStruct->hpScale) == 0)
                stringPtr = sText_PkmnThatsEnough;
            else if (*(&gBattleStruct->hpScale) == 1 || IsDoubleBattle())
                stringPtr = sText_PkmnComeBack;
            else if (*(&gBattleStruct->hpScale) == 2)
                stringPtr = sText_PkmnOkComeBack;
            else
                stringPtr = sText_PkmnGoodComeBack;
        }
        else
        {
            if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_LINK_OPPONENT || gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
            {
                if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    stringPtr = sText_LinkTrainer2WithdrewPkmn;
                else
                    stringPtr = sText_LinkTrainer1WithdrewPkmn;
            }
            else
            {
                if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                {
                    if (GetBattlerPosition(battler) == B_POSITION_OPPONENT_LEFT)
                        stringPtr = sText_Trainer1WithdrewPkmn;
                    else
                        stringPtr = sText_Trainer2WithdrewPkmn;

                }
                else
                {
                    stringPtr = sText_Trainer1WithdrewPkmn;
                }
            }
        }
        break;
    case STRINGID_SWITCHINMON: // switch-in msg
        if (IsOnPlayerSide(gBattleScripting.battler))
        {
            if ((gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER) && (GetBattlerAtPosition(gBattleScripting.battler) == 2))
                stringPtr = sText_InGamePartnerSentOutPkmn2;
            else if (*(&gBattleStruct->hpScale) == 0 || IsDoubleBattle())
                stringPtr = sText_GoPkmn2;
            else if (*(&gBattleStruct->hpScale) == 1)
                stringPtr = sText_DoItPkmn;
            else if (*(&gBattleStruct->hpScale) == 2)
                stringPtr = sText_GoForItPkmn;
            else
                stringPtr = sText_YourFoesWeakGetEmPkmn;
        }
        else
        {
            if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
            {
                if (gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI)
                {
                    if (gBattleScripting.battler == 1)
                        stringPtr = sText_Trainer1SentOutPkmn2;
                    else
                        stringPtr = sText_Trainer2SentOutPkmn;
                }
                else
                {
                    if (TESTING && gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    {
                        if (gBattleScripting.battler == 1)
                        {
                            stringPtr = sText_Trainer1SentOutPkmn;
                        }
                        else
                        {
                            if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                                stringPtr = sText_Trainer2SentOutPkmn;
                            else
                                stringPtr = sText_Trainer1SentOutPkmn2;
                        }
                    }
                    else if (TESTING && gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                    {
                        if (gBattleScripting.battler == 1)
                            stringPtr = sText_Trainer1SentOutPkmn;
                        else
                            stringPtr = sText_Trainer2SentOutPkmn;
                    }
                    else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    {
                        stringPtr = sText_LinkTrainerMultiSentOutPkmn;
                    }
                    else if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_UNION_ROOM)
                    {
                        stringPtr = sText_Trainer1SentOutPkmn2;
                    }
                    else
                    {
                        stringPtr = sText_LinkTrainerSentOutPkmn2;
                    }
                }
            }
            else
            {
                if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                {
                    if (gBattleScripting.battler == 1)
                        stringPtr = sText_Trainer1SentOutPkmn2;
                    else
                        stringPtr = sText_Trainer2SentOutPkmn;
                }
                else
                {
                    stringPtr = sText_Trainer1SentOutPkmn2;
                }
            }
        }
        break;
    case STRINGID_USEDMOVE: // Pokémon used a move msg
        if (gBattleMsgDataPtr->currentMove >= MOVES_COUNT
         && !IsZMove(gBattleMsgDataPtr->currentMove)
         && !IsMaxMove(gBattleMsgDataPtr->currentMove))
            StringCopy(gBattleTextBuff3, gTypesInfo[*(&gBattleStruct->stringMoveType)].generic);
        else
            StringCopy(gBattleTextBuff3, GetMoveName(gBattleMsgDataPtr->currentMove));
        stringPtr = sText_AttackerUsedX;
        break;
    case STRINGID_BATTLEEND: // battle end
        if (gBattleTextBuff1[0] & B_OUTCOME_LINK_BATTLE_RAN)
        {
            gBattleTextBuff1[0] &= ~(B_OUTCOME_LINK_BATTLE_RAN);
            if (!IsOnPlayerSide(battler) && gBattleTextBuff1[0] != B_OUTCOME_DREW)
                gBattleTextBuff1[0] ^= (B_OUTCOME_LOST | B_OUTCOME_WON);

            if (gBattleTextBuff1[0] == B_OUTCOME_LOST || gBattleTextBuff1[0] == B_OUTCOME_DREW)
                stringPtr = sText_GotAwaySafely;
            else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                stringPtr = sText_TwoWildFled;
            else
                stringPtr = sText_WildFled;
        }
        else
        {
            if (!IsOnPlayerSide(battler) && gBattleTextBuff1[0] != B_OUTCOME_DREW)
                gBattleTextBuff1[0] ^= (B_OUTCOME_LOST | B_OUTCOME_WON);

            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                switch (gBattleTextBuff1[0])
                {
                case B_OUTCOME_WON:
                    if (gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI)
                        stringPtr = sText_TwoInGameTrainersDefeated;
                    else
                        stringPtr = sText_TwoLinkTrainersDefeated;
                    break;
                case B_OUTCOME_LOST:
                    stringPtr = sText_PlayerLostToTwo;
                    break;
                case B_OUTCOME_DREW:
                    stringPtr = sText_PlayerBattledToDrawVsTwo;
                    break;
                }
            }
            else if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_UNION_ROOM)
            {
                switch (gBattleTextBuff1[0])
                {
                case B_OUTCOME_WON:
                    stringPtr = sText_PlayerDefeatedLinkTrainerTrainer1;
                    break;
                case B_OUTCOME_LOST:
                    stringPtr = sText_PlayerLostAgainstTrainer1;
                    break;
                case B_OUTCOME_DREW:
                    stringPtr = sText_PlayerBattledToDrawTrainer1;
                    break;
                }
            }
            else
            {
                switch (gBattleTextBuff1[0])
                {
                case B_OUTCOME_WON:
                    stringPtr = sText_PlayerDefeatedLinkTrainer;
                    break;
                case B_OUTCOME_LOST:
                    stringPtr = sText_PlayerLostAgainstLinkTrainer;
                    break;
                case B_OUTCOME_DREW:
                    stringPtr = sText_PlayerBattledToDrawLinkTrainer;
                    break;
                }
            }
        }
        break;
    case STRINGID_TRAINERSLIDE:
        stringPtr = gBattleStruct->trainerSlideMsg;
        break;
    default: // load a string from the table
        if (stringID >= STRINGID_COUNT)
        {
            gDisplayedStringBattle[0] = EOS;
            return;
        }
        else
        {
            stringPtr = gBattleStringsTable[stringID];
        }
        break;
    }

    BattleStringExpandPlaceholdersToDisplayedString(stringPtr);
}

u32 BattleStringExpandPlaceholdersToDisplayedString(const u8 *src)
{
#ifndef NDEBUG
    u32 j, strWidth;
    u32 dstID = BattleStringExpandPlaceholders(src, gDisplayedStringBattle, sizeof(gDisplayedStringBattle));
    for (j = 1;; j++)
    {
        strWidth = GetStringLineWidth(0, gDisplayedStringBattle, 0, j, sizeof(gDisplayedStringBattle));
        if (strWidth == 0)
            break;
    }
    return dstID;
#else
    return BattleStringExpandPlaceholders(src, gDisplayedStringBattle, sizeof(gDisplayedStringBattle));
#endif
}

static const u8 *TryGetStatusString(u8 *src)
{
    u32 i;
    u8 status[8];
    u32 chars1, chars2;
    u8 *statusPtr;

    memcpy(status, sText_EmptyStatus, min(ARRAY_COUNT(status), ARRAY_COUNT(sText_EmptyStatus)));

    statusPtr = status;
    for (i = 0; i < ARRAY_COUNT(status); i++)
    {
        if (*src == EOS) break; // one line required to match -g
        *statusPtr = *src;
        src++;
        statusPtr++;
    }

    chars1 = *(u32 *)(&status[0]);
    chars2 = *(u32 *)(&status[4]);

    for (i = 0; i < ARRAY_COUNT(gStatusConditionStringsTable); i++)
    {
        if (chars1 == *(u32 *)(&gStatusConditionStringsTable[i][0][0])
            && chars2 == *(u32 *)(&gStatusConditionStringsTable[i][0][4]))
            return gStatusConditionStringsTable[i][1];
    }
    return NULL;
}

static void GetBattlerNick(u32 battler, u8 *dst)
{
    struct Pokemon *illusionMon = GetIllusionMonPtr(battler);
    struct Pokemon *mon = GetBattlerMon(battler);

    if (illusionMon != NULL)
        mon = illusionMon;
    GetMonData(mon, MON_DATA_NICKNAME, dst);
    StringGet_Nickname(dst);
}

#define HANDLE_NICKNAME_STRING_CASE(battler)                            \
    if (!IsOnPlayerSide(battler))                       \
    {                                                                   \
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)                     \
            toCpy = sText_FoePkmnPrefix;                                \
        else                                                            \
            toCpy = sText_WildPkmnPrefix;                               \
        while (*toCpy != EOS)                                           \
        {                                                               \
            dst[dstID] = *toCpy;                                        \
            dstID++;                                                    \
            toCpy++;                                                    \
        }                                                               \
    }                                                                   \
    GetBattlerNick(battler, text);                                      \
    toCpy = text;

#define HANDLE_NICKNAME_STRING_LOWERCASE(battler)                       \
    if (!IsOnPlayerSide(battler))                       \
    {                                                                   \
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)                     \
            toCpy = sText_FoePkmnPrefixLower;                           \
        else                                                            \
            toCpy = sText_WildPkmnPrefixLower;                          \
        while (*toCpy != EOS)                                           \
        {                                                               \
            dst[dstID] = *toCpy;                                        \
            dstID++;                                                    \
            toCpy++;                                                    \
        }                                                               \
    }                                                                   \
    GetBattlerNick(battler, text);                                      \
    toCpy = text;

static const u8 *BattleStringGetOpponentNameByTrainerId(u16 trainerId, u8 *text, u8 multiplayerId, u8 battler)
{
    const u8 *toCpy = NULL;

    if (gBattleTypeFlags & BATTLE_TYPE_SECRET_BASE)
    {
        u32 i;
        for (i = 0; i < ARRAY_COUNT(gBattleResources->secretBase->trainerName); i++)
            text[i] = gBattleResources->secretBase->trainerName[i];
        text[i] = EOS;
        ConvertInternationalString(text, gBattleResources->secretBase->language);
        toCpy = text;
    }
    else if (trainerId == TRAINER_UNION_ROOM)
    {
        toCpy = gLinkPlayers[multiplayerId ^ BIT_SIDE].name;
    }
    else if (trainerId == TRAINER_LINK_OPPONENT)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            toCpy = gLinkPlayers[GetBattlerMultiplayerId(battler)].name;
        else
            toCpy = gLinkPlayers[GetBattlerMultiplayerId(battler) & BIT_SIDE].name;
    }
    else if (trainerId == TRAINER_FRONTIER_BRAIN)
    {
        CopyFrontierBrainTrainerName(text);
        toCpy = text;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
    {
        GetFrontierTrainerName(text, trainerId);
        toCpy = text;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
    {
        GetTrainerHillTrainerName(text, trainerId);
        toCpy = text;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
    {
        GetEreaderTrainerName(text);
        toCpy = text;
    }
    else
    {
        toCpy = GetTrainerNameFromId(trainerId);
    }

    return toCpy;
}

static const u8 *BattleStringGetOpponentName(u8 *text, u8 multiplayerId, u8 battler)
{
    const u8 *toCpy = NULL;

    switch (GetBattlerPosition(battler))
    {
    case B_POSITION_OPPONENT_LEFT:
        toCpy = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentA, text, multiplayerId, battler);
        break;
    case B_POSITION_OPPONENT_RIGHT:
        if (gBattleTypeFlags & (BATTLE_TYPE_TWO_OPPONENTS | BATTLE_TYPE_MULTI) && !BATTLE_TWO_VS_ONE_OPPONENT)
            toCpy = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentB, text, multiplayerId, battler);
        else
            toCpy = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentA, text, multiplayerId, battler);
        break;
    }

    return toCpy;
}

static const u8 *BattleStringGetPlayerName(u8 *text, u8 battler)
{
    const u8 *toCpy = NULL;

    switch (GetBattlerPosition(battler))
    {
    case B_POSITION_PLAYER_LEFT:
        if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
            toCpy = gLinkPlayers[0].name;
        else
            toCpy = gSaveBlock2Ptr->playerName;
        break;
    case B_POSITION_PLAYER_RIGHT:
        if (((gBattleTypeFlags & BATTLE_TYPE_RECORDED) && !(gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER)))
            || gTestRunnerEnabled)
        {
            toCpy = gLinkPlayers[0].name;
        }
        else if ((gBattleTypeFlags & BATTLE_TYPE_LINK) && gBattleTypeFlags & (BATTLE_TYPE_RECORDED | BATTLE_TYPE_MULTI))
        {
            toCpy = gLinkPlayers[2].name;
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
        {
            GetFrontierTrainerName(text, gPartnerTrainerId);
            toCpy = text;
        }
        else
        {
            toCpy = gSaveBlock2Ptr->playerName;
        }
        break;
    }

    return toCpy;
}

static const u8 *BattleStringGetTrainerName(u8 *text, u8 multiplayerId, u8 battler)
{
    if (IsOnPlayerSide(battler))
        return BattleStringGetPlayerName(text, battler);
    else
        return BattleStringGetOpponentName(text, multiplayerId, battler);
}

static const u8 *BattleStringGetOpponentClassByTrainerId(u16 trainerId)
{
    const u8 *toCpy;

    if (gBattleTypeFlags & BATTLE_TYPE_SECRET_BASE)
        toCpy = gTrainerClasses[GetSecretBaseTrainerClass()].name;
    else if (trainerId == TRAINER_UNION_ROOM)
        toCpy = gTrainerClasses[GetUnionRoomTrainerClass()].name;
    else if (trainerId == TRAINER_FRONTIER_BRAIN)
        toCpy = gTrainerClasses[GetFrontierBrainTrainerClass()].name;
    else if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
        toCpy = gTrainerClasses[GetFrontierOpponentClass(trainerId)].name;
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
        toCpy = gTrainerClasses[GetTrainerHillOpponentClass(trainerId)].name;
    else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
        toCpy = gTrainerClasses[GetEreaderTrainerClassId()].name;
    else
        toCpy = gTrainerClasses[GetTrainerClassFromId(trainerId)].name;

    return toCpy;
}

// Ensure the defined length for an item name can contain the full defined length of a berry name.
// This ensures that custom Enigma Berry names will fit in the text buffer at the top of BattleStringExpandPlaceholders.
STATIC_ASSERT(BERRY_NAME_LENGTH + ARRAY_COUNT(sText_BerrySuffix) <= ITEM_NAME_LENGTH, BerryNameTooLong);

u32 BattleStringExpandPlaceholders(const u8 *src, u8 *dst, u32 dstSize)
{
    u32 dstID = 0; // if they used dstID, why not use srcID as well?
    const u8 *toCpy = NULL;
    u8 text[max(max(max(32, TRAINER_NAME_LENGTH + 1), POKEMON_NAME_LENGTH + 1), ITEM_NAME_LENGTH)];
    u8 *textStart = &text[0];
    u8 multiplayerId;
    u8 fontId = FONT_NORMAL;

    if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
        multiplayerId = gRecordedBattleMultiplayerId;
    else
        multiplayerId = GetMultiplayerId();

    // Clear destination first
    while (dstID < dstSize)
    {
        dst[dstID] = EOS;
        dstID++;
    }

    dstID = 0;
    while (*src != EOS)
    {
        toCpy = NULL;

        if (*src == PLACEHOLDER_BEGIN)
        {
            src++;
            u32 classLength = 0;
            u32 nameLength = 0;
            const u8 *classString;
            const u8 *nameString;
            switch (*src)
            {
            case B_TXT_BUFF1:
                if (gBattleTextBuff1[0] == B_BUFF_PLACEHOLDER_BEGIN)
                {
                    ExpandBattleTextBuffPlaceholders(gBattleTextBuff1, gStringVar1);
                    toCpy = gStringVar1;
                }
                else
                {
                    toCpy = TryGetStatusString(gBattleTextBuff1);
                    if (toCpy == NULL)
                        toCpy = gBattleTextBuff1;
                }
                break;
            case B_TXT_BUFF2:
                if (gBattleTextBuff2[0] == B_BUFF_PLACEHOLDER_BEGIN)
                {
                    ExpandBattleTextBuffPlaceholders(gBattleTextBuff2, gStringVar2);
                    toCpy = gStringVar2;
                }
                else
                {
                    toCpy = gBattleTextBuff2;
                }
                break;
            case B_TXT_BUFF3:
                if (gBattleTextBuff3[0] == B_BUFF_PLACEHOLDER_BEGIN)
                {
                    ExpandBattleTextBuffPlaceholders(gBattleTextBuff3, gStringVar3);
                    toCpy = gStringVar3;
                }
                else
                {
                    toCpy = gBattleTextBuff3;
                }
                break;
            case B_TXT_COPY_VAR_1:
                toCpy = gStringVar1;
                break;
            case B_TXT_COPY_VAR_2:
                toCpy = gStringVar2;
                break;
            case B_TXT_COPY_VAR_3:
                toCpy = gStringVar3;
                break;
            case B_TXT_PLAYER_MON1_NAME: // first player poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT), text);
                toCpy = text;
                break;
            case B_TXT_OPPONENT_MON1_NAME: // first enemy poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), text);
                toCpy = text;
                break;
            case B_TXT_PLAYER_MON2_NAME: // second player poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT), text);
                toCpy = text;
                break;
            case B_TXT_OPPONENT_MON2_NAME: // second enemy poke name
                GetBattlerNick(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT), text);
                toCpy = text;
                break;
            case B_TXT_LINK_PLAYER_MON1_NAME: // link first player poke name
                GetBattlerNick(gLinkPlayers[multiplayerId].id, text);
                toCpy = text;
                break;
            case B_TXT_LINK_OPPONENT_MON1_NAME: // link first opponent poke name
                GetBattlerNick(gLinkPlayers[multiplayerId].id ^ 1, text);
                toCpy = text;
                break;
            case B_TXT_LINK_PLAYER_MON2_NAME: // link second player poke name
                GetBattlerNick(gLinkPlayers[multiplayerId].id ^ 2, text);
                toCpy = text;
                break;
            case B_TXT_LINK_OPPONENT_MON2_NAME: // link second opponent poke name
                GetBattlerNick(gLinkPlayers[multiplayerId].id ^ 3, text);
                toCpy = text;
                break;
            case B_TXT_ATK_NAME_WITH_PREFIX_MON1: // Unused, to change into sth else.
                break;
            case B_TXT_ATK_PARTNER_NAME: // attacker partner name
                GetBattlerNick(BATTLE_PARTNER(gBattlerAttacker), text);
                toCpy = text;
                break;
            case B_TXT_ATK_NAME_WITH_PREFIX: // attacker name with prefix
                HANDLE_NICKNAME_STRING_CASE(gBattlerAttacker)
                break;
            case B_TXT_DEF_NAME_WITH_PREFIX: // target name with prefix
                HANDLE_NICKNAME_STRING_CASE(gBattlerTarget)
                break;
            case B_TXT_DEF_NAME: // target name
                GetBattlerNick(gBattlerTarget, text);
                toCpy = text;
                break;
            case B_TXT_DEF_PARTNER_NAME: // partner target name
                GetBattlerNick(BATTLE_PARTNER(gBattlerTarget), text);
                toCpy = text;
                break;
            case B_TXT_EFF_NAME_WITH_PREFIX: // effect battler name with prefix
                HANDLE_NICKNAME_STRING_CASE(gEffectBattler)
                break;
            case B_TXT_SCR_ACTIVE_NAME_WITH_PREFIX: // scripting active battler name with prefix
                HANDLE_NICKNAME_STRING_CASE(gBattleScripting.battler)
                break;
            case B_TXT_CURRENT_MOVE: // current move name
                if (gBattleMsgDataPtr->currentMove >= MOVES_COUNT
                 && !IsZMove(gBattleMsgDataPtr->currentMove)
                 && !IsMaxMove(gBattleMsgDataPtr->currentMove))
                    toCpy = gTypesInfo[gBattleStruct->stringMoveType].generic;
                else
                    toCpy = GetMoveName(gBattleMsgDataPtr->currentMove);
                break;
            case B_TXT_LAST_MOVE: // originally used move name
                if (gBattleMsgDataPtr->originallyUsedMove >= MOVES_COUNT
                 && !IsZMove(gBattleMsgDataPtr->currentMove)
                 && !IsMaxMove(gBattleMsgDataPtr->currentMove))
                    toCpy = gTypesInfo[gBattleStruct->stringMoveType].generic;
                else
                    toCpy = GetMoveName(gBattleMsgDataPtr->originallyUsedMove);
                break;
            case B_TXT_LAST_ITEM: // last used item
                if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
                {
                    if (gLastUsedItem == ITEM_ENIGMA_BERRY_E_READER)
                    {
                        if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
                        {
                            if ((gBattleScripting.multiplayerId != 0 && (gPotentialItemEffectBattler & BIT_SIDE))
                                || (gBattleScripting.multiplayerId == 0 && !(gPotentialItemEffectBattler & BIT_SIDE)))
                            {
                                StringCopy(text, gEnigmaBerries[gPotentialItemEffectBattler].name);
                                StringAppend(text, sText_BerrySuffix);
                                toCpy = text;
                            }
                            else
                            {
                                toCpy = sText_EnigmaBerry;
                            }
                        }
                        else
                        {
                            if (gLinkPlayers[gBattleScripting.multiplayerId].id == gPotentialItemEffectBattler)
                            {
                                StringCopy(text, gEnigmaBerries[gPotentialItemEffectBattler].name);
                                StringAppend(text, sText_BerrySuffix);
                                toCpy = text;
                            }
                            else
                            {
                                toCpy = sText_EnigmaBerry;
                            }
                        }
                    }
                    else
                    {
                        CopyItemName(gLastUsedItem, text);
                        toCpy = text;
                    }
                }
                else
                {
                    CopyItemName(gLastUsedItem, text);
                    toCpy = text;
                }
                break;
            case B_TXT_LAST_ABILITY: // last used ability
                toCpy = gAbilitiesInfo[gLastUsedAbility].name;
                break;
            case B_TXT_ATK_ABILITY: // attacker ability
                toCpy = gAbilitiesInfo[sBattlerAbilities[gBattlerAttacker]].name;
                break;
            case B_TXT_DEF_ABILITY: // target ability
                toCpy = gAbilitiesInfo[sBattlerAbilities[gBattlerTarget]].name;
                break;
            case B_TXT_SCR_ACTIVE_ABILITY: // scripting active ability
                toCpy = gAbilitiesInfo[sBattlerAbilities[gBattleScripting.battler]].name;
                break;
            case B_TXT_EFF_ABILITY: // effect battler ability
                toCpy = gAbilitiesInfo[sBattlerAbilities[gEffectBattler]].name;
                break;
            case B_TXT_TRAINER1_CLASS: // trainer class name
                toCpy = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                break;
            case B_TXT_TRAINER1_NAME: // trainer1 name
                toCpy = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentA, text, multiplayerId, GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT));
                break;
            case B_TXT_TRAINER1_NAME_WITH_CLASS: // trainer1 name with trainer class
                toCpy = textStart;
                classString = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                while (classString[classLength] != EOS)
                {
                    textStart[classLength] = classString[classLength];
                    classLength++;
                }
                textStart[classLength] = CHAR_SPACE;
                textStart += classLength + 1;
                nameString = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentA, textStart, multiplayerId, GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT));
                if (nameString != textStart)
                {
                    while (nameString[nameLength] != EOS)
                    {
                        textStart[nameLength] = nameString[nameLength];
                        nameLength++;
                    }
                    textStart[nameLength] = EOS;
                }
                break;
            case B_TXT_LINK_PLAYER_NAME: // link player name
                toCpy = gLinkPlayers[multiplayerId].name;
                break;
            case B_TXT_LINK_PARTNER_NAME: // link partner name
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(BATTLE_PARTNER(gLinkPlayers[multiplayerId].id))].name;
                break;
            case B_TXT_LINK_OPPONENT1_NAME: // link opponent 1 name
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(BATTLE_OPPOSITE(gLinkPlayers[multiplayerId].id))].name;
                break;
            case B_TXT_LINK_OPPONENT2_NAME: // link opponent 2 name
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(BATTLE_PARTNER(BATTLE_OPPOSITE(gLinkPlayers[multiplayerId].id)))].name;
                break;
            case B_TXT_LINK_SCR_TRAINER_NAME: // link scripting active name
                toCpy = gLinkPlayers[GetBattlerMultiplayerId(gBattleScripting.battler)].name;
                break;
            case B_TXT_PLAYER_NAME: // player name
                toCpy = BattleStringGetPlayerName(text, GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
                break;
            case B_TXT_TRAINER1_LOSE_TEXT: // trainerA lose text
                if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
                {
                    CopyFrontierTrainerText(FRONTIER_PLAYER_WON_TEXT, TRAINER_BATTLE_PARAM.opponentA);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
                {
                    CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_WON, TRAINER_BATTLE_PARAM.opponentA);
                    toCpy = gStringVar4;
                }
                else
                {
                    toCpy = GetTrainerALoseText();
                }
                break;
            case B_TXT_TRAINER1_WIN_TEXT: // trainerA win text
                if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
                {
                    CopyFrontierTrainerText(FRONTIER_PLAYER_LOST_TEXT, TRAINER_BATTLE_PARAM.opponentA);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
                {
                    CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_LOST, TRAINER_BATTLE_PARAM.opponentA);
                    toCpy = gStringVar4;
                }
                break;
            case B_TXT_26: // ?
                if (!IsOnPlayerSide(gBattleScripting.battler))
                {
                    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                        toCpy = sText_FoePkmnPrefix;
                    else
                        toCpy = sText_WildPkmnPrefix;
                    while (*toCpy != EOS)
                    {
                        dst[dstID] = *toCpy;
                        dstID++;
                        toCpy++;
                    }
                    GetMonData(&gEnemyParty[gBattleStruct->scriptPartyIdx], MON_DATA_NICKNAME, text);
                }
                else
                {
                    GetMonData(&gPlayerParty[gBattleStruct->scriptPartyIdx], MON_DATA_NICKNAME, text);
                }
                StringGet_Nickname(text);
                toCpy = text;
                break;
            case B_TXT_PC_CREATOR_NAME: // lanette pc
                if (FlagGet(FLAG_SYS_PC_LANETTE))
                    toCpy = sText_Lanettes;
                else
                    toCpy = sText_Someones;
                break;
            case B_TXT_ATK_PREFIX2:
                if (IsOnPlayerSide(gBattlerAttacker))
                    toCpy = sText_AllyPkmnPrefix2;
                else
                    toCpy = sText_FoePkmnPrefix3;
                break;
            case B_TXT_DEF_PREFIX2:
                if (IsOnPlayerSide(gBattlerTarget))
                    toCpy = sText_AllyPkmnPrefix2;
                else
                    toCpy = sText_FoePkmnPrefix3;
                break;
            case B_TXT_ATK_PREFIX1:
                if (IsOnPlayerSide(gBattlerAttacker))
                    toCpy = sText_AllyPkmnPrefix;
                else
                    toCpy = sText_FoePkmnPrefix2;
                break;
            case B_TXT_DEF_PREFIX1:
                if (IsOnPlayerSide(gBattlerTarget))
                    toCpy = sText_AllyPkmnPrefix;
                else
                    toCpy = sText_FoePkmnPrefix2;
                break;
            case B_TXT_ATK_PREFIX3:
                if (IsOnPlayerSide(gBattlerAttacker))
                    toCpy = sText_AllyPkmnPrefix3;
                else
                    toCpy = sText_FoePkmnPrefix4;
                break;
            case B_TXT_DEF_PREFIX3:
                if (IsOnPlayerSide(gBattlerTarget))
                    toCpy = sText_AllyPkmnPrefix3;
                else
                    toCpy = sText_FoePkmnPrefix4;
                break;
            case B_TXT_TRAINER2_CLASS:
                toCpy = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentB);
                break;
            case B_TXT_TRAINER2_NAME:
                toCpy = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentB, text, multiplayerId, GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT));
                break;
            case B_TXT_TRAINER2_NAME_WITH_CLASS:
                toCpy = textStart;
                classString = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentB);
                while (classString[classLength] != EOS)
                {
                    textStart[classLength] = classString[classLength];
                    classLength++;
                }
                textStart[classLength] = CHAR_SPACE;
                textStart += classLength + 1;
                nameString = BattleStringGetOpponentNameByTrainerId(TRAINER_BATTLE_PARAM.opponentB, textStart, multiplayerId, GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT));
                if (nameString != textStart)
                {
                    while (nameString[nameLength] != EOS)
                    {
                        textStart[nameLength] = nameString[nameLength];
                        nameLength++;
                    }
                    textStart[nameLength] = EOS;
                }
                break;
            case B_TXT_TRAINER2_LOSE_TEXT:
                if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
                {
                    CopyFrontierTrainerText(FRONTIER_PLAYER_WON_TEXT, TRAINER_BATTLE_PARAM.opponentB);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
                {
                    CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_WON, TRAINER_BATTLE_PARAM.opponentB);
                    toCpy = gStringVar4;
                }
                else
                {
                    toCpy = GetTrainerBLoseText();
                }
                break;
            case B_TXT_TRAINER2_WIN_TEXT:
                if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
                {
                    CopyFrontierTrainerText(FRONTIER_PLAYER_LOST_TEXT, TRAINER_BATTLE_PARAM.opponentB);
                    toCpy = gStringVar4;
                }
                else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
                {
                    CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_LOST, TRAINER_BATTLE_PARAM.opponentB);
                    toCpy = gStringVar4;
                }
                break;
            case B_TXT_PARTNER_CLASS:
                toCpy = gTrainerClasses[GetFrontierOpponentClass(gPartnerTrainerId)].name;
                break;
            case B_TXT_PARTNER_NAME:
                toCpy = BattleStringGetPlayerName(text, GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT));
                break;
            case B_TXT_PARTNER_NAME_WITH_CLASS:
                toCpy = textStart;
                classString = gTrainerClasses[GetFrontierOpponentClass(gPartnerTrainerId)].name;
                while (classString[classLength] != EOS)
                {
                    textStart[classLength] = classString[classLength];
                    classLength++;
                }
                textStart[classLength] = CHAR_SPACE;
                textStart += classLength + 1;
                nameString = BattleStringGetPlayerName(textStart, GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT));
                if (nameString != textStart)
                {
                    while (nameString[nameLength] != EOS)
                    {
                        textStart[nameLength] = nameString[nameLength];
                        nameLength++;
                    }
                    textStart[nameLength] = EOS;
                }
                break;
            case B_TXT_ATK_TRAINER_NAME:
                toCpy = BattleStringGetTrainerName(text, multiplayerId, gBattlerAttacker);
                break;
            case B_TXT_ATK_TRAINER_CLASS:
                switch (GetBattlerPosition(gBattlerAttacker))
                {
                case B_POSITION_PLAYER_RIGHT:
                    if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
                        toCpy = gTrainerClasses[GetFrontierOpponentClass(gPartnerTrainerId)].name;
                    break;
                case B_POSITION_OPPONENT_LEFT:
                    toCpy = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                    break;
                case B_POSITION_OPPONENT_RIGHT:
                    if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && !BATTLE_TWO_VS_ONE_OPPONENT)
                        toCpy = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentB);
                    else
                        toCpy = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                    break;
                }
                break;
            case B_TXT_ATK_TRAINER_NAME_WITH_CLASS:
                toCpy = textStart;
                if (GetBattlerPosition(gBattlerAttacker) == B_POSITION_PLAYER_LEFT)
                {
                    textStart = StringCopy(textStart, BattleStringGetTrainerName(textStart, multiplayerId, gBattlerAttacker));
                }
                else
                {
                    classString = NULL;
                    switch (GetBattlerPosition(gBattlerAttacker))
                    {
                    case B_POSITION_PLAYER_RIGHT:
                        if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
                            classString = gTrainerClasses[GetFrontierOpponentClass(gPartnerTrainerId)].name;
                        break;
                    case B_POSITION_OPPONENT_LEFT:
                        classString = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                        break;
                    case B_POSITION_OPPONENT_RIGHT:
                        if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && !BATTLE_TWO_VS_ONE_OPPONENT)
                            classString = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentB);
                        else
                            classString = BattleStringGetOpponentClassByTrainerId(TRAINER_BATTLE_PARAM.opponentA);
                        break;
                    }
                    classLength = 0;
                    nameLength = 0;
                    while (classString[classLength] != EOS)
                    {
                        textStart[classLength] = classString[classLength];
                        classLength++;
                    }
                    textStart[classLength] = CHAR_SPACE;
                    textStart += 1 + classLength;
                    nameString = BattleStringGetTrainerName(textStart, multiplayerId, gBattlerAttacker);
                    if (nameString != textStart)
                    {
                        while (nameString[nameLength] != EOS)
                        {
                            textStart[nameLength] = nameString[nameLength];
                            nameLength++;
                        }
                        textStart[nameLength] = EOS;
                    }
                }
                break;
            case B_TXT_ATK_TEAM1:
                if (IsOnPlayerSide(gBattlerAttacker))
                    toCpy = sText_Your1;
                else
                    toCpy = sText_Opposing1;
                break;
            case B_TXT_ATK_TEAM2:
                if (IsOnPlayerSide(gBattlerAttacker))
                    toCpy = sText_Your2;
                else
                    toCpy = sText_Opposing2;
                break;
            case B_TXT_DEF_TEAM1:
                if (IsOnPlayerSide(gBattlerTarget))
                    toCpy = sText_Your1;
                else
                    toCpy = sText_Opposing1;
                break;
            case B_TXT_DEF_TEAM2:
                if (IsOnPlayerSide(gBattlerTarget))
                    toCpy = sText_Your2;
                else
                    toCpy = sText_Opposing2;
                break;
            case B_TXT_EFF_TEAM1:
                if (IsOnPlayerSide(gEffectBattler))
                    toCpy = sText_Your1;
                else
                    toCpy = sText_Opposing1;
                break;
            case B_TXT_EFF_TEAM2:
                if (IsOnPlayerSide(gEffectBattler))
                    toCpy = sText_Your2;
                else
                    toCpy = sText_Opposing2;
                break;
            case B_TXT_ATK_NAME_WITH_PREFIX2:
                HANDLE_NICKNAME_STRING_LOWERCASE(gBattlerAttacker)
                break;
            case B_TXT_DEF_NAME_WITH_PREFIX2:
                HANDLE_NICKNAME_STRING_LOWERCASE(gBattlerTarget)
                break;
            case B_TXT_EFF_NAME_WITH_PREFIX2:
                HANDLE_NICKNAME_STRING_LOWERCASE(gEffectBattler)
                break;
            case B_TXT_SCR_ACTIVE_NAME_WITH_PREFIX2:
                HANDLE_NICKNAME_STRING_LOWERCASE(gBattleScripting.battler)
                break;
            }

            if (toCpy != NULL)
            {
                while (*toCpy != EOS)
                {
                    if (*toCpy == CHAR_SPACE)
                        dst[dstID] = CHAR_NBSP;
                    else
                        dst[dstID] = *toCpy;
                    dstID++;
                    toCpy++;
                }
            }

            if (*src == B_TXT_TRAINER1_LOSE_TEXT || *src == B_TXT_TRAINER2_LOSE_TEXT
                || *src == B_TXT_TRAINER1_WIN_TEXT || *src == B_TXT_TRAINER2_WIN_TEXT)
            {
                dst[dstID] = EXT_CTRL_CODE_BEGIN;
                dstID++;
                dst[dstID] = EXT_CTRL_CODE_PAUSE_UNTIL_PRESS;
                dstID++;
            }
        }
        else
        {
            dst[dstID] = *src;
            dstID++;
        }
        src++;
    }

    dst[dstID] = *src;
    dstID++;

    BreakStringAutomatic(dst, BATTLE_MSG_MAX_WIDTH, BATTLE_MSG_MAX_LINES, fontId, SHOW_SCROLL_PROMPT);

    return dstID;
}

static void IllusionNickHack(u32 battler, u32 partyId, u8 *dst)
{
    u32 id = PARTY_SIZE;
    // we know it's gEnemyParty
    struct Pokemon *mon = &gEnemyParty[partyId], *partnerMon;

    if (GetMonAbility(mon) == ABILITY_ILLUSION)
    {
        if (IsBattlerAlive(BATTLE_PARTNER(battler)))
            partnerMon = GetBattlerMon(BATTLE_PARTNER(battler));
        else
            partnerMon = mon;

        id = GetIllusionMonPartyId(gEnemyParty, mon, partnerMon, battler);
    }

    if (id != PARTY_SIZE)
        GetMonData(&gEnemyParty[id], MON_DATA_NICKNAME, dst);
    else
        GetMonData(mon, MON_DATA_NICKNAME, dst);
}

void ExpandBattleTextBuffPlaceholders(const u8 *src, u8 *dst)
{
    u32 srcID = 1;
    u32 value = 0;
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u16 hword;

    *dst = EOS;
    while (src[srcID] != B_BUFF_EOS)
    {
        switch (src[srcID])
        {
        case B_BUFF_STRING: // battle string
            hword = T1_READ_16(&src[srcID + 1]);
            StringAppend(dst, gBattleStringsTable[hword]);
            srcID += 3;
            break;
        case B_BUFF_NUMBER: // int to string
            switch (src[srcID + 1])
            {
            case 1:
                value = src[srcID + 3];
                break;
            case 2:
                value = T1_READ_16(&src[srcID + 3]);
                break;
            case 4:
                value = T1_READ_32(&src[srcID + 3]);
                break;
            }
            ConvertIntToDecimalStringN(dst, value, STR_CONV_MODE_LEFT_ALIGN, src[srcID + 2]);
            srcID += src[srcID + 1] + 3;
            break;
        case B_BUFF_MOVE: // move name
            StringAppend(dst, GetMoveName(T1_READ_16(&src[srcID + 1])));
            srcID += 3;
            break;
        case B_BUFF_TYPE: // type name
            StringAppend(dst, gTypesInfo[src[srcID + 1]].name);
            srcID += 2;
            break;
        case B_BUFF_MON_NICK_WITH_PREFIX: // poke nick with prefix
        case B_BUFF_MON_NICK_WITH_PREFIX_LOWER: // poke nick with lowercase prefix
            if (IsOnPlayerSide(src[srcID + 1]))
            {
                GetMonData(&gPlayerParty[src[srcID + 2]], MON_DATA_NICKNAME, nickname);
            }
            else
            {
                if (src[srcID] == B_BUFF_MON_NICK_WITH_PREFIX_LOWER)
                {
                    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                        StringAppend(dst, sText_FoePkmnPrefixLower);
                    else
                        StringAppend(dst, sText_WildPkmnPrefixLower);
                }
                else
                {
                    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                        StringAppend(dst, sText_FoePkmnPrefix);
                    else
                        StringAppend(dst, sText_WildPkmnPrefix);
                }

                GetMonData(&gEnemyParty[src[srcID + 2]], MON_DATA_NICKNAME, nickname);
            }
            StringGet_Nickname(nickname);
            StringAppend(dst, nickname);
            srcID += 3;
            break;
        case B_BUFF_STAT: // stats
            StringAppend(dst, gStatNamesTable[src[srcID + 1]]);
            srcID += 2;
            break;
        case B_BUFF_SPECIES: // species name
            StringCopy(dst, GetSpeciesName(T1_READ_16(&src[srcID + 1])));
            srcID += 3;
            break;
        case B_BUFF_MON_NICK: // poke nick without prefix
            if (src[srcID + 2] == gBattlerPartyIndexes[src[srcID + 1]])
            {
                GetBattlerNick(src[srcID + 1], dst);
            }
            else if (gBattleScripting.illusionNickHack) // for STRINGID_ENEMYABOUTTOSWITCHPKMN
            {
                gBattleScripting.illusionNickHack = 0;
                IllusionNickHack(src[srcID + 1], src[srcID + 2], dst);
                StringGet_Nickname(dst);
            }
            else
            {
                if (IsOnPlayerSide(src[srcID + 1]))
                    GetMonData(&gPlayerParty[src[srcID + 2]], MON_DATA_NICKNAME, dst);
                else
                    GetMonData(&gEnemyParty[src[srcID + 2]], MON_DATA_NICKNAME, dst);
                StringGet_Nickname(dst);
            }
            srcID += 3;
            break;
        case B_BUFF_NEGATIVE_FLAVOR: // flavor table
            StringAppend(dst, gPokeblockWasTooXStringTable[src[srcID + 1]]);
            srcID += 2;
            break;
        case B_BUFF_ABILITY: // ability names
            StringAppend(dst, gAbilitiesInfo[T1_READ_16(&src[srcID + 1])].name);
            srcID += 3;
            break;
        case B_BUFF_ITEM: // item name
            hword = T1_READ_16(&src[srcID + 1]);
            if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
            {
                if (hword == ITEM_ENIGMA_BERRY_E_READER)
                {
                    if (gLinkPlayers[gBattleScripting.multiplayerId].id == gPotentialItemEffectBattler)
                    {
                        StringCopy(dst, gEnigmaBerries[gPotentialItemEffectBattler].name);
                        StringAppend(dst, sText_BerrySuffix);
                    }
                    else
                    {
                        StringAppend(dst, sText_EnigmaBerry);
                    }
                }
                else
                {
                    CopyItemName(hword, dst);
                }
            }
            else
            {
                CopyItemName(hword, dst);
            }
            srcID += 3;
            break;
        }
    }
}

// Loads one of two text strings into the provided buffer. This is functionally
// unused, since the value loaded into the buffer is not read; it loaded one of
// two particles (either "?" or "?") which works in tandem with ChooseTypeOfMoveUsedString
// below to effect changes in the meaning of the line.
static void UNUSED ChooseMoveUsedParticle(u8 *textBuff)
{
    s32 counter = 0;
    u32 i = 0;

    while (counter != MAX_MON_MOVES)
    {
        if (sGrammarMoveUsedTable[i] == 0)
            counter++;
        if (sGrammarMoveUsedTable[i++] == gBattleMsgDataPtr->currentMove)
            break;
    }

    if (counter >= 0)
    {
        if (counter <= 2)
            StringCopy(textBuff, sText_SpaceIs); // is
        else if (counter <= MAX_MON_MOVES)
            StringCopy(textBuff, sText_ApostropheS); // 's
    }
}

// Appends "!" to the text buffer `dst`. In the original Japanese this looked
// into the table of moves at sGrammarMoveUsedTable and varied the line accordingly.
//
// sText_ExclamationMark was a plain "!", used for any attack not on the list.
// It resulted in the translation "<NAME>'s <ATTACK>!".
//
// sText_ExclamationMark2 was "? ????!". This resulted in the translation
// "<NAME> used <ATTACK>!", which was used for all attacks in English.
//
// sText_ExclamationMark3 was "??!". This was used for those moves whose
// names were verbs, such as Recover, and resulted in translations like "<NAME>
// recovered itself!".
//
// sText_ExclamationMark4 was "? ??!" This resulted in a translation of
// "<NAME> did an <ATTACK>!".
//
// sText_ExclamationMark5 was " ????!" This resulted in a translation of
// "<NAME>'s <ATTACK> attack!".
static void UNUSED ChooseTypeOfMoveUsedString(u8 *dst)
{
    s32 counter = 0;
    s32 i = 0;

    while (*dst != EOS)
        dst++;

    while (counter != MAX_MON_MOVES)
    {
        if (sGrammarMoveUsedTable[i] == MOVE_NONE)
            counter++;
        if (sGrammarMoveUsedTable[i++] == gBattleMsgDataPtr->currentMove)
            break;
    }

    switch (counter)
    {
    case 0:
        StringCopy(dst, sText_ExclamationMark);
        break;
    case 1:
        StringCopy(dst, sText_ExclamationMark2);
        break;
    case 2:
        StringCopy(dst, sText_ExclamationMark3);
        break;
    case 3:
        StringCopy(dst, sText_ExclamationMark4);
        break;
    case 4:
        StringCopy(dst, sText_ExclamationMark5);
        break;
    }
}

void BattlePutTextOnWindow(const u8 *text, u8 windowId)
{
    const struct BattleWindowText *textInfo = sBattleTextOnWindowsInfo[gBattleScripting.windowsType];
    bool32 copyToVram;
    struct TextPrinterTemplate printerTemplate;
    u8 speed;

    if (windowId & B_WIN_COPYTOVRAM)
    {
        windowId &= ~B_WIN_COPYTOVRAM;
        copyToVram = FALSE;
    }
    else
    {
        FillWindowPixelBuffer(windowId, textInfo[windowId].fillValue);
        copyToVram = TRUE;
    }

    printerTemplate.currentChar = text;
    printerTemplate.windowId = windowId;
    printerTemplate.fontId = textInfo[windowId].fontId;
    printerTemplate.x = textInfo[windowId].x;
    printerTemplate.y = textInfo[windowId].y;
    printerTemplate.currentX = printerTemplate.x;
    printerTemplate.currentY = printerTemplate.y;
    printerTemplate.letterSpacing = textInfo[windowId].letterSpacing;
    printerTemplate.lineSpacing = textInfo[windowId].lineSpacing;
    printerTemplate.unk = 0;
    printerTemplate.fgColor = textInfo[windowId].fgColor;
    printerTemplate.bgColor = textInfo[windowId].bgColor;
    printerTemplate.shadowColor = textInfo[windowId].shadowColor;

    if (B_WIN_MOVE_NAME_1 <= windowId && windowId <= B_WIN_MOVE_NAME_4)
    {
        // We cannot check the actual width of the window because
        // B_WIN_MOVE_NAME_1 and B_WIN_MOVE_NAME_3 are 16 wide for
        // Z-move details.
        if (gBattleStruct->zmove.viewing && windowId == B_WIN_MOVE_NAME_1)
            printerTemplate.fontId = GetFontIdToFit(text, printerTemplate.fontId, printerTemplate.letterSpacing, 16 * TILE_WIDTH);
        else
            printerTemplate.fontId = GetFontIdToFit(text, printerTemplate.fontId, printerTemplate.letterSpacing, 8 * TILE_WIDTH);
    }

    if (printerTemplate.x == 0xFF)
    {
        u32 width = GetBattleWindowTemplatePixelWidth(gBattleScripting.windowsType, windowId);
        s32 alignX = GetStringCenterAlignXOffsetWithLetterSpacing(printerTemplate.fontId, printerTemplate.currentChar, width, printerTemplate.letterSpacing);
        printerTemplate.x = printerTemplate.currentX = alignX;
    }

    if (windowId == ARENA_WIN_JUDGMENT_TEXT)
        gTextFlags.useAlternateDownArrow = FALSE;
    else
        gTextFlags.useAlternateDownArrow = TRUE;

    if ((gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED)) || gTestRunnerEnabled)
        gTextFlags.autoScroll = TRUE;
    else
        gTextFlags.autoScroll = FALSE;

    if (windowId == B_WIN_MSG || windowId == ARENA_WIN_JUDGMENT_TEXT)
    {
        if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
            speed = 1;
        else if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
            speed = sRecordedBattleTextSpeeds[GetTextSpeedInRecordedBattle()];
        else
            speed = GetPlayerTextSpeedDelay();

        gTextFlags.canABSpeedUpPrint = 1;
    }
    else
    {
        speed = textInfo[windowId].speed;
        gTextFlags.canABSpeedUpPrint = 0;
    }

    AddTextPrinter(&printerTemplate, speed, NULL);

    if (copyToVram)
    {
        PutWindowTilemap(windowId);
        CopyWindowToVram(windowId, COPYWIN_FULL);
    }
}

void SetPpNumbersPaletteInMoveSelection(u32 battler)
{
    struct ChooseMoveStruct *chooseMoveStruct = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
    const u16 *palPtr = gPPTextPalette;
    u8 var;

    if (!gBattleStruct->zmove.viewing)
        var = GetCurrentPpToMaxPpState(chooseMoveStruct->currentPp[gMoveSelectionCursor[battler]],
                         chooseMoveStruct->maxPp[gMoveSelectionCursor[battler]]);
    else
        var = 3;

    gPlttBufferUnfaded[BG_PLTT_ID(5) + 12] = palPtr[(var * 2) + 0];
    gPlttBufferUnfaded[BG_PLTT_ID(5) + 11] = palPtr[(var * 2) + 1];

    CpuCopy16(&gPlttBufferUnfaded[BG_PLTT_ID(5) + 12], &gPlttBufferFaded[BG_PLTT_ID(5) + 12], PLTT_SIZEOF(1));
    CpuCopy16(&gPlttBufferUnfaded[BG_PLTT_ID(5) + 11], &gPlttBufferFaded[BG_PLTT_ID(5) + 11], PLTT_SIZEOF(1));
}

u8 GetCurrentPpToMaxPpState(u8 currentPp, u8 maxPp)
{
    if (maxPp == currentPp)
    {
        return 3;
    }
    else if (maxPp <= 2)
    {
        if (currentPp > 1)
            return 3;
        else
            return 2 - currentPp;
    }
    else if (maxPp <= 7)
    {
        if (currentPp > 2)
            return 3;
        else
            return 2 - currentPp;
    }
    else
    {
        if (currentPp == 0)
            return 2;
        if (currentPp <= maxPp / 4)
            return 1;
        if (currentPp > maxPp / 2)
            return 3;
    }

    return 0;
}
