#ifndef VIGRIDR_UTILS_LOG_H
#define VIGRIDR_UTILS_LOG_H

#include <exception>
#include <iostream>
#include <utility>  

#define CHECK(condition, ...) (condition ? true : mjollnir::vigridr::crash(\
                                          __FILE__,\
                                          __LINE__,\
                                          #condition,\
                                          __VA_ARGS__))

namespace mjollnir { namespace vigridr {

template <typename... Args>
bool crash(const char* file, const int line, 
           const char* condition, const char* message, Args... args) {
  std::cerr << "\033[1;31m[" << file << ":" << line << "] CHECK failed: " 
            << condition << "\033[0m" << std::endl;
  std::cerr << "Error message: ";
  fprintf(stderr, message, std::forward<Args>(args)..., 0);
  std::cerr << std::endl;
  std::terminate();
  return false;
}

}}

#endif  // VIGRIDR_UTILS_CHECK_H