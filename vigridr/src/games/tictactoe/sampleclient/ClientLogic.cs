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
     *     gameInit - contains a property named GameDescription, which itself contains a property named MyType.
     *                MyType is of type Marker, which is an enum. Marker has three fields: UNMARKED, X and O.
     */
    public Solution(GameInit gameInit)
    {
        random = new Random();

        Console.WriteLine("C# Client");
        Console.WriteLine("PlayerType: " + gameInit.GameDescription.MyType);
    }

    /*
     * This method is called once for every turn.
     * This specific example solution returns a random valid position.
     *
     * Parameter:
     *     wm - an instance of the WorldModel class that contains a property called Table which is a List of Lists of Markers.
     *
     * Returns:
     *     A Command instance - a Command contains a property called Coordinate of class Coordinate.
     *                          A Coordinate contains two properties of type int, X and Y.
     */
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

        Console.WriteLine(command.Coordinate.ToString());
        return command;
    }
}

