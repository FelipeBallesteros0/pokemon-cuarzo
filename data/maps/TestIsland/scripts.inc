TestIsland_MapScripts::
	.byte 0

TestIsland_EventScript_BounceRock::
	lockall
	specialvar VAR_TEMP_4, Special_GetFirstBounceMonIndex
	goto_if_eq VAR_TEMP_4, PARTY_SIZE, TestIsland_EventScript_BounceRock_Cant
	msgbox TestIsland_Text_BouncePrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_BounceRock_End
	getplayerxy VAR_TEMP_2, VAR_TEMP_3
	goto_if_eq VAR_TEMP_2, 28, TestIsland_EventScript_BounceRock_LeftFromWest_CheckY
	goto_if_eq VAR_TEMP_2, 30, TestIsland_EventScript_BounceRock_LeftFromEast_CheckY
	goto_if_eq VAR_TEMP_2, 29, TestIsland_EventScript_BounceRock_LeftFromVertical_CheckY
	goto_if_eq VAR_TEMP_2, 36, TestIsland_EventScript_BounceRock_RightFromEast_CheckY
	goto_if_eq VAR_TEMP_2, 34, TestIsland_EventScript_BounceRock_RightFromWest_CheckY
	goto_if_eq VAR_TEMP_2, 35, TestIsland_EventScript_BounceRock_RightFromVertical_CheckY
	goto TestIsland_EventScript_BounceRock_Cant

TestIsland_EventScript_BounceRock_Prepare:
	hidefollower 1
	setflag FLAG_TEMP_HIDE_FOLLOWER
	updatefollowingmon
	setfieldeffectargument 0, VAR_TEMP_4
	dofieldeffect FLDEFF_FIELD_MOVE_SHOW_MON_INIT
	waitfieldeffect FLDEFF_FIELD_MOVE_SHOW_MON
	return

TestIsland_EventScript_BounceRock_Finish:
	waitmovement 0
	resetobjectsubpriority LOCALID_PLAYER, MAP_TEST_ISLAND
	setflag FLAG_TEMP_BOUNCE_FOLLOWER_RESPAWN
	releaseall
	end

TestIsland_EventScript_BounceRock_LeftFromWest_CheckY:
	goto_if_ne VAR_TEMP_3, 23, TestIsland_EventScript_BounceRock_Cant
	call TestIsland_EventScript_BounceRock_Prepare
	setobjectsubpriority LOCALID_PLAYER, MAP_TEST_ISLAND, 3
	applymovement OBJ_EVENT_ID_PLAYER, TestIsland_Movement_BounceRight
	goto TestIsland_EventScript_BounceRock_Finish

TestIsland_EventScript_BounceRock_LeftFromEast_CheckY:
	goto_if_ne VAR_TEMP_3, 23, TestIsland_EventScript_BounceRock_Cant
	call TestIsland_EventScript_BounceRock_Prepare
	setobjectsubpriority LOCALID_PLAYER, MAP_TEST_ISLAND, 3
	applymovement OBJ_EVENT_ID_PLAYER, TestIsland_Movement_BounceRight_FromEast
	goto TestIsland_EventScript_BounceRock_Finish

TestIsland_EventScript_BounceRock_LeftFromVertical_CheckY:
	goto_if_eq VAR_TEMP_3, 22, TestIsland_EventScript_BounceRock_LeftFromNorth
	goto_if_eq VAR_TEMP_3, 24, TestIsland_EventScript_BounceRock_LeftFromSouth
	goto TestIsland_EventScript_BounceRock_Cant

TestIsland_EventScript_BounceRock_LeftFromNorth:
	call TestIsland_EventScript_BounceRock_Prepare
	setobjectsubpriority LOCALID_PLAYER, MAP_TEST_ISLAND, 3
	applymovement OBJ_EVENT_ID_PLAYER, TestIsland_Movement_BounceRight_FromNorth
	goto TestIsland_EventScript_BounceRock_Finish

TestIsland_EventScript_BounceRock_LeftFromSouth:
	call TestIsland_EventScript_BounceRock_Prepare
	setobjectsubpriority LOCALID_PLAYER, MAP_TEST_ISLAND, 3
	applymovement OBJ_EVENT_ID_PLAYER, TestIsland_Movement_BounceRight_FromSouth
	goto TestIsland_EventScript_BounceRock_Finish

TestIsland_EventScript_BounceRock_RightFromEast_CheckY:
	goto_if_ne VAR_TEMP_3, 23, TestIsland_EventScript_BounceRock_Cant
	call TestIsland_EventScript_BounceRock_Prepare
	setobjectsubpriority LOCALID_PLAYER, MAP_TEST_ISLAND, 3
	applymovement OBJ_EVENT_ID_PLAYER, TestIsland_Movement_BounceLeft
	goto TestIsland_EventScript_BounceRock_Finish

