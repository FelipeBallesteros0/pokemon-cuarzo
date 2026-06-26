Pueblo_Ciendra_MapScripts::
	.byte 0

Pueblo_Ciendra_EventScript_Vendedora::
	lock
	faceplayer
	createfieldmugshot MUGSHOT_MAID, EMOTE_NORMAL
	msgbox Pueblo_Ciendra_Text_Vendedora, MSGBOX_DEFAULT
	removefieldmugshot
	release
	end

Pueblo_Ciendra_Text_Vendedora:
	.string "¡Bienvenido a Pueblo Ciendra!\n"
	.string "Soy la dependienta del PokéMart.\p"
	.string "{CREATE_MUGSHOT MUGSHOT_MAID EMOTE_ALT}¿A que es bonito poder verme\n"
	.string "la cara mientras charlamos?$"
