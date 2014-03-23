#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "gen-cpp/Game.h"

using ::apache::thrift::protocol::TBinaryProtocolFactory;
using ::apache::thrift::protocol::TProtocolFactory;
using ::apache::thrift::server::TProcessor;
using ::apache::thrift::server::TSimpleServer;
using ::apache::thrift::transport::TBufferedTransportFactory;
using ::apache::thrift::transport::TServerSocket;
using ::apache::thrift::transport::TServerTransport;
using ::apache::thrift::transport::TTransportFactory;

namespace mjollnir { namespace vigridr {

class GameHandler : virtual public GameIf {
 public:
  GameHandler() {
    // Your initialization goes here
  }

  void gameInfo( ::mjollnir::vigridr::GameInfo& _return) {
    printf("gameInfo\n");
  }

};

}}  // namespaces

using namespace mjollnir::vigridr;

int main(int argc, char **argv) {
  int port = 9090;
  boost::shared_ptr<GameHandler> handler(new GameHandler());
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
  return 0;
}

