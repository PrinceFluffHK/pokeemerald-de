#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_arena.h"
#include "battle_controllers.h"
#include "battle_dome.h"
#include "battle_interface.h"
#include "battle_message.h"
#include "battle_setup.h"
#include "battle_tv.h"
#include "battle_z_move.h"
#include "battle_gimmick.h"
#include "bg.h"
#include "data.h"
#include "item.h"
#include "item_menu.h"
#include "link.h"
#include "main.h"
#include "m4a.h"
#include "palette.h"
#include "party_menu.h"
#include "pokeball.h"
#include "pokemon.h"
#include "random.h"
#include "recorded_battle.h"
#include "reshow_battle_screen.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "test_runner.h"
#include "text.h"
#include "util.h"
#include "window.h"
#include "field_weather.h"
#include "line_break.h"
#include "constants/battle_anim.h"
#include "constants/battle_move_effects.h"
#include "constants/battle_partner.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/songs.h"
#include "constants/trainers.h"
#include "constants/rgb.h"
#include "caps.h"
#include "debug.h"
#include "event_data.h"
#include "menu.h"
#include "pokemon_summary_screen.h"
#include "type_icons.h"
#include "pokedex.h"
#include "test/battle.h"
#include "test/test_runner_battle.h"

extern void SpriteCB_BlinkVisible(struct Sprite *sprite);

static void PlayerHandleLoadMonSprite(enum BattlerId battler);
static void PlayerHandleDrawTrainerPic(enum BattlerId battler);
static void PlayerHandleTrainerSlide(enum BattlerId battler);
static void PlayerHandleTrainerSlideBack(enum BattlerId battler);
static void PlayerHandlePaletteFade(enum BattlerId battler);
static void PlayerHandlePause(enum BattlerId battler);
static void PlayerHandleChooseAction(enum BattlerId battler);
static void PlayerHandleYesNoBox(enum BattlerId battler);
static void PlayerHandleChooseItem(enum BattlerId battler);
static void PlayerHandleChoosePokemon(enum BattlerId battler);
static void PlayerHandleCmd23(enum BattlerId battler);
static void PlayerHandleStatusXor(enum BattlerId battler);
static void PlayerHandleDMA3Transfer(enum BattlerId battler);
static void PlayerHandlePlayBGM(enum BattlerId battler);
static void PlayerHandleTwoReturnValues(enum BattlerId battler);
static void PlayerHandleChosenMonReturnValue(enum BattlerId battler);
static void PlayerHandleOneReturnValue(enum BattlerId battler);
static void PlayerHandleOneReturnValue_Duplicate(enum BattlerId battler);
static void PlayerHandleIntroTrainerBallThrow(enum BattlerId battler);
static void PlayerHandleDrawPartyStatusSummary(enum BattlerId battler);
static void PlayerHandleEndBounceEffect(enum BattlerId battler);
static void PlayerHandleLinkStandbyMsg(enum BattlerId battler);
static void PlayerHandleResetActionMoveSelection(enum BattlerId battler);
static void PlayerHandleEndLinkBattle(enum BattlerId battler);
static void PlayerHandleBattleDebug(enum BattlerId battler);

static void PlayerBufferRunCommand(enum BattlerId battler);
static void MoveSelectionDisplayPpNumber(enum BattlerId battler);
static void MoveSelectionDisplayPpString(enum BattlerId battler);
static void MoveSelectionDisplayMoveType(enum BattlerId battler);
static void MoveSelectionDisplayMoveNames(enum BattlerId battler);
static void TryMoveSelectionDisplayMoveDescription(enum BattlerId battler);
static void MoveSelectionDisplayMoveDescription(enum BattlerId battler);
static void WaitForMonSelection(enum BattlerId battler);
static void CompleteWhenChoseItem(enum BattlerId battler);
static void Task_LaunchLvlUpAnim(u8);
static void Task_PrepareToGiveExpWithExpBar(u8);
static void Task_SetControllerToWaitForString(u8);
static void Task_GiveExpWithExpBar(u8);
static void Task_UpdateLvlInHealthbox(u8);
static void PrintLinkStandbyMsg(void);
static void CreateInfoWindow(u32 battler);
static void HideAllTargets(void);
static void HideShownTargets(enum BattlerId battler);
static void ShowMovePreviewTargets(enum BattlerId battler);
static void TryShowAsTarget(enum BattlerId battler);
static void CreateSpeedTiersWindow(u32 battler);

static void ReloadMoveNames(enum BattlerId battler);
static u32 CheckTypeEffectiveness(enum BattlerId battlerAtk, enum BattlerId battlerDef);
static u32 CheckTargetTypeEffectiveness(enum BattlerId battler);
static void MoveSelectionDisplayMoveEffectiveness(u32 foeEffectiveness, enum BattlerId battler);

static void (*const sPlayerBufferCommands[CONTROLLER_CMDS_COUNT])(enum BattlerId battler) =
{
    [CONTROLLER_GETMONDATA]               = BtlController_HandleGetMonData,
    [CONTROLLER_GETRAWMONDATA]            = BtlController_HandleGetRawMonData,
    [CONTROLLER_SETMONDATA]               = BtlController_HandleSetMonData,
    [CONTROLLER_SETRAWMONDATA]            = BtlController_HandleSetRawMonData,
    [CONTROLLER_LOADMONSPRITE]            = PlayerHandleLoadMonSprite,
    [CONTROLLER_SWITCHINANIM]             = BtlController_HandleSwitchInAnim,
    [CONTROLLER_RETURNMONTOBALL]          = BtlController_HandleReturnMonToBall,
    [CONTROLLER_DRAWTRAINERPIC]           = PlayerHandleDrawTrainerPic,
    [CONTROLLER_TRAINERSLIDE]             = PlayerHandleTrainerSlide,
    [CONTROLLER_TRAINERSLIDEBACK]         = PlayerHandleTrainerSlideBack,
    [CONTROLLER_FAINTANIMATION]           = BtlController_HandleFaintAnimation,
    [CONTROLLER_PALETTEFADE]              = PlayerHandlePaletteFade,
    [CONTROLLER_BALLTHROWANIM]            = BtlController_HandleBallThrowAnim,
    [CONTROLLER_PAUSE]                    = PlayerHandlePause,
    [CONTROLLER_MOVEANIMATION]            = BtlController_HandleMoveAnimation,
    [CONTROLLER_PRINTSTRING]              = BtlController_HandlePrintString,
    [CONTROLLER_PRINTSTRINGPLAYERONLY]    = BtlController_HandlePrintStringPlayerOnly,
    [CONTROLLER_CHOOSEACTION]             = PlayerHandleChooseAction,
    [CONTROLLER_YESNOBOX]                 = PlayerHandleYesNoBox,
    [CONTROLLER_CHOOSEMOVE]               = PlayerHandleChooseMove,
    [CONTROLLER_OPENBAG]                  = PlayerHandleChooseItem,
    [CONTROLLER_CHOOSEPOKEMON]            = PlayerHandleChoosePokemon,
    [CONTROLLER_23]                       = PlayerHandleCmd23,
    [CONTROLLER_HEALTHBARUPDATE]          = BtlController_HandleHealthBarUpdate,
    [CONTROLLER_EXPUPDATE]                = PlayerHandleExpUpdate,
    [CONTROLLER_STATUSICONUPDATE]         = BtlController_HandleStatusIconUpdate,
    [CONTROLLER_STATUSANIMATION]          = BtlController_HandleStatusAnimation,
    [CONTROLLER_STATUSXOR]                = PlayerHandleStatusXor,
    [CONTROLLER_DATATRANSFER]             = BtlController_Empty,
    [CONTROLLER_DMA3TRANSFER]             = PlayerHandleDMA3Transfer,
    [CONTROLLER_PLAYBGM]                  = PlayerHandlePlayBGM,
    [CONTROLLER_32]                       = BtlController_Empty,
    [CONTROLLER_TWORETURNVALUES]          = PlayerHandleTwoReturnValues,
    [CONTROLLER_CHOSENMONRETURNVALUE]     = PlayerHandleChosenMonReturnValue,
    [CONTROLLER_ONERETURNVALUE]           = PlayerHandleOneReturnValue,
    [CONTROLLER_ONERETURNVALUE_DUPLICATE] = PlayerHandleOneReturnValue_Duplicate,
    [CONTROLLER_HITANIMATION]             = BtlController_HandleHitAnimation,
    [CONTROLLER_CANTSWITCH]               = BtlController_Empty,
    [CONTROLLER_PLAYSE]                   = BtlController_HandlePlaySE,
    [CONTROLLER_PLAYFANFAREORBGM]         = BtlController_HandlePlayFanfareOrBGM,
    [CONTROLLER_FAINTINGCRY]              = BtlController_HandleFaintingCry,
    [CONTROLLER_INTROSLIDE]               = BtlController_HandleIntroSlide,
    [CONTROLLER_INTROTRAINERBALLTHROW]    = PlayerHandleIntroTrainerBallThrow,
    [CONTROLLER_DRAWPARTYSTATUSSUMMARY]   = PlayerHandleDrawPartyStatusSummary,
    [CONTROLLER_HIDEPARTYSTATUSSUMMARY]   = BtlController_HandleHidePartyStatusSummary,
    [CONTROLLER_ENDBOUNCE]                = PlayerHandleEndBounceEffect,
    [CONTROLLER_SPRITEINVISIBILITY]       = BtlController_HandleSpriteInvisibility,
    [CONTROLLER_BATTLEANIMATION]          = BtlController_HandleBattleAnimation,
    [CONTROLLER_LINKSTANDBYMSG]           = PlayerHandleLinkStandbyMsg,
    [CONTROLLER_RESETACTIONMOVESELECTION] = PlayerHandleResetActionMoveSelection,
    [CONTROLLER_ENDLINKBATTLE]            = PlayerHandleEndLinkBattle,
    [CONTROLLER_DEBUGMENU]                = PlayerHandleBattleDebug,
    [CONTROLLER_TERMINATOR_NOP]           = BtlController_TerminatorNop
};

void SetControllerToPlayer(enum BattlerId battler)
{
    gBattlerBattleController[battler] = BATTLE_CONTROLLER_PLAYER;
    gBattlerControllerEndFuncs[battler] = PlayerBufferExecCompleted;
    gBattlerControllerFuncs[battler] = PlayerBufferRunCommand;
    gDoingBattleAnim = FALSE;
    gPlayerDpadHoldFrames = 0;
}

void PlayerBufferExecCompleted(enum BattlerId battler)
{
    gBattlerControllerFuncs[battler] = PlayerBufferRunCommand;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        u8 playerId = GetMultiplayerId();

        PrepareBufferDataTransferLink(battler, B_COMM_CONTROLLER_IS_DONE, 4, &playerId);
        gBattleResources->bufferA[battler][0] = CONTROLLER_TERMINATOR_NOP;
    }
    else
    {
        MarkBattleControllerIdleOnLocal(battler);
    }
}

static void PlayerBufferRunCommand(enum BattlerId battler)
{
    if (IsBattleControllerActiveOnLocal(battler))
    {
        if (gBattleResources->bufferA[battler][0] < ARRAY_COUNT(sPlayerBufferCommands))
            sPlayerBufferCommands[gBattleResources->bufferA[battler][0]](battler);
        else
            BtlController_Complete(battler);
    }
}

static void CompleteOnBattlerSpritePosX_0(enum BattlerId battler)
{
    if (gSprites[gBattlerSpriteIds[battler]].x2 == 0)
        BtlController_Complete(battler);
}

static u16 GetPrevBall(u16 ballId)
{
    s32 i;
    s32 index = ItemIdToBallId(ballId);
    u32 newBall = 0;
     for (i = 0; i < POKEBALL_COUNT; i++)
    {
        index--;
        if (index == -1)
            index = POKEBALL_COUNT - 1;
        newBall = gPokeBalls[index].itemId;
        if (CheckBagHasItem(newBall, 1))
            return newBall;
    }
    return ballId;
}

static u32 GetNextBall(u32 ballId)
{
    s32 i;
    s32 index = ItemIdToBallId(ballId);
    u32 newBall = 0;
    for (i = 0; i < POKEBALL_COUNT; i++)
    {
        index++;
        if (index == POKEBALL_COUNT)
            index = 0;
        newBall = gPokeBalls[index].itemId;
        if (CheckBagHasItem(newBall, 1))
            return newBall;
    }
    return ballId;
}

