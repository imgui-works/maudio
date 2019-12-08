#pragma once

#include "mutils/chibi/chibi.h"

// TOOD: Fix 
#undef min
#undef max

void scheme_init(const std::string& basePath);
MUtils::Chibi& scheme_get_chibi();