TestIsland_EventScript_BounceRock_RightFromWest_CheckY:
	goto_if_ne VAR_TEMP_3, 23, TestIsland_EventScript_BounceRock_Cant
	call TestIsland_EventScript_BounceRock_Prepare
	setobjectsubpriority LOCALID_PLAYER, MAP_TEST_ISLAND, 3
	applymovement OBJ_EVENT_ID_PLAYER, TestIsland_Movement_BounceLeft_FromWest
	goto TestIsland_EventScript_BounceRock_Finish

TestIsland_EventScript_BounceRock_RightFromVertical_CheckY:
	goto_if_eq VAR_TEMP_3, 22, TestIsland_EventScript_BounceRock_RightFromNorth
	goto_if_eq VAR_TEMP_3, 24, TestIsland_EventScript_BounceRock_RightFromSouth
	goto TestIsland_EventScript_BounceRock_Cant

TestIsland_EventScript_BounceRock_RightFromNorth:
	call TestIsland_EventScript_BounceRock_Prepare
	setobjectsubpriority LOCALID_PLAYER, MAP_TEST_ISLAND, 3
	applymovement OBJ_EVENT_ID_PLAYER, TestIsland_Movement_BounceLeft_FromNorth
	goto TestIsland_EventScript_BounceRock_Finish

TestIsland_EventScript_BounceRock_RightFromSouth:
	call TestIsland_EventScript_BounceRock_Prepare
	setobjectsubpriority LOCALID_PLAYER, MAP_TEST_ISLAND, 3
	applymovement OBJ_EVENT_ID_PLAYER, TestIsland_Movement_BounceLeft_FromSouth
	goto TestIsland_EventScript_BounceRock_Finish

TestIsland_EventScript_BounceRock_Cant:
	msgbox TestIsland_Text_BounceCant, MSGBOX_DEFAULT
	releaseall
	end

TestIsland_EventScript_BounceRock_End:
	closemessage
	releaseall
	end

TestIsland_Movement_BounceRight:
	jump_special_right
	delay_4
	jump_long_right
	jump_special_right
	step_end

TestIsland_Movement_BounceLeft:
	jump_special_left
	delay_4
	jump_long_left
	jump_special_left
	step_end

TestIsland_Movement_BounceRight_FromEast:
	jump_special_left
	delay_4
	jump_long_right
	jump_special_right
	step_end

TestIsland_Movement_BounceRight_FromNorth:
	jump_special_down
	delay_4
	jump_long_right
	jump_special_right
	step_end

TestIsland_Movement_BounceRight_FromSouth:
	jump_special_up
	delay_4
	jump_long_right
	jump_special_right
	step_end

TestIsland_Movement_BounceLeft_FromWest:
	jump_special_right
	delay_4
	jump_long_left
	jump_special_left
	step_end

TestIsland_Movement_BounceLeft_FromNorth:
	jump_special_down
	delay_4
	jump_long_left
	jump_special_left
	step_end

TestIsland_Movement_BounceLeft_FromSouth:
	jump_special_up
	delay_4
	jump_long_left
	jump_special_left
	step_end

TestIsland_Text_BouncePrompt:
	.string "una roca alta, un pokemon podria saltar\n"
	.string "al otro lado, usar Rebote?$"

TestIsland_Text_BounceCant:
	.string "una roca alta, un pokemon podria saltar\n"
	.string "al otro lado.$"

TestIsland_EventScript_FollowerNPCToggle::
	lock
	faceplayer
	setflag FLAG_RECEIVED_RUNNING_SHOES
	setflag FLAG_SYS_B_DASH
	checkfollowernpc
	goto_if_eq VAR_RESULT, TRUE, TestIsland_EventScript_FollowerNPCToggle_AlreadyActive
	msgbox TestIsland_Text_FollowerNPC_Enable, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_FollowerNPCToggle_End
	setfollowernpc 3, FNPC_ALL, TestIsland_EventScript_FollowerNPCTalk, PARTNER_STEVEN
	msgbox TestIsland_Text_FollowerNPC_Enabled, MSGBOX_DEFAULT
	release
	end

TestIsland_EventScript_FollowerNPCToggle_AlreadyActive:
	msgbox TestIsland_Text_FollowerNPC_Disable_FromFollower, MSGBOX_DEFAULT
	release
	end

TestIsland_EventScript_FollowerNPCToggle_End:
	release
	end

TestIsland_EventScript_FollowerNPCTalk::
	lock
	facefollowernpc
	call TestIsland_EventScript_FollowerNPCTalk_HandleFollowerHair
	call TestIsland_EventScript_FollowerNPCTalk_HandlePlayerHair
	goto TestIsland_EventScript_FollowerNPCTalk_DisablePrompt

TestIsland_EventScript_FollowerNPCTalk_HandleFollowerHair:
	specialvar VAR_RESULT, Special_IsFollowerNPCHairBlonde
	goto_if_eq VAR_RESULT, TRUE, TestIsland_EventScript_FollowerNPCTalk_AskOriginalHair
	msgbox TestIsland_Text_FollowerNPC_Hair_Blonde_Ask, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_FollowerNPCTalk_FollowerHairDone
	special Special_ToggleFollowerNPCHairColor
	msgbox TestIsland_Text_FollowerNPC_Hair_Blonde_Done, MSGBOX_DEFAULT
	return

