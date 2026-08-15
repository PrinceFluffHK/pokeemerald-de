#ifndef GUARD_CONSTANTS_TMS_HMS_H
#define GUARD_CONSTANTS_TMS_HMS_H

#define FOREACH_TM(F) \
    F(WORK_UP) \
    F(DRAGON_CLAW) \
    F(BRINE) \
    F(CALM_MIND) \
    F(ROAR) \
    F(TOXIC) \
    F(SNOWSCAPE) \
    F(BULK_UP) \
    F(TRAILBLAZE) \
    F(GIGA_IMPACT) \
    F(SUNNY_DAY) \
    F(TAUNT) \
    F(ICE_BEAM) \
    F(BLIZZARD) \
    F(HYPER_BEAM) \
    F(LIGHT_SCREEN) \
    F(PROTECT) \
    F(RAIN_DANCE) \
    F(GIGA_DRAIN) \
    F(SAFEGUARD) \
    F(STOMPING_TANTRUM) \
    F(GRASS_KNOT) \
    F(IRON_TAIL) \
    F(THUNDERBOLT) \
    F(THUNDER) \
    F(EARTHQUAKE) \
    F(ALLURING_VOICE) \
    F(DIG) \
    F(PSYCHIC) \
    F(SHADOW_BALL) \
    F(BRICK_BREAK) \
    F(FALSE_SWIPE) \
    F(REFLECT) \
    F(THUNDER_WAVE) \
    F(FLAMETHROWER) \
    F(SLUDGE_BOMB) \
    F(SANDSTORM) \
    F(FIRE_BLAST) \
    F(ROCK_TOMB) \
    F(AERIAL_ACE) \
    F(SWORDS_DANCE) \
    F(FACADE) \
    F(EARTH_POWER) \
    F(REST) \
    F(HELPING_HAND) \
    F(THIEF) \
    F(FLASH_CANNON) \
    F(SKILL_SWAP) \
    F(TRICK_ROOM) \
    F(OVERHEAT) \
    F(GRASSY_TERRAIN) \
    F(ELECTRIC_TERRAIN) \
    F(MISTY_TERRAIN) \
    F(PSYCHIC_TERRAIN) 

#define FOREACH_HM(F) \
    F(CUT) \
    F(FLY) \
    F(SURF) \
    F(STRENGTH) \
    F(FLASH) \
    F(ROCK_SMASH) \
    F(WATERFALL) \
    F(DIVE)

#define FOREACH_TMHM(F) \
    FOREACH_TM(F) \
    FOREACH_HM(F)

#endif
