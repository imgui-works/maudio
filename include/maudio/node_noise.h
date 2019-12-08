#pragma once

#include "mutils/file/file.h"
#include "maudio/node_audiobase.h"

namespace MAudio
{

class Noise : public NodeAudioBase
{
public:
    DECLARE_NODE(Noise);
    Noise(const std::string& strName);
    virtual ~Noise();

    virtual void Compute() override;
private:
    std::shared_ptr<IAudioData> m_spOutData;
    sp_noise* m_pSPNoise = nullptr;
    Pin* m_pInput = nullptr;
    Pin* m_pAmplitude = nullptr;
    Pin* m_pOutput = nullptr;
};

} // namespace MAudio
