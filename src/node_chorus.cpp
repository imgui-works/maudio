#include "maudio/node_chorus.h"

using namespace MUtils;

namespace MAudio
{

Chorus::~Chorus()
{
    sp_delay_destroy(&m_pSPChorus);
}

Chorus::Chorus(const std::string& strName)
    : NodeAudioBase(strName)
{
    m_spOutData = std::make_shared<IAudioData>();

    m_pInput = AddInput("Input", (IFlowData*)nullptr);
    m_pOutput = AddOutput("Output", (IFlowData*)m_spOutData.get());

    // Create chorus effect
    sp_delay_create(&m_pSPChorus);
    sp_delay_init(m_pSP, m_pSPChorus, 0.01f);
    m_pSPChorus->feedback = 0.0f;
}

void Chorus::Compute()
{
    auto pFlowData = m_pInput->GetFlowData<IAudioData>();
    m_spOutData->SetNumSamples(pFlowData->GetNumSamples());

    for (uint32_t i = 0; i < pFlowData->GetNumSamples(); i++)
    {
        sp_delay_compute(m_pSP, m_pSPChorus, &pFlowData->GetSamples()[i], &m_spOutData->GetSamples()[i]);
    }
}

} // namespace MAudio
