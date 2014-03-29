#include <memory>
#include <functional>
#include <thread>

#include <gflags/gflags.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "GameManager.h"
#include "GameService.h"
#include "gen-cpp/Game.h"

DEFINE_int32(port1, 9090, "Port used by the first client.");
DEFINE_int32(port2, 9091, "Port used by the second client.");

using ::apache::thrift::protocol::TBinaryProtocolFactory;
using ::apache::thrift::protocol::TProtocolFactory;
using ::apache::thrift::server::TProcessor;
using ::apache::thrift::server::TSimpleServer;
using ::apache::thrift::transport::TBufferedTransportFactory;
using ::apache::thrift::transport::TServerSocket;
using ::apache::thrift::transport::TServerTransport;
using ::apache::thrift::transport::TTransportFactory;

using namespace mjollnir::vigridr;

const char* const kVersion = "v1.1";
const char* const kUsageMessage = 
  "This program is the server for mjollnir matches";

int main(int argc, char **argv) {
  gflags::SetVersionString(kVersion);
  gflags::SetUsageMessage(kUsageMessage);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  auto gameManager = std::make_shared<GameManager>(FLAGS_port1, FLAGS_port2);
  auto service = [&](int32_t port) {  
    boost::shared_ptr<GameService> handler(new GameService(gameManager, port));
    boost::shared_ptr<TProcessor> processor(new GameProcessor(handler));
    boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    boost::shared_ptr<TTransportFactory> transportFactory(
      new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory> protocolFactory(
      new TBinaryProtocolFactory());

    TSimpleServer server(processor, serverTransport, 
                         transportFactory, protocolFactory);
    printf("Starting server...\n");
    server.serve();
    printf("Done");  
  };
  std::thread a(service, FLAGS_port1);
  std::thread b(service, FLAGS_port2);
  a.join();
  b.join();
  
  return 0;
}

