#pragma once

#include <cmath>
#include <cstdint>

#include "maudio/maudio_value.h"
#include "maudio/wavetable.h"
#include "maudio/node_audiobase.h"


namespace MAudio
{

class Oscillator : public NodeAudioBase
{
public:
    DECLARE_NODE(Oscillator);

    Oscillator(const std::string& strName, WaveTableType type, float frequency = 440.0f, float phase = 0.0f);
    virtual ~Oscillator();

    virtual void Compute() override;

    NodeGraph::Pin* GetFrequency() const { return m_pFrequency; }

protected:
    // Inputs
    NodeGraph::Pin* m_pFrequency;
    NodeGraph::Pin* m_pAmplitude;
    NodeGraph::Pin* m_pPhase;
    NodeGraph::Pin* m_pType;

    // Outputs
    NodeGraph::Pin* m_pOutput;

    float phase = 0.0;
    std::shared_ptr<IAudioData> spData;
    sp_osc* m_pSPOsc = nullptr;
    sp_ftbl* m_pSPTable = nullptr;
};

} // namespace MAudio
