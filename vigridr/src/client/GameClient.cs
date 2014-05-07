using System;
using System.Threading;
using Thrift;
using Thrift.Protocol;
using Thrift.Server;
using Thrift.Transport;
using System.Diagnostics;

public class Solution
{
  public static Random random;
  public static Command playTurn(WorldModel wm) {
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
    return command;
  }

  public static void init()
  {
    random = new Random();
  }

}

public class CSharpClient
{
  public static void printWorldModel(WorldModel wm) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        char toPrint;
        switch(wm.Table[i][j]) {
          case Marker.X:
            toPrint = 'X'; 
            break;
          case Marker.O:
            toPrint = 'O';
            break;
          default:
            toPrint = '-';
            break;
        }
        Console.Write(toPrint + " ");
      }
      Console.WriteLine("\n");
    }
     Console.WriteLine("");
  }


  public static void synchronize(int t) {
    if(t > 0)
      Thread.Sleep(t);
  }

  public static void playGame(Game.Client client) {
    Solution.init();
    GameInfo gameInfo = client.ready();
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
        Command command = Solution.playTurn(wm);
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
