#pragma once

#include "maudio/node_audiobase.h"

namespace MAudio
{

class AudioOut : public NodeAudioBase
{
public:
    DECLARE_NODE(AudioOut);

    AudioOut(const std::string& strName)
        : NodeAudioBase(strName)
    {
        AddInput("Input", (IFlowData*)nullptr);
    }
};

} // namespace MAudio
