#include "maudio/node_adsr.h"
#include "maudio/maudio.h"

#include "mutils/animation/timer.h"

using namespace MUtils;

namespace MAudio
{

ADSR::~ADSR()
{
    sp_adsr_destroy(&m_pSPADSR);
}

ADSR::ADSR(const std::string& strName)
    : NodeAudioBase(strName)
{
    m_spOutData = std::make_shared<IAudioData>();

    m_pInput = AddInput("Input", (IFlowData*)nullptr);

    m_pGate = AddInput("Gate", true);

    m_pAttack = AddInput("Attack", .1f);
    m_pDecay = AddInput("Decay", .1f);
    m_pSustain = AddInput("Sustain", .5f);
    m_pRelease = AddInput("Release", .3f);

    m_pOutput = AddOutput("Output", (IFlowData*)m_spOutData.get());

    // Create adsr effect
    sp_adsr_create(&m_pSPADSR);
    sp_adsr_init(m_pSP, m_pSPADSR);
    //m_pSPADSR->atk_time = 48000 * 2;
    //m_pSPADSR->atk = .5f;


    // TODO: Would you ever time vary these signals?
    m_pSPADSR->atk = m_pAttack->GetValue<float>();
    m_pSPADSR->dec = m_pDecay->GetValue<float>();
    m_pSPADSR->sus = m_pSustain->GetValue<float>();
    m_pSPADSR->rel = m_pRelease->GetValue<float>();

}

void ADSR::Compute()
{
    float gate = m_pGate->GetValue<bool>() ? 1.0f : 0.0f;

    auto pFlowData = m_pInput->GetFlowData<IAudioData>();
    m_spOutData->SetNumSamples(pFlowData->GetNumSamples());

    for (uint32_t i = 0; i < pFlowData->GetNumSamples(); i++)
    {
        float adsr = 1.0f;
        sp_adsr_compute(m_pSP, m_pSPADSR, &gate, &adsr);
        m_spOutData->GetSamples()[i] = pFlowData->GetSamples()[i] * adsr;
    }
}

} // namespace MAudio