static void HandleInputChooseAction(enum BattlerId battler)
{
    enum Item itemId = gBattleResources->bufferA[battler][2] | (gBattleResources->bufferA[battler][3] << 8);

    DoBounceEffect(battler, BOUNCE_HEALTHBOX, 7, 1);
    DoBounceEffect(battler, BOUNCE_MON, 7, 1);

    if (JOY_REPEAT(DPAD_ANY) && gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A)
        gPlayerDpadHoldFrames++;
    else
        gPlayerDpadHoldFrames = 0;

    if (B_LAST_USED_BALL == TRUE && B_LAST_USED_BALL_CYCLE == TRUE
    && !(B_LAST_USED_BALL_BUTTON == L_BUTTON && gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A))
    {
        if (!gLastUsedBallMenuPresent)
        {
            gBattleStruct->ackBallUseBtn = FALSE;
        }
        else if (JOY_NEW(B_LAST_USED_BALL_BUTTON))
        {
            gBattleStruct->ackBallUseBtn = TRUE;
            gBattleStruct->ballSwapped = FALSE;
            ArrowsChangeColorLastBallCycle(TRUE);
        }

        if (gBattleStruct->ackBallUseBtn)
        {
            if (JOY_HELD(B_LAST_USED_BALL_BUTTON) && (JOY_NEW(DPAD_DOWN) || JOY_NEW(DPAD_RIGHT)))
            {
                bool32 sameBall = FALSE;
                u32 nextBall = GetNextBall(gBallToDisplay);
                gBattleStruct->ballSwapped = TRUE;
                if (gBallToDisplay == nextBall)
                    sameBall = TRUE;
                else
                    gBallToDisplay = nextBall;
                SwapBallToDisplay(sameBall);
                PlaySE(SE_SELECT);
            }
            else if (JOY_HELD(B_LAST_USED_BALL_BUTTON) && (JOY_NEW(DPAD_UP) || JOY_NEW(DPAD_LEFT)))
            {
                bool32 sameBall = FALSE;
                u32 prevBall = GetPrevBall(gBallToDisplay);
                gBattleStruct->ballSwapped = TRUE;
                if (gBallToDisplay == prevBall)
                    sameBall = TRUE;
                else
                    gBallToDisplay = prevBall;
                SwapBallToDisplay(sameBall);
                PlaySE(SE_SELECT);
            }
            else if (JOY_NEW(B_BUTTON) || (!JOY_HELD(B_LAST_USED_BALL_BUTTON) && gBattleStruct->ballSwapped))
            {
                gBattleStruct->ackBallUseBtn = FALSE;
                gBattleStruct->ballSwapped = FALSE;
                ArrowsChangeColorLastBallCycle(FALSE);
            }
            else if (!JOY_HELD(B_LAST_USED_BALL_BUTTON) && CanThrowLastUsedBall())
            {
                gBattleStruct->ackBallUseBtn = FALSE;
                PlaySE(SE_SELECT);
                ArrowsChangeColorLastBallCycle(FALSE);
                // TryToHideMovePreviewWindow();
                TryHideLastUsedBall();
                BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_THROW_BALL, 0);
                BtlController_Complete(battler);
            }
            return;
        }
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        // TryToHideMovePreviewWindow(); // FTP
        TryHideLastUsedBall();

        if (gBattleStruct->movePreviewDisplayed > 0
         && (gActionSelectionCursor[battler] == 0 || gActionSelectionCursor[battler] == 3))
            HideAllTargets();

        switch (gActionSelectionCursor[battler])
        {
        case 0: // Top left
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_USE_MOVE, 0);
            break;
        case 1: // Top right
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_USE_ITEM, 0);
            break;
        case 2: // Bottom left
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_SWITCH, 0);
            break;
        case 3: // Bottom right
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_RUN, 0);
            break;
        }
        BtlController_Complete(battler);
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        if (gActionSelectionCursor[battler] & 1) // if is B_ACTION_USE_ITEM or B_ACTION_RUN
        {
            PlaySE(SE_SELECT);
            ActionSelectionDestroyCursorAt(gActionSelectionCursor[battler]);
            gActionSelectionCursor[battler] ^= 1;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[battler], 0);
        }
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        if (!(gActionSelectionCursor[battler] & 1)) // if is B_ACTION_USE_MOVE or B_ACTION_SWITCH
        {
            PlaySE(SE_SELECT);
            ActionSelectionDestroyCursorAt(gActionSelectionCursor[battler]);
            gActionSelectionCursor[battler] ^= 1;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[battler], 0);
        }
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if (gActionSelectionCursor[battler] & 2) // if is B_ACTION_SWITCH or B_ACTION_RUN
        {
            PlaySE(SE_SELECT);
            ActionSelectionDestroyCursorAt(gActionSelectionCursor[battler]);
            gActionSelectionCursor[battler] ^= 2;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[battler], 0);
        }
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (!(gActionSelectionCursor[battler] & 2)) // if is B_ACTION_USE_MOVE or B_ACTION_USE_ITEM
        {
            PlaySE(SE_SELECT);
            ActionSelectionDestroyCursorAt(gActionSelectionCursor[battler]);
            gActionSelectionCursor[battler] ^= 2;
            ActionSelectionCreateCursorAt(gActionSelectionCursor[battler], 0);
        }
    }
    else if (JOY_NEW(B_BUTTON) || gPlayerDpadHoldFrames > 59)
    {
        if (IsDoubleBattle()
         && GetBattlerPosition(battler) == B_POSITION_PLAYER_RIGHT
         && !(gAbsentBattlerFlags & (1u << GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)))
         && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
        {
            // Return item to bag if partner had selected one (if consumable).
            if (gBattleResources->bufferA[battler][1] == B_ACTION_USE_ITEM && GetItemConsumability(itemId))
            {
                AddBagItem(itemId, 1);
            }
            PlaySE(SE_SELECT);
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_CANCEL_PARTNER, 0);
            BtlController_Complete(battler);
        }
        else if (B_QUICK_MOVE_CURSOR_TO_RUN)
        {
            if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER)) // If wild battle, pressing B moves cursor to "Run".
            {
                PlaySE(SE_SELECT);
                ActionSelectionDestroyCursorAt(gActionSelectionCursor[battler]);
                gActionSelectionCursor[battler] = 3;
                ActionSelectionCreateCursorAt(gActionSelectionCursor[battler], 0);
            }
        }
    }
    else if (JOY_NEW(START_BUTTON))
    {
        PlaySE(SE_SELECT);
        SwapHpBarsWithHpText();
    }
    else if (DEBUG_BATTLE_MENU == TRUE 
        && JOY_NEW(SELECT_BUTTON))
    {
        BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_DEBUG, 0);
        BtlController_Complete(battler);
    }
    else if (B_LAST_USED_BALL == TRUE 
        && B_LAST_USED_BALL_CYCLE == FALSE
        && JOY_NEW(B_LAST_USED_BALL_BUTTON) 
        && CanThrowLastUsedBall())
    {
        PlaySE(SE_SELECT);
        TryHideLastUsedBall();
        // TryToHideMovePreviewWindow();
        BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_THROW_BALL, 0);
        BtlController_Complete(battler);
    }
    else if (JOY_NEW(SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        switch(gBattleStruct->movePreviewDisplayed)
        {
            case 0:
                // if(gSaveBlock2Ptr->optionsPreviewStyle == OPTIONS_PREVIEW_LIMITED
                //   || gSaveBlock2Ptr->optionsPreviewStyle == OPTIONS_PREVIEW_FULL)
                // {
                    CreateSpeedTiersWindow(battler);
                    gBattleStruct->movePreviewDisplayed=1;
                // }
                break;
            case 1:
                if (gSaveBlock2Ptr->optionsPreviewStyle == OPTIONS_PREVIEW_FULL)
                {
                    if(IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT)))
                    {
                        CreateMovePreviewText(B_POSITION_OPPONENT_RIGHT);
                        gBattleStruct->movePreviewDisplayed=2;
                    }
                    else
                    {
                        CreateMovePreviewText(B_POSITION_OPPONENT_LEFT);
                        gBattleStruct->movePreviewDisplayed=3;
                    }
                }
                else
                {
                    CreateInfoWindow(battler);
                    gBattleStruct->movePreviewDisplayed=0;
                }
                break;
            case 2:
                if(IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)))
                {
                    CreateMovePreviewText(B_POSITION_OPPONENT_LEFT);
                    gBattleStruct->movePreviewDisplayed=3;
                }
                else
                {
                    HideAllTargets();
                    CreateInfoWindow(battler);
                    gBattleStruct->movePreviewDisplayed=0;
                }
                break;
            case 3:
                    HideAllTargets();
                    CreateInfoWindow(battler);
                    gBattleStruct->movePreviewDisplayed=0;
            break;
        }
    }
}

static void AppendSpeed(u32 battler)
{
    ConvertUIntToDecimalStringN(
        gStringVar2,
        GetBattlerTotalSpeedStat(
            battler,
            GetBattlerAbility(battler),
            GetBattlerHoldEffect(battler)
        ),
        STR_CONV_MODE_LEFT_ALIGN,
        3
    );
    StringAppend(gStringVar1, gStringVar2);
}

static void AppendMoveTarget(u32 battler, bool32 isRightSide)
{
    u32 move = gBattleMons[battler].moves[gBattleStruct->chosenMovePositions[battler]];
    u32 moveTarget = GetBattlerMoveTargetType(battler, move);
    bool32 needsExtraSpace = !isRightSide;
    
    if (move == MOVE_REFLECT 
        || move == MOVE_LIGHT_SCREEN
        || move == MOVE_AURORA_VEIL
        || move == MOVE_TAILWIND
        || move == MOVE_SAFEGUARD
        || move == MOVE_LUCKY_CHANT
        || move == MOVE_MIST
        || move == MOVE_HEAL_BELL
        || move == MOVE_AROMATHERAPY
        || move == MOVE_WIDE_GUARD
        || move == MOVE_QUICK_GUARD
        || move == MOVE_CRAFTY_SHIELD
        || move == MOVE_MAGNETIC_FLUX
        || move == MOVE_MAT_BLOCK
        || move == MOVE_GEAR_UP
        || move == MOVE_LIFE_DEW
    ) {
        moveTarget = TARGET_USER_AND_ALLY;
    }

    switch (moveTarget)
    {
    case TARGET_USER:
    case TARGET_DEPENDS:
    case TARGET_OPPONENT:
        StringAppend(gStringVar1,
            isRightSide ? (needsExtraSpace ? COMPOUND_STRING(" {UP_ARROW}-") : COMPOUND_STRING("{UP_ARROW}-"))
                        : (needsExtraSpace ? COMPOUND_STRING(" -{UP_ARROW}") : COMPOUND_STRING("-{UP_ARROW}")));
        break;

    case TARGET_SELECTED:
    case TARGET_USER_OR_ALLY:
    {
        u32 target = gAiBattleData->chosenTarget[battler];

        if (target == B_POSITION_OPPONENT_LEFT)
            StringAppend(gStringVar1, needsExtraSpace ? COMPOUND_STRING(" -{UP_ARROW}") : COMPOUND_STRING("-{UP_ARROW}"));
        else if (target == B_POSITION_OPPONENT_RIGHT)
            StringAppend(gStringVar1, needsExtraSpace ? COMPOUND_STRING(" {UP_ARROW}-") : COMPOUND_STRING("{UP_ARROW}-"));
        else if (target == B_POSITION_PLAYER_LEFT)
            StringAppend(gStringVar1, needsExtraSpace ? COMPOUND_STRING(" {DOWN_ARROW}-") : COMPOUND_STRING("{DOWN_ARROW}-"));
        else if (target == B_POSITION_PLAYER_RIGHT)
            StringAppend(gStringVar1, needsExtraSpace ? COMPOUND_STRING(" -{DOWN_ARROW}") : COMPOUND_STRING("-{DOWN_ARROW}"));
        break;
    }

    case TARGET_ALLY:
        StringAppend(gStringVar1,
            isRightSide ? (needsExtraSpace ? COMPOUND_STRING(" -{UP_ARROW}") : COMPOUND_STRING("-{UP_ARROW}"))
                        : (needsExtraSpace ? COMPOUND_STRING(" {UP_ARROW}-") : COMPOUND_STRING("{UP_ARROW}-")));
        break;

    case TARGET_RANDOM:
        StringAppend(gStringVar1, needsExtraSpace ? COMPOUND_STRING(" ??") : COMPOUND_STRING("??"));
        break;

    case TARGET_USER_AND_ALLY:
        StringAppend(gStringVar1, needsExtraSpace ? COMPOUND_STRING(" {UP_ARROW}{UP_ARROW}") : COMPOUND_STRING("{UP_ARROW}{UP_ARROW}"));
        break;

    case TARGET_BOTH:
    case TARGET_SMART:
        StringAppend(gStringVar1, needsExtraSpace ? COMPOUND_STRING(" {DOWN_ARROW}{DOWN_ARROW}") : COMPOUND_STRING("{DOWN_ARROW}{DOWN_ARROW}"));
        break;

    case TARGET_OPPONENTS_FIELD:
        StringAppend(gStringVar1, needsExtraSpace ? COMPOUND_STRING(" xx") : COMPOUND_STRING("xx"));
        break;

    case TARGET_FOES_AND_ALLY:
        StringAppend(gStringVar1,
            isRightSide ? (needsExtraSpace ? COMPOUND_STRING(" {DOWN_ARROW}{V_D_ARROW}") : COMPOUND_STRING("{DOWN_ARROW}{V_D_ARROW}"))
                        : (needsExtraSpace ? COMPOUND_STRING(" {V_D_ARROW}{DOWN_ARROW}") : COMPOUND_STRING("{V_D_ARROW}{DOWN_ARROW}")));
        break;

    case TARGET_FIELD:
    case TARGET_ALL_BATTLERS:
        StringAppend(gStringVar1, needsExtraSpace ? COMPOUND_STRING(" {V_D_ARROW}{V_D_ARROW}") : COMPOUND_STRING("{V_D_ARROW}{V_D_ARROW}"));
        break;
    }
}

static void AppendBattlerInfo(u32 position, bool32 isRightSide, bool32 showSpeedFirst)
{
    u32 battler = GetBattlerAtPosition(position);
    if (!IsBattlerAlive(battler))
        return;

    if (gBattleStruct->monToSwitchIntoId[battler] != PARTY_SIZE)
    {
        StringAppend(gStringVar1, COMPOUND_STRING("Switch"));
        return;
    }

    if (showSpeedFirst)
    {
        AppendSpeed(battler);
        if (gSaveBlock2Ptr->optionsPreviewStyle != OPTIONS_PREVIEW_NONE){
            AppendMoveTarget(battler, isRightSide);
        }
    }
    else
    {
        if (gSaveBlock2Ptr->optionsPreviewStyle != OPTIONS_PREVIEW_NONE){
            AppendMoveTarget(battler, isRightSide);
            StringAppend(gStringVar1, COMPOUND_STRING(" "));
        }
        AppendSpeed(battler);
    }
}

static void CreateSpeedTiersWindow(u32 battler)
{
    if (gSaveBlock2Ptr->optionsPreviewStyle == OPTIONS_PREVIEW_NONE) {
        StringCopy(gStringVar1, COMPOUND_STRING("Speed    "));
    }
    else 
        StringCopy(gStringVar1, COMPOUND_STRING("OPP   "));

    bool32 rightAlive = IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT));
    bool32 leftAlive  = IsBattlerAlive(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT));

    if (rightAlive)
        AppendBattlerInfo(B_POSITION_OPPONENT_RIGHT, TRUE, FALSE);

    if (leftAlive)
    {
        if (rightAlive)
            StringAppend(gStringVar1, COMPOUND_STRING(" / "));

        AppendBattlerInfo(B_POSITION_OPPONENT_LEFT, FALSE, TRUE);
    }

    if (gSaveBlock2Ptr->optionsPreviewStyle == OPTIONS_PREVIEW_NONE) {
        StringAppend(gStringVar1, COMPOUND_STRING("\nTiers   "));
    } 
    else 
        StringAppend(gStringVar1, COMPOUND_STRING("\nYOU   "));
    
    bool32 pLeftAlive  = IsBattlerAlive(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
    bool32 pRightAlive = IsBattlerAlive(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT));
    
    if (pLeftAlive)
    {
        AppendSpeed(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));

        if (battler == GetBattlerAtPosition(B_POSITION_PLAYER_LEFT))
            StringAppend(gStringVar1, COMPOUND_STRING("?"));
    }

    if (pRightAlive)
    {
        if (pLeftAlive)
            StringAppend(gStringVar1, COMPOUND_STRING(" / "));

        AppendSpeed(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT));

        if (battler == GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT))
            StringAppend(gStringVar1, COMPOUND_STRING("?"));
    }

    BattlePutTextOnWindow(gStringVar1, B_WIN_ACTION_PROMPT);
}

void CreateMovePreviewText(u32 battlerPosition)
{
        u32 switchMon;
        u32 battler = GetBattlerAtPosition(battlerPosition);

        HideAllTargets();

        if (gBattleStruct->monToSwitchIntoId[GetBattlerAtPosition(battlerPosition)] != PARTY_SIZE) // If the opponent is switching:
        {
            switchMon = GetMonData(&gEnemyParty[gAiLogicData->mostSuitableMonId[battler]], MON_DATA_SPECIES);
            StringCopy(gStringVar1, GetSpeciesName(GetMonData(GetBattlerMon(GetBattlerAtPosition(battlerPosition)), MON_DATA_SPECIES, NULL)));
            StringAppend(gStringVar1, COMPOUND_STRING(" will switch\nto "));
            StringAppend(gStringVar1, GetSpeciesName(switchMon));
            StringAppend(gStringVar1, COMPOUND_STRING("!"));
        }
        else
        {
            StringCopy(gStringVar1, GetSpeciesName(GetMonData(GetBattlerMon(GetBattlerAtPosition(battlerPosition)), MON_DATA_SPECIES, NULL)));
            StringAppend(gStringVar1, COMPOUND_STRING(" will use:\n"));
            u32 move = gBattleMons[battlerPosition].moves[gBattleStruct->chosenMovePositions[battlerPosition]];
            StringAppend(gStringVar1, GetMoveName(move));

            if (battlerPosition == B_POSITION_OPPONENT_RIGHT)
                StringAppend(gStringVar1, COMPOUND_STRING(" "));

            if (battlerPosition == B_POSITION_OPPONENT_RIGHT)
                AppendMoveTarget(battler, TRUE);
            else 
                AppendMoveTarget(battler, FALSE);
        }
        BattlePutTextOnWindow(gStringVar1, B_WIN_ACTION_PROMPT);
        ShowMovePreviewTargets(battler);
}

