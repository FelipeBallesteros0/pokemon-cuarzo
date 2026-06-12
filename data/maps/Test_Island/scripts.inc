Test_Island_MapScripts::
	.byte 0

Test_Island_EventScript_DebugBerryNPC::
	lock
	faceplayer
	goto_if_set FLAG_UNUSED_0x863, Test_Island_EventScript_DebugBerryNPC_Done
	giveitem ITEM_CHERI_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	giveitem ITEM_ORAN_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	giveitem ITEM_SITRUS_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	giveitem ITEM_LUM_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	giveitem ITEM_ASPEAR_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	giveitem ITEM_HM03
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	giveitem ITEM_OLD_ROD
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	givemon SPECIES_GYARADOS, 30, move1=MOVE_SURF @ para probar Surf con sprite real
	givemon SPECIES_LAPRAS, 30, move1=MOVE_SURF
	givemon SPECIES_BLASTOISE, 30, move1=MOVE_SURF
	setflag FLAG_UNUSED_0x021 @ DN_FLAG_DEXNAV_GET
	setflag FLAG_UNUSED_0x022 @ DN_FLAG_DETECTOR_MODE
	setflag FLAG_UNUSED_0x863
	release
	end

Test_Island_EventScript_DebugBerryNPC_Done::
	msgbox Test_Island_Text_BerryNPC_Done, MSGBOX_DEFAULT
	release
	end

Test_Island_Text_BerryNPC_Done:
	.string "Ya te di las bayas, la MO,\nla caña y el DexNav.$"

@ ---- Ponyta NPC: da un PONYTA que sabe REBOTE (una sola vez) ----
Test_Island_EventScript_PonytaNPC::
	lock
	faceplayer
	@ OJO: 0x020-0x022 NO estan libres, los reserva DexNav (config/dexnav.h)
	goto_if_set FLAG_UNUSED_0x023, Test_Island_EventScript_PonytaNPC_Done
	msgbox Test_Island_Text_PonytaNPC_Intro, MSGBOX_DEFAULT
	givemon SPECIES_PONYTA, 20
	special Special_TeachLastMonBounce
	setflag FLAG_UNUSED_0x023
	msgbox Test_Island_Text_PonytaNPC_Given, MSGBOX_DEFAULT
	release
	end

Test_Island_EventScript_PonytaNPC_Done::
	msgbox Test_Island_Text_PonytaNPC_Done, MSGBOX_DEFAULT
	release
	end

@ ---- Roca alta: activador A del Rebote (interacción) ----
Test_Island_EventScript_BounceRock::
	lockall
	specialvar VAR_RESULT, Special_CanBounceHere
	goto_if_eq VAR_RESULT, FALSE, Test_Island_EventScript_BounceRock_Cant
	msgbox Test_Island_Text_BouncePrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Test_Island_EventScript_BounceRock_Done
	special Special_StartBounceJump
	waitstate
	releaseall
	end

Test_Island_EventScript_BounceRock_Cant:
	msgbox Test_Island_Text_BounceCant, MSGBOX_DEFAULT
	releaseall
	end

Test_Island_EventScript_BounceRock_Done:
	releaseall
	end

@ ---- Vendedor: tienda de POKé BALLS ----
Test_Island_EventScript_BallVendor::
	lock
	faceplayer
	message Test_Island_Text_BallVendor_Greet
	waitmessage
	pokemart Test_Island_Pokemart_Balls
	msgbox Test_Island_Text_BallVendor_Bye, MSGBOX_DEFAULT
	release
	end

	.align 2
Test_Island_Pokemart_Balls:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	pokemartlistend

Test_Island_Text_PonytaNPC_Intro:
	.string "¡Toma este PONYTA!\p"
	.string "Sabe REBOTE: puede saltar por encima\n"
	.string "de las rocas altas.$"

Test_Island_Text_PonytaNPC_Given:
	.string "Párate frente a una roca alta y usa\n"
	.string "REBOTE para saltar al otro lado.$"

Test_Island_Text_PonytaNPC_Done:
	.string "¿Ya probaste REBOTE en las\nrocas altas?$"

Test_Island_Text_BouncePrompt:
	.string "Una roca alta.\n"
	.string "¿Usar REBOTE para saltarla?$"

Test_Island_Text_BounceCant:
	.string "Una roca alta. Quizás un POKéMON\n"
	.string "podría saltar al otro lado.$"

Test_Island_Text_BallVendor_Greet:
	.string "¡Bienvenido! Vendo POKé BALLS,\n"
	.string "SUPER BALLS y ULTRA BALLS.$"

Test_Island_Text_BallVendor_Bye:
	.string "¡Gracias! ¡Vuelve cuando quieras!$"
