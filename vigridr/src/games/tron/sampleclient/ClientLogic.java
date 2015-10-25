public class ClientLogic {
    /*
     * Constructor: called at the beginning of the game.
     * You may do initialization here.
     *
     * Parameter:
     *     gameInit - contains an attribute named gameDescription, which itself contains attributes myIndex and field.
     *                myIndex is an int and represents the index of the world model table of your snake.
     *                field is an instance of the class Field, and contains the attributes width and height, of type int.
     *                It also contains an attribute called gameInfo with another attribute, worldModel, of type WorldModel, described below.
     */
    public ClientLogic(GameInit gameInit) {
        System.out.println("Java Client");
    }

    /*
     * This method is called once for every turn.
     * It returns an empty command. For a more interesting example, see the python sample solution.
     *
     * Parameters:
     *     wm   - an instance of the WorldModel class that contains an attribute called players which is a List of Players.
     *            A Player has an attribute named body which is a List of Coordinates,
     *            and which represent the coordinates of the body parts of the snake, in order.
     *            A Coordinate has two attributes, x and y, of type int.
     *     turn - the index of the turn.
     *            If you receive twice the same number, don't worry, just ignore it.
     *
     * Returns:
     *     A Command instance - a Command contains an attribute called direction of type enum Direction.
     *                          Direction values: RIGHT, UP, LEFT, DOWN.
     */
    public Command playTurn(WorldModel wm, int turn) {
       return null;
    }

    /*
     * This method is called at the end of the game.
     *
     * Parameters:
     *     result - an instance of the GameResult enum, which can be GameResult.WON, GameResult.TIED or GameResult.LOST.
     */
    public void endOfGame(GameResult result) {
        System.out.println("End of game - " + result.toString());
    }
}
