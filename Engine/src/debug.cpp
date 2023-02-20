#include "debug.hpp"

void LogCall(const char* tag, std::string log)
{
    std::cout << "[Log call] " << tag << ": " << log << std::endl;
}

void ErrorLog(std::string str, const char* file, int line)
{
    std::cout << "[Error] (" << str << ")" << file << ":" << line << std::endl;
}

bool LogAssert(const char* function, const char* file, int line)
{
    std::cout << "[Assert triggered] " << function <<
        " " << file << ":" << line << std::endl;
    return false;
}
