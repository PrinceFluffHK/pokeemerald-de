#ifndef GUARD_CONSTANTS_GENERATIONAL_CHANGES_H
#define GUARD_CONSTANTS_GENERATIONAL_CHANGES_H

/* Config definitions */
#define BATTLE_CONFIG_DEFINITIONS(F) \
    /* Calculation settings */ \
    F(CRIT_CHANCE,               critChance,              (u32, GEN_COUNT - 1)) \
    F(CRIT_MULTIPLIER,           critMultiplier,          (u32, GEN_COUNT - 1)) \
    F(PARALYSIS_SPEED,           paralysisSpeed,          (u32, GEN_COUNT - 1)) \
    F(CONFUSION_SELF_DMG_CHANCE, confusionSelfDmgChance,  (u32, GEN_COUNT - 1)) \
    F(MULTI_HIT_CHANCE,          multiHitChance,          (u32, GEN_COUNT - 1)) \
    F(WHITEOUT_MONEY,            whiteoutMoney,           (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(LIGHT_BALL_ATTACK_BOOST,   lightBallAttackBoost,    (u32, GEN_COUNT - 1)) \
    /* Experience settings */ \
    F(EXP_CATCH,                 expCatch,                (u32, GEN_COUNT - 1)) \
    F(TRAINER_EXP_MULTIPLIER,    trainerExpMultiplier,    (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(SPLIT_EXP,                 splitExp,                (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(SCALED_EXP,                scaledExp,               (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(UNEVOLVED_EXP_MULTIPLIER,  unevolvedExpMultiplier,  (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(LEVEL_UP_NOTIFICATION,     levelUpNotification,     (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    /* Stat settings */ \
    F(BADGE_BOOST,               badgeBoost,              (u32, GEN_COUNT - 1)) \
    F(FRIENDSHIP_BOOST,          friendshipBoost,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(MAX_LEVEL_EV_GAINS,        maxLevelEvGains,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(RECALCULATE_STATS,         recalculateStats,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    /* Damage settings */ \
    F(BURN_DAMAGE,               burnDamage,              (u32, GEN_COUNT - 1)) \
    F(BURN_FACADE_DMG,           burnFacadeDmg,           (u32, GEN_COUNT - 1)) \
    F(BINDING_DAMAGE,            bindingDamage,           (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(PSYWAVE_DMG,               psywaveDmg,              (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(PAYBACK_SWITCH_BOOST,      paybackSwitchBoost,      (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(HIDDEN_POWER_DMG,          hiddenPowerDmg,          (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(ROUGH_SKIN_DMG,            roughSkinDmg,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(KNOCK_OFF_DMG,             knockOffDmg,             (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(SPORT_DMG_REDUCTION,       sportDmgReduction,       (u32, GEN_COUNT - 1)) \
    F(EXPLOSION_DEFENSE,         explosionDefense,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(PARENTAL_BOND_DMG,         parentalBondDmg,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(MULTIPLE_TARGETS_DMG,      multipleTargetsDmg,      (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    /* Type settings */ \
    F(B_GHOSTS_ESCAPE,             ghostsEscape,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_PARALYZE_ELECTRIC,         paralyzeElectric,        (u32, GEN_COUNT - 1)) \
    F(B_POWDER_GRASS,              powderGrass,             (u32, GEN_COUNT - 1)) \
    F(B_POWDER_OVERCOAT,           powderOvercoat,          (u32, GEN_COUNT - 1)) \
    F(B_UPDATED_TYPE_MATCHUPS,     updatedTypeMatchups,     (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_PRANKSTER_DARK_TYPES,      pranksterDarkTypes,      (u32, GEN_COUNT - 1)) \
    F(B_SHEER_COLD_IMMUNITY,       sheerColdImmunity,       (u32, GEN_COUNT - 1)) \
    F(B_ROOST_PURE_FLYING,         roostPureFlying,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_STATUS_TYPE_IMMUNITY,      statusTypeImmunity,      (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_HIDDEN_POWER_COUNTER,      hiddenPowerCounter,      (u32, GEN_COUNT - 1)) \
    /* Turn settings */ \
    F(B_BINDING_TURNS,             bindingTurns,            (u32, GEN_COUNT - 1)) \
    F(B_UPROAR_TURNS,              uproarTurns,             (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_UPROAR_IGNORE_SOUNDPROOF,  uproarIgnoreSoundproof,  (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_DISABLE_TURNS,             disableTurns,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_TAILWIND_TURNS,            tailwindTurns,           (u32, GEN_COUNT - 1)) \
    F(B_SLEEP_TURNS,               sleepTurns,              (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_TAUNT_TURNS,               tauntTurns,              (u32, GEN_COUNT - 1)) \
    F(B_SPORT_TURNS,               sportTurns,              (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_MEGA_EVO_TURN_ORDER,       megaEvoTurnOrder,        (u32, GEN_COUNT - 1)) \
    F(B_RECALC_TURN_AFTER_ACTIONS, recalcTurnAfterActions,  (u32, GEN_COUNT - 1)) \
    F(B_FAINT_SWITCH_IN,           faintSwitchIn,           (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    /* Move data settings */ \
    F(B_UPDATED_MOVE_DATA,         updatedMoveData,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_UPDATED_MOVE_TYPES,        updatedMoveTypes,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_UPDATED_MOVE_FLAGS,        updatedMoveFlags,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_PHYSICAL_SPECIAL_SPLIT,    physicalSpecialSplit,    (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_RECOIL_IF_MISS_DMG,        recoilIfMissDmg,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_KLUTZ_FLING_INTERACTION,   klutzFlingInteraction,   (u32, GEN_COUNT - 1)) \
    F(B_UPDATED_CONVERSION,        updatedConversion,       (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_UPDATED_CONVERSION_2,      updatedConversion2,      (u32, GEN_COUNT - 1)) \
    F(B_PP_REDUCED_BY_SPITE,       ppReducedBySpite,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_EXTRAPOLATED_MOVE_FLAGS,   extrapolatedMoveFlags,   (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_MODERN_TRICK_CHOICE_LOCK,  modernTrickChoiceLock,   (u32, GEN_COUNT - 1)) \
    /* Ability data settings */ \
    F(UPDATED_ABILITY_DATA,      updatedAbilityData,      (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    /* Move accuracy settings */ \
    F(TOXIC_NEVER_MISS,          toxicNeverMiss,          (u32, GEN_COUNT - 1)) \
    F(MINIMIZE_DMG_ACC,          minimizeDmgAcc,          (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(BLIZZARD_HAIL,             blizzardHail,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(SHEER_COLD_ACC,            sheerColdAcc,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    /* Move stat change settings */ \
    F(FELL_STINGER_STAT_RAISE,   fellStingerStatRaise,    (u32, GEN_COUNT - 1)) \
    F(KINGS_SHIELD_LOWER_ATK,    kingsShieldLowerAtk,     (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(SPEED_BUFFING_RAPID_SPIN,  speedBuffingRapidSpin,   (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(CHARGE_SPDEF_RAISE,        chargeSpDefRaise,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(MINIMIZE_EVASION,          minimizeEvasion,         (u32, GEN_COUNT - 1)) \
    F(GROWTH_STAT_RAISE,         growthStatRaise,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(FOCUS_ENERGY_CRIT_RATIO,   focusEnergyCritRatio,    (u32, GEN_COUNT - 1)) \
    /* Other move settings */ \
    F(B_INCINERATE_GEMS,           incinerateGems,          (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_CAN_SPITE_FAIL,            canSpiteFail,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_CRASH_IF_TARGET_IMMUNE,    crashIfTargetImmune,     (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_MEMENTO_FAIL,              mementoFail,             (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_PARTING_SHOT_SWITCH,       partingShotSwitch,       (u32, GEN_COUNT - 1)) \
    F(B_BATON_PASS_TRAPPING,       batonPassTrapping,       (u32, GEN_COUNT - 1)) \
    F(B_GLARE_GHOST,               glareGhost,              (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_SKILL_SWAP,                skillSwap,               (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_BRICK_BREAK,               brickBreak,              (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_WISH_HP_SOURCE,            wishHpSource,            (u32, GEN_COUNT - 1)) \
    F(B_RAMPAGE_CANCELLING,        rampageCancelling,       (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_HEAL_BLOCKING,             healBlocking,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_ROOTED_GROUNDING,          rootedGrounding,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_METRONOME_MOVES,           metronomeMoves,          (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_TELEPORT_BEHAVIOR,         teleportBehavior,        (u32, GEN_COUNT - 1)) \
    F(B_BEAT_UP,                   beatUp,                  (u32, GEN_COUNT - 1)) \
    F(B_DARK_VOID_FAIL,            darkVoidFail,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_HIT_THAW,                  hitThaw,                 (u32, GEN_COUNT - 1)) \
    F(B_HEALING_WISH_SWITCH,       healingWishSwitch,       (u32, GEN_COUNT - 1)) \
    F(B_DEFOG_EFFECT_CLEARING,     defogEffectClearing,     (u32, GEN_COUNT - 1)) \
    F(B_STOCKPILE_RAISES_DEFS,     stockpileRaisesDefs,     (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_TRANSFORM_SEMI_INV_FAIL,   transformSemiInvFail,    (u32, GEN_COUNT - 1)) \
    F(B_TRANSFORM_TARGET_FAIL,     transformTargetFail,     (u32, GEN_COUNT - 1)) \
    F(B_TRANSFORM_USER_FAIL,       transformUserFail,       (u32, GEN_COUNT - 1)) \
    F(B_TRANSFORM_SUBSTITUTE_FAIL, transformSubstituteFail, (u32, GEN_COUNT - 1)) \
    F(B_TRANSFORM_SHINY,           transformShiny,          (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_TRANSFORM_FORM_CHANGES,    transformFormChanges,    (u32, GEN_COUNT - 1)) \
    F(B_WIDE_GUARD,                wideGuard,               (u32, GEN_COUNT - 1)) \
    F(B_QUICK_GUARD,               quickGuard,              (u32, GEN_COUNT - 1)) \
    F(B_IMPRISON,                  imprison,                (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_TAUNT_ME_FIRST,            tauntMeFirst,            (u32, GEN_COUNT - 1)) \
    F(B_ALLY_SWITCH_FAIL_CHANCE,   allySwitchFailChance,    (u32, GEN_COUNT - 1)) \
    F(B_SKETCH_BANS,               sketchBans,              (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_KNOCK_OFF_REMOVAL,         knockOffRemoval,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_HEAL_BELL_SOUNDPROOF,      healBellSoundproof,      (u32, GEN_COUNT - 1)) \
    F(B_CHARGE,                    charge,                  (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_POWDER_STATUS_HEAVY_RAIN,  powderStatusHeavyRain,   (u32, GEN_COUNT - 1)) \
    F(B_AFTER_YOU_TURN_ORDER,      afterYouTurnOrder,       (u32, GEN_COUNT - 1)) \
    F(B_QUASH_TURN_ORDER,          quashTurnOrder,          (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_DESTINY_BOND_FAIL,         destinyBondFail,         (u32, GEN_COUNT - 1)) \
    F(B_FORESIGHT_FAIL,            foresightFail,           (u32, GEN_COUNT - 1)) \
    F(B_MIRACLE_EYE_FAIL,          miracleEyeFail,          (u32, GEN_COUNT - 1)) \
    F(B_PURSUIT_TARGET,            pursuitTarget,           (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_SKIP_RECHARGE,             skipRecharge,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_ENCORE_TARGET,             encoreTarget,            (u32, GEN_COUNT - 1)) \
    F(B_TIME_OF_DAY_HEALING_MOVES, timeOfDayHealingMoves,   (u32, GEN_COUNT - 1)) \
    F(B_DREAM_EATER_LIQUID_OOZE,   dreamEaterLiquidOoze,    (u32, GEN_COUNT - 1)) \
    F(B_FOCUS_PUNCH_FAILURE,       focusPunchFailure,       (u32, GEN_COUNT - 1)) \
    F(B_RAGE_BUILDS,               rageBuilds,              (u32, GEN_COUNT - 1)) \
    F(B_CHECK_USER_FAILURE,        checkUserFailure,        (u32, GEN_COUNT - 1)) \
    F(B_ABSORB_MESSAGE,            absorbMessge,            (u32, GEN_COUNT - 1)) \
    /* Ability settings */ \
    F(B_GALE_WINGS,                galeWings,               (u32, GEN_COUNT - 1)) \
    F(B_STANCE_CHANGE_FAIL,        stanceChangeFail,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_SHADOW_TAG_ESCAPE,         shadowTagEscape,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_MOODY_ACC_EVASION,         moodyAccEvasion,         (u32, GEN_COUNT - 1)) \
    F(B_FLASH_FIRE_FROZEN,         flashFireFrozen,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_SYNCHRONIZE_TOXIC,         synchronizeToxic,        (u32, GEN_COUNT - 1)) \
    F(B_UPDATED_INTIMIDATE,        updatedIntimidate,       (u32, GEN_COUNT - 1)) \
    F(B_OBLIVIOUS_TAUNT,           obliviousTaunt,          (u32, GEN_COUNT - 1)) \
    F(B_STURDY,                    sturdy,                  (u32, GEN_COUNT - 1)) \
    F(B_PLUS_MINUS_INTERACTION,    plusMinusInteraction,    (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_WEATHER_FORMS,             weatherForms,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_SYMBIOSIS_GEMS,            symbiosisGems,           (u32, GEN_COUNT - 1)) \
    F(B_REDIRECT_ABILITY_IMMUNITY, redirectAbilityImmunity, (u32, GEN_COUNT - 1)) \
    F(B_REDIRECT_ABILITY_ALLIES,   redirectAbilityAllies,   (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_LEAF_GUARD_PREVENTS_REST,  leafGuardPreventsRest,   (u32, GEN_COUNT - 1)) \
    F(B_TRANSISTOR_BOOST,          transistorBoost,         (u32, GEN_COUNT - 1)) \
    F(B_ILLUMINATE_EFFECT,         illuminateEffect,        (u32, GEN_COUNT - 1)) \
    F(B_WEAK_ARMOR_SPEED,          weakArmorSpeed,          (u32, GEN_COUNT - 1)) \
    F(B_PROTEAN_LIBERO,            proteanLibero,           (u32, GEN_COUNT - 1)) \
    F(B_INTREPID_SWORD,            intrepidSword,           (u32, GEN_COUNT - 1)) \
    F(B_DAUNTLESS_SHIELD,          dauntlessShield,         (u32, GEN_COUNT - 1)) \
    F(B_DISGUISE_HP_LOSS,          disguiseHpLoss,          (u32, GEN_COUNT - 1)) \
    F(B_ABILITY_TRIGGER_CHANCE,    abilityTriggerChance,    (u32, GEN_COUNT - 1)) \
    F(B_PICKUP_WILD,               pickupWild,              (u32, GEN_COUNT - 1)) \
    F(B_MAGIC_GUARD,               magicGuard,              (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_BATTLE_BOND,               battleBond,              (u32, GEN_COUNT - 1)) \
    F(B_ATE_MULTIPLIER,            ateMultiplier,           (u32, GEN_COUNT - 1)) \
    F(B_DEFIANT_STICKY_WEB,        defiantStickyWeb,        (u32, GEN_COUNT - 1)) \
    F(B_INFILTRATOR_SUBSTITUTE,    infiltratorSubstitute,   (u32, GEN_COUNT - 1)) \
    /* Item settings */ \
    F(CONFUSE_BERRIES_HEAL,      confuseBerriesHeal,      (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(X_ITEMS_BUFF,              xItemsBuff,              (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(MENTAL_HERB,               mentalHerb,              (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(TRAINERS_KNOCK_OFF_ITEMS,  trainersKnockOffItems,   (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(RETURN_STOLEN_NPC_ITEMS,   returnStolenNpcItems,    (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(STEAL_WILD_ITEMS,          stealWildItems,          (u32, GEN_COUNT - 1)) \
    F(RESTORE_HELD_BATTLE_ITEMS, restoreHeldBattleItems,  (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(SOUL_DEW_BOOST,            soulDewBoost,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(NET_BALL_MODIFIER,         netBallModifier,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(DIVE_BALL_MODIFIER,        diveBallModifier,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(NEST_BALL_MODIFIER,        nestBallModifier,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(REPEAT_BALL_MODIFIER,      repeatBallModifier,      (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(TIMER_BALL_MODIFIER,       timerBallModifier,       (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(DUSK_BALL_MODIFIER,        duskBallModifier,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(QUICK_BALL_MODIFIER,       quickBallModifier,       (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(LURE_BALL_MODIFIER,        lureBallModifier,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(HEAVY_BALL_MODIFIER,       heavyBallModifier,       (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(DREAM_BALL_MODIFIER,       dreamBallModifier,       (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(SPORT_BALL_MODIFIER,       sportBallModifier,       (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(SAFARI_BALL_MODIFIER,      safariBallModifier,      (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(FRIEND_BALL_MODIFIER,      friendBallModifier,      (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(SERENE_GRACE_BOOST,        sereneGraceBoost,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(IRON_BALL,                 ironBall,                (u32, GEN_COUNT - 1)) \
    /* Weather settings */ \
    F(ABILITY_WEATHER,           abilityWeather,          (u32, GEN_COUNT - 1)) \
    F(SANDSTORM_SPDEF_BOOST,     sandstormSpDefBoost,     (u32, GEN_COUNT - 1)) \
    F(OVERWORLD_FOG,             overworldFog,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(OVERWORLD_SNOW,            overworldSnow,           (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(SNOW_WARNING,              snowWarning,             (u32, GEN_COUNT - 1)) \
    F(PREFERRED_ICE_WEATHER,     preferredIceWeather,     (u32, B_ICE_WEATHER_SNOW)) /* TODO: use in tests */ \
    /* Terrain settings */ \
    F(B_TERRAIN_TYPE_BOOST,        terrainTypeBoost,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_SECRET_POWER_EFFECT,       secretPowerEffect,       (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_SECRET_POWER_ANIMATION,    secretPowerAnimation,    (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_NATURE_POWER_MOVES,        naturePowerMoves,        (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_CAMOUFLAGE_TYPES,          camouflageTypes,         (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    /* Catching settings */ \
    F(B_SEMI_INVULNERABLE_CATCH,   semiInvulnerableCatch,   (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_INCAPACITATED_CATCH_BONUS, incapacitatedCatchBonus, (u32, GEN_COUNT - 1)) \
    F(B_LOW_LEVEL_CATCH_BONUS,     lowLevlCatchBonus,       (u32, GEN_COUNT - 1)) \
    F(B_MISSING_BADGE_CATCH_MALUS, missingBadgeCatchMalue,  (u32, GEN_COUNT - 1)) \
    F(B_CRITICAL_CAPTURE_IF_OWNED, criticalCaptureIfOwned,  (u32, GEN_COUNT - 1)) \
    /* Other settings */ \
    F(B_WILD_NATURAL_ENEMIES,      wildNaturalEnemies,      (u32, TRUE))          /* TODO: use in tests */ \
    F(B_AFFECTION_MECHANICS,       affectionMechanics,      (u32, TRUE))          /* TODO: use in tests */ \
    F(B_OBEDIENCE_MECHANICS,       obedienceMechanics,      (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_USE_FROSTBITE,             useFrostbite,            (u32, GEN_COUNT - 1)) /* TODO: use in tests */ \
    F(B_SANDSTORM_SOLAR_BEAM,      standstormSolarBeam,     (u32, GEN_COUNT - 1)) \
    F(B_COUNTER_MIRROR_COAT_ALLY,  counterMirrorCoatAlly,   (u32, GEN_COUNT - 1)) \
    F(B_COUNTER_TRY_HIT_PARTNER,   counterTryHitPartner,    (u32, GEN_COUNT - 1)) \


#define POKEMON_CONFIG_DEFINITIONS(F) \
    F(POKERUS_ENABLED,           pokerusEnabled,          (u32, TRUE))          \
    F(POKERUS_SPREAD_ADJACENCY,  pokerusSpreadAdjacency,  (u32, GEN_COUNT - 1)) \
    F(POKERUS_SPREAD_DAYS_LEFT,  pokerusSpreadDaysLeft,   (u32, GEN_COUNT - 1)) \
    F(POKERUS_INFECT_AGAIN,      pokerusInfectAgain,      (u32, GEN_COUNT - 1)) \
    F(POKERUS_INFECT_EGG,        pokerusInfectEgg,        (u32, TRUE))          \
    F(POKERUS_HERD_IMMUNITY,     pokerusHerdImmunity,     (u32, TRUE))          \
    F(POKERUS_WEAK_VARIANT,      pokerusWeakVariant,      (u32, TRUE))          \


#define GET_CONFIG_MAXIMUM(_typeMaxValue, ...) INVOKE_WITH_B(GET_CONFIG_MAXIMUM_, _typeMaxValue)
#define GET_CONFIG_MAXIMUM_(_type, ...) FIRST(__VA_OPT__(FIRST(__VA_ARGS__),) MAX_BITS((sizeof(_type) * 8)))

#define UNPACK_CONFIG_ENUMS(_name, ...) CONFIG_##_name,

enum ConfigTag
{
    BATTLE_CONFIG_DEFINITIONS(UNPACK_CONFIG_ENUMS)
    POKEMON_CONFIG_DEFINITIONS(UNPACK_CONFIG_ENUMS)
    CONFIG_COUNT
};

#endif // GUARD_CONSTANTS_GENERATIONAL_CHANGES_H
