using System;
using System.Threading;
using System.Diagnostics;

public class Solution
{
    public Random random;
    public Command playTurn(WorldModel wm) {
        Command command = new Command(new Coordinate());
        while(true) 
        {
            int x = random.Next(3);
            int y = random.Next(3);
            if (wm.Table[x][y] == Marker.UNMARKED) {
                command.Coordinate.X = x;
                command.Coordinate.Y = y;
                break;
            }
        }
        return command;
    }

    public Solution(GameInit gameInit)
    {
        random = new Random();
    }

}
