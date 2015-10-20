using System;

public class Solution
{
    /*
     * Constructor: called at the beginning of the game.
     * You may do initialization here.
     *
     * Parameter:
     *     gameInit - contains a property named GameDescription, which itself contains properties MyIndex and Field.
     *                MyIndex is an int and represents the index of the world model table of your snake.
     *                Field is an instance of the class Field, and contains the properties Width and Height, of type int.
     *                It also contains a field called GameInfo with another field, WorldModel, of type WorldModel, described below.
     */
    public Solution(GameInit gameInit)
    {
        Console.WriteLine("C# Example");
    }

    /*
     * This method is called once for every turn.
     * It returns an empty command. For a more interesting example, see the python sample solution.
     *
     * Parameter:
     *     wm - an instance of the WorldModel class that contains a property called Players which is a List of Players.
     *          A Player has a property named Body which is a List of Coordinates,
     *          and which represent the coordinates of the body parts of the snake, in order.
     *          A Coordinate has two properties, X and Y, of type int.
     *
     * Returns:
     *     A Command instance - a Command contains a property called Direction of type enum Direction.
     *                          Direction values: RIGHT, UP, LEFT, DOWN.
     */
    public Command playTurn(WorldModel wm)
    {
       return null;
    }
}