void HandleInputChooseTarget(enum BattlerId battler)
{
    enum BattlerId i;
    static const enum BattlerPosition identities[MAX_BATTLERS_COUNT] =
    {
        B_POSITION_PLAYER_LEFT,
        B_POSITION_PLAYER_RIGHT,
        B_POSITION_OPPONENT_RIGHT,
        B_POSITION_OPPONENT_LEFT,
    };
    enum Move move = GetMonData(GetBattlerMon(battler), MON_DATA_MOVE1 + gMoveSelectionCursor[battler]);
    enum MoveTarget moveTarget = GetBattlerMoveTargetType(battler, move);

    DoBounceEffect(gMultiUsePlayerCursor, BOUNCE_HEALTHBOX, 15, 1);
    for (i = 0; i < gBattlersCount; i++)
    {
        if (i != gMultiUsePlayerCursor)
            EndBounceEffect(i, BOUNCE_HEALTHBOX);
    }

    if (JOY_HELD(DPAD_ANY) && gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A)
        gPlayerDpadHoldFrames++;
    else
        gPlayerDpadHoldFrames = 0;

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_HideAsMoveTarget;
        if (gBattleStruct->gimmick.playerSelect)
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, gMoveSelectionCursor[battler] | RET_GIMMICK | (gMultiUsePlayerCursor << 8));
        else
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, gMoveSelectionCursor[battler] | (gMultiUsePlayerCursor << 8));
        EndBounceEffect(gMultiUsePlayerCursor, BOUNCE_HEALTHBOX);
        TryHideLastUsedBall();
        // TryToHideMovePreviewWindow(); // FTP
        HideGimmickTriggerSprite();
        BtlController_Complete(battler);
    }
    else if (JOY_NEW(B_BUTTON) || gPlayerDpadHoldFrames > 59)
    {
        PlaySE(SE_SELECT);
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_HideAsMoveTarget;
        gBattlerControllerFuncs[battler] = HandleInputChooseMove;
        if (gBattleStruct->gimmick.playerSelect == 1 && gBattleStruct->gimmick.usableGimmick[battler] == GIMMICK_Z_MOVE)
        {
            gBattleStruct->gimmick.playerSelect = 0;
            gBattleStruct->zmove.viewing = TRUE;
            ReloadMoveNames(battler);
        }
        TryToAddMoveInfoWindow();
        DoBounceEffect(battler, BOUNCE_HEALTHBOX, 7, 1);
        DoBounceEffect(battler, BOUNCE_MON, 7, 1);
        EndBounceEffect(gMultiUsePlayerCursor, BOUNCE_HEALTHBOX);
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_UP))
    {
        PlaySE(SE_SELECT);
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_HideAsMoveTarget;

        if (moveTarget == TARGET_USER_OR_ALLY)
        {
            gMultiUsePlayerCursor ^= BIT_FLANK;
        }
        else
        {
            bool32 validTarget = FALSE;
            do
            {
                enum BattlerPosition currSelIdentity = GetBattlerPosition(gMultiUsePlayerCursor);

                for (i = 0; i < MAX_BATTLERS_COUNT; i++)
                {
                    if (currSelIdentity == identities[i])
                        break;
                }
                do
                {
                    if (i == 0)
                        i = MAX_BATTLERS_COUNT - 1;
                    else
                        i--;
                    gMultiUsePlayerCursor = GetBattlerAtPosition(identities[i]);
                } while (gMultiUsePlayerCursor >= gBattlersCount);

                switch (GetBattlerPosition(gMultiUsePlayerCursor))
                {
                case B_POSITION_PLAYER_LEFT:
                case B_POSITION_PLAYER_RIGHT:
                    if (battler != gMultiUsePlayerCursor)
                        validTarget = TRUE;
                    break;
                case B_POSITION_OPPONENT_LEFT:
                case B_POSITION_OPPONENT_RIGHT:
                    validTarget = TRUE;
                    break;
                default:
                    break;
                }

                if (gAbsentBattlerFlags & (1u << gMultiUsePlayerCursor)
                 || !CanTargetBattler(battler, gMultiUsePlayerCursor, move)
                 || (moveTarget == TARGET_OPPONENT && IsOnPlayerSide(gMultiUsePlayerCursor)))
                    validTarget = FALSE;
                
                if (B_SHOW_EFFECTIVENESS && validTarget)
                    MoveSelectionDisplayMoveEffectiveness(CheckTypeEffectiveness(battler, gMultiUsePlayerCursor), battler);

            } while (!validTarget);
        }
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_ShowAsMoveTarget;
    }
    else if (JOY_NEW(DPAD_RIGHT | DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_HideAsMoveTarget;

        if (moveTarget == TARGET_USER_OR_ALLY)
        {
            gMultiUsePlayerCursor ^= BIT_FLANK;
        }
        else
        {
            do
            {
                enum BattlerPosition currSelIdentity = GetBattlerPosition(gMultiUsePlayerCursor);

                for (i = 0; i < MAX_BATTLERS_COUNT; i++)
                {
                    if (currSelIdentity == identities[i])
                        break;
                }
                do
                {
                    if (++i > 3)
                        i = 0;
                    gMultiUsePlayerCursor = GetBattlerAtPosition(identities[i]);
                } while (gMultiUsePlayerCursor == gBattlersCount);

                i = 0;
                switch (GetBattlerPosition(gMultiUsePlayerCursor))
                {
                case B_POSITION_PLAYER_LEFT:
                case B_POSITION_PLAYER_RIGHT:
                    if (battler != gMultiUsePlayerCursor)
                        i++;
                    break;
                case B_POSITION_OPPONENT_LEFT:
                case B_POSITION_OPPONENT_RIGHT:
                    i++;
                    break;
                default:
                    break;
                }
                if (B_SHOW_EFFECTIVENESS)
                    MoveSelectionDisplayMoveEffectiveness(CheckTypeEffectiveness(battler, gMultiUsePlayerCursor), battler);

                if (gAbsentBattlerFlags & (1u << gMultiUsePlayerCursor)
                 || !CanTargetBattler(battler, gMultiUsePlayerCursor, move)
                 || (moveTarget == TARGET_OPPONENT && IsOnPlayerSide(gMultiUsePlayerCursor)))
                    i = 0;
            } while (i == 0);
        }

        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_ShowAsMoveTarget;
    }
}

static void HideAllTargets(void)
{
    for (enum BattlerId i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        if (!IsBattlerAlive(i))
            continue;

        struct Sprite *sprite = &gSprites[gBattlerSpriteIds[i]];
        if (sprite->callback == SpriteCB_ShowAsMoveTarget
         || sprite->callback == SpriteCB_BlinkVisible)
        {
            sprite->invisible = sprite->data[4];
            sprite->data[4] = FALSE;
            sprite->callback = SpriteCallbackDummy_2;
            EndBounceEffect(i, BOUNCE_HEALTHBOX);
        }
    }
}

static void HideShownTargets(enum BattlerId battler)
{
    s32 i;
    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        if (!IsBattlerAlive(i))
            continue;

        u8 spriteId = gBattlerSpriteIds[i];
        if (gSprites[spriteId].callback == SpriteCB_ShowAsMoveTarget
         || gSprites[spriteId].callback == SpriteCB_BlinkVisible)
        {
            gSprites[spriteId].callback = SpriteCB_HideAsMoveTarget;
            EndBounceEffect(i, BOUNCE_HEALTHBOX);
        }
    }
}

static void ShowMovePreviewTargets(enum BattlerId battler)
{
    enum BattlerId targetBattler;
    enum Move move = gBattleMons[battler].moves[gBattleStruct->chosenMovePositions[battler]];
    enum MoveTarget moveTarget = GetBattlerMoveTargetType(battler, move);

    if (move == MOVE_REFLECT 
        || move == MOVE_LIGHT_SCREEN
        || move == MOVE_AURORA_VEIL
        || move == MOVE_TAILWIND
        || move == MOVE_SAFEGUARD
        || move == MOVE_LUCKY_CHANT
        || move == MOVE_MIST
        || move == MOVE_HEAL_BELL
        || move == MOVE_AROMATHERAPY
        || move == MOVE_WIDE_GUARD
        || move == MOVE_QUICK_GUARD
        || move == MOVE_CRAFTY_SHIELD
        || move == MOVE_MAGNETIC_FLUX
        || move == MOVE_MAT_BLOCK
        || move == MOVE_GEAR_UP
        || move == MOVE_LIFE_DEW
    ) {
        moveTarget = TARGET_USER_AND_ALLY;
    }

    HideAllTargets();

    switch (moveTarget)
    {
    case TARGET_USER:
    case TARGET_DEPENDS:
    case TARGET_RANDOM:
    case TARGET_USER_OR_ALLY:
        if (IsBattlerAlive(battler))
            TryShowAsTarget(battler);
        break;
    case TARGET_ALLY:
        targetBattler = BATTLE_PARTNER(battler);
        if (IsBattlerAlive(targetBattler))
            TryShowAsTarget(targetBattler);
        break;
    case TARGET_USER_AND_ALLY:
        if (IsBattlerAlive(battler))
            TryShowAsTarget(battler);
        targetBattler = BATTLE_PARTNER(battler);
        if (IsBattlerAlive(targetBattler))
            TryShowAsTarget(targetBattler);
        break;
    case TARGET_SELECTED:
    case TARGET_OPPONENT:
    case TARGET_SMART:
        targetBattler = gAiBattleData->chosenTarget[battler];
        if (targetBattler < MAX_BATTLERS_COUNT && IsBattlerAlive(targetBattler))
            TryShowAsTarget(targetBattler);
        break;
    case TARGET_OPPONENTS_FIELD:
    case TARGET_BOTH:
        for (targetBattler = 0; targetBattler < MAX_BATTLERS_COUNT; targetBattler++)
        {
            if (IsBattlerAlive(targetBattler) && IsOnPlayerSide(targetBattler))
                TryShowAsTarget(targetBattler);
        }
        break;
    case TARGET_FOES_AND_ALLY:
        // Show both battlers on the opposing (player) side
        for (targetBattler = 0; targetBattler < MAX_BATTLERS_COUNT; targetBattler++)
        {
            if (IsBattlerAlive(targetBattler) && IsOnPlayerSide(targetBattler))
                TryShowAsTarget(targetBattler);
        }

        // Also show the attacker's partner (ally) if alive
        if (IsBattlerAlive(BATTLE_PARTNER(battler)))
            TryShowAsTarget(BATTLE_PARTNER(battler));
        break;
    case TARGET_FIELD:
    case TARGET_ALL_BATTLERS:
        for (targetBattler = 0; targetBattler < MAX_BATTLERS_COUNT; targetBattler++)
        {
            if (IsBattlerAlive(targetBattler))
                TryShowAsTarget(targetBattler);
        }
        break;
    default:
        break;
    }
}

void HandleInputShowEntireFieldTargets(enum BattlerId battler)
{
    if (JOY_HELD(DPAD_ANY) && gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A)
        gPlayerDpadHoldFrames++;
    else
        gPlayerDpadHoldFrames = 0;

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        HideAllTargets();
        if (gBattleStruct->gimmick.playerSelect)
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, gMoveSelectionCursor[battler] | RET_GIMMICK | (gMultiUsePlayerCursor << 8));
        else
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, gMoveSelectionCursor[battler] | (gMultiUsePlayerCursor << 8));
        HideGimmickTriggerSprite();
        BtlController_Complete(battler);
    }
    else if (JOY_NEW(B_BUTTON) || gPlayerDpadHoldFrames > 59)
    {
        PlaySE(SE_SELECT);
        HideAllTargets();
        gBattlerControllerFuncs[battler] = HandleInputChooseMove;
        DoBounceEffect(battler, BOUNCE_HEALTHBOX, 7, 1);
        DoBounceEffect(battler, BOUNCE_MON, 7, 1);
    }
}

void HandleInputShowTargets(enum BattlerId battler)
{
    if (JOY_HELD(DPAD_ANY) && gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A)
        gPlayerDpadHoldFrames++;
    else
        gPlayerDpadHoldFrames = 0;

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        HideShownTargets(battler);
        if (gBattleStruct->gimmick.playerSelect)
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, gMoveSelectionCursor[battler] | RET_GIMMICK | (gMultiUsePlayerCursor << 8));
        else
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, gMoveSelectionCursor[battler] | (gMultiUsePlayerCursor << 8));
        HideGimmickTriggerSprite();
        TryHideLastUsedBall();
        BtlController_Complete(battler);
    }
    else if (JOY_NEW(B_BUTTON) || gPlayerDpadHoldFrames > 59)
    {
        PlaySE(SE_SELECT);
        HideShownTargets(battler);
        gBattlerControllerFuncs[battler] = HandleInputChooseMove;
        DoBounceEffect(battler, BOUNCE_HEALTHBOX, 7, 1);
        DoBounceEffect(battler, BOUNCE_MON, 7, 1);
    }
}

static void TryShowAsTarget(enum BattlerId battler)
{
    if (IsBattlerAlive(battler))
    {
        struct Sprite *sprite = &gSprites[gBattlerSpriteIds[battler]];

        sprite->data[4] = sprite->invisible;
        DoBounceEffect(battler, BOUNCE_HEALTHBOX, 15, 1);
        sprite->callback = SpriteCB_ShowAsMoveTarget;
    }
}

static bool32 CanSelectBattler(enum MoveTarget target)
{
    switch (target)
    {
    case TARGET_RANDOM:
    case TARGET_BOTH:
    case TARGET_DEPENDS:
    case TARGET_FOES_AND_ALLY:
    case TARGET_OPPONENTS_FIELD:
    case TARGET_USER:
    case TARGET_ALLY:
    case TARGET_USER_OR_ALLY:
    case TARGET_USER_AND_ALLY:
        return TRUE;
    default:
        break;
    }

    return FALSE;
}

static void LoadMovesIntoBuffer(struct ChooseMoveStruct *dst, enum BattlerId src)
{
    for (int i = 0; i < MAX_MON_MOVES; i++)
    {
        dst->moves[i]     = gBattleMons[src].moves[i];
        dst->currentPp[i] = gBattleMons[src].pp[i];
        dst->maxPp[i]     = gBattleMons[src].pp[i];
    }
}

void SwapMoveView(enum BattlerId currentBattler, enum BattlerId targetBattler, enum BattlerId playerBattler)
{
    MoveSelectionDisplayMoveNames(targetBattler);
    MoveSelectionDisplayPpNumber(targetBattler);
    MoveSelectionDisplayMoveType(targetBattler);
    MoveSelectionCreateCursorAt(gMoveSelectionCursor[playerBattler], 0);
    EndBounceEffect(currentBattler, BOUNCE_HEALTHBOX);
    EndBounceEffect(currentBattler, BOUNCE_MON);
    DoBounceEffect(targetBattler, BOUNCE_HEALTHBOX, 7, 1);
    DoBounceEffect(targetBattler, BOUNCE_MON, 7, 1);
}

void HandleViewOpposingMoves(enum BattlerId playerBattler, bool32 pressedRButton, struct ChooseMoveStruct *moveInfo)
{
    enum BattlerId oppLeft  = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
    enum BattlerId oppRight = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);

    enum BattlerId target = pressedRButton ? oppRight : oppLeft;
    bool32 viewing = gBattleStruct->viewingOpponentMoves;
    bool32 viewingRight = gBattleStruct->viewingRightOpponent;

    PlaySE(SE_SELECT);

    // --- ENTER VIEW ---
    if (!viewing)
    {
        gBattleStruct->viewingOpponentMoves = TRUE;
        gBattleStruct->viewingRightOpponent = pressedRButton;

        // Save player data (non-destructive preview)
        for (int i = 0; i < MAX_MON_MOVES; i++)
        {
            gBattleStruct->savedPlayerMoves[i] = moveInfo->moves[i];
            gBattleStruct->savedPlayerPP[i]    = moveInfo->currentPp[i];
            gBattleStruct->savedPlayerMaxPP[i] = moveInfo->maxPp[i];
        }

        LoadMovesIntoBuffer(moveInfo, target);
        SwapMoveView(playerBattler, target, playerBattler);
        return;
    }

    // --- EXIT VIEW (pressed same side again) ---
    if ((viewingRight && pressedRButton) || (!viewingRight && !pressedRButton))
    {
        gBattleStruct->viewingOpponentMoves = FALSE;

        // Restore player data
        for (int i = 0; i < MAX_MON_MOVES; i++)
        {
            moveInfo->moves[i]     = gBattleStruct->savedPlayerMoves[i];
            moveInfo->currentPp[i] = gBattleStruct->savedPlayerPP[i];
            moveInfo->maxPp[i]     = gBattleStruct->savedPlayerMaxPP[i];
        }

        SwapMoveView(target, playerBattler, playerBattler);
        return;
    }

    // --- SWITCH OPPONENT ---
    gBattleStruct->viewingRightOpponent = pressedRButton;

    LoadMovesIntoBuffer(moveInfo, target);

    // Determine previous opponent for clean bounce removal
    enum BattlerId prev = viewingRight ? oppRight : oppLeft;
    SwapMoveView(prev, target, playerBattler);
}

