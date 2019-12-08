#pragma once

#include "mutils/file/file.h"
#include "maudio/node_audiobase.h"

namespace MAudio
{

class BitCrush : public NodeAudioBase
{
public:
    DECLARE_NODE(BitCrush);
    BitCrush(const std::string& strName);
    virtual ~BitCrush();

    virtual void Compute() override;

private:
    std::shared_ptr<IAudioData> m_spOutData;
    sp_bitcrush* m_pSPBitCrush = nullptr;
    Pin* m_pInput = nullptr;
    Pin* m_pBitCount = nullptr;
    Pin* m_pRate = nullptr;
    Pin* m_pOutput = nullptr;
};

} // namespace MAudio
