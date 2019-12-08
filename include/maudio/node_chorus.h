#pragma once

#include "mutils/file/file.h"
#include "maudio/node_audiobase.h"

namespace MAudio
{

class Chorus : public NodeAudioBase
{
public:
    DECLARE_NODE(Chorus);
    Chorus(const std::string& strName);
    virtual ~Chorus();

    virtual void Compute() override;

private:
    std::shared_ptr<IAudioData> m_spOutData;
    sp_delay* m_pSPChorus = nullptr;
    Pin* m_pInput = nullptr;
    Pin* m_pOutput = nullptr;
};

} // namespace MAudio
