LilycoveCity_DepartmentStore_4F_MapScripts::
	.byte 0

LilycoveCity_DepartmentStore_4F_EventScript_Gentleman::
	msgbox LilycoveCity_DepartmentStore_4F_Text_AttackOrDefenseTM, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_Woman::
	msgbox LilycoveCity_DepartmentStore_4F_Text_FiftyDifferentTMs, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_Youngster::
	msgbox LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_ClerkLeft::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_4F_Pokemart_TMs
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_4F_Pokemart_TMs:
	.2byte ITEM_TM_REFLECT
	.2byte ITEM_TM_FIRE_BLAST
	.2byte ITEM_TM_BLIZZARD
	.2byte ITEM_TM_THUNDER
	.2byte ITEM_TM_HYPER_BEAM
	.2byte ITEM_TM_GIGA_IMPACT
	pokemartlistend

LilycoveCity_DepartmentStore_4F_EventScript_ClerkRight::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_4F_Pokemart_HeldItems
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_4F_Pokemart_HeldItems:
	.2byte ITEM_THROAT_SPRAY		@2000
	.2byte ITEM_RAZOR_FANG			@2000
	.2byte ITEM_RAZOR_CLAW			@2000
	.2byte ITEM_UTILITY_UMBRELLA	@2000
	.2byte ITEM_EJECT_PACK			@2000
	.2byte ITEM_RED_CARD			@2000
	.2byte ITEM_HEAVY_DUTY_BOOTS	@2000
	.2byte ITEM_AIR_BALLOON			@2000
	.2byte ITEM_SAFETY_GOGGLES		@2000
	.2byte ITEM_PROTECTIVE_PADS		@2000
	.2byte ITEM_EXPERT_BELT			@2000
	.2byte ITEM_FLAME_ORB			@2000
	.2byte ITEM_TOXIC_ORB			@1000
	.2byte ITEM_BINDING_BAND		@1000
	.2byte ITEM_PUNCHING_GLOVE		@1000
	.2byte ITEM_FOCUS_BAND			@1000
	.2byte ITEM_QUICK_CLAW			@1000
	.2byte ITEM_BRIGHT_POWDER		@1000
	.2byte ITEM_ZOOM_LENS			@1000
	.2byte ITEM_FLOAT_STONE			@1000
	.2byte ITEM_RING_TARGET			@1000
	.2byte ITEM_METRONOME			@1000
	.2byte ITEM_IRON_BALL			@1000
	.2byte ITEM_LAGGING_TAIL		@1000
	.2byte ITEM_ROOM_SERVICE		@1000
	.2byte ITEM_BLACK_SLUDGE		@1000
	.2byte ITEM_GRIP_CLAW			@1000
	.2byte ITEM_STICKY_BARB			@1000
	.2byte ITEM_SHED_SHELL			@1000
	.2byte ITEM_BIG_ROOT			@1000
	.2byte ITEM_TERRAIN_EXTENDER	@1000
	.2byte ITEM_BLUNDER_POLICY		@1000
	.2byte ITEM_LUMINOUS_MOSS		@1000
	.2byte ITEM_ABSORB_BULB			@1000
	.2byte ITEM_CELL_BATTERY		@1000
	.2byte ITEM_SNOWBALL			@1000
	.2byte ITEM_ADRENALINE_ORB		@1000
	pokemartlistend

LilycoveCity_DepartmentStore_4F_Text_AttackOrDefenseTM:
	.string "Hmm…\p"
	.string "An attacking move…\n"
	.string "Or a defensive move…\p"
	.string "It's no easy matter to decide which TM\n"
	.string "moves should be taught to POKéMON…$"

LilycoveCity_DepartmentStore_4F_Text_FiftyDifferentTMs:
	.string "There are so many different kinds of\n"
	.string "TM moves.\p"
	.string "A catalog I read said there are fifty\n"
	.string "different kinds.$"

LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves:
	.string "I'd like to get all the different TMs,\n"
	.string "but a POKéMON learns only four moves.$"

