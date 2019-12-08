#pragma once

// Single header include
#ifdef MAUDIO_SINGLE_HEADER_BUILD
#include "../src/maudio.cpp"
#include "../src/mnode.cpp"
#include "../src/moscillators.cpp"
#include "../src/maudio_utils.cpp"
#include "../src/msamples.cpp"
#include "../m3rdparty/wavetable/src/wave_table_utils.cpp"
#include "../src/utils/file/file.cpp"
#include "../src/utils/logger.cpp"
#else
#include "maudio/maudio.h"
#include "maudio/mnode.h"
#include "maudio/moscillators.h"
#endif


