#pragma once

#include "maudio/node_audiobase.h"

namespace MAudio
{

class Mixer : public NodeAudioBase
{
public:
    DECLARE_NODE(Mixer);
    Mixer(const std::string& strName);
    virtual ~Mixer();

    virtual void Compute() override;
private:
    std::shared_ptr<IAudioData> m_spOutData;

    Pin* m_pMix = nullptr;
    Pin* m_pInputA = nullptr;
    Pin* m_pInputB = nullptr;
    Pin* m_pOutput = nullptr;
};

} // namespace MAudio
