import java.util.List;
import java.util.Random;

public class ClientLogic {
    public Random random;

    public ClientLogic(GameInit gameInit) {
        random = new Random();

        System.out.println("Java Client");
    }

    public Command playTurn(WorldModel wm) {
        Command command = new Command();
        int move = random.nextInt(4);
        
        if(move == 0){
            System.out.println("FORWARD");
            command.action = Action.FORWARD;
        } else if(move == 1) {
            System.out.println("TURNRIGHT");
            command.action = Action.TURNRIGHT;   
        } else if(move == 2) {
            System.out.println("TURNLEFT");
            command.action = Action.TURNRIGHT;   
        } else {
            System.out.println("STAY");
            command.action = Action.STAY;   
        }
        
        return command;
    }

}

