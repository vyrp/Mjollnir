import java.util.List;
import java.util.Random;

public class ClientLogic {
    public Random random;

    public ClientLogic(GameInit gameInit) {
        random = new Random();
    }

    public Command playTurn(WorldModel wm) {
        Command command = new Command(new Coordinate());

        Marker[][] table = new Marker[3][];
        int i = 0;
        for (List<Marker> row : wm.table) {
            table[i] = new Marker[3];
            row.toArray(table[i]);
            i++;
        }

        while(true) {
            int x = random.nextInt(3);
            int y = random.nextInt(3);
            if (Marker.UNMARKED.equals(table[x][y])) {
                command.coordinate.x = x;
                command.coordinate.y = y;
                break;
            }
        }
        return command;
    }

}
