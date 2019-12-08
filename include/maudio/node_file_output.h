#pragma once

#include "mutils/file/file.h"
#include "maudio/node_audiobase.h"

namespace MAudio
{

class FileOutput : public NodeAudioBase
{
public:
    DECLARE_NODE(FileOutput);
    FileOutput(const std::string& strName, const fs::path& path);
    virtual ~FileOutput();

    virtual void Compute() override;
private:
    std::vector<float> m_data;
    sp_wavout* m_pSPOut = nullptr;
    Pin* m_pInput = nullptr;
};

} // namespace MAudio
