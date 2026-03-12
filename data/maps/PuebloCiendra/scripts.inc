PuebloCiendra_MapScripts::
	.byte 0

PuebloCiendra_EventScript_Pikachu::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_PIKACHU, CRY_MODE_NORMAL
	waitmoncry
	release
	end

PuebloCiendra_EventScript_Eevee::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_EEVEE, CRY_MODE_NORMAL
	waitmoncry
	release
	end

PuebloCiendra_EventScript_Vulpix::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_VULPIX, CRY_MODE_NORMAL
	waitmoncry
	release
	end

PuebloCiendra_EventScript_Psyduck::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_PSYDUCK, CRY_MODE_NORMAL
	waitmoncry
	release
	end

PuebloCiendra_EventScript_Growlithe::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_GROWLITHE, CRY_MODE_NORMAL
	waitmoncry
	release
	end

PuebloCiendra_EventScript_WarpToTestIsland::
	lock
	faceplayer
	msgbox PuebloCiendra_Text_WarpToTestIsland, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PuebloCiendra_EventScript_WarpToTestIsland_Cancel
	warp MAP_TEST_ISLAND, 40, 23
	waitstate
	release
	end

PuebloCiendra_EventScript_WarpToTestIsland_Cancel:
	release
	end

PuebloCiendra_Text_WarpToTestIsland:
	.string "quieres ir a Isla de pruebas?$"