void HandleInputChooseMove(enum BattlerId battler)
{
    u32 canSelectTarget = 0;
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);

    if (JOY_HELD(DPAD_ANY) && gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A)
        gPlayerDpadHoldFrames++;
    else
        gPlayerDpadHoldFrames = 0;

    if (JOY_NEW(L_BUTTON) 
        && IsDoubleBattle() 
        && !(gAbsentBattlerFlags & (1u << GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT)))) 
    {
        HandleViewOpposingMoves(battler, FALSE, moveInfo);
    }

    if (JOY_NEW(R_BUTTON) 
        && IsDoubleBattle() 
        && !(gAbsentBattlerFlags & (1u << GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT)))) 
    {
        HandleViewOpposingMoves(battler, TRUE, moveInfo);
    }

    if (JOY_NEW(A_BUTTON) 
        && !gBattleStruct->descriptionSubmenu 
        && !gBattleStruct->viewingOpponentMoves)
    // if (JOY_NEW(A_BUTTON) && !gBattleStruct->descriptionSubmenu)
    {
        TryToHideMoveInfoWindow();
        PlaySE(SE_SELECT);

        enum MoveTarget moveTarget = GetBattlerMoveTargetType(battler, moveInfo->moves[gMoveSelectionCursor[battler]]);
        bool32 isUserOrAlly = moveTarget == TARGET_USER || moveTarget == TARGET_USER_OR_ALLY || moveTarget == TARGET_USER_AND_ALLY;

        if (gBattleStruct->zmove.viewing)
        {
            gBattleStruct->zmove.viewing = FALSE;
            if (GetMoveCategory(moveInfo->moves[gMoveSelectionCursor[battler]]) != DAMAGE_CATEGORY_STATUS)
                moveTarget = TARGET_SELECTED;  //damaging z moves always have selected target
        }

        // Status moves turn into Max Guard when Dynamaxed, targets user.
        if (GetActiveGimmick(battler) == GIMMICK_DYNAMAX || IsGimmickSelected(battler, GIMMICK_DYNAMAX))
            moveTarget = GetMoveTarget(GetMaxMove(battler, moveInfo->moves[gMoveSelectionCursor[battler]]));

        if (isUserOrAlly)
            gMultiUsePlayerCursor = battler;
        else if (moveTarget == TARGET_ALLY)
            gMultiUsePlayerCursor = BATTLE_PARTNER(battler);
        else
            gMultiUsePlayerCursor = GetOpposingSideBattler(battler);

        if (gBattleResources->bufferA[battler][1]) // a double battle
        {
            if (!CanSelectBattler(moveTarget))
                canSelectTarget = 1; // either selected or user
            if (moveTarget == TARGET_USER_OR_ALLY && IsBattlerAlive(BATTLE_PARTNER(battler)))
                canSelectTarget = 1;

            if (moveInfo->currentPp[gMoveSelectionCursor[battler]] == 0)
            {
                canSelectTarget = 0;
            }
            else if (isUserOrAlly && CountAliveMonsInBattle(BATTLE_ALIVE_EXCEPT_BATTLER, battler) <= 1)
            {
                gMultiUsePlayerCursor = GetDefaultMoveTarget(battler);
                canSelectTarget = 0;
            }

            if (B_SHOW_TARGETS == TRUE)
            {
                // Show all available targets for multi-target moves
                if (moveTarget == TARGET_ALL_BATTLERS || moveTarget == TARGET_FIELD)
                {
                    for (enum BattlerId i = 0; i < gBattlersCount; i++)
                        TryShowAsTarget(i);

                    canSelectTarget = 3;
                }
                else if (IsSpreadMove(moveTarget) || moveTarget == TARGET_OPPONENTS_FIELD || moveTarget == TARGET_USER_AND_ALLY)
                {
                    TryShowAsTarget(gMultiUsePlayerCursor);
                    TryShowAsTarget(BATTLE_PARTNER(gMultiUsePlayerCursor));
                    if (moveTarget == TARGET_FOES_AND_ALLY)
                        TryShowAsTarget(BATTLE_PARTNER(battler));
                    canSelectTarget = 2;
                }
            }
        }

        switch (canSelectTarget)
        {
        case 0:
        default:
            if (gBattleStruct->gimmick.playerSelect)
                BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, gMoveSelectionCursor[battler] | RET_GIMMICK | (gMultiUsePlayerCursor << 8));
            else
                BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, gMoveSelectionCursor[battler] | (gMultiUsePlayerCursor << 8));
            HideGimmickTriggerSprite();
            TryHideLastUsedBall();
            // TryToHideMovePreviewWindow(); // FTP
            BtlController_Complete(battler);
            break;
        case 1:
            gBattlerControllerFuncs[battler] = HandleInputChooseTarget;

            if (moveTarget == TARGET_USER || moveTarget == TARGET_USER_OR_ALLY)
                gMultiUsePlayerCursor = battler;
            else if (gAbsentBattlerFlags & (1u << GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)))
                gMultiUsePlayerCursor = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
            else
                gMultiUsePlayerCursor = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            if (B_SHOW_EFFECTIVENESS)
                MoveSelectionDisplayMoveEffectiveness(CheckTypeEffectiveness(battler, gMultiUsePlayerCursor), battler);

            gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_ShowAsMoveTarget;
            break;
        case 2:
            gBattlerControllerFuncs[battler] = HandleInputShowTargets;
            break;
        case 3: // Entire field
            gBattlerControllerFuncs[battler] = HandleInputShowEntireFieldTargets;
            break;
        }
    }
    else if ((JOY_NEW(B_BUTTON) || gPlayerDpadHoldFrames > 59)  && !gBattleStruct->descriptionSubmenu)
    {
        PlaySE(SE_SELECT);
        gBattleStruct->gimmick.playerSelect = FALSE;
        if (gBattleStruct->zmove.viewing)
        {
            ReloadMoveNames(battler);
            ChangeGimmickTriggerSprite(gBattleStruct->gimmick.triggerSpriteId, gBattleStruct->gimmick.playerSelect);
        }
        else
        {
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, 0xFFFF);
            HideGimmickTriggerSprite();
            BtlController_Complete(battler);
            TryToHideMoveInfoWindow();
        }

        // Always exit opponent view cleanly
        if (gBattleStruct->viewingOpponentMoves)
        {
            enum BattlerId opponentLeft = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            enum BattlerId opponentRight = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
            EndBounceEffect(opponentLeft, BOUNCE_HEALTHBOX);
            EndBounceEffect(opponentLeft, BOUNCE_MON);
            EndBounceEffect(opponentRight, BOUNCE_HEALTHBOX);
            EndBounceEffect(opponentRight, BOUNCE_MON);
            DoBounceEffect(battler, BOUNCE_HEALTHBOX, battler, 1);
            DoBounceEffect(battler, BOUNCE_MON, battler, 1);

            gBattleStruct->viewingOpponentMoves = FALSE;
            ReloadMoveNames(battler);
            // gSprites[gBattlerSpriteIds[opponentLeft]].callback = SpriteCallbackDummy;
        }

        CreateInfoWindow(battler);  // ← your requirement
        return;
    }
    else if (JOY_NEW(DPAD_LEFT) 
        && !gBattleStruct->zmove.viewing)
        // && !gBattleStruct->viewingOpponentMoves)
    {
        if (gMoveSelectionCursor[battler] & 1)
        {
            MoveSelectionDestroyCursorAt(gMoveSelectionCursor[battler]);
            gMoveSelectionCursor[battler] ^= 1;
            PlaySE(SE_SELECT);
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
            if (B_SHOW_EFFECTIVENESS)
                MoveSelectionDisplayMoveEffectiveness(CheckTargetTypeEffectiveness(battler), battler);
            MoveSelectionDisplayPpNumber(battler);
            MoveSelectionDisplayMoveType(battler);
            TryMoveSelectionDisplayMoveDescription(battler);
            TryChangeZTrigger(battler, gMoveSelectionCursor[battler]);
        }
    }
    else if (JOY_NEW(DPAD_RIGHT) 
        && !gBattleStruct->zmove.viewing)
        // && !gBattleStruct->viewingOpponentMoves)
    {
        if (!(gMoveSelectionCursor[battler] & 1)
         && (gMoveSelectionCursor[battler] ^ 1) < gNumberOfMovesToChoose)
        {
            MoveSelectionDestroyCursorAt(gMoveSelectionCursor[battler]);
            gMoveSelectionCursor[battler] ^= 1;
            PlaySE(SE_SELECT);
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
            if (B_SHOW_EFFECTIVENESS)
                MoveSelectionDisplayMoveEffectiveness(CheckTargetTypeEffectiveness(battler), battler);
            MoveSelectionDisplayPpNumber(battler);
            MoveSelectionDisplayMoveType(battler);
            TryMoveSelectionDisplayMoveDescription(battler);
            TryChangeZTrigger(battler, gMoveSelectionCursor[battler]);
        }
    }
    else if (JOY_NEW(DPAD_UP) 
        && !gBattleStruct->zmove.viewing)
        // && !gBattleStruct->viewingOpponentMoves)
    {
        if (gMoveSelectionCursor[battler] & 2)
        {
            MoveSelectionDestroyCursorAt(gMoveSelectionCursor[battler]);
            gMoveSelectionCursor[battler] ^= 2;
            PlaySE(SE_SELECT);
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
            if (B_SHOW_EFFECTIVENESS)
                MoveSelectionDisplayMoveEffectiveness(CheckTargetTypeEffectiveness(battler), battler);
            MoveSelectionDisplayPpNumber(battler);
            MoveSelectionDisplayMoveType(battler);
            TryMoveSelectionDisplayMoveDescription(battler);
            TryChangeZTrigger(battler, gMoveSelectionCursor[battler]);
        }
    }
    else if (JOY_NEW(DPAD_DOWN) 
        && !gBattleStruct->zmove.viewing)
        // && !gBattleStruct->viewingOpponentMoves)
    {
        if (!(gMoveSelectionCursor[battler] & 2)
         && (gMoveSelectionCursor[battler] ^ 2) < gNumberOfMovesToChoose)
        {
            MoveSelectionDestroyCursorAt(gMoveSelectionCursor[battler]);
            gMoveSelectionCursor[battler] ^= 2;
            PlaySE(SE_SELECT);
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
            if (B_SHOW_EFFECTIVENESS)
                MoveSelectionDisplayMoveEffectiveness(CheckTargetTypeEffectiveness(battler), battler);
            MoveSelectionDisplayPpNumber(battler);
            MoveSelectionDisplayMoveType(battler);
            TryMoveSelectionDisplayMoveDescription(battler);
            TryChangeZTrigger(battler, gMoveSelectionCursor[battler]);
        }
    }
    else if (B_MOVE_REARRANGEMENT_IN_BATTLE < GEN_4 
            && JOY_NEW(SELECT_BUTTON) 
            && !gBattleStruct->zmove.viewing 
            && !gBattleStruct->descriptionSubmenu
            && !gBattleStruct->viewingOpponentMoves)
    {
        if (gNumberOfMovesToChoose > 1 && !(gBattleTypeFlags & BATTLE_TYPE_LINK))
        {
            MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 29);

            if (gMoveSelectionCursor[battler] != 0)
                gMultiUsePlayerCursor = 0;
            else
                gMultiUsePlayerCursor = gMoveSelectionCursor[battler] + 1;

            MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 27);
            BattlePutTextOnWindow(gText_BattleSwitchWhich, B_WIN_SWITCH_PROMPT);
            gBattlerControllerFuncs[battler] = HandleMoveSwitching;
        }
    }
    else if (gBattleStruct->descriptionSubmenu)
    {
        if (JOY_NEW(B_MOVE_DESCRIPTION_BUTTON) || JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        {
            gBattleStruct->descriptionSubmenu = FALSE;
            if (gCategoryIconSpriteId != 0xFF)
            {
                DestroySprite(&gSprites[gCategoryIconSpriteId]);
                gCategoryIconSpriteId = 0xFF;
            }

            FillWindowPixelBuffer(B_WIN_MOVE_DESCRIPTION, PIXEL_FILL(0));
            ClearStdWindowAndFrame(B_WIN_MOVE_DESCRIPTION, FALSE);
            CopyWindowToVram(B_WIN_MOVE_DESCRIPTION, COPYWIN_GFX);
            PlaySE(SE_SELECT);
            if (B_SHOW_EFFECTIVENESS)
                MoveSelectionDisplayMoveEffectiveness(CheckTargetTypeEffectiveness(battler), battler);
            MoveSelectionDisplayPpNumber(battler);
            MoveSelectionDisplayMoveType(battler);
        }
    }
    else if (JOY_NEW(B_MOVE_DESCRIPTION_BUTTON) &&
        !(B_MOVE_DESCRIPTION_BUTTON == L_BUTTON && gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A))
    {
        gBattleStruct->descriptionSubmenu = TRUE;
        TryMoveSelectionDisplayMoveDescription(battler);
    }
    else if (JOY_NEW(START_BUTTON))
    {
        if (gBattleStruct->gimmick.usableGimmick[battler] != GIMMICK_NONE
            && !HasTrainerUsedGimmick(battler, gBattleStruct->gimmick.usableGimmick[battler])
            && !(gBattleStruct->gimmick.usableGimmick[battler] == GIMMICK_Z_MOVE
                 && GetUsableZMove(battler, moveInfo->moves[gMoveSelectionCursor[battler]]) == MOVE_NONE))
        {
            gBattleStruct->gimmick.playerSelect ^= 1;
            ReloadMoveNames(battler);
            ChangeGimmickTriggerSprite(gBattleStruct->gimmick.triggerSpriteId, gBattleStruct->gimmick.playerSelect);
            PlaySE(SE_SELECT);
        }
    }
}

static void ReloadMoveNames(enum BattlerId battler)
{
    if (gBattleStruct->zmove.viable && !gBattleStruct->zmove.viewing)
    {
        struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
        MoveSelectionDisplayZMove(GetUsableZMove(battler, moveInfo->moves[gMoveSelectionCursor[battler]]), battler);
    }
    else
    {
        gBattleStruct->zmove.viewing = FALSE;
        MoveSelectionDestroyCursorAt(battler);
        MoveSelectionDisplayMoveNames(battler);
        MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
        if (B_SHOW_EFFECTIVENESS)
            MoveSelectionDisplayMoveEffectiveness(CheckTargetTypeEffectiveness(battler), battler);
        MoveSelectionDisplayPpNumber(battler);
        MoveSelectionDisplayMoveType(battler);
    }
}

static u32 UNUSED HandleMoveInputUnused(enum BattlerId battler)
{
    u32 var = 0;

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        var = 1;
    }
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gBattle_BG0_X = 0;
        gBattle_BG0_Y = DISPLAY_HEIGHT * 2;
        var = 0xFF;
    }
    if (JOY_NEW(DPAD_LEFT) && gMoveSelectionCursor[battler] & 1)
    {
        MoveSelectionDestroyCursorAt(gMoveSelectionCursor[battler]);
        gMoveSelectionCursor[battler] ^= 1;
        PlaySE(SE_SELECT);
        MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
    }
    if (JOY_NEW(DPAD_RIGHT) && !(gMoveSelectionCursor[battler] & 1)
        && (gMoveSelectionCursor[battler] ^ 1) < gNumberOfMovesToChoose)
    {
        MoveSelectionDestroyCursorAt(gMoveSelectionCursor[battler]);
        gMoveSelectionCursor[battler] ^= 1;
        PlaySE(SE_SELECT);
        MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
    }
    if (JOY_NEW(DPAD_UP) && gMoveSelectionCursor[battler] & 2)
    {
        MoveSelectionDestroyCursorAt(gMoveSelectionCursor[battler]);
        gMoveSelectionCursor[battler] ^= 2;
        PlaySE(SE_SELECT);
        MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
    }
    if (JOY_NEW(DPAD_DOWN) && !(gMoveSelectionCursor[battler] & 2)
        && (gMoveSelectionCursor[battler] ^ 2) < gNumberOfMovesToChoose)
    {
        MoveSelectionDestroyCursorAt(gMoveSelectionCursor[battler]);
        gMoveSelectionCursor[battler] ^= 2;
        PlaySE(SE_SELECT);
        MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
    }

    return var;
}

