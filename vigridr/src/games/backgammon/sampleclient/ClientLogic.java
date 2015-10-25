import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ClientLogic {
    private int me;
    private int other;
    private int lastTurn;
    private Command lastCommand;
    private int direction;
    private int start;
    private int end;

    /*
     * Constructor: called at the beginning of the game.
     * You may do initialization here.
     *
     * Parameter:
     *     gameInit - contains an attribute named GameDescription, which itself contains an attribute named myColor.
     *                MyColor is of type PlayerColor, which is an enum. PlayerColor has two fields: RED and WHITE.
     */
    public ClientLogic(GameInit gameInit) {
        System.out.println("Java Backgammon Client");

        PlayerColor color = gameInit.gameDescription.myColor;
        System.out.println("PlayerColor: " + color);

        this.me = color.getValue();
        this.lastTurn = -1;
        this.lastCommand = null;

        if (color == PlayerColor.RED) {
            this.other = PlayerColor.WHITE.getValue();
            this.direction = +1;
            this.start = 0;
            this.end = 23;
        }
        else {
            this.other = PlayerColor.RED.getValue();
            this.direction = -1;
            this.start = 23;
            this.end = 0;
        }
    }

    /*
     * This method is called once for every turn.
     * This specific example solution tries to move the checkers at the highest points.
     * However, it is not complete, so it sometimes sends invalid commands.
     *
     * Parameters:
     *     wm   - an instance of the WorldModel class that contains the following attributes:
     *            bar       - type Point. The number of checkers for each player in the bar.
     *            board     - List of Point. Always contains 24 elements.
     *            borne_off - type Point. The number of checkers that each player has borne off.
     *            dice      - List of Integer. Always contains 2 elements.
     *            A Point is an alias for a List of Integer, with 2 elements, that represent
     *            the number of red and white checkers in that point, in that order. Hint: rembember that RED=0 and WHITE=1.
     *            Remember that a "point" is that triangle on the board where the checkers can be.
     *     turn - the index of the turn.
     *            If you receive twice the same number, don't worry, just ignore it.
     *
     * Returns:
     *     A Command instance - a Command contains an attribute called moves, which is a List of Move.
     *                          A Move contains two attributes of type int, src and dst.
     *                          src and dst must be in the interval [0, 24).
     *                          Additionally, src can be CommandConstants.FROM_BAR and dst can be CommandConstants.BEAR_OFF.
     */
    public Command playTurn(WorldModel wm, int turn) {
        // If repeated turn index, return the command that we already calculated
        if (turn == this.lastTurn) {
            return this.lastCommand;
        }

        this.lastTurn = turn;
        System.out.print(turn + ": " + Extensions.Representation(wm.dice) + " ");

        // Conversions so it is easier to use
        int[] wm_bar = new int[] { wm.bar.get(0), wm.bar.get(1) };
        int[][] wm_board = toMatrix(wm.board);

        // Calculate the several dice combinations
        List<int[]> diceCombinations = new ArrayList<>();
        if (wm.dice.get(0) == wm.dice.get(1)) {
            int[] wm_dice = new int[4];
            Arrays.fill(wm_dice, wm.dice.get(0));
            diceCombinations.add(wm_dice);
        }
        else {
            int[] wm_dice = new int[] { wm.dice.get(1), wm.dice.get(0) };
            diceCombinations.add(wm_dice);

            wm_dice = new int[] { wm_dice[1], wm_dice[0] };
            diceCombinations.add(wm_dice);
        }

        Command command = new Command(new ArrayList<Move>());
        for (int[] dice : diceCombinations) {
            for (int die : dice) {
                // If I have a checkers in the bar, I must move it
                if (wm_bar[this.me] > 0) {
                    int src = CommandConstants.FROM_BAR;
                    int dst = this.start - this.direction + die * this.direction;
                    if (wm_board[dst][this.other] <= 1) {
                        command.moves.add(new Move(src, dst));
                        wm_bar[this.me]--;
                        wm_board[dst][this.me]++;
                        // If I hit an opponent
                        if (wm_board[dst][this.other] == 1) {
                            wm_board[dst][this.other]--;
                            wm_bar[this.other]++;
                        }
                        continue;
                    }
                    else {
                        break;
                    }
                 }

                // In order, try to move a piece
                for (int src = this.start; src != this.end + this.direction; src += this.direction) {
                    int dst = src + die * this.direction;
                    if (0 <= dst && dst <= 23 && wm_board[src][this.me] > 0 && wm_board[dst][this.other] <= 1) {
                        command.moves.add(new Move(src, dst));
                        wm_board[src][this.me]--;
                        wm_board[dst][this.me]++;
                        // If I hit an opponent
                        if (wm_board[dst][this.other] == 1) {
                            wm_board[dst][this.other]--;
                            wm_bar[this.other]++;
                        }
                        break;
                    }
                }
            }
            if (command.moves.size() == wm.dice.size()) {
                break;
            }
        }
        // Finally send command
        System.out.println("Command: " + Extensions.Representation(command));
        this.lastCommand = command;
        return command;
    }

    /*
     * This method is called at the end of the game.
     *
     * Parameters:
     *     result - an instance of the GameResult class, which contains two boolean attributes, Won and Invalid.
     *              The Invalid property is true if you lost because you sent an invalid command.
     */
    public void endOfGame(GameResult result) {
        System.out.println("End of game - " + (result.won ? "Won!" : "Lost..."));
        if (result.invalid) {
            System.out.println("[WARNING] Invalid command");
        }
    }

    /*
     * Helper function to convert a List of List of Integer to an array of arrays of int
     * (aka matrix of int).
     */
    private static int[][] toMatrix(List<List<Integer>> board) {
        int[][] matrix = new int[board.size()][];

        int i = 0;
        for (List<Integer> point : board) {
            matrix[i] = new int[] { point.get(0), point.get(1) };
            i++;
        }

        return matrix;
    }
}

/*
 * Helper class to pretty print some values. They are not mandatory for a solution.
 */
class Extensions {
    public static String Representation(Move move) {
        return move.src + "->" + move.dst;
    }

    public static String Representation(List<Integer> l) {
        StringBuilder builder = new StringBuilder("[ ");
        for (int i : l) {
            builder.append(i + ", ");
        }
        builder.append("]");
        return builder.toString();
    }

    public static String Representation(Command command) {
        StringBuilder builder = new StringBuilder("[ ");
        for (Move move : command.moves) {
            builder.append(Extensions.Representation(move) + ", ");
        }
        builder.append("]");
        return builder.toString();
    }
}
