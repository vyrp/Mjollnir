using System;
using System.Threading;
using System.Diagnostics;

public class Solution
{
    public Random random;

    /*
     * Constructor: called at the beginning of the game.
     * You may do initialization here.
     *
     * Parameter:
     *     gameInit - not used for Wumpus
     */
    public Solution(GameInit gameInit)
    {
        random = new Random();

        Console.WriteLine("C# Client");
    }

    /*
     * This method is called once for every turn.
     * This specific example solution returns a random action.
     *
     * Parameter:
     *     wm - an instance of the WorldModel class that contains a property called Sensors of class Sensors.
     *          Sensors contains the boolean properties: Breeze, Stench, Glitter, Bump and Scream.
     *
     * Returns:
     *     A Command instance - a Command contains a property called Action of enum Action.
     *                          Action fields: FORWARD, TURNRIGHT, TURNLEFT, STAY, SHOOT, GRAB and CLIMB.
     */
    public Command playTurn(WorldModel wm) {
        Command command = new Command();
        int move = random.Next(3);

        if (move == 0)
        {
            Console.WriteLine("FORWARD");
            command.Action = Action.FORWARD;
        }
        else if(move == 1)
        {
            Console.WriteLine("TURNRIGHT");
            command.Action = Action.TURNRIGHT;
        }
        else
        {
            Console.WriteLine("TURNLEFT");
            command.Action = Action.TURNLEFT;
        }

        return command;
    }
}

