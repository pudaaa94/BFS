#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <deque>
#include <queue>
#include <unordered_map>
#include <chrono>
#include <Windows.h>

#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"
//#define THREADS_ENABLED
//#define DEBUG_ENABLED

/* printing can take up to 0.5 milliseconds, so it is useful to disable it */
#ifdef DEBUG_ENABLED
#define DEBUG_MSG(...) do { std::cout << __VA_ARGS__ << std::endl; } while (0)
#else
#define DEBUG_MSG(...)
#endif

using namespace std;