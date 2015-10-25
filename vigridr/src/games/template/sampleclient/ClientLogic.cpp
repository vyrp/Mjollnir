#include "ClientLogic.h"

using namespace ::mjollnir::vigridr;

/*
 * This function is called at the beginning of the game.
 * You may do initialization here.
 *
 * Parameter:
 *     gameInit - depends on the game. It will contain necessary information for initialization.
 */
void init(const GameInit& gameInit) {
}

/*
 * This function is called once for every turn.
 * This specific example solution returns an empty action.
 *
 * Parameters:
 *     wm   - depends on the game. It will contain the observable part of the world model.
 *     turn - the index of the turn.
 *            If you receive twice the same number, don't worry, just ignore it.
 *
 * Returns:
 *     A Command instance - depends on the game. It's your command for this turn.
 */
Command playTurn(const WorldModel& wm, int32_t turn) {
  return Command();
}

/*
 * This function is called at the end of the game.
 *
 * Parameters:
 *     result - depends on the game. It will contain the result of the game.
 */
void endOfGame(const GameResult& result) {
}
