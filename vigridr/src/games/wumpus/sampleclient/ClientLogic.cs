using System;
using System.Threading;
using System.Diagnostics;

public class Solution
{
    public Random random;
    public Command playTurn(WorldModel wm) {
        Command command = new Command();
        int move = random.Next(4);

        if (move == 0) {
            Console.WriteLine("FORWARD");
            command.Action = Action.FORWARD;
        } else if(move == 1){
            Console.WriteLine("TURNRIGHT");
            command.Action = Action.TURNRIGHT;
        } else if(move == 2){
            Console.WriteLine("TURNLEFT");
            command.Action = Action.TURNLEFT;
        } else {
            Console.WriteLine("STAY");
            command.Action = Action.STAY;
        }

        return command;
    }

    public Solution(GameInit gameInit)
    {
        random = new Random();
    }

}
