#include "mutils/string/murmur_hash.h"
#include "maudio/node_audiobase.h"

namespace MAudio
{

NodeAudioBase::~NodeAudioBase()
{
    sp_destroy(&m_pSP);
}

// TODO: Validate unique hash
NodeAudioBase::NodeAudioBase(const std::string& strName)
    : Node(strName)
{
    sp_create(&m_pSP);
    m_pSP->nchan = 1;
    m_pSP->sr = maud.sampleRate;
}

} // namespace MAudio
