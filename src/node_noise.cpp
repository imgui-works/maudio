#include "maudio/node_noise.h"

using namespace MUtils;

namespace MAudio
{

Noise::~Noise()
{
    sp_noise_destroy(&m_pSPNoise);
}

Noise::Noise(const std::string& strName)
    : NodeAudioBase(strName)
{
    m_spOutData = std::make_shared<IAudioData>();
    m_pAmplitude = AddInput("Amplitude", 1.0f);
    m_pOutput = AddOutput("Output", (IFlowData*)m_spOutData.get());

    // Create Bitcrush effect
    sp_noise_create(&m_pSPNoise);
    sp_noise_init(m_pSP, m_pSPNoise);
}

void Noise::Compute()
{
    m_spOutData->SetNumSamples(maud.genFrameCount);

    for (uint32_t i = 0; i < maud.genFrameCount; i++)
    {
        if (i % 8 == 0)
        {
            m_pSPNoise->amp = m_pAmplitude->GetValue<float>();
        }
        sp_noise_compute(m_pSP, m_pSPNoise, nullptr, &m_spOutData->GetSamples()[i]);
    }
}

} // namespace MAudio
