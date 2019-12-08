#pragma once

#include "maudio/msamples.h"

extern "C"
{
#include <soundpipe/lib/dr_wav/dr_wav.h>
}

#include "mutils/logger/logger.h"
#include "mutils/file/file.h"

using namespace MUtils;

namespace MAudio
{

SampleBank& SampleBank::Instance()
{
    static SampleBank inst;
    return inst;
}

void SampleBank::AddSampleFolder(const std::string& strPath)
{
    fs::path path(strPath);
    auto files = MUtils::file_gather_files(path);
    std::sort(files.begin(), files.end(), [](const fs::path& lhs, const fs::path& rhs) {
        return lhs.filename().string() < rhs.filename().string();
    });

    uint32_t count = 0;
    for (auto& file : files)
    {
        if (file.extension() == ".wav")
        {
            unsigned int channels;
            unsigned int sampleRate;
            drwav_uint64 totalPCMFrameCount;
            float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(file.string().c_str(), &channels, &sampleRate, &totalPCMFrameCount);
            if (pSampleData)
            {
                auto pSample = std::make_shared<Sample>();
                pSample->channels = channels;
                pSample->sampleRate = sampleRate;
                pSample->totalPCMFrameCount = totalPCMFrameCount;
                pSample->pData = pSampleData;

                auto name = file.parent_path().stem().string() + ":" + std::to_string(count);

                AddSample(name, pSample);
                count++;

                LOG(INFO) << "Added Sample: " << name << ", from source: " << file.string();
            }
        }
    }
}

void SampleBank::AddSample(const std::string& strName, std::shared_ptr<Sample> spSample)
{
    std::lock_guard<MUtilsLockableBase(std::mutex)> guard(Instance().sample_mutex);

    m_samples[strName] = spSample;
}

Sample* SampleBank::GetSample(const std::string& sample)
{
    std::lock_guard<MUtilsLockableBase(std::mutex)> guard(Instance().sample_mutex);
    auto itr = m_samples.find(sample);
    if (itr != m_samples.end())
    {
        return itr->second.get();
    }
    return nullptr;
}

} // namespace Jorvik
