#include "Log.h"

namespace mjollnir { namespace vigridr {

std::shared_ptr<Logger> Logger::instance() {
  std::unique_lock<std::mutex> lock(instanceMutex);
  static auto logger = std::shared_ptr<Logger>(new Logger());
  return logger;
}

std::mutex Logger::instanceMutex;

}}