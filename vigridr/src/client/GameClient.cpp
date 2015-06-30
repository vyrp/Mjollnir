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

#include "../thrifts/gen-cpp/Game.h"
#include "ClientLogic.h"

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
using ::mjollnir::vigridr::GameClient;
using ::mjollnir::vigridr::GameInfo;
using ::mjollnir::vigridr::GameInit;
using ::mjollnir::vigridr::GameStatus;
using ::mjollnir::vigridr::WorldModel;

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::high_resolution_clock;

//DEBUG
void printWorldModel(const WorldModel& wm) {
  
}

void synchronize(int32_t t) {
  auto sleeptime = high_resolution_clock::now() + milliseconds(t);
  std::this_thread::sleep_until(sleeptime);
}

void playGame(GameClient& client) {
  GameInit gameInit;
  client.ready(gameInit);
  init(gameInit);
  GameInfo gameInfo = gameInit.gameInfo;
  auto startTime = high_resolution_clock::now();
  while (true) {
    auto processingTimeMs = duration_cast<milliseconds>(
      high_resolution_clock::now() - startTime).count();
    synchronize(gameInfo.nextWorldModelTimeEstimateMs - processingTimeMs);
    client.getGameInfo(gameInfo);
    startTime = high_resolution_clock::now();
    const WorldModel& wm = gameInfo.worldModel;
    printWorldModel(wm);
    if (gameInfo.gameStatus == GameStatus::FINISHED) {
      break;
    }
    if (gameInfo.isMyTurn) {
      Command command = playTurn(wm);
      client.sendCommand(command);
    }
  }
}

int main(int argc, char** argv) {
  gflags::SetVersionString(kVersion);
  gflags::SetUsageMessage(kUsageMessage);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  try {
    boost::shared_ptr<TTransport> socket(new TSocket("localhost", FLAGS_port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    GameClient client(protocol);
    transport->open();  
    playGame(client);
    transport->close();
  } catch (TException &tx) {
    printf("ERROR: %s\n", tx.what());
  }
}
