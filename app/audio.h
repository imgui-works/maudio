#pragma once

#include <string>
#include "nodegraph/model/graph.h"

bool audio_init();
void audio_update();
void audio_destroy();
void audio_play(const std::string& strNote);
void audio_add_graph(std::shared_ptr<NodeGraph::Graph> spGraph);