void HandleMoveSwitching(enum BattlerId battler)
{
    u8 perMovePPBonuses[MAX_MON_MOVES];
    struct ChooseMoveStruct moveStruct;
    u8 totalPPBonuses;

    if (JOY_NEW(A_BUTTON | SELECT_BUTTON))
    {
        struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
        PlaySE(SE_SELECT);

        if (gMoveSelectionCursor[battler] != gMultiUsePlayerCursor)
        {
            struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
            s32 i;

            // swap moves and pp
            i = moveInfo->moves[gMoveSelectionCursor[battler]];
            moveInfo->moves[gMoveSelectionCursor[battler]] = moveInfo->moves[gMultiUsePlayerCursor];
            moveInfo->moves[gMultiUsePlayerCursor] = i;

            i = moveInfo->currentPp[gMoveSelectionCursor[battler]];
            moveInfo->currentPp[gMoveSelectionCursor[battler]] = moveInfo->currentPp[gMultiUsePlayerCursor];
            moveInfo->currentPp[gMultiUsePlayerCursor] = i;

            i = moveInfo->maxPp[gMoveSelectionCursor[battler]];
            moveInfo->maxPp[gMoveSelectionCursor[battler]] = moveInfo->maxPp[gMultiUsePlayerCursor];
            moveInfo->maxPp[gMultiUsePlayerCursor] = i;

            if (gBattleMons[battler].volatiles.mimickedMoves & (1u << gMoveSelectionCursor[battler]))
            {
                gBattleMons[battler].volatiles.mimickedMoves &= ~(1u << gMoveSelectionCursor[battler]);
                gBattleMons[battler].volatiles.mimickedMoves |= 1u << gMultiUsePlayerCursor;
            }

            MoveSelectionDisplayMoveNames(battler);

            for (i = 0; i < MAX_MON_MOVES; i++)
                perMovePPBonuses[i] = (gBattleMons[battler].ppBonuses & (3 << (i * 2))) >> (i * 2);

            totalPPBonuses = perMovePPBonuses[gMoveSelectionCursor[battler]];
            perMovePPBonuses[gMoveSelectionCursor[battler]] = perMovePPBonuses[gMultiUsePlayerCursor];
            perMovePPBonuses[gMultiUsePlayerCursor] = totalPPBonuses;

            totalPPBonuses = 0;
            for (i = 0; i < MAX_MON_MOVES; i++)
                totalPPBonuses |= perMovePPBonuses[i] << (i * 2);

            gBattleMons[battler].ppBonuses = totalPPBonuses;

            for (i = 0; i < MAX_MON_MOVES; i++)
            {
                gBattleMons[battler].moves[i] = moveInfo->moves[i];
                gBattleMons[battler].pp[i] = moveInfo->currentPp[i];
            }

            if (!(gBattleMons[battler].volatiles.transformed))
            {
                for (i = 0; i < MAX_MON_MOVES; i++)
                {
                    moveStruct.moves[i] = GetMonData(GetBattlerMon(battler), MON_DATA_MOVE1 + i);
                    moveStruct.currentPp[i] = GetMonData(GetBattlerMon(battler), MON_DATA_PP1 + i);
                }

                totalPPBonuses = GetMonData(GetBattlerMon(battler), MON_DATA_PP_BONUSES);
                for (i = 0; i < MAX_MON_MOVES; i++)
                    perMovePPBonuses[i] = (totalPPBonuses & (3 << (i * 2))) >> (i * 2);

                i = moveStruct.moves[gMoveSelectionCursor[battler]];
                moveStruct.moves[gMoveSelectionCursor[battler]] = moveStruct.moves[gMultiUsePlayerCursor];
                moveStruct.moves[gMultiUsePlayerCursor] = i;

                i = moveStruct.currentPp[gMoveSelectionCursor[battler]];
                moveStruct.currentPp[gMoveSelectionCursor[battler]] = moveStruct.currentPp[gMultiUsePlayerCursor];
                moveStruct.currentPp[gMultiUsePlayerCursor] = i;

                totalPPBonuses = perMovePPBonuses[gMoveSelectionCursor[battler]];
                perMovePPBonuses[gMoveSelectionCursor[battler]] = perMovePPBonuses[gMultiUsePlayerCursor];
                perMovePPBonuses[gMultiUsePlayerCursor] = totalPPBonuses;

                totalPPBonuses = 0;
                for (i = 0; i < MAX_MON_MOVES; i++)
                    totalPPBonuses |= perMovePPBonuses[i] << (i * 2);

                for (i = 0; i < MAX_MON_MOVES; i++)
                {
                    SetMonData(GetBattlerMon(battler), MON_DATA_MOVE1 + i, &moveStruct.moves[i]);
                    SetMonData(GetBattlerMon(battler), MON_DATA_PP1 + i, &moveStruct.currentPp[i]);
                }

                SetMonData(GetBattlerMon(battler), MON_DATA_PP_BONUSES, &totalPPBonuses);
            }
        }

        if (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE)
            gBattlerControllerFuncs[battler] = OakOldManHandleInputChooseMove;
        else
            gBattlerControllerFuncs[battler] = HandleInputChooseMove;
        gMoveSelectionCursor[battler] = gMultiUsePlayerCursor;
        MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
        if (B_SHOW_EFFECTIVENESS)
            MoveSelectionDisplayMoveEffectiveness(CheckTargetTypeEffectiveness(battler), battler);
        else
            MoveSelectionDisplayPpString(battler);
        MoveSelectionDisplayPpNumber(battler);
        MoveSelectionDisplayMoveType(battler);
        AssignUsableZMoves(battler, moveInfo->moves);
    }
    else if (JOY_NEW(B_BUTTON | SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        MoveSelectionDestroyCursorAt(gMultiUsePlayerCursor);
        MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);

        if (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE)
            gBattlerControllerFuncs[battler] = OakOldManHandleInputChooseMove;
        else
            gBattlerControllerFuncs[battler] = HandleInputChooseMove;

        if (B_SHOW_EFFECTIVENESS)
            MoveSelectionDisplayMoveEffectiveness(CheckTargetTypeEffectiveness(battler), battler);
        else
            MoveSelectionDisplayPpString(battler);
        MoveSelectionDisplayPpNumber(battler);
        MoveSelectionDisplayMoveType(battler);
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        if (gMultiUsePlayerCursor & 1)
        {
            if (gMultiUsePlayerCursor == gMoveSelectionCursor[battler])
                MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 29);
            else
                MoveSelectionDestroyCursorAt(gMultiUsePlayerCursor);

            gMultiUsePlayerCursor ^= 1;
            PlaySE(SE_SELECT);

            if (gMultiUsePlayerCursor == gMoveSelectionCursor[battler])
                MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0);
            else
                MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 27);
        }
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        if (!(gMultiUsePlayerCursor & 1) && (gMultiUsePlayerCursor ^ 1) < gNumberOfMovesToChoose)
        {
            if (gMultiUsePlayerCursor == gMoveSelectionCursor[battler])
                MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 29);
            else
                MoveSelectionDestroyCursorAt(gMultiUsePlayerCursor);

            gMultiUsePlayerCursor ^= 1;
            PlaySE(SE_SELECT);

            if (gMultiUsePlayerCursor == gMoveSelectionCursor[battler])
                MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0);
            else
                MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 27);
        }
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if (gMultiUsePlayerCursor & 2)
        {
            if (gMultiUsePlayerCursor == gMoveSelectionCursor[battler])
                MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 29);
            else
                MoveSelectionDestroyCursorAt(gMultiUsePlayerCursor);

            gMultiUsePlayerCursor ^= 2;
            PlaySE(SE_SELECT);

            if (gMultiUsePlayerCursor == gMoveSelectionCursor[battler])
                MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0);
            else
                MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 27);
        }
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (!(gMultiUsePlayerCursor & 2) && (gMultiUsePlayerCursor ^ 2) < gNumberOfMovesToChoose)
        {
            if (gMultiUsePlayerCursor == gMoveSelectionCursor[battler])
                MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 29);
            else
                MoveSelectionDestroyCursorAt(gMultiUsePlayerCursor);

            gMultiUsePlayerCursor ^= 2;
            PlaySE(SE_SELECT);

            if (gMultiUsePlayerCursor == gMoveSelectionCursor[battler])
                MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 0);
            else
                MoveSelectionCreateCursorAt(gMultiUsePlayerCursor, 27);
        }
    }
}

static void SetLinkBattleEndCallbacks(enum BattlerId battler)
{
    if (gWirelessCommType == 0)
    {
        if (gReceivedRemoteLinkPlayers == 0)
        {
            m4aSongNumStop(SE_LOW_HEALTH);
            gMain.inBattle = FALSE;
            gMain.callback1 = gPreBattleCallback1;
            SetMainCallback2(CB2_InitEndLinkBattle);
            if (gBattleOutcome == B_OUTCOME_WON)
                TryPutLinkBattleTvShowOnAir();
            FreeAllWindowBuffers();
        }
    }
    else
    {
        if (IsLinkTaskFinished())
        {
            m4aSongNumStop(SE_LOW_HEALTH);
            gMain.inBattle = FALSE;
            gMain.callback1 = gPreBattleCallback1;
            SetMainCallback2(CB2_InitEndLinkBattle);
            if (gBattleOutcome == B_OUTCOME_WON)
                TryPutLinkBattleTvShowOnAir();
            FreeAllWindowBuffers();
        }
    }
}

// Despite handling link battles separately, this is only ever used by link battles
void SetBattleEndCallbacks(enum BattlerId battler)
{
    if (!gPaletteFade.active)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            if (IsLinkTaskFinished())
            {
                if (gWirelessCommType == 0)
                    SetCloseLinkCallback();
                else
                    SetLinkStandbyCallback();

                gBattlerControllerFuncs[battler] = SetLinkBattleEndCallbacks;
            }
        }
        else
        {
            m4aSongNumStop(SE_LOW_HEALTH);
            gMain.inBattle = FALSE;
            gMain.callback1 = gPreBattleCallback1;
            SetMainCallback2(gMain.savedCallback);
        }
    }
}

static void Intro_WaitForShinyAnimAndHealthbox(enum BattlerId battler)
{
    bool8 healthboxAnimDone = FALSE;

    // Check if healthbox has finished sliding in
    if (TwoPlayerIntroMons(battler) && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
    {
        if (gSprites[gHealthboxSpriteIds[battler]].callback == SpriteCallbackDummy
         && gSprites[gHealthboxSpriteIds[BATTLE_PARTNER(battler)]].callback == SpriteCallbackDummy)
            healthboxAnimDone = TRUE;
    }
    else
    {
        if (gSprites[gHealthboxSpriteIds[battler]].callback == SpriteCallbackDummy)
            healthboxAnimDone = TRUE;
    }

    // If healthbox and shiny anim are done
    if (healthboxAnimDone && gBattleSpritesDataPtr->healthBoxesData[battler].finishedShinyMonAnim
        && gBattleSpritesDataPtr->healthBoxesData[BATTLE_PARTNER(battler)].finishedShinyMonAnim)
    {
        // Reset shiny anim (even if it didn't occur)
        gBattleSpritesDataPtr->healthBoxesData[battler].triedShinyMonAnim = FALSE;
        gBattleSpritesDataPtr->healthBoxesData[battler].finishedShinyMonAnim = FALSE;
        gBattleSpritesDataPtr->healthBoxesData[BATTLE_PARTNER(battler)].triedShinyMonAnim = FALSE;
        gBattleSpritesDataPtr->healthBoxesData[BATTLE_PARTNER(battler)].finishedShinyMonAnim = FALSE;
        FreeShinyStars();

        HandleLowHpMusicChange(GetBattlerMon(battler), battler);

        if (TwoPlayerIntroMons(battler))
            HandleLowHpMusicChange(GetBattlerMon(BATTLE_PARTNER(battler)), BATTLE_PARTNER(battler));

        gBattleSpritesDataPtr->healthBoxesData[battler].introEndDelay = 3;
        gBattlerControllerFuncs[battler] = BtlController_Intro_DelayAndEnd;
    }
}

static void Intro_TryShinyAnimShowHealthbox(enum BattlerId battler)
{
    bool32 bgmRestored = FALSE;
    bool32 battlerAnimsDone = FALSE;

    // Start shiny animation if applicable for 1st Pokémon
    if (!gBattleSpritesDataPtr->healthBoxesData[battler].triedShinyMonAnim
     && !gBattleSpritesDataPtr->healthBoxesData[battler].ballAnimActive)
        TryShinyAnimation(battler, GetBattlerMon(battler));

    // Start shiny animation if applicable for 2nd Pokémon
    if (!gBattleSpritesDataPtr->healthBoxesData[BATTLE_PARTNER(battler)].triedShinyMonAnim
     && !gBattleSpritesDataPtr->healthBoxesData[BATTLE_PARTNER(battler)].ballAnimActive)
        TryShinyAnimation(BATTLE_PARTNER(battler), GetBattlerMon(BATTLE_PARTNER(battler)));

    // Show healthbox after ball anim
    if (!gBattleSpritesDataPtr->healthBoxesData[battler].ballAnimActive
     && !gBattleSpritesDataPtr->healthBoxesData[BATTLE_PARTNER(battler)].ballAnimActive)
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[battler].healthboxSlideInStarted)
        {
            if (TwoPlayerIntroMons(battler) && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
            {
                UpdateHealthboxAttribute(gHealthboxSpriteIds[BATTLE_PARTNER(battler)], GetBattlerMon(BATTLE_PARTNER(battler)), HEALTHBOX_ALL);
                StartHealthboxSlideIn(BATTLE_PARTNER(battler));
                SetHealthboxSpriteVisible(gHealthboxSpriteIds[BATTLE_PARTNER(battler)]);
            }
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], GetBattlerMon(battler), HEALTHBOX_ALL);
            StartHealthboxSlideIn(battler);
            SetHealthboxSpriteVisible(gHealthboxSpriteIds[battler]);
        }
        gBattleSpritesDataPtr->healthBoxesData[battler].healthboxSlideInStarted = TRUE;
    }

    // Restore bgm after cry has played and healthbox anim is started
    if (!gBattleSpritesDataPtr->healthBoxesData[battler].waitForCry
        && gBattleSpritesDataPtr->healthBoxesData[battler].healthboxSlideInStarted
        && !gBattleSpritesDataPtr->healthBoxesData[BATTLE_PARTNER(battler)].waitForCry
        && !IsCryPlayingOrClearCrySongs())
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[battler].bgmRestored)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI && gBattleTypeFlags & BATTLE_TYPE_LINK)
                m4aMPlayContinue(&gMPlayInfo_BGM);
            else
                m4aMPlayVolumeControl(&gMPlayInfo_BGM, TRACKS_ALL, 0x100);
        }
        gBattleSpritesDataPtr->healthBoxesData[battler].bgmRestored = TRUE;
        bgmRestored = TRUE;
    }

    // Wait for battler anims
    if (TwoPlayerIntroMons(battler) && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
    {
        if (gSprites[gBattleControllerData[battler]].callback == SpriteCallbackDummy
            && gSprites[gBattlerSpriteIds[battler]].callback == SpriteCallbackDummy
            && gSprites[gBattleControllerData[BATTLE_PARTNER(battler)]].callback == SpriteCallbackDummy
            && gSprites[gBattlerSpriteIds[BATTLE_PARTNER(battler)]].callback == SpriteCallbackDummy)
        {
            battlerAnimsDone = TRUE;
        }
    }
    else
    {
        if (gSprites[gBattleControllerData[battler]].callback == SpriteCallbackDummy
            && gSprites[gBattlerSpriteIds[battler]].callback == SpriteCallbackDummy)
        {
            battlerAnimsDone = TRUE;
        }
    }

    // Clean up
    if (bgmRestored && battlerAnimsDone)
    {
        if (TwoPlayerIntroMons(battler) && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
            DestroySprite(&gSprites[gBattleControllerData[BATTLE_PARTNER(battler)]]);
        DestroySprite(&gSprites[gBattleControllerData[battler]]);

        gBattleSpritesDataPtr->animationData->introAnimActive = FALSE;
        gBattleSpritesDataPtr->healthBoxesData[battler].bgmRestored = FALSE;
        gBattleSpritesDataPtr->healthBoxesData[battler].healthboxSlideInStarted = FALSE;

        gBattlerControllerFuncs[battler] = Intro_WaitForShinyAnimAndHealthbox;
    }
}

void Task_PlayerController_RestoreBgmAfterCry(u8 taskId)
{
    if (!IsCryPlayingOrClearCrySongs())
    {
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, TRACKS_ALL, 0x100);
        DestroyTask(taskId);
    }
}

#define tExpTask_monId          data[0]
#define tExpTask_battler        data[2]
#define tExpTask_gainedExp_1    data[3]
#define tExpTask_gainedExp_2    data[4] // Stored as two half-words containing a word.
#define tExpTask_frames         data[10]

static void DynamaxModifyHPLevelUp(struct Pokemon *mon, enum BattlerId battler, u32 oldMaxHP)
{
    ApplyDynamaxHPMultiplier(mon);
    gBattleScripting.levelUpHP = GetMonData(mon, MON_DATA_MAX_HP) - oldMaxHP; // overwrite levelUpHP since it overflows
    gBattleMons[battler].hp += gBattleScripting.levelUpHP;
    SetMonData(mon, MON_DATA_HP, &gBattleMons[battler].hp);
}

static s32 GetTaskExpValue(u8 taskId)
{
    return (u16)(gTasks[taskId].tExpTask_gainedExp_1) | (gTasks[taskId].tExpTask_gainedExp_2 << 16);
}

