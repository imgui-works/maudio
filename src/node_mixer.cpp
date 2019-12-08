#include "maudio/node_mixer.h"

namespace MAudio
{

Mixer::~Mixer()
{
}

Mixer::Mixer(const std::string& strName)
    : NodeAudioBase(strName)
{
    m_spOutData = std::make_shared<IAudioData>();
    m_pOutput = AddOutput("Output", (IFlowData*)m_spOutData.get());

    m_pInputA = AddInput("Input A", (IFlowData*)nullptr);
    m_pInputB = AddInput("Input B", (IFlowData*)nullptr);
    m_pMix = AddInput("Mix", 0.5f);
}

void Mixer::Compute()
{
    auto pFlowDataA = m_pInputA->GetFlowData<IAudioData>();
    auto pFlowDataB = m_pInputB->GetFlowData<IAudioData>();

    m_spOutData->SetNumSamples(pFlowDataA->GetNumSamples());

    float weight = 0.5f;
    for (uint32_t i = 0; i < pFlowDataA->GetNumSamples(); i++)
    {
        if (i % 8 == 0)
        {
            weight = m_pMix->GetValue<float>();
        }
        m_spOutData->GetSamples()[i] = (pFlowDataA->GetSamples()[i] * (1.0f - weight)) + (pFlowDataB->GetSamples()[i] * weight);
    }
}

} // namespace MAudio
