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
	setflag FLAG_UNUSED_0x863
	release
	end

Test_Island_EventScript_DebugBerryNPC_Done::
	msgbox Test_Island_Text_BerryNPC_Done, MSGBOX_DEFAULT
	release
	end

Test_Island_Text_BerryNPC_Done:
	.string "Ya te di las bayas, la MO\ny la caña.$"