static void Task_GiveExpToMon(u8 taskId)
{
    u32 monId = (u8)(gTasks[taskId].tExpTask_monId);
    enum BattlerId battler = gTasks[taskId].tExpTask_battler;
    s32 gainedExp = GetTaskExpValue(taskId);

    if (GetBattlerCoordsIndex(battler) == BATTLE_COORDS_DOUBLES || monId != gBattlerPartyIndexes[battler]) // Give exp without moving the expbar.
    {
        struct Pokemon *mon = &gPlayerParty[monId];
        u16 species = GetMonData(mon, MON_DATA_SPECIES);
        u8 level = GetMonData(mon, MON_DATA_LEVEL);
        u32 currExp = GetMonData(mon, MON_DATA_EXP);
        u32 nextLvlExp = gExperienceTables[gSpeciesInfo[species].growthRate][level + 1];
        u32 expAfterGain = currExp + gainedExp;
        u32 oldMaxHP = GetMonData(mon, MON_DATA_MAX_HP);

        if (expAfterGain >= nextLvlExp)
        {
            SetMonData(mon, MON_DATA_EXP, (B_LEVEL_UP_NOTIFICATION >= GEN_9) ? &expAfterGain : &nextLvlExp);

            CalculateMonStats(mon);

            // Reapply Dynamax HP multiplier after stats are recalculated.
            if (GetActiveGimmick(battler) == GIMMICK_DYNAMAX && monId == gBattlerPartyIndexes[battler])
                DynamaxModifyHPLevelUp(mon, battler, oldMaxHP);

            gainedExp -= nextLvlExp - currExp;
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, RET_VALUE_LEVELED_UP, (B_LEVEL_UP_NOTIFICATION >= GEN_9) ? 0 : gainedExp);

            if (IsDoubleBattle() == TRUE
             && (monId == gBattlerPartyIndexes[battler] || monId == gBattlerPartyIndexes[BATTLE_PARTNER(battler)]))
                gTasks[taskId].func = Task_LaunchLvlUpAnim;
            else
                gTasks[taskId].func = Task_SetControllerToWaitForString;
        }
        else
        {
            currExp += gainedExp;
            SetMonData(mon, MON_DATA_EXP, &currExp);
            gBattlerControllerFuncs[battler] = Controller_WaitForString;
            DestroyTask(taskId);
        }
    }
    else
    {
        gTasks[taskId].func = Task_PrepareToGiveExpWithExpBar;
    }
}

static void Task_PrepareToGiveExpWithExpBar(u8 taskId)
{
    u8 monIndex = gTasks[taskId].tExpTask_monId;
    s32 gainedExp = GetTaskExpValue(taskId);
    enum BattlerId battler = gTasks[taskId].tExpTask_battler;
    struct Pokemon *mon = &gPlayerParty[monIndex];
    u8 level = GetMonData(mon, MON_DATA_LEVEL);
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    u32 exp = GetMonData(mon, MON_DATA_EXP);
    u32 currLvlExp = gExperienceTables[gSpeciesInfo[species].growthRate][level];
    u32 expToNextLvl;

    exp -= currLvlExp;
    expToNextLvl = gExperienceTables[gSpeciesInfo[species].growthRate][level + 1] - currLvlExp;
    SetBattleBarStruct(battler, gHealthboxSpriteIds[battler], expToNextLvl, exp, -gainedExp);
    TestRunner_Battle_RecordExp(battler, exp, -gainedExp);
    PlaySE(SE_EXP);
    gTasks[taskId].func = Task_GiveExpWithExpBar;
}

static void Task_GiveExpWithExpBar(u8 taskId)
{
    u32 level, expAfterGain;
    u16 species;
    u32 oldMaxHP;
    s32 currExp, expOnNextLvl, newExpPoints;

    if (gTasks[taskId].tExpTask_frames < 13)
    {
        gTasks[taskId].tExpTask_frames++;
    }
    else
    {
        u8 monId = gTasks[taskId].tExpTask_monId;
        s32 gainedExp = GetTaskExpValue(taskId);
        enum BattlerId battler = gTasks[taskId].tExpTask_battler;
        struct Pokemon *mon = &gPlayerParty[monId];

        newExpPoints = MoveBattleBar(battler, gHealthboxSpriteIds[battler], EXP_BAR, 0);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[battler]);
        if (newExpPoints == -1) // The bar has been filled with given exp points.
        {
            m4aSongNumStop(SE_EXP);
            level = GetMonData(mon, MON_DATA_LEVEL);
            currExp = GetMonData(mon, MON_DATA_EXP);
            species = GetMonData(mon, MON_DATA_SPECIES);
            oldMaxHP = GetMonData(mon, MON_DATA_MAX_HP);
            expOnNextLvl = gExperienceTables[gSpeciesInfo[species].growthRate][level + 1];

            expAfterGain = currExp + gainedExp;
            if (expAfterGain >= expOnNextLvl)
            {
                if (B_LEVEL_UP_NOTIFICATION >= GEN_9)
                    SetMonData(mon, MON_DATA_EXP, &expAfterGain);
                else
                    SetMonData(mon, MON_DATA_EXP, &expOnNextLvl);

                CalculateMonStats(mon);

                // Reapply Dynamax HP multiplier after stats are recalculated.
                if (GetActiveGimmick(battler) == GIMMICK_DYNAMAX && monId == gBattlerPartyIndexes[battler])
                    DynamaxModifyHPLevelUp(mon, battler, oldMaxHP);

                gainedExp -= expOnNextLvl - currExp;
                BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, RET_VALUE_LEVELED_UP, (B_LEVEL_UP_NOTIFICATION >= GEN_9) ? 0 : gainedExp);
                gTasks[taskId].func = Task_LaunchLvlUpAnim;
            }
            else
            {
                currExp += gainedExp;
                SetMonData(mon, MON_DATA_EXP, &currExp);
                gBattlerControllerFuncs[battler] = Controller_WaitForString;
                DestroyTask(taskId);
            }
        }
    }
}

static void Task_LaunchLvlUpAnim(u8 taskId)
{
    enum BattlerId battler = gTasks[taskId].tExpTask_battler;
    u8 monIndex = gTasks[taskId].tExpTask_monId;

    if (IsDoubleBattle() == TRUE && monIndex == gBattlerPartyIndexes[BATTLE_PARTNER(battler)])
        battler ^= BIT_FLANK;

    InitAndLaunchSpecialAnimation(battler, battler, battler, B_ANIM_LVL_UP);
    gTasks[taskId].func = Task_UpdateLvlInHealthbox;
}

static void Task_UpdateLvlInHealthbox(u8 taskId)
{
    enum BattlerId battler = gTasks[taskId].tExpTask_battler;

    if (!gBattleSpritesDataPtr->healthBoxesData[battler].specialAnimActive)
    {
        u8 monIndex = gTasks[taskId].tExpTask_monId;

        if (IsDoubleBattle() == TRUE && monIndex == gBattlerPartyIndexes[BATTLE_PARTNER(battler)])
            UpdateHealthboxAttribute(gHealthboxSpriteIds[BATTLE_PARTNER(battler)], &gPlayerParty[monIndex], HEALTHBOX_ALL);
        else
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], &gPlayerParty[monIndex], HEALTHBOX_ALL);

        gTasks[taskId].func = Task_SetControllerToWaitForString;
    }
}

static void Task_SetControllerToWaitForString(u8 taskId)
{
    enum BattlerId battler = gTasks[taskId].tExpTask_battler;
    gBattlerControllerFuncs[battler] = Controller_WaitForString;
    DestroyTask(taskId);
}

static void OpenPartyMenuToChooseMon(enum BattlerId battler)
{
    if (!gPaletteFade.active)
    {
        u8 caseId;

        gBattlerControllerFuncs[battler] = WaitForMonSelection;
        caseId = gTasks[gBattleControllerData[battler]].data[0];
        DestroyTask(gBattleControllerData[battler]);
        FreeAllWindowBuffers();
        OpenPartyMenuInBattle(caseId);
    }
}

static void WaitForMonSelection(enum BattlerId battler)
{
    if (gMain.callback2 == BattleMainCB2 && !gPaletteFade.active)
    {
        if (gPartyMenuUseExitCallback == TRUE)
            BtlController_EmitChosenMonReturnValue(battler, B_COMM_TO_ENGINE, gSelectedMonPartyId, gBattlePartyCurrentOrder);
        else
            BtlController_EmitChosenMonReturnValue(battler, B_COMM_TO_ENGINE, PARTY_SIZE, NULL);

        if (gBattleResources->bufferA[battler][1] == PARTY_ACTION_SEND_OUT)
            PrintLinkStandbyMsg();

        BtlController_Complete(battler);
    }
}

static void OpenBagAndChooseItem(enum BattlerId battler)
{
    if (!gPaletteFade.active)
    {
        gBattlerControllerFuncs[battler] = CompleteWhenChoseItem;
        ReshowBattleScreenDummy();
        FreeAllWindowBuffers();
        CB2_BagMenuFromBattle();
    }
}

static void CompleteWhenChoseItem(enum BattlerId battler)
{
    if (gMain.callback2 == BattleMainCB2 && !gPaletteFade.active)
    {
        BtlController_EmitOneReturnValue(battler, B_COMM_TO_ENGINE, gSpecialVar_ItemId);
        BtlController_Complete(battler);
    }
}

static void PlayerHandleYesNoInput(enum BattlerId battler)
{
    if (JOY_NEW(DPAD_UP) && gMultiUsePlayerCursor != 0)
    {
        PlaySE(SE_SELECT);
        BattleDestroyYesNoCursorAt(gMultiUsePlayerCursor);
        gMultiUsePlayerCursor = 0;
        BattleCreateYesNoCursorAt(0);
    }
    if (JOY_NEW(DPAD_DOWN) && gMultiUsePlayerCursor == 0)
    {
        PlaySE(SE_SELECT);
        BattleDestroyYesNoCursorAt(gMultiUsePlayerCursor);
        gMultiUsePlayerCursor = 1;
        BattleCreateYesNoCursorAt(1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        HandleBattleWindow(YESNOBOX_X_Y, WINDOW_CLEAR);
        PlaySE(SE_SELECT);

        if (gMultiUsePlayerCursor != 0)
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_UNK_14, 0);
        else
            BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_NOTHING_FAINTED, 0);

        BtlController_Complete(battler);
    }
    if (JOY_NEW(B_BUTTON))
    {
        HandleBattleWindow(YESNOBOX_X_Y, WINDOW_CLEAR);
        PlaySE(SE_SELECT);
        BtlController_Complete(battler);
    }
}

static void MoveSelectionDisplayMoveNames(enum BattlerId battler)
{
    s32 i;
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
    gNumberOfMovesToChoose = 0;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        MoveSelectionDestroyCursorAt(i);
        if (IsGimmickSelected(battler, GIMMICK_DYNAMAX) || GetActiveGimmick(battler) == GIMMICK_DYNAMAX)
            StringCopy(gDisplayedStringBattle, GetMoveName(GetMaxMove(battler, moveInfo->moves[i])));
        else
            StringCopy(gDisplayedStringBattle, GetMoveName(moveInfo->moves[i]));
        // Prints on windows B_WIN_MOVE_NAME_1, B_WIN_MOVE_NAME_2, B_WIN_MOVE_NAME_3, B_WIN_MOVE_NAME_4
        BattlePutTextOnWindow(gDisplayedStringBattle, i + B_WIN_MOVE_NAME_1);
        if (moveInfo->moves[i] != MOVE_NONE)
            gNumberOfMovesToChoose++;
    }
}

static void MoveSelectionDisplayPpString(enum BattlerId battler)
{
    StringCopy(gDisplayedStringBattle, gText_MoveInterfacePP);
    BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_PP);
}

static void MoveSelectionDisplayPpNumber(enum BattlerId battler)
{
    u8 *txtPtr;
    struct ChooseMoveStruct *moveInfo;

    if (gBattleResources->bufferA[battler][2] == TRUE) // check if we didn't want to display pp number
        return;

    SetPpNumbersPaletteInMoveSelection(battler);
    moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
    txtPtr = ConvertIntToDecimalStringN(gDisplayedStringBattle, moveInfo->currentPp[gMoveSelectionCursor[battler]], STR_CONV_MODE_RIGHT_ALIGN, 2);
    *(txtPtr)++ = CHAR_SLASH;
    ConvertIntToDecimalStringN(txtPtr, moveInfo->maxPp[gMoveSelectionCursor[battler]], STR_CONV_MODE_RIGHT_ALIGN, 2);

    BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_PP_REMAINING);
}

static void MoveSelectionDisplayMoveType(enum BattlerId battler)
{
    u8 *txtPtr, *end;
    u32 speciesId = gBattleMons[battler].species;
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
    txtPtr = StringCopy(gDisplayedStringBattle, gText_MoveInterfaceType);
    enum Move move = moveInfo->moves[gMoveSelectionCursor[battler]];
    enum Type type = GetMoveType(move);
    enum BattleMoveEffects effect = GetMoveEffect(move);

    if (effect == EFFECT_TERA_BLAST)
    {
        if (IsGimmickSelected(battler, GIMMICK_TERA) || GetActiveGimmick(battler) == GIMMICK_TERA)
            type = GetBattlerTeraType(battler);
    }
    else if (effect == EFFECT_IVY_CUDGEL)
    {
        if (speciesId == SPECIES_OGERPON_WELLSPRING || speciesId == SPECIES_OGERPON_WELLSPRING_TERA
         || speciesId == SPECIES_OGERPON_HEARTHFLAME || speciesId == SPECIES_OGERPON_HEARTHFLAME_TERA
         || speciesId == SPECIES_OGERPON_CORNERSTONE || speciesId == SPECIES_OGERPON_CORNERSTONE_TERA)
            type = GetSpeciesType(speciesId, 1);
    }
    else if (GetMoveCategory(move) == DAMAGE_CATEGORY_STATUS
             && (GetActiveGimmick(battler) == GIMMICK_DYNAMAX || IsGimmickSelected(battler, GIMMICK_DYNAMAX)))
    {
        type = TYPE_NORMAL; // Max Guard is always a Normal-type move
    }
    else if (effect == EFFECT_TERA_STARSTORM)
    {
        if (speciesId == SPECIES_TERAPAGOS_STELLAR
        || (IsGimmickSelected(battler, GIMMICK_TERA) && speciesId == SPECIES_TERAPAGOS_TERASTAL))
            type = TYPE_STELLAR;
    }
    else if (P_SHOW_DYNAMIC_TYPES) // Non-vanilla changes to battle UI showing dynamic types
    {
        struct Pokemon *mon = GetBattlerMon(battler);
        type = CheckDynamicMoveType(mon, move, battler, MON_IN_BATTLE);
    }
    end = StringCopy(txtPtr, gTypesInfo[type].name);

    PrependFontIdToFit(txtPtr, end, FONT_NORMAL, WindowWidthPx(B_WIN_MOVE_TYPE) - 25);
    BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MOVE_TYPE);
}

static void TryMoveSelectionDisplayMoveDescription(enum BattlerId battler)
{
    if (!B_SHOW_MOVE_DESCRIPTION)
        return;

    if (gBattleStruct->descriptionSubmenu)
        MoveSelectionDisplayMoveDescription(battler);
}

static void MoveSelectionDisplayMoveDescription(enum BattlerId battler)
{
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct*)(&gBattleResources->bufferA[battler][4]);
    enum Move move = moveInfo->moves[gMoveSelectionCursor[battler]];
    u16 pwr = GetMovePower(move);
    u16 acc = GetMoveAccuracy(move);
    enum DamageCategory cat = GetBattleMoveCategory(move);

    if (GetActiveGimmick(battler) == GIMMICK_DYNAMAX || IsGimmickSelected(battler, GIMMICK_DYNAMAX))
    {
        pwr = GetMaxMovePower(move);
        move = GetMaxMove(battler, move);
        acc = 0;
    }

    u8 pwr_num[3], acc_num[3];
    u8 cat_desc[7] = _("CAT: ");
    u8 pwr_desc[7] = _("PWR: ");
    u8 acc_desc[7] = _("ACC: ");
    u8 cat_start[] = _("{CLEAR_TO 0x03}");
    u8 pwr_start[] = _("{CLEAR_TO 0x38}");
    u8 acc_start[] = _("{CLEAR_TO 0x6C}");
    LoadMessageBoxAndBorderGfx();
    DrawStdWindowFrame(B_WIN_MOVE_DESCRIPTION, FALSE);
    if (pwr < 2)
        StringCopy(pwr_num, gText_BattleSwitchWhich5);
    else
        ConvertIntToDecimalStringN(pwr_num, pwr, STR_CONV_MODE_LEFT_ALIGN, 3);
    if (acc < 2)
        StringCopy(acc_num, gText_BattleSwitchWhich5);
    else
        ConvertIntToDecimalStringN(acc_num, acc, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringCopy(gDisplayedStringBattle, cat_start);
    StringAppend(gDisplayedStringBattle, cat_desc);
    StringAppend(gDisplayedStringBattle, pwr_start);
    StringAppend(gDisplayedStringBattle, pwr_desc);
    StringAppend(gDisplayedStringBattle, pwr_num);
    StringAppend(gDisplayedStringBattle, acc_start);
    StringAppend(gDisplayedStringBattle, acc_desc);
    StringAppend(gDisplayedStringBattle, acc_num);
    StringAppend(gDisplayedStringBattle, gText_NewLine);
    StringAppend(gDisplayedStringBattle, GetMoveDescription(move));
    BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_MOVE_DESCRIPTION);

    if (gCategoryIconSpriteId == 0xFF)
        gCategoryIconSpriteId = CreateSprite(&gSpriteTemplate_CategoryIcons, 38, 64, 1);

    StartSpriteAnim(&gSprites[gCategoryIconSpriteId], cat);

    CopyWindowToVram(B_WIN_MOVE_DESCRIPTION, COPYWIN_FULL);
}

