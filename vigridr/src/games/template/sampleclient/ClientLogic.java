public class ClientLogic {
    /*
     * Constructor: called at the beginning of the game.
     * You may do initialization here.
     *
     * Parameter:
     *     gameInit - depends on the game. It will contain necessary information for initialization.
     */
    public ClientLogic(GameInit gameInit) {
    }

    /*
     * This method is called once for every turn.
     * This specific example solution returns an empty action.
     *
     * Parameters:
     *     wm   - depends on the game. It will contain the observable part of the world model.
     *     turn - the index of the turn.
     *            If you receive twice the same number, then it means that you still have some time to think and send another command.
     *
     * Returns:
     *     A Command instance - depends on the game. It's your command for this turn.
     */
    public Command playTurn(WorldModel wm, int turn) {
       return null;
    }
}
