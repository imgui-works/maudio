#pragma once

#include "mutils/file/file.h"
#include "maudio/node_audiobase.h"

namespace MAudio
{

class ADSR : public NodeAudioBase
{
public:
    DECLARE_NODE(ADSR);
    ADSR(const std::string& strName);
    virtual ~ADSR();
    virtual void Compute() override;

private:
    std::shared_ptr<IAudioData> m_spOutData;
    sp_adsr* m_pSPADSR = nullptr;
    Pin* m_pInput = nullptr;
    Pin* m_pOutput = nullptr;
    
    Pin* m_pGate = nullptr;
    Pin* m_pAttack = nullptr;
    Pin* m_pDecay = nullptr;
    Pin* m_pSustain = nullptr;
    Pin* m_pRelease = nullptr;
};

} // namespace MAudio
