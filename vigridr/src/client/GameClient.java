import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.transport.TTransportException;
import org.apache.thrift.TException;

public class GameClient
{
    public static void synchronize(long t) {
        if(t > 0) {
            try {
                Thread.sleep(t);
            }
            catch(InterruptedException e) {
                e.printStackTrace();
                System.exit(1);
            }
        }
    }

    public static void playGame(Game.Client client) throws TException {
        GameInit gameInit = client.ready();
        GameInfo gameInfo = gameInit.getGameInfo();
        ClientLogic solution = new ClientLogic(gameInit);
        long start = System.nanoTime();
        while (true) {
            long end = System.nanoTime();
            synchronize(gameInfo.getNextWorldModelTimeEstimateMs() - (end - start)/1000000);
            gameInfo = client.getGameInfo();
            start = System.nanoTime();
            if (GameStatus.FINISHED.equals(gameInfo.getGameStatus())) {
                solution.endOfGame(gameInfo.gameResult);
                break;
            }
            if (gameInfo.isIsMyTurn()) {
                Command command = solution.playTurn(gameInfo.worldModel, gameInfo.cycle);
                client.sendCommand(command);
            }
        }
    }

    public static void main(String[] args) {
        int port = 9090;
        for (int i = 0; i < args.length; i++) {
            switch(args[i]) {
                case "--port":
                    port = new Integer(args[++i]);
                    break;
                case "--help":
                    System.out.println("--port: Port used to connect with the server. (int)");
                    return;
            }
        }

        try {
            TSocket tSocket = new TSocket("localhost", port);
            TTransport transport = tSocket;
            TProtocol protocol = new TBinaryProtocol(transport);
            Game.Client client = new Game.Client(protocol);
            transport.open();
            playGame(client);
            transport.close();
        }
        catch (Exception e) {
            e.printStackTrace();
            System.exit(1);
        }
    }
}
