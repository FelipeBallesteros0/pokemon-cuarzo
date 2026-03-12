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
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_UtilityNpc_End
	setvar VAR_ROUTE101_STATE, 3
	warp MAP_ROUTE101, 16, 9
	waitstate
	release
	end

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
