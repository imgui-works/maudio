#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <map>
#include <mutex>
#include <variant>

#include "mutils/profile/profile.h"
#include "maudio/maudio_utils.h"
#include "nodegraph/model/graph.h"

#if _WIN32
#pragma warning(once : 4061)
#endif

namespace MAudio
{

struct MAudioState
{
    std::map<std::string, std::shared_ptr<NodeGraph::Graph>> graphs;

    // Sample rate
    uint32_t sampleRate = 44000;
    double time = 0.0f;
    double deltaTime = 1.0f / (double)sampleRate;
    double bpm = 120.0;

    uint32_t genFrameCount = 0;
    int64_t totalFrames = 0;
    MUtilsLockable(std::mutex, audio_mutex);
};

extern MAudioState maud;

enum class Drum
{
    HighHat = 0, // '-'
    Kick = 1, // 'x'
    Snare = 2, // 'o'
    None = 3 //  ~
    // TODO: More drums
};


//using PinValue = std::variant<float, double, int64_t, bool, std::string>;
/*struct PinValue
{
    PinValue(ValueType t = ValueType::Number)
        : type(t)
        , val(0.0f)
    {
    }

    PinValue(ValueType t, float v)
        : type(t)
        , val(v)
    {
    }

    PinValue(const PinValue& rhs)
        : type(rhs.type)
        , val(rhs.val)
    {
    }

    PinValue(Drum d)
        : type(ValueType::Drum)
        , val((float)d)
    {
    }

    PinValue& operator=(const PinValue& rhs)
    {
        type = rhs.type;
        val = rhs.val;
        return *this;
    }

    // Allow conversion to raw double
    operator float() const
    {
        return val;
    }

    float Get() const
    {
        return val;
    }

    /*
    Drum AsDrum() const
    {
        if (ValueType::Drum == type)
        {
            return (Drum)((int)val);
        }
        return Drum::None;
    }

    /*
    PinValue AsFrequency() const
    {
        switch (type)
        {
            // Straight copies
        default:
        case ValueType::Frequency:
        case ValueType::Number:
            return *this;
        case ValueType::Time:
            return PinValue(ValueType::Frequency, 1.0f / val);
        case ValueType::Decibel:
        case ValueType::Semitone:
        case ValueType::Drum:
            assert(!"Invalid");
            return PinValue(ValueType::Frequency, 0.0f);
            break;
        case ValueType::Midi:
            return PinValue(ValueType::Frequency, midi_to_frequency(val));
            break;
        }
    }

    // Data
    //float val;
   // ValueType type;
};
*/

/*
inline float audiovalue_as_time(PinValue& v)
{
    switch (v.type)
    {
        // Straight copies
    default:
    case ValueType::Number:
    case ValueType::Time:
        return v.val;
    case ValueType::Frequency:
        return 1.0f / v.val;
    case ValueType::Decibel:
        assert(!"Invalid");
        return 0.0f;
        break;
    case ValueType::Drum:
    case ValueType::Semitone:
    case ValueType::Midi:
        assert(!"Invalid");
        return 0.0f;
        break;
    }
}
*/

inline float midi_from_string(const char* pNote)
{
    return note_string_to_midi(pNote);
}

inline float frequency_from_midi(const char* pNote)
{
    return midi_to_frequency(note_string_to_midi(pNote));
}

} // namespace MAudio
