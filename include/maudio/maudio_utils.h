#pragma once	

#include <cmath>
#include <cstdint>
#include <string>
#include <memory>

namespace MAudio
{

static const float A4 = 440.0f;

void mutils_init();

inline float decibels_to_gain(float db)
{
    return std::powf(10.0f, db / 20.0f);
}

inline float gain_to_decibels(float gain)
{
    return 20.0f * std::logf(gain) / std::logf(10.0f);
}

inline float amp_to_decibels(float gain)
{
    return 20.0f * std::log10(gain);
}

inline float interval_to_frequency_ratio(float interval)
{
    return std::powf(2.0f, (interval / 12.0f));
}
    
inline float midi_to_frequency(float midi)
{
    return A4 * std::powf(2.0f, ((midi - 69.0f) / 12.0f));
}

float time_string_to_time(const std::string& str);
float note_string_to_midi(const std::string& str);
    
inline float note_string_to_frequency(const std::string& str)
{
    return midi_to_frequency(note_string_to_midi(str));
}

} // MAudio
