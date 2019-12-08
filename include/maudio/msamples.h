#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "mutils/profile/profile.h"

namespace MAudio
{

struct Sample
{
    unsigned int channels = 2;
    uint32_t sampleRate = 0;
    uint64_t totalPCMFrameCount = 0;

    float* pData = nullptr;
};

class SampleBank
{
public:
    static SampleBank& Instance();

    void AddSampleFolder(const std::string& strPath);
    Sample* GetSample(const std::string& sample);

private:
    void AddSample(const std::string&, std::shared_ptr<Sample> spSample);

private:
    std::map<std::string, std::shared_ptr<Sample>> m_samples;
    MUtilsLockable(std::mutex, sample_mutex);
};

} // namespace MAudio
