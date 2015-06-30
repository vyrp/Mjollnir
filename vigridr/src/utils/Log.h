#ifndef VIGRIDR_UTILS_LOG_H
#define VIGRIDR_UTILS_LOG_H

#include <cstdio>
#include <exception>
#include <iostream>
#include <utility>
#include <memory>
#include <mutex>

#define CHECK(condition, ...) \
  (condition ? true : mjollnir::vigridr::Logger::instance()->crash(\
    __FILE__,\
    __LINE__,\
    #condition,\
    __VA_ARGS__))

#define LOG(...) mjollnir::vigridr::Logger::instance()->log(\
  __FILE__,\
  __LINE__,\
  __VA_ARGS__)

namespace mjollnir { namespace vigridr {

class Logger {
 private:
  std::mutex stderrMutex;
  static std::mutex instanceMutex;
  //private constructor to make it singleton
  Logger() {}

 public:
  static std::shared_ptr<Logger> instance();

  template <typename... Args>
  bool crash(const char* file, const int line,
             const char* condition, const char* message, Args... args) {
    std::unique_lock<std::mutex> lock(stderrMutex);
    std::cerr << "\033[1;31m[" << file << ":" << line << "] CHECK failed: "
              << condition << "\033[0m" << std::endl;
    std::cerr << "Error message: ";
    fprintf(stderr, message, std::forward<Args>(args)..., 0);
    std::cerr << std::endl;
    std::terminate();
    return false;
  }

  template <typename... Args>
  bool log(const char* file, const int line, const char* message, Args... args) {
    std::unique_lock<std::mutex> lock(stderrMutex);
    std::cerr << "[" << file << ":" << line << "]: ";
    fprintf(stderr, message, std::forward<Args>(args)..., 0);
    std::cerr << std::endl;
    return false;
  }

};

}}

#endif  // VIGRIDR_UTILS_CHECK_H