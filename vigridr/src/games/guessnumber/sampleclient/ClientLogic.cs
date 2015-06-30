using System;
using System.Threading;
using System.Diagnostics;

public class Solution
{
    public Random random;
    public Command playTurn(WorldModel wm) {
        int guess = random.Next(10) + 1;
        Command command = new Command(guess);
        return command;
    }

    public Solution(GameInit gameInit)
    {
        random = new Random();
    }

}