void MoveSelectionCreateCursorAt(u8 cursorPosition, u8 baseTileNum)
{
    u16 src[2];
    src[0] = baseTileNum + 1;
    src[1] = baseTileNum + 2;

    CopyToBgTilemapBufferRect_ChangePalette(0, src, 9 * (cursorPosition & 1) + 1, 55 + (cursorPosition & 2), 1, 2, 0x11);
    CopyBgTilemapBufferToVram(0);
}

void MoveSelectionDestroyCursorAt(u8 cursorPosition)
{
    u16 src[2];
    src[0] = 0x1016;
    src[1] = 0x1016;

    CopyToBgTilemapBufferRect_ChangePalette(0, src, 9 * (cursorPosition & 1) + 1, 55 + (cursorPosition & 2), 1, 2, 0x11);
    CopyBgTilemapBufferToVram(0);
}

void ActionSelectionCreateCursorAt(u8 cursorPosition, u8 baseTileNum)
{
    u16 src[2];
    src[0] = 1;
    src[1] = 2;

    CopyToBgTilemapBufferRect_ChangePalette(0, src, 7 * (cursorPosition & 1) + 16, 35 + (cursorPosition & 2), 1, 2, 0x11);
    CopyBgTilemapBufferToVram(0);
}

void ActionSelectionDestroyCursorAt(u8 cursorPosition)
{
    u16 src[2];
    src[0] = 0x1016;
    src[1] = 0x1016;

    CopyToBgTilemapBufferRect_ChangePalette(0, src, 7 * (cursorPosition & 1) + 16, 35 + (cursorPosition & 2), 1, 2, 0x11);
    CopyBgTilemapBufferToVram(0);
}

void CB2_SetUpReshowBattleScreenAfterMenu(void)
{
    SetMainCallback2(ReshowBattleScreenAfterMenu);
}

void CB2_SetUpReshowBattleScreenAfterMenu2(void)
{
    SetMainCallback2(ReshowBattleScreenAfterMenu);
}

static void PrintLinkStandbyMsg(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        gBattle_BG0_X = 0;
        gBattle_BG0_Y = 0;
        BattlePutTextOnWindow(gText_LinkStandby, B_WIN_MSG);
    }
}

static void PlayerHandleLoadMonSprite(enum BattlerId battler)
{
    BattleLoadMonSpriteGfx(GetBattlerMon(battler), battler);
    gSprites[gBattlerSpriteIds[battler]].oam.paletteNum = battler;
    gBattlerControllerFuncs[battler] = CompleteOnBattlerSpritePosX_0;
}

enum TrainerPicID LinkPlayerGetTrainerPicId(u32 multiplayerId)
{
    enum TrainerPicID trainerPicId;

    u8 gender = gLinkPlayers[multiplayerId].gender;
    enum GameVersion version = gLinkPlayers[multiplayerId].version & 0xFF;

    if (version == VERSION_FIRE_RED || version == VERSION_LEAF_GREEN)
        trainerPicId = gender + TRAINER_PIC_BACK_RED;
    else if (version == VERSION_RUBY || version == VERSION_SAPPHIRE)
        trainerPicId = gender + TRAINER_PIC_BACK_RUBY_SAPPHIRE_BRENDAN;
    else
        trainerPicId = gender + TRAINER_PIC_BACK_BRENDAN;

    return trainerPicId;
}

static enum TrainerPicID PlayerGetTrainerBackPicId(void)
{
    enum TrainerPicID trainerPicId;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        trainerPicId = LinkPlayerGetTrainerPicId(GetMultiplayerId());
    else
        trainerPicId = gSaveBlock2Ptr->playerGender == FEMALE ? TRAINER_BACK_PIC_PLAYER_FEMALE : TRAINER_BACK_PIC_PLAYER_MALE;

    return trainerPicId;
}

// In emerald it's possible to have a tag battle in the battle frontier facilities with AI
// which use the front sprite for both the player and the partner as opposed to any other battles (including the one with Steven)
// that use an animated back pic.
static void PlayerHandleDrawTrainerPic(enum BattlerId battler)
{
    bool32 isFrontPic;
    s16 xPos, yPos;
    enum TrainerPicID trainerPicId;

    if (IsMultibattleTest())
    {
        trainerPicId = TRAINER_PIC_BACK_BRENDAN;
        if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
            xPos = 32;
        else
            xPos = 80;
        yPos = (8 - gTrainerBacksprites[trainerPicId].coordinates.size) * 4 + 80;
    }
    else
    {
        trainerPicId = PlayerGetTrainerBackPicId();

        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        {
            if ((GetBattlerPosition(battler) & BIT_FLANK) != B_FLANK_LEFT) // Second mon, on the right.
                xPos = 90;
            else // First mon, on the left.
                xPos = 32;

            if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER && gPartnerTrainerId < TRAINER_PARTNER(PARTNER_NONE))
            {
                xPos = 90;
                yPos = 80;
            }
            else
            {
                yPos = (8 - gTrainerBacksprites[trainerPicId].coordinates.size) * 4 + 80;
            }
        }
        else
        {
            xPos = 80;
            yPos = (8 - gTrainerBacksprites[trainerPicId].coordinates.size) * 4 + 80;
        }
    }

    // Use front pic table for any tag battles unless your partner is Steven or a custom partner.
    if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER && gPartnerTrainerId < TRAINER_PARTNER(PARTNER_NONE))
    {
        trainerPicId = PlayerGenderToFrontTrainerPicId(gSaveBlock2Ptr->playerGender);
        isFrontPic = TRUE;
    }
    else // Use back pic in all the other usual circumstances.
    {
        isFrontPic = FALSE;
    }

    BtlController_HandleDrawTrainerPic(battler, trainerPicId, isFrontPic, xPos, yPos, -1);
}

static void PlayerHandleTrainerSlide(enum BattlerId battler)
{
    enum TrainerPicID trainerPicId = PlayerGetTrainerBackPicId();
    BtlController_HandleTrainerSlide(battler, trainerPicId);
}

static void PlayerHandleTrainerSlideBack(enum BattlerId battler)
{
    BtlController_HandleTrainerSlideBack(battler, 50, TRUE);
}

static void PlayerHandlePaletteFade(enum BattlerId battler)
{
    BeginNormalPaletteFade(PALETTES_ALL, 2, 0, 16, RGB_BLACK);
    BtlController_Complete(battler);
}

static void PlayerHandlePause(enum BattlerId battler)
{
    u8 timer = gBattleResources->bufferA[battler][1];

    while (timer != 0)
        timer--;

    BtlController_Complete(battler);
}

static void HandleChooseActionAfterDma3(enum BattlerId battler)
{
    if (!IsDma3ManagerBusyWithBgCopy())
    {
        gBattle_BG0_X = 0;
        gBattle_BG0_Y = DISPLAY_HEIGHT;
        if (gBattleStruct->aiDelayTimer != 0)
        {
            if (DEBUG_AI_DELAY_TIMER)
            {
                static const u8 sFramesText[] = _(" frames thinking\n");
                static const u8 sCyclesText[] = _(" cycles");
                ConvertIntToDecimalStringN(gDisplayedStringBattle, gBattleStruct->aiDelayFrames, STR_CONV_MODE_RIGHT_ALIGN, 3);
                u8* end = StringAppend(gDisplayedStringBattle, sFramesText);
                ConvertIntToDecimalStringN(end, gBattleStruct->aiDelayCycles, STR_CONV_MODE_RIGHT_ALIGN, 8);
                // Clear old result once read out
                gBattleStruct->aiDelayCycles = 0;
                StringAppend(gDisplayedStringBattle, sCyclesText);
                BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_ACTION_PROMPT);
            }
            gBattleStruct->aiDelayTimer = 0;
            gBattleStruct->aiDelayFrames = 0;
        }
        gBattlerControllerFuncs[battler] = HandleInputChooseAction;
    }
}

static void PlayerHandleChooseAction(enum BattlerId battler)
{
    s32 i;

    gBattlerControllerFuncs[battler] = HandleChooseActionAfterDma3;
    BattleTv_ClearExplosionFaintCause();
    BattlePutTextOnWindow(gText_BattleMenu, B_WIN_ACTION_MENU);

    for (i = 0; i < 4; i++)
        ActionSelectionDestroyCursorAt(i);

    TryRestoreLastUsedBall();
    // TryToAddMovePreviewWindow();
    ActionSelectionCreateCursorAt(gActionSelectionCursor[battler], 0);
    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, battler, gBattlerPartyIndexes[battler]);
    BattleStringExpandPlaceholdersToDisplayedString(gText_WhatWillPkmnDo);

    enum BattlerId partner = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
    if (B_SHOW_PARTNER_TARGET && gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER && IsBattlerAlive(partner))
    {
        StringCopy(gStringVar1, COMPOUND_STRING("Partner will use:\n"));
        enum Move move = GetBattlerChosenMove(partner);
        StringAppend(gStringVar1, GetMoveName(move));
        enum MoveTarget moveTarget = GetBattlerMoveTargetType(partner, move);
        if (moveTarget == TARGET_SELECTED || moveTarget == TARGET_SMART)
        {
            if (gAiBattleData->chosenTarget[partner] == B_POSITION_OPPONENT_LEFT)
                StringAppend(gStringVar1, COMPOUND_STRING(" -{UP_ARROW}"));
            else if (gAiBattleData->chosenTarget[partner] == B_POSITION_OPPONENT_RIGHT)
                StringAppend(gStringVar1, COMPOUND_STRING(" {UP_ARROW}-"));
            else if (gAiBattleData->chosenTarget[partner] == B_POSITION_PLAYER_LEFT)
                StringAppend(gStringVar1, COMPOUND_STRING(" {DOWN_ARROW}-"));
            else if (gAiBattleData->chosenTarget[partner] == B_POSITION_PLAYER_RIGHT)
                StringAppend(gStringVar1, COMPOUND_STRING(" -{DOWN_ARROW}"));
        }
        else if (moveTarget == TARGET_USER_AND_ALLY)
        {
            StringAppend(gStringVar1, COMPOUND_STRING(" {DOWN_ARROW}{DOWN_ARROW}"));
        }
        else if (moveTarget == TARGET_BOTH)
        {
            StringAppend(gStringVar1, COMPOUND_STRING(" {UP_ARROW}{UP_ARROW}"));
        }
        else if (moveTarget == TARGET_FOES_AND_ALLY)
        {
            StringAppend(gStringVar1, COMPOUND_STRING(" {V_D_ARROW}{UP_ARROW}"));
        }
        else if (moveTarget == TARGET_ALL_BATTLERS || moveTarget == TARGET_FIELD)
        {
            StringAppend(gStringVar1, COMPOUND_STRING(" {V_D_ARROW}{V_D_ARROW}"));
        }
        BattlePutTextOnWindow(gStringVar1, B_WIN_ACTION_PROMPT);
    }
    else
    {
        // BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_ACTION_PROMPT);

        HideAllTargets();

        if (FlagGet(FLAG_IS_TURN_START) == TRUE 
            || gSaveBlock2Ptr->optionsPreviewStyle == OPTIONS_PREVIEW_NONE)
        {
            FlagClear(FLAG_IS_TURN_START);
            gBattleStruct->movePreviewDisplayed = 0;
            CreateInfoWindow(battler);
        }
        else if (gSaveBlock2Ptr->optionsPreviewStyle == OPTIONS_PREVIEW_LIMITED 
            || gSaveBlock2Ptr->optionsPreviewStyle == OPTIONS_PREVIEW_FULL)
        {
            gBattleStruct->movePreviewDisplayed = 1;
            CreateSpeedTiersWindow(battler);
        }
    }
}

static void CreateInfoWindow(u32 battler)
{
    u8 aliveCount = 0;
    u8 totalCount = 0;
    u8 trainerAPartySize = GetTrainerPartySizeFromId(TRAINER_BATTLE_PARAM.opponentA);
    u8 trainerBPartySize = 0;
    u8 i;

    if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && trainerAPartySize > PARTY_SIZE / 2)
        trainerAPartySize = PARTY_SIZE / 2;

    if (TRAINER_BATTLE_PARAM.opponentB != TRAINER_NONE
        && TRAINER_BATTLE_PARAM.opponentB != 0xFFFF)
    {
        trainerBPartySize = GetTrainerPartySizeFromId(TRAINER_BATTLE_PARAM.opponentB);
        if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && trainerBPartySize > PARTY_SIZE / 2)
            trainerBPartySize = PARTY_SIZE / 2;
    }

    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        totalCount = trainerAPartySize + trainerBPartySize;

        for (i = 0; i < trainerAPartySize; i++)
        {
            if (GetMonData(&gEnemyParty[i], MON_DATA_HP) != 0)
                aliveCount++;
        }

        if (trainerBPartySize != 0)
        {
            for (i = PARTY_SIZE / 2; i < PARTY_SIZE / 2 + trainerBPartySize; i++)
            {
                if (GetMonData(&gEnemyParty[i], MON_DATA_HP) != 0)
                    aliveCount++;
            }
        }

        StringCopy(gStringVar1, COMPOUND_STRING("Turn "));
        ConvertUIntToDecimalStringN(gStringVar2, gBattleResults.battleTurnCounter + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringAppend(gStringVar1, gStringVar2);
        StringAppend(gStringVar1, COMPOUND_STRING(" - "));
        ConvertUIntToDecimalStringN(gStringVar2, aliveCount, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringAppend(gStringVar1, gStringVar2);
        StringAppend(gStringVar1, COMPOUND_STRING("/"));
        ConvertUIntToDecimalStringN(gStringVar2, totalCount, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringAppend(gStringVar1, gStringVar2);
        if (gBattleWeather == B_WEATHER_NONE)
            StringAppend(gStringVar1, COMPOUND_STRING(" {PKMN}"));
        else
        {
            StringAppend(gStringVar1, COMPOUND_STRING(" - "));
            StringAppend(gStringVar1, GetBattleWeatherName());
            // StringAppend(gStringVar1, COMPOUND_STRING(")"));
        }
    }
    else
    {
        StringCopy(gStringVar1, COMPOUND_STRING("Turn "));
        ConvertUIntToDecimalStringN(gStringVar2, gBattleResults.battleTurnCounter + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringAppend(gStringVar1, gStringVar2);
        if (gBattleWeather != B_WEATHER_NONE)
        {
            StringAppend(gStringVar1, COMPOUND_STRING(" - "));
            StringAppend(gStringVar1, GetBattleWeatherName());
            // StringAppend(gStringVar1, COMPOUND_STRING(")"));
        }
    }

    if (!FlagGet(FLAG_HIDE_BATTLE_TUTORIAL)
        && (gSaveBlock2Ptr->optionsPreviewStyle == OPTIONS_PREVIEW_LIMITED
        || gSaveBlock2Ptr->optionsPreviewStyle == OPTIONS_PREVIEW_FULL))
    {
        FlagSet(FLAG_HIDE_BATTLE_TUTORIAL);
        StringAppend(gStringVar1, COMPOUND_STRING("\nSELECT: Spd + Target"));
    }
    else {
        StringAppend(gStringVar1, COMPOUND_STRING("\nGo, "));
        GetMonData(GetBattlerMon(battler), MON_DATA_NICKNAME, gStringVar2);
        StringAppend(gStringVar1, gStringVar2);
        StringAppend(gStringVar1, COMPOUND_STRING("!"));
    }

    BattlePutTextOnWindow(gStringVar1, B_WIN_ACTION_PROMPT);
}

static void PlayerHandleYesNoBox(enum BattlerId battler)
{
    if (IsOnPlayerSide(battler))
    {
        HandleBattleWindow(YESNOBOX_X_Y, 0);
        BattlePutTextOnWindow(gText_BattleYesNoChoice, B_WIN_YESNO);
        gMultiUsePlayerCursor = 1;
        BattleCreateYesNoCursorAt(1);
        gBattlerControllerFuncs[battler] = PlayerHandleYesNoInput;
    }
    else
    {
        BtlController_Complete(battler);
    }
}

void HandleChooseMoveAfterDma3(enum BattlerId battler)
{
    if (!IsDma3ManagerBusyWithBgCopy())
    {
        gBattle_BG0_X = 0;
        gBattle_BG0_Y = DISPLAY_HEIGHT * 2;
        gBattlerControllerFuncs[battler] = HandleInputChooseMove;
    }
}

// arenaMindPoints is used here as a placeholder for a timer.

static void PlayerChooseMoveInBattlePalace(enum BattlerId battler)
{
    if (--gBattleStruct->arenaMindPoints[battler] == 0)
    {
        gBattlePalaceMoveSelectionRngValue = gRngValue;
        BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, ChooseMoveAndTargetInBattlePalace(battler));
        BtlController_Complete(battler);
    }
}

void PlayerHandleChooseMove(enum BattlerId battler)
{
    gBattleStruct->viewingOpponentMoves = FALSE;
    if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
    {
        gBattleStruct->arenaMindPoints[battler] = 8;
        gBattlerControllerFuncs[battler] = PlayerChooseMoveInBattlePalace;
    }
    else
    {
        struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);

        InitMoveSelectionsVarsAndStrings(battler);
        gBattleStruct->gimmick.playerSelect = FALSE;
        TryToAddMoveInfoWindow();
        // TryToHideMovePreviewWindow(); // FTP

        AssignUsableZMoves(battler, moveInfo->moves);
        gBattleStruct->zmove.viable = (gBattleStruct->zmove.possibleZMoves[battler] & (1u << gMoveSelectionCursor[battler])) != 0;

        if (!IsGimmickTriggerSpriteActive())
            gBattleStruct->gimmick.triggerSpriteId = 0xFF;
        else if (!IsGimmickTriggerSpriteMatchingBattler(battler))
            DestroyGimmickTriggerSprite();
        if (!(gBattleStruct->gimmick.usableGimmick[battler] == GIMMICK_Z_MOVE && !gBattleStruct->zmove.viable))
            CreateGimmickTriggerSprite(battler);

        gBattlerControllerFuncs[battler] = HandleChooseMoveAfterDma3;
    }
}

