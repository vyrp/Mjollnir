using System;
using System.Collections.Generic;
using System.Linq;

public class Solution
{
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
     *     gameInit - contains a property named GameDescription, which itself contains a property named MyColor.
     *                MyColor is of type PlayerColor, which is an enum. PlayerColor has two fields: RED and WHITE.
     */
    public Solution(GameInit gameInit)
    {
        Console.WriteLine("C# Backgammon Client");

        var color = gameInit.GameDescription.MyColor;
        Console.WriteLine("PlayerColor: " + color);

        this.me = (int)color;
        this.lastTurn = -1;
        this.lastCommand = null;

        if (color == PlayerColor.RED)
        {
            this.other = (int)PlayerColor.WHITE;
            this.direction = +1;
            this.start = 0;
            this.end = 23;
        }
        else
        {
            this.other = (int)PlayerColor.RED;
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
     *     wm   - an instance of the WorldModel class that contains the following properties:
     *            Bar       - type Point. The number of checkers for each player in the bar.
     *            Board     - List of Point. Always contains 24 elements.
     *            Borne_off - type Point. The number of checkers that each player has borne off.
     *            Dice      - List of int. Always contains 2 elements.
     *            A Point is an alias for a List of int, with 2 elements, that represent
     *            the number of red and white checkers in that point, in that order. Hint: rembember that RED=0 and WHITE=1.
     *            Remember that a "point" is that triangle on the board where the checkers can be.
     *     turn - the index of the turn.
     *            If you receive twice the same number, don't worry, just ignore it.
     *
     * Returns:
     *     A Command instance - a Command contains a property called Moves, which is a list of Move.
     *                          A Move contains two properties of type int, Src and Dst.
     *                          Src and Dst must be in the interval [0, 24).
     *                          Additionally, Src can be CommandConstants.FROM_BAR and Dst can be CommandConstants.BEAR_OFF.
     */
    public Command playTurn(WorldModel wm, int turn)
    {
        // If repeated turn index, return the command that we already calculated
        if (turn == this.lastTurn)
        {
            return this.lastCommand;
        }

        this.lastTurn = turn;
        Console.Write(turn + ": " + wm.Dice.Representation() + " ");

        // Calculate the several dice combinations
        var diceCombinations = new List<List<int>>();
        if (wm.Dice[0] == wm.Dice[1])
        {
            wm.Dice.AddRange(wm.Dice);
            diceCombinations.Add(wm.Dice);
        }
        else
        {
            diceCombinations.Add(wm.Dice);
            diceCombinations.Add(Enumerable.Reverse(wm.Dice).ToList());
        }

        var command = new Command(new List<Move>());
        foreach (var dice in diceCombinations)
        {
            foreach (var die in dice)
            {
                // If I have a checkers in the bar, I must move it
                if (wm.Bar[this.me] > 0)
                {
                    var src = CommandConstants.FROM_BAR;
                    var dst = this.start - this.direction + die * this.direction;
                    if (wm.Board[dst][this.other] <= 1)
                    {
                        command.Moves.Add(new Move(src, dst));
                        wm.Bar[this.me]--;
                        wm.Board[dst][this.me]++;
                        // If I hit an opponent
                        if (wm.Board[dst][this.other] == 1)
                        {
                            wm.Board[dst][this.other]--;
                            wm.Bar[this.other]++;
                        }
                        continue;
                    }
                    else
                    {
                        break;
                    }
                 }

                // In order, try to move a piece
                for (int src = this.start; src != this.end + this.direction; src += this.direction)
                {
                    var dst = src + die * this.direction;
                    if (0 <= dst && dst <= 23 && wm.Board[src][this.me] > 0 && wm.Board[dst][this.other] <= 1)
                    {
                        command.Moves.Add(new Move(src, dst));
                        wm.Board[src][this.me]--;
                        wm.Board[dst][this.me]++;
                        // If I hit an opponent
                        if (wm.Board[dst][this.other] == 1)
                        {
                            wm.Board[dst][this.other]--;
                            wm.Bar[this.other]++;
                        }
                        break;
                    }
                }
            }
            if (command.Moves.Count == wm.Dice.Count)
            {
                break;
            }
        }
        // Finally send command
        Console.WriteLine("Command: " + command.Representation());
        this.lastCommand = command;
        return command;
    }

    /*
     * This method is called at the end of the game.
     *
     * Parameters:
     *     result - an instance of the GameResult class, which contains two boolean properties, Won and Invalid.
     *              The Invalid property is true if you lost because you sent an invalid command.
     */
    public void EndOfGame(GameResult result)
    {
        Console.WriteLine("End of game - " + (result.Won ? "Won!" : "Lost..."));
        if (result.Invalid)
        {
            Console.WriteLine("[WARNING] Invalid command");
        }
    }
}

/*
 * Helper class to pretty print some values. They are not mandatory for a solution.
 */
internal static class Extensions
{
    public static string Representation(this Move move)
    {
        return move.Src + "->" + move.Dst;
    }

    public static string Representation(this List<int> l)
    {
        return "[ " + string.Join(", ", l.Select(i => i.ToString()).ToArray()) + " ]";
    }

    public static string Representation(this Command command)
    {
        return "[ " + string.Join(", ", command.Moves.Select(m => m.Representation()).ToArray()) + " ]";
    }
}
