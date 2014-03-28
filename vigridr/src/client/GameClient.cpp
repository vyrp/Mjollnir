#include <exception>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <unistd.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../server/gen-cpp/Game.h"

using ::apache::thrift::protocol::TBinaryProtocol;
using ::apache::thrift::protocol::TProtocol;
using ::apache::thrift::TException;
using ::apache::thrift::transport::TBufferedTransport;
using ::apache::thrift::transport::TSocket;
using ::apache::thrift::transport::TTransport;

using ::mjollnir::vigridr::Command;
using ::mjollnir::vigridr::Coordinate;
using ::mjollnir::vigridr::GameClient;
using ::mjollnir::vigridr::GameInfo;
using ::mjollnir::vigridr::WorldModel;

using ::mjollnir::vigridr::Marker;

void playGame(GameClient& client) {
  int x, y;
  while (true) {
    GameInfo gameInfo;
    client.gameInfo(gameInfo);
    WorldModel wm = gameInfo.worldModel;
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
    auto status = scanf("%d %d", &x, &y);
    std::cout << std::endl;
    if(x > 2 || x < 0 || y > 2 || y < 0 || status != 2) { continue; } 
    Command command;
    command.coordinate.x = x;
    command.coordinate.y = y;
    client.update(command);
  }
}

int main(int argc, char** argv) {
  if (argc <= 1) {
    std::cout << "No port specified." << std::endl;
    return 0;
  }
  int32_t port;
  try {
    port = std::stoi(argv[1]);
  } catch (const std::exception& e) {
    std::cout << "Invalid port number." << std::endl;
    return 0;
  }

  boost::shared_ptr<TTransport> socket(new TSocket("localhost", port));
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
