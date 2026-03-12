TestIsland_MapScripts::
	.byte 0

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