void InitMoveSelectionsVarsAndStrings(enum BattlerId battler)
{
    LoadTypeIcons(battler);
    MoveSelectionDisplayMoveNames(battler);
    gMultiUsePlayerCursor = 0xFF;
    MoveSelectionCreateCursorAt(gMoveSelectionCursor[battler], 0);
    if (B_SHOW_EFFECTIVENESS)
        MoveSelectionDisplayMoveEffectiveness(CheckTargetTypeEffectiveness(battler), battler);
    else
        MoveSelectionDisplayPpString(battler);
    MoveSelectionDisplayPpNumber(battler);
    MoveSelectionDisplayMoveType(battler);
}

static void PlayerHandleChooseItem(enum BattlerId battler)
{
    s32 i;

    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
    gBattlerControllerFuncs[battler] = OpenBagAndChooseItem;
    gBattlerInMenuId = battler;

    for (i = 0; i < ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
        gBattlePartyCurrentOrder[i] = gBattleResources->bufferA[battler][1 + i];
}

static void PlayerHandleChoosePokemon(enum BattlerId battler)
{
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
        gBattlePartyCurrentOrder[i] = gBattleResources->bufferA[battler][4 + i];

    if (gBattleTypeFlags & BATTLE_TYPE_ARENA && gBattleResources->bufferA[battler][1] != PARTY_ACTION_CANT_SWITCH
        && gBattleResources->bufferA[battler][1] != PARTY_ACTION_CHOOSE_FAINTED_MON
        && gBattleResources->bufferA[battler][1] != PARTY_ACTION_SEND_MON_TO_BOX)
    {
        BtlController_EmitChosenMonReturnValue(battler, B_COMM_TO_ENGINE, gBattlerPartyIndexes[battler] + 1, gBattlePartyCurrentOrder);
        BtlController_Complete(battler);
    }
    else
    {
        gBattleControllerData[battler] = CreateTask(TaskDummy, 0xFF);
        gTasks[gBattleControllerData[battler]].data[0] = gBattleResources->bufferA[battler][1];
        *(&gBattleStruct->battlerPreventingSwitchout) = gBattleResources->bufferA[battler][8];
        *(&gBattleStruct->prevSelectedPartySlot) = gBattleResources->bufferA[battler][2];
        *(&gBattleStruct->abilityPreventingSwitchout) = (gBattleResources->bufferA[battler][3] & 0xFF) | (gBattleResources->bufferA[battler][7] << 8);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
        gBattlerControllerFuncs[battler] = OpenPartyMenuToChooseMon;
        gBattlerInMenuId = battler;
    }
}

static void PlayerHandleCmd23(enum BattlerId battler)
{
    BattleStopLowHpSound();
    BeginNormalPaletteFade(PALETTES_ALL, 2, 0, 16, RGB_BLACK);
    BtlController_Complete(battler);
}

void PlayerHandleExpUpdate(enum BattlerId battler)
{
    u8 monId = gBattleResources->bufferA[battler][1];
    s32 taskId, expPointsToGive;

    if (GetMonData(&gPlayerParty[monId], MON_DATA_LEVEL) >= MAX_LEVEL)
    {
        BtlController_Complete(battler);
    }
    else
    {
        LoadBattleBarGfx(1);
        expPointsToGive = T1_READ_32(&gBattleResources->bufferA[battler][2]);
        taskId = CreateTask(Task_GiveExpToMon, 10);
        gTasks[taskId].tExpTask_monId = monId;
        gTasks[taskId].tExpTask_gainedExp_1 = expPointsToGive;
        gTasks[taskId].tExpTask_gainedExp_2 = expPointsToGive >> 16;
        gTasks[taskId].tExpTask_battler = battler;
        gBattlerControllerFuncs[battler] = BattleControllerDummy;
    }
}

#undef tExpTask_monId
#undef tExpTask_battler
#undef tExpTask_gainedExp_1
#undef tExpTask_gainedExp_2
#undef tExpTask_frames

static void PlayerHandleStatusXor(enum BattlerId battler)
{
    u32 val = GetMonData(GetBattlerMon(battler), MON_DATA_STATUS) ^ gBattleResources->bufferA[battler][1];

    SetMonData(GetBattlerMon(battler), MON_DATA_STATUS, &val);
    BtlController_Complete(battler);
}

static void PlayerHandleDMA3Transfer(enum BattlerId battler)
{
    u32 dstArg = gBattleResources->bufferA[battler][1]
            | (gBattleResources->bufferA[battler][2] << 8)
            | (gBattleResources->bufferA[battler][3] << 16)
            | (gBattleResources->bufferA[battler][4] << 24);
    u16 sizeArg = gBattleResources->bufferA[battler][5] | (gBattleResources->bufferA[battler][6] << 8);

    const u8 *src = &gBattleResources->bufferA[battler][7];
    u8 *dst = (u8 *)(dstArg);
    u32 size = sizeArg;

    while (1)
    {
        if (size <= 0x1000)
        {
            DmaCopy16(3, src, dst, size);
            break;
        }
        DmaCopy16(3, src, dst, 0x1000);
        src += 0x1000;
        dst += 0x1000;
        size -= 0x1000;
    }
    BtlController_Complete(battler);
}

static void PlayerHandlePlayBGM(enum BattlerId battler)
{
    PlayBGM(gBattleResources->bufferA[battler][1] | (gBattleResources->bufferA[battler][2] << 8));
    BtlController_Complete(battler);
}

static void PlayerHandleTwoReturnValues(enum BattlerId battler)
{
    BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_USE_MOVE, 0);
    BtlController_Complete(battler);
}

static void PlayerHandleChosenMonReturnValue(enum BattlerId battler)
{
    BtlController_EmitChosenMonReturnValue(battler, B_COMM_TO_ENGINE, 0, NULL);
    BtlController_Complete(battler);
}

static void PlayerHandleOneReturnValue(enum BattlerId battler)
{
    BtlController_EmitOneReturnValue(battler, B_COMM_TO_ENGINE, 0);
    BtlController_Complete(battler);
}

static void PlayerHandleOneReturnValue_Duplicate(enum BattlerId battler)
{
    BtlController_EmitOneReturnValue_Duplicate(battler, B_COMM_TO_ENGINE, 0);
    BtlController_Complete(battler);
}

static void PlayerHandleIntroTrainerBallThrow(enum BattlerId battler)
{
    const u32 paletteIndex = PlayerGetTrainerBackPicId() - TRAINER_PIC_FRONT_COUNT;
    const u16 *trainerPal = gTrainerBacksprites[paletteIndex].palette.data;
    BtlController_HandleIntroTrainerBallThrow(battler, 0xD6F8, trainerPal, 31, Intro_TryShinyAnimShowHealthbox);
}

static void PlayerHandleDrawPartyStatusSummary(enum BattlerId battler)
{
    BtlController_HandleDrawPartyStatusSummary(battler, B_SIDE_PLAYER, TRUE);
}

static void PlayerHandleEndBounceEffect(enum BattlerId battler)
{
    EndBounceEffect(battler, BOUNCE_HEALTHBOX);
    EndBounceEffect(battler, BOUNCE_MON);
    BtlController_Complete(battler);
}

static void PlayerHandleLinkStandbyMsg(enum BattlerId battler)
{
    RecordedBattle_RecordAllBattlerData(&gBattleResources->bufferA[battler][2]);
    switch (gBattleResources->bufferA[battler][1])
    {
    case LINK_STANDBY_MSG_STOP_BOUNCE:
        PrintLinkStandbyMsg();
        // fall through
    case LINK_STANDBY_STOP_BOUNCE_ONLY:
        EndBounceEffect(battler, BOUNCE_HEALTHBOX);
        EndBounceEffect(battler, BOUNCE_MON);
        break;
    case LINK_STANDBY_MSG_ONLY:
        PrintLinkStandbyMsg();
        break;
    }
    BtlController_Complete(battler);
}

static void PlayerHandleResetActionMoveSelection(enum BattlerId battler)
{
    switch (gBattleResources->bufferA[battler][1])
    {
    case RESET_ACTION_MOVE_SELECTION:
        gActionSelectionCursor[battler] = 0;
        gMoveSelectionCursor[battler] = 0;
        break;
    case RESET_ACTION_SELECTION:
        gActionSelectionCursor[battler] = 0;
        break;
    case RESET_MOVE_SELECTION:
        gMoveSelectionCursor[battler] = 0;
        break;
    }
    BtlController_Complete(battler);
}

static void PlayerHandleEndLinkBattle(enum BattlerId battler)
{
    RecordedBattle_RecordAllBattlerData(&gBattleResources->bufferA[battler][4]);
    gBattleOutcome = gBattleResources->bufferA[battler][1];
    gSaveBlock2Ptr->frontier.disableRecordBattle = gBattleResources->bufferA[battler][2];
    FadeOutMapMusic(5);
    BeginFastPaletteFade(3);
    BtlController_Complete(battler);
    gBattlerControllerFuncs[battler] = SetBattleEndCallbacks;
}

static void Controller_WaitForDebug(enum BattlerId battler)
{
    if (gMain.callback2 == BattleMainCB2 && !gPaletteFade.active)
    {
        BtlController_Complete(battler);
    }
}

static void PlayerHandleBattleDebug(enum BattlerId battler)
{
    BeginNormalPaletteFade(-1, 0, 0, 0x10, 0);
    SetMainCallback2(CB2_BattleDebugMenu);
    gBattlerControllerFuncs[battler] = Controller_WaitForDebug;
}

// Order based numerically, with EFFECTIVENESS_CANNOT_VIEW at 0 to always prioritize any other effectiveness during comparison
enum
{
    EFFECTIVENESS_CANNOT_VIEW,
    EFFECTIVENESS_NO_EFFECT,
    EFFECTIVENESS_NOT_VERY_EFFECTIVE,
    EFFECTIVENESS_NORMAL,
    EFFECTIVENESS_SUPER_EFFECTIVE,
};

static bool32 ShouldShowTypeEffectiveness(u32 targetId)
{
    if (IsGhostBattleWithoutScope())
        return FALSE;

    if (B_SHOW_EFFECTIVENESS == SHOW_EFFECTIVENESS_CAUGHT)
        return GetSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[targetId].species), FLAG_GET_CAUGHT);

    if (B_SHOW_EFFECTIVENESS == SHOW_EFFECTIVENESS_SEEN)
        return GetSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[targetId].species), FLAG_GET_SEEN);

    return TRUE;
}

static u32 CheckTypeEffectiveness(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battlerAtk][4]);
    struct BattleContext ctx = {0};
    ctx.battlerAtk = battlerAtk;
    ctx.battlerDef = battlerDef;
    ctx.move = moveInfo->moves[gMoveSelectionCursor[battlerAtk]];
    ctx.moveType = CheckDynamicMoveType(GetBattlerMon(battlerAtk), ctx.move, battlerAtk, MON_IN_BATTLE);
    ctx.updateFlags = FALSE;
    ctx.abilityAtk = GetBattlerAbility(battlerAtk);
    ctx.abilityDef = GetBattlerAbility(battlerDef);
    ctx.holdEffectAtk = GetBattlerHoldEffect(battlerAtk);
    ctx.holdEffectDef = GetBattlerHoldEffect(battlerDef);

    uq4_12_t modifier = CalcTypeEffectivenessMultiplier(&ctx);

    if (!ShouldShowTypeEffectiveness(battlerDef))
        return EFFECTIVENESS_CANNOT_VIEW;

    if (modifier == UQ_4_12(0.0))
        return EFFECTIVENESS_NO_EFFECT; // No effect
    else if (modifier <= UQ_4_12(0.5))
        return EFFECTIVENESS_NOT_VERY_EFFECTIVE; // Not very effective
    else if (modifier >= UQ_4_12(2.0))
        return EFFECTIVENESS_SUPER_EFFECTIVE; // Super effective
    return EFFECTIVENESS_NORMAL; // Normal effectiveness
}

static u32 CheckTargetTypeEffectiveness(enum BattlerId battler)
{
    enum BattlerId battlerFoe = BATTLE_OPPOSITE(battler);
    u32 foeEffectiveness = CheckTypeEffectiveness(battler, battlerFoe);

    if (IsDoubleBattle())
    {
        enum BattlerId partnerFoe = BATTLE_PARTNER(battlerFoe);
        u32 partnerFoeEffectiveness = CheckTypeEffectiveness(battler, partnerFoe);
        if (!IsBattlerAlive(battlerFoe))
            return partnerFoeEffectiveness;
        if (IsBattlerAlive(battlerFoe) && IsBattlerAlive(partnerFoe)
         && partnerFoeEffectiveness > foeEffectiveness)
            return partnerFoeEffectiveness;
    }
    return foeEffectiveness; // fallthrough for any other circumstance
}

static void MoveSelectionDisplayMoveEffectiveness(u32 foeEffectiveness, enum BattlerId battler)
{
    static const u8 noIcon[] =  _("");
    static const u8 effectiveIcon[] =  _("{CIRCLE_HOLLOW}");
    static const u8 superEffectiveIcon[] =  _("{CIRCLE_DOT}");
    static const u8 notVeryEffectiveIcon[] =  _("{TRIANGLE}");
    static const u8 immuneIcon[] =  _("{BIG_MULT_X}");
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
    u8 *txtPtr;

    txtPtr = StringCopy(gDisplayedStringBattle, gText_MoveInterfacePP);

    if (!IsBattleMoveStatus(moveInfo->moves[gMoveSelectionCursor[battler]]))
    {
        switch (foeEffectiveness)
        {
        case EFFECTIVENESS_SUPER_EFFECTIVE:
            StringCopy(txtPtr, superEffectiveIcon);
            break;
        case EFFECTIVENESS_NOT_VERY_EFFECTIVE:
            StringCopy(txtPtr, notVeryEffectiveIcon);
            break;
        case EFFECTIVENESS_NO_EFFECT:
            StringCopy(txtPtr, immuneIcon);
            break;
        case EFFECTIVENESS_NORMAL:
            StringCopy(txtPtr, effectiveIcon);
            break;
        default:
        case EFFECTIVENESS_CANNOT_VIEW:
            StringCopy(txtPtr, noIcon);
            break;
        }
    }

    BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_PP);
}
