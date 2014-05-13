using System;
using System.Threading;
using Thrift;
using Thrift.Protocol;
using Thrift.Server;
using Thrift.Transport;
using System.Diagnostics;

public class CSharpClient
{
    // DEBUG
    public static void printWorldModel(WorldModel wm) {

    }


    public static void synchronize(int t) {
        if(t > 0)
            Thread.Sleep(t);
    }

    public static void playGame(Game.Client client) {
        GameInit gameInit = client.ready();
        GameInfo gameInfo = gameInit.GameInfo;
        Solution solution = new Solution(gameInit);
        Stopwatch stopwatch = new Stopwatch();
        stopwatch.Start();
        while (true) {
            stopwatch.Stop();
            synchronize(
                gameInfo.NextWorldModelTimeEstimateMs - stopwatch.Elapsed.Milliseconds);
            gameInfo = client.getGameInfo();
            stopwatch.Reset();
            stopwatch.Start ();
            WorldModel wm = gameInfo.WorldModel;
            printWorldModel(wm);
            if (gameInfo.GameStatus == GameStatus.FINISHED) 
            {
                break;
            }
            if (gameInfo.IsMyTurn) 
            {
                Command command = solution.playTurn(wm);
                client.sendCommand(command);
            }
        }
    }

    public static void Main(string[] args)
    {
        int port = 9090;
        for (int i = 0; i < args.Length; i++) 
        {
            switch(args[i])
            { 
                case "--port":
                    port = int.Parse(args[++i]);
                    break;
                case "--help":
                    Console.WriteLine("--port: Port used to connect with the server. (int)");
                    break;
            }
        }

        try
        {
            TSocket tSocket = new TSocket("localhost", port);
            tSocket.TcpClient.NoDelay = true;
            TTransport transport = tSocket;
            TProtocol protocol = new TBinaryProtocol(transport);
            Game.Client client = new Game.Client(protocol);
            transport.Open();
            playGame(client);         
            transport.Close();
        }
        catch (TApplicationException x)
        {
            Console.WriteLine(x.StackTrace);
        }
    }
}
