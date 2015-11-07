using System;
using System.Collections.Generic;
using System.Linq;

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
     * Parameters:
     *     wm   - an instance of the WorldModel class that contains a property called Table which is a List of Lists of Markers.
     *     turn - the index of the turn.
     *            If you receive twice the same number, don't worry, just ignore it.
     *
     * Returns:
     *     A Command instance - a Command contains a property called Coordinate of class Coordinate.
     *                          A Coordinate contains two properties of type int, X and Y.
     */
    public Command playTurn(WorldModel wm, int turn) {
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

        Console.WriteLine(turn + ": " + command.Coordinate.ToString());
        return command;
    }

    /*
     * This method is called at the end of the game.
     *
     * Parameters:
     *     result - an instance of the GameResult enum, which can be GameResult.WON, GameResult.TIED or GameResult.LOST.
     */
    public void EndOfGame(GameResult result)
    {
        Console.WriteLine("End of game - " + result.ToString());
    }
}

