#include <chrono>
#include <exception>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <thread>
#include <unistd.h>

#include <gflags/gflags.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../server/gen-cpp/Game.h"
#include "../server/GameConfig.h"

DEFINE_int32(port, 9090, "Port used to connect with the server.");

const char* const kVersion = "v1.1";
const char* const kUsageMessage = 
  "This program is the client for mjollnir matches";

using ::apache::thrift::protocol::TBinaryProtocol;
using ::apache::thrift::protocol::TProtocol;
using ::apache::thrift::TException;
using ::apache::thrift::transport::TBufferedTransport;
using ::apache::thrift::transport::TSocket;
using ::apache::thrift::transport::TTransport;

using ::mjollnir::vigridr::Command;
using ::mjollnir::vigridr::config::cycleWaitMs;
using ::mjollnir::vigridr::Coordinate;
using ::mjollnir::vigridr::GameClient;
using ::mjollnir::vigridr::GameInfo;
using ::mjollnir::vigridr::GameStatus;
using ::mjollnir::vigridr::WorldModel;
using ::mjollnir::vigridr::Marker;

void printWorldModel(const WorldModel& wm) {
  for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          char toPrint;
          switch(wm.table[i][j]) {
            case Marker::X:
              toPrint = 'X'; break;
            case Marker::O:
              toPrint = 'O';break;
            default:
              toPrint = '-';break;
          }
          std::cout << toPrint << " ";
        }
        std::cout << "\n\n";
  }
}

#include <stdlib.h>
#include <time.h>
Command playTurn(const WorldModel& wm) {
  Command command;
  while(true) {
    size_t x = rand()%3;
    size_t y = rand()%3;
    if (wm.table[x][y] == Marker::UNMARKED) {
      command.coordinate.x = x;
      command.coordinate.y = y;
      break;
    }
  }
  return command;
}

void synchronize(int32_t t) {
  auto sleeptime = std::chrono::high_resolution_clock::now() + 
    std::chrono::milliseconds(t);
  std::this_thread::sleep_until(sleeptime);
}

void playGame(GameClient& client) {
  srand(time(NULL));
  GameInfo gameInfo;
  client.ready(gameInfo);
  while (true) {
    synchronize(gameInfo.nextWorldModelTimeEstimateMs);
    client.gameInfo(gameInfo);
    const WorldModel& wm = gameInfo.worldModel;
    printWorldModel(wm);
    if (gameInfo.gameStatus == GameStatus::FINISHED) {
      break;
    }
    printf("%d\n", gameInfo.isMyTurn);
    if (gameInfo.isMyTurn) {
      Command command = playTurn(wm);
      client.update(command);
    }
  }

}

int main(int argc, char** argv) {
  gflags::SetVersionString(kVersion);
  gflags::SetUsageMessage(kUsageMessage);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  boost::shared_ptr<TTransport> socket(new TSocket("localhost", FLAGS_port));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  GameClient client(protocol);

  try {
    transport->open();
    mjollnir::vigridr::GameInfo gameInfo;
    playGame(client);
    transport->close();
  } catch (TException &tx) {
    printf("ERROR: %s\n", tx.what());
  }
}
