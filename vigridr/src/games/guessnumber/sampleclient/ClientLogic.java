import java.util.Random;

public class ClientLogic {
    public Random random;

    public ClientLogic(GameInit gameInit) {
        random = new Random();
    }

    public Command playTurn(WorldModel wm) {
        return new Command(random.nextInt(10) + 1);
    }
}
