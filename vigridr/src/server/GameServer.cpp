#include <thread>
#include <functional>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "GameService.h"
#include "gen-cpp/Game.h"

using ::apache::thrift::protocol::TBinaryProtocolFactory;
using ::apache::thrift::protocol::TProtocolFactory;
using ::apache::thrift::server::TProcessor;
using ::apache::thrift::server::TSimpleServer;
using ::apache::thrift::transport::TBufferedTransportFactory;
using ::apache::thrift::transport::TServerSocket;
using ::apache::thrift::transport::TServerTransport;
using ::apache::thrift::transport::TTransportFactory;

using namespace mjollnir::vigridr;

int main(int argc, char **argv) {
  if (argc <= 2) {
    std::cout << "No port specified." << std::endl;
    return 0;
  }
  int32_t port1, port2;
  try {
    port1 = std::stoi(argv[1]);
    port2 = std::stoi(argv[2]);
  } catch (const std::exception& e) {
    std::cout << "Invalid port number." << std::endl;
    return 0;
  }
  GameLogic gameLogic;
  auto service = [&](int32_t port) {  
    boost::shared_ptr<GameService> handler(new GameService(gameLogic, port));
    boost::shared_ptr<TProcessor> processor(new GameProcessor(handler));
    boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    boost::shared_ptr<TTransportFactory> transportFactory(
      new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory> protocolFactory(
      new TBinaryProtocolFactory());

    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    printf("Starting server...\n");
    server.serve();
    printf("Done");  
  };
  std::thread a(std::bind(service, port1));
  std::thread b(std::bind(service, port2));
  a.join();
  b.join();
  
  return 0;
}

