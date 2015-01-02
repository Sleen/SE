#pragma once

#include "Macro.h"

SE_BEGIN

int64_t GetMicroSecond();

inline int64_t GetMilliSecond(){
	return GetMicroSecond() / 1000;
}

SE_END
