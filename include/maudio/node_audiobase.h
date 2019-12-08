#pragma once

#include <cstdint>
#include <gsl/gsl.hpp>

#include "maudio/maudio_value.h"
#include "mutils/string/murmur_hash.h"
#include "nodegraph/model/graph.h"

extern "C" {
#include "soundpipe/h/soundpipe.h"
}

namespace MAudio
{

using namespace NodeGraph;

class IAudioData : public NodeGraph::IFlowData
{
public:
    gsl::span<float> GetSamples()
    {
        return gsl::span<float>(&m_samples[0], &m_samples[0] + numSamples);
    }
    void SetNumSamples(uint32_t num)
    {
        if (m_samples.size() <= num)
        {
            m_samples.resize(num);
        }
        numSamples = num;
    }
    uint32_t GetNumSamples() const
    {
        return numSamples;
    }
    virtual ~IAudioData() {}

private:
    uint32_t numSamples = 0;
    std::vector<float> m_samples;
};

class NodeAudioBase : public Node
{
public:
    NodeAudioBase(const std::string& strName);
    virtual ~NodeAudioBase();

protected:
    sp_data* m_pSP = nullptr;
};

} // namespace MAudio
