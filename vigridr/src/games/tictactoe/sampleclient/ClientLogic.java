import java.util.List;
import java.util.Random;

public class ClientLogic {
    public Random random;

    public ClientLogic(GameInit gameInit) {
        random = new Random();

        System.out.println("Java Client");
        System.out.println("PlayerType: " + gameInit.gameDescription.myType);
    }

    public Command playTurn(WorldModel wm) {
        Command command = new Command(new Coordinate());

        Marker[][] table = toMatrix(wm.table);

        while(true) {
            int x = random.nextInt(3);
            int y = random.nextInt(3);
            if (Marker.UNMARKED.equals(table[x][y])) {
                command.coordinate.x = x;
                command.coordinate.y = y;
                break;
            }
        }

        System.out.println(command.coordinate.toString());
        return command;
    }

    private Marker[][] toMatrix(List<List<Marker>> table) {
        Marker[][] matrix = new Marker[3][];

        int i = 0;
        for (List<Marker> row : table) {
            matrix[i] = new Marker[3];
            row.toArray(matrix[i]);
            i++;
        }

        return matrix;
    }
}

