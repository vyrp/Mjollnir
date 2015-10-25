import java.util.List;
import java.util.Random;

public class ClientLogic {
    public Random random;

    /*
     * Constructor: called at the beginning of the game.
     * You may do initialization here.
     *
     * Parameter:
     *     gameInit - not used for Wumpus
     */
    public ClientLogic(GameInit gameInit) {
        random = new Random();

        System.out.println("Java Client");
    }

    /*
     * This method is called once for every turn.
     * This specific example solution returns a random action.
     *
     * Parameters:
     *     wm   - an instance of the WorldModel class that contains a field called sensors of class Sensors.
     *            Sensors contains the boolean fields: breeze, stench, glitter, bump and scream.
     *     turn - the index of the turn.
     *            If you receive twice the same number, then it means that you still have some time to think and send another command.
     *
     * Returns:
     *     A Command instance - a Command contains a field called action of enum Action.
     *                          Action fields: FORWARD, TURNRIGHT, TURNLEFT, STAY, SHOOT, GRAB and CLIMB.
     */
    public Command playTurn(WorldModel wm, int turn) {
        Command command = new Command();
        int move = random.nextInt(3);

        System.out.print(turn + ": ");
        if(move == 0){
            System.out.println("FORWARD");
            command.action = Action.FORWARD;
        } else if(move == 1) {
            System.out.println("TURNRIGHT");
            command.action = Action.TURNRIGHT;
        } else {
            System.out.println("TURNLEFT");
            command.action = Action.TURNRIGHT;
        }

        return command;
    }
}

