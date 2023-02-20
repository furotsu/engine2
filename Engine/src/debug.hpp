#pragma once

#include <string>
#include <iostream>

void LogCall(const char* tag, std::string log);
void ErrorLog(std::string str, const char* file, int line);
bool LogAssert(const char* funciton, const char* file, int line);
#define ALWAYS_ASSERT(expr) \
	do \
	{ \
		if (!(expr)) \
		{ \
			LogAssert(#expr, __FILE__, __LINE__); \
		} \
	} \
	while (0) \

#ifndef NDEBUG

#define BREAK __debugbreak();

#define DEV_ASSERT(expression, ...) ALWAYS_ASSERT(expression, __VA_ARGS__);

#define LOG(tag, str) LogCall(tag, str) 


#define ASSERT(expr) \
	do \
	{ \
		if (!(expr)) \
		{ \
			LogAssert(#expr, __FILE__, __LINE__); \
		} \
	} \
	while (0) \

#else  // NDEBUG is defined
	// These are the release mode definitions for the macros above.  They are all defined in such a way as to be 
	// ignored completely by the compiler.
#define DEV_ASSERT(...)
#define BREAK 
#define LOG(tag, str)  
#define ASSERT(expr) 
#endif  // !defined NDEBUG

