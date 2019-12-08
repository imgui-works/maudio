#include "maudio/maudio.h"
#include "maudio/maudio_utils.h"

#include "oscpack/osc/OscPacketListener.h"
#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/ip/UdpSocket.h"

#include "mutils/logger/logger.h"
#include "mutils/string/stringutils.h"

#include <cstdio>
#include <iostream>

using namespace MUtils;

namespace MAudio
{

MAudioState maud;

NodeGraph::Graph* maud_create_synth(const std::string& name)
{
    std::lock_guard<MUtilsLockableBase(std::mutex)> guard(maud.audio_mutex);
    auto spGraph = std::make_shared<NodeGraph::Graph>();
    maud.graphs[name] = spGraph;
    return spGraph.get();
}

NodeAudioBase* maud_create_node(NodeGraph::Graph* graph, const std::string& nodeType, const std::string& nodeName)
{
    std::lock_guard<MUtilsLockableBase(std::mutex)> guard(maud.audio_mutex);
    if (!graph)
    {
        return nullptr;
    }

    if (string_tolower(nodeType) == "osc")
    {
        auto pNode = graph->CreateNode<Oscillator>(nodeName, WaveTableType::Sine);
        return pNode;
    }
    return nullptr;
}

void maud_init()
{
    mutils_init();
}

void maud_destroy()
{
    for (auto& spGraph : maud.graphs)
    {
        spGraph.second->Destroy();
    }
    maud.graphs.clear();
}

void maud_set_rate(uint32_t rate)
{
    maud.sampleRate = rate;
    maud.deltaTime = 1.0f / float(rate);
}

} // namespace MAudio
