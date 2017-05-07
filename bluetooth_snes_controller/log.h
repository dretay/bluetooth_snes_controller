#pragma once

//simple debug logging
#include "SEGGER_RTT.h"
#if DEBUG == 1
#define LOG(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#else
#define LOG(...)
#endif
