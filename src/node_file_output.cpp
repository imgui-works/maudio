#include "maudio/node_file_output.h"

#include "mutils/file/file.h"

using namespace MUtils;
namespace MAudio
{

FileOutput::~FileOutput()
{
    sp_wavout_destroy(&m_pSPOut);
}

FileOutput::FileOutput(const std::string& strName, const fs::path& filePath)
    : NodeAudioBase(strName)
{
    m_pInput = AddInput("Input", (IFlowData*)nullptr);

    // Write to a file
    sp_wavout_create(&m_pSPOut);

    fs::create_directories(filePath.parent_path());

    sp_wavout_init(m_pSP, m_pSPOut, filePath.string().c_str());
}

void FileOutput::Compute()
{
    auto pFlowData = m_pInput->GetFlowData<IAudioData>();
    for (uint32_t i = 0; i < pFlowData->GetNumSamples(); i++)
    {
        float out;
        sp_wavout_compute(m_pSP, m_pSPOut, &pFlowData->GetSamples()[i], &out);
    }
}

} // namespace MAudio
