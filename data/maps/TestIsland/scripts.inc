TestIsland_MapScripts::
	.byte 0

TestIsland_EventScript_UtilityNpc::
	lock
	faceplayer
	msgbox TestIsland_Text_UtilityNpc_Intro, MSGBOX_DEFAULT
	msgbox TestIsland_Text_UtilityNpc_GiveKit, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, TestIsland_EventScript_UtilityNpc_DoGiveKit
	goto TestIsland_EventScript_UtilityNpc_AskWarpRoute101

TestIsland_EventScript_UtilityNpc_DoGiveKit:
	setflag FLAG_SYS_POKEMON_GET
	setflag FLAG_ADVENTURE_STARTED
	setflag FLAG_SYS_POKEDEX_GET
	setflag FLAG_SYS_POKENAV_GET
	setflag FLAG_RECEIVED_POKEDEX_FROM_BIRCH
	special SetUnlockedPokedexFlags
	setflag FLAG_RECEIVED_RUNNING_SHOES
	setflag FLAG_SYS_B_DASH
	setflag FLAG_RECEIVED_BIKE
	additem ITEM_MACH_BIKE
	additem ITEM_ACRO_BIKE
	additem ITEM_BICYCLE
	additem ITEM_OLD_ROD
	msgbox TestIsland_Text_UtilityNpc_KitGiven, MSGBOX_DEFAULT

TestIsland_EventScript_UtilityNpc_AskWarpRoute101:
	msgbox TestIsland_Text_UtilityNpc_WarpRoute101, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_UtilityNpc_AskDnsTime
	setvar VAR_ROUTE101_STATE, 3
	warp MAP_ROUTE101, 16, 9
	waitstate
	release
	end

TestIsland_EventScript_UtilityNpc_AskDnsTime:
	msgbox TestIsland_Text_UtilityNpc_AskDnsTime, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_UtilityNpc_End
	msgbox TestIsland_Text_UtilityNpc_ChooseMorning, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, TestIsland_EventScript_UtilityNpc_SetMorning
	msgbox TestIsland_Text_UtilityNpc_ChooseDay, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, TestIsland_EventScript_UtilityNpc_SetDay
	msgbox TestIsland_Text_UtilityNpc_ChooseEvening, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, TestIsland_EventScript_UtilityNpc_SetEvening
	msgbox TestIsland_Text_UtilityNpc_ChooseNight, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, TestIsland_EventScript_UtilityNpc_SetNight
	goto TestIsland_EventScript_UtilityNpc_End

TestIsland_EventScript_UtilityNpc_SetMorning:
	setvar VAR_0x8004, 8
	goto TestIsland_EventScript_UtilityNpc_ApplyDnsTime

TestIsland_EventScript_UtilityNpc_SetDay:
	setvar VAR_0x8004, 13
	goto TestIsland_EventScript_UtilityNpc_ApplyDnsTime

TestIsland_EventScript_UtilityNpc_SetEvening:
	setvar VAR_0x8004, 20
	goto TestIsland_EventScript_UtilityNpc_ApplyDnsTime

TestIsland_EventScript_UtilityNpc_SetNight:
	setvar VAR_0x8004, 23
	goto TestIsland_EventScript_UtilityNpc_ApplyDnsTime

TestIsland_EventScript_UtilityNpc_ApplyDnsTime:
	special Special_SetTimeOfDayFromVar8004
	dotimebasedevents
	msgbox TestIsland_Text_UtilityNpc_DnsTimeSet, MSGBOX_DEFAULT
	goto TestIsland_EventScript_UtilityNpc_End

TestIsland_EventScript_UtilityNpc_End:
	release
	end

TestIsland_EventScript_WarpToPuebloCiendra::
	lock
	faceplayer
	msgbox TestIsland_Text_WarpToPuebloCiendra, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_WarpToPuebloCiendra_Cancel
	warp MAP_PUEBLO_CIENDRA, 13, 38
	waitstate
	release
	end

TestIsland_EventScript_WarpToPuebloCiendra_Cancel:
	release
	end

TestIsland_EventScript_FollowerNpcToggle::
	lock
	faceplayer
	checkfollowernpc
	goto_if_eq VAR_RESULT, FALSE, TestIsland_EventScript_FollowerNpc_AskEnable
	msgbox TestIsland_Text_FollowerNpc_AlreadyActive, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, TestIsland_EventScript_FollowerNpc_Disable
	release
	end

TestIsland_EventScript_FollowerNpc_AskEnable:
	msgbox TestIsland_Text_FollowerNpc_AskEnable, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_FollowerNpc_End
	setfollowernpc LOCALID_TEST_ISLAND_FOLLOWER_TOGGLE, FNPC_ALL, TestIsland_EventScript_FollowerNpc_Companion, PARTNER_MAY
	msgbox TestIsland_Text_FollowerNpc_Enabled, MSGBOX_DEFAULT
	releaseall
	end

TestIsland_EventScript_FollowerNpc_Companion:
	lock
	facefollowernpc
	msgbox TestIsland_Text_FollowerNpc_CompanionPrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, TestIsland_EventScript_FollowerNpc_Disable
	msgbox TestIsland_Text_FollowerNpc_Stay, MSGBOX_DEFAULT
	releaseall
	end

TestIsland_EventScript_FollowerNpc_Disable:
	hidefollowernpc 1
	destroyfollowernpc
	msgbox TestIsland_Text_FollowerNpc_Disabled, MSGBOX_DEFAULT
	releaseall
	end

TestIsland_EventScript_FollowerNpc_End:
	release
	end

TestIsland_Text_WarpToPuebloCiendra:
	.string "quieres ir a Pueblo Ciendra?$"

TestIsland_Text_UtilityNpc_Intro:
	.string "soy npc utilitario de pruebas.$"

TestIsland_Text_UtilityNpc_GiveKit:
	.string "quieres kit de pruebas?\n"
	.string "(pokedex, pokenav, zapatillas,\l"
	.string "bicis y cana vieja).$"

TestIsland_Text_UtilityNpc_KitGiven:
	.string "listo, kit de pruebas entregado.$"

TestIsland_Text_UtilityNpc_WarpRoute101:
	.string "quieres que te lleve a ruta 101?$"

TestIsland_Text_UtilityNpc_AskDnsTime:
	.string "quieres ajustar la hora del dia\n"
	.string "para probar DNS?$"

TestIsland_Text_UtilityNpc_ChooseMorning:
	.string "poner MANANA (08:00)?$"

TestIsland_Text_UtilityNpc_ChooseDay:
	.string "poner DIA (13:00)?$"

TestIsland_Text_UtilityNpc_ChooseEvening:
	.string "poner TARDE (20:00)?$"

TestIsland_Text_UtilityNpc_ChooseNight:
	.string "poner NOCHE (23:00)?$"

TestIsland_Text_UtilityNpc_DnsTimeSet:
	.string "listo, hora aplicada para DNS.$"

TestIsland_Text_FollowerNpc_AskEnable:
	.string "quieres que te acompane?$"

TestIsland_Text_FollowerNpc_AlreadyActive:
	.string "ya te estoy acompanando.\n"
	.string "quieres que me vaya?$"

TestIsland_Text_FollowerNpc_Enabled:
	.string "listo, te sigo desde ahora.$"

TestIsland_Text_FollowerNpc_CompanionPrompt:
	.string "quieres que deje de seguirte?$"

TestIsland_Text_FollowerNpc_Stay:
	.string "perfecto, sigo contigo.$"

TestIsland_Text_FollowerNpc_Disabled:
	.string "listo, dejo de seguirte.$"