TestIsland_EventScript_FollowerNPCTalk_AskOriginalHair:
	msgbox TestIsland_Text_FollowerNPC_Hair_Original_Ask, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_FollowerNPCTalk_FollowerHairDone
	special Special_ToggleFollowerNPCHairColor
	msgbox TestIsland_Text_FollowerNPC_Hair_Original_Done, MSGBOX_DEFAULT

TestIsland_EventScript_FollowerNPCTalk_FollowerHairDone:
	return

TestIsland_EventScript_FollowerNPCTalk_HandlePlayerHair:
	specialvar VAR_RESULT, Special_IsPlayerBackHairBlonde
	goto_if_eq VAR_RESULT, TRUE, TestIsland_EventScript_FollowerNPCTalk_AskPlayerOriginalHair
	msgbox TestIsland_Text_Player_Hair_Blonde_Ask, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_FollowerNPCTalk_PlayerHairDone
	special Special_TogglePlayerBackHairColor
	msgbox TestIsland_Text_Player_Hair_Blonde_Done, MSGBOX_DEFAULT
	return

TestIsland_EventScript_FollowerNPCTalk_AskPlayerOriginalHair:
	msgbox TestIsland_Text_Player_Hair_Original_Ask, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_FollowerNPCTalk_PlayerHairDone
	special Special_TogglePlayerBackHairColor
	msgbox TestIsland_Text_Player_Hair_Original_Done, MSGBOX_DEFAULT

TestIsland_EventScript_FollowerNPCTalk_PlayerHairDone:
	return

TestIsland_EventScript_FollowerNPCTalk_DisablePrompt:
	msgbox TestIsland_Text_FollowerNPC_Disable, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TestIsland_EventScript_FollowerNPCTalk_Keep
	hidefollowernpc 1
	destroyfollowernpc
	msgbox TestIsland_Text_FollowerNPC_Disabled, MSGBOX_DEFAULT
	release
	end

TestIsland_EventScript_FollowerNPCTalk_Keep:
	msgbox TestIsland_Text_FollowerNPC_Talk, MSGBOX_DEFAULT
	release
	end

TestIsland_Text_FollowerNPC_Enable:
	.string "activar follower npc de prueba?$"

TestIsland_Text_FollowerNPC_Disable:
	.string "desactivar follower npc de prueba?$"

TestIsland_Text_FollowerNPC_Disable_FromFollower:
	.string "para desactivar al seguidor, habla con\n"
	.string "el mismo.$"

TestIsland_Text_FollowerNPC_Enabled:
	.string "follower npc activado.$"

TestIsland_Text_FollowerNPC_Disabled:
	.string "follower npc desactivado.$"

TestIsland_Text_FollowerNPC_Talk:
	.string "estoy siguiendote.$"

TestIsland_Text_FollowerNPC_Hair_Blonde_Ask:
	.string "quieres que cambie mi color de pelo a\n"
	.string "rubio?$"

TestIsland_Text_FollowerNPC_Hair_Original_Ask:
	.string "quieres que vuelva a mi color de pelo\n"
	.string "original?$"

TestIsland_Text_FollowerNPC_Hair_Blonde_Done:
	.string "listo, ahora tengo el pelo rubio.$"

TestIsland_Text_FollowerNPC_Hair_Original_Done:
	.string "listo, volvi a mi color de pelo\n"
	.string "original.$"

TestIsland_Text_Player_Hair_Blonde_Ask:
	.string "quieres cambiar tu color de pelo a\n"
	.string "rubio en combate?$"

TestIsland_Text_Player_Hair_Original_Ask:
	.string "quieres volver tu color de pelo\n"
	.string "original en combate?$"

TestIsland_Text_Player_Hair_Blonde_Done:
	.string "listo, tu pelo ahora se vera rubio\n"
	.string "en combate.$"

TestIsland_Text_Player_Hair_Original_Done:
	.string "listo, volviste a tu color de pelo\n"
	.string "original en combate.$"

TestIsland_EventScript_WarpToRoute101::
	lock
	faceplayer
	warp MAP_PUEBLO_CIENDRA, 13, 38
	end

TestIsland_Text_SetTime_Intro:
	.string "elige hora del dia para probar DNS.$"

TestIsland_Text_SetTime_Morning:
	.string "poner manana?$"

TestIsland_Text_SetTime_Day:
	.string "poner dia?$"

TestIsland_Text_SetTime_Evening:
	.string "poner tarde?$"

TestIsland_Text_SetTime_Night:
	.string "poner noche?$"

TestIsland_Text_SetTime_DoneMorning:
	.string "hora cambiada a manana.$"

TestIsland_Text_SetTime_DoneDay:
	.string "hora cambiada a dia.$"

TestIsland_Text_SetTime_DoneEvening:
	.string "hora cambiada a tarde.$"

TestIsland_Text_SetTime_DoneNight:
	.string "hora cambiada a noche.$"

TestIsland_Text_SetTime_Cancel:
	.string "ok, no cambie la hora.$"
