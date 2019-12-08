#pragma once

#include <cassert>
#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <variant>
#include <vector>
#include <cmath>

#include "maudio/maudio_value.h"
#include "maudio/maudio_utils.h"
#include "maudio/msamples.h"

#include "nodegraph/model/graph.h"

#include "maudio/node_oscillator.h"
#include "maudio/node_adsr.h"

namespace MAudio
{

void maud_set_rate(uint32_t rate);
void maud_tick();
void maud_init();
void maud_destroy();

NodeGraph::Graph* maud_create_synth(const std::string& strName);
NodeAudioBase* maud_create_node(NodeGraph::Graph* pGraph, const std::string& strNodeType, const std::string& strNodeName);

} // namespace MAudio
