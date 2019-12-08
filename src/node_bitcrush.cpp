#include "maudio/node_bitcrush.h"

using namespace MUtils;

namespace MAudio
{

BitCrush::~BitCrush()
{
    sp_bitcrush_destroy(&m_pSPBitCrush);
}

BitCrush::BitCrush(const std::string& strName)
    : NodeAudioBase(strName)
{
    m_spOutData = std::make_shared<IAudioData>();

    m_pInput = AddInput("Input", (IFlowData*)nullptr);
    m_pBitCount = AddInput("Bits", int64_t(8));
    m_pRate = AddInput("Rate", int64_t(44000));

    m_pOutput = AddOutput("Output", (IFlowData*)m_spOutData.get());

    // Create Bitcrush effect
    sp_bitcrush_create(&m_pSPBitCrush);
    sp_bitcrush_init(m_pSP, m_pSPBitCrush);
}

void BitCrush::Compute()
{
    auto pFlowData = m_pInput->GetFlowData<IAudioData>();
    m_spOutData->SetNumSamples(pFlowData->GetNumSamples());

    for (uint32_t i = 0; i < pFlowData->GetNumSamples(); i++)
    {
        if (i % 8 == 0)
        {
            m_pSPBitCrush->bitdepth = (float)m_pBitCount->GetValue<int64_t>();
            m_pSPBitCrush->srate = (float)m_pRate->GetValue<int64_t>();
        }
        sp_bitcrush_compute(m_pSP, m_pSPBitCrush, &pFlowData->GetSamples()[i], &m_spOutData->GetSamples()[i]);
    }
}

} // namespace MAudio
