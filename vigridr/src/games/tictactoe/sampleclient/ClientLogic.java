import java.util.List;
import java.util.Random;

public class ClientLogic {
    public Random random;

    /*
     * Constructor: called at the beginning of the game.
     * You may do initialization here.
     *
     * Parameter:
     *     gameInit - contains an attribute named gameDescription, which itself contains an attribute named myType.
     *                myType is of type Marker, which is an enum. Marker has three fields: UNMARKED, X and O.
     */
    public ClientLogic(GameInit gameInit) {
        random = new Random();

        System.out.println("Java Client");
        System.out.println("PlayerType: " + gameInit.gameDescription.myType);
    }

    /*
     * This method is called once for every turn.
     * This specific example solution returns a random valid position.
     *
     * Parameters:
     *     wm   - an instance of the WorldModel class that contains an attribute called table which is a List of Lists of Markers.
     *     turn - the index of the turn.
     *            If you receive twice the same number, don't worry, just ignore it.
     *
     * Returns:
     *     A Command instance - a Command contains an attribute called coordinate of class coordinate.
     *                          A Coordinate contains two attributes of type int, x and y.
     */
    public Command playTurn(WorldModel wm, int turn) {
        Command command = new Command(new Coordinate());

        Marker[][] table = toMatrix(wm.table);

        while(true) {
            int x = random.nextInt(3);
            int y = random.nextInt(3);
            if (Marker.UNMARKED.equals(table[x][y])) {
                command.coordinate.x = x;
                command.coordinate.y = y;
                break;
            }
        }

        System.out.println(turn + ": " + command.coordinate.toString());
        return command;
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

    /*
     * Helper function to conver a List of List of Markers to an array of arrays of Markers
     * (aka matrix of Markers).
     */
    private Marker[][] toMatrix(List<List<Marker>> table) {
        Marker[][] matrix = new Marker[3][];

        int i = 0;
        for (List<Marker> row : table) {
            matrix[i] = new Marker[3];
            row.toArray(matrix[i]);
            i++;
        }

        return matrix;
    }
}

