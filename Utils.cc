#include "Utils.h"

#include <algorithm>
#include <exception>
#include <sstream>
#include <string>


namespace utils {

bool str_2_uint(const std::string& str, unsigned& result) {
  std::stringstream ss(str);
  ss >> result;
  return ss.fail() == false && ss.bad() == false;
}

void ltrim(std::string& str) {
  str.erase(std::begin(str), std::find_if(std::begin(str),
                                          std::end(str),
                                          [] (char c) -> bool { return std::isspace(c) == false; }));
}

unsigned getMemoryUsageOfCurrentProcess() {
  unsigned memory_consumption = 0u;
  FILE* desc = fopen("/proc/self/status", "r");
  if (desc == nullptr) {
    throw std::runtime_error("Can't open /proc/self/status");
  }
  char buff[128];
  while (fgets(buff, 128, desc) != nullptr) {
    std::string line(buff);
    auto pos = line.find("VmSize:");
    if (pos != 0) {
      continue;
    }
    std::string sub = line.substr(7);  // 7 == strlen("VmSize:")
    utils::ltrim(sub);
    pos = sub.find(" kB");
    if (pos == std::string::npos) {
      throw std::runtime_error("Error during parsing /proc/self/status");
    }
    std::string memory_consumption_str = sub.substr(0, pos);
    if (utils::str_2_uint(memory_consumption_str, memory_consumption) == false) {
      throw std::runtime_error("Error during reading memory consumption from /proc/self/status");
    }
  }
  if (memory_consumption == 0) {
    throw std::runtime_error("No information about VmSize in /proc/self/status");
  }
  return memory_consumption;
}

}  // namespace utils
