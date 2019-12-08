#include "maudio/node_oscillator.h"
#include "maudio/maudio.h"
#include "maudio/wavetable.h"

#include "mutils/profile//profile.h"

namespace MAudio
{

Oscillator::~Oscillator()
{
    sp_osc_destroy(&m_pSPOsc);
    sp_ftbl_destroy(&m_pSPTable);
}

Oscillator::Oscillator(const std::string& strName, WaveTableType t, float f, float p)
    : NodeAudioBase(strName)
    , phase(p)
{
    // Output Data flow
    spData = std::make_shared<IAudioData>();

    // Output pins
    m_pOutput = AddOutput("Out", spData.get());

    // Input Pins
    m_pFrequency = AddInput("Frequency", f);
    m_pAmplitude = AddInput("Amplitude", 1.0f);
    m_pPhase = AddInput("Phase", 0.0f);
    m_pType = AddInput("Wave", std::string(""));

    // The wave table to use
    WaveTable table;
    wave_table_create(table, t, p, 4096);

    // Create the wave table and the oscillator
    sp_ftbl_create(m_pSP, &m_pSPTable, table.data.size());
    sp_osc_create(&m_pSPOsc);

    // Setup the oscillator
    sp_osc_init(m_pSP, m_pSPOsc, m_pSPTable, 0.0f);

    // Create our wavetable for the oscillator
    for (size_t i = 0; i < table.data.size(); i++)
    {
        m_pSPTable->tbl[i] = table.data[i];
    }
}

void Oscillator::Compute()
{
    spData->SetNumSamples(maud.genFrameCount);
    for (uint32_t i = 0; i < maud.genFrameCount; i++)
    {
        if (i % 8 == 0)
        {
            m_pSPOsc->freq = m_pFrequency->GetValue<float>();
            m_pSPOsc->amp = m_pAmplitude->GetValue<float>();
            m_pSPOsc->iphs = m_pPhase->GetValue<float>();
        }
        sp_osc_compute(m_pSP, m_pSPOsc, nullptr, &spData->GetSamples()[i]);
    }
}

} // namespace MAudio
