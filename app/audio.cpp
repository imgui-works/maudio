#include <mutils/file/file.h>
#include <mutils/logger/logger.h>

#include "audio.h"

#define NOMINMAX
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#include "maudio/maudio.h"

#include "maudio/node_bitcrush.h"
#include "maudio/node_noise.h"
#include "maudio/node_file_output.h"
#include "maudio/node_output.h"
#include "maudio/node_mixer.h"
#include "maudio/node_chorus.h"

#include <cstdint>

using namespace MAudio;
using namespace MUtils;
using namespace NodeGraph;

ma_decoder decoder;
ma_device device;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
#ifdef INPUT
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL)
    {
        return;
    }
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);
#endif

    std::lock_guard<MUtilsLockableBase(std::mutex)> guard(maud.audio_mutex);
    auto samples = (float*)pOutput;

#ifdef INPUT
    if (inputBuffer != nullptr)
    {
        device->GetAnalysis().Update((float*)inputBuffer, nBufferFrames);
    }
#endif

    if (samples)
    {
        maud.genFrameCount = frameCount;

        try
        {
            std::vector<float*> dataFeeds;
            for (auto& spGraph : maud.graphs)
            {
                auto& graph = *spGraph.second;
                auto outputs = graph.Find<Node>({ ctti::type_id<FileOutput>(), ctti::type_id<AudioOut>() });
                graph.CalculateFlowPath(outputs);

                graph.Compute(maud.totalFrames);
                maud.totalFrames += frameCount;

                // TODO: Find a path through the graph, tick the data
                auto& outputNodes = graph.GetOutputs();
                auto audioOutputs = graph.Find<Node>(AudioOut::TypeID());
                // For each output node
                for (auto& out : audioOutputs)
                {
                    auto pTarget = samples;

                    // For each input to this output node
                    for (auto& signalPin : out->GetInputs())
                    {
                        auto pFlowData = signalPin->GetFlowData<IAudioData>();
                        if (pFlowData->GetNumSamples() == maud.genFrameCount)
                        {
                            dataFeeds.push_back(&pFlowData->GetSamples()[0]);
                        }
                    }
                }
            }

            // Mux all the output data into the samples.
            // TODO: Handle more channels, stereo output, etc.
            for (uint32_t index = 0; index < frameCount; index++)
            {
                float total = 0.0f;
                for (auto& val : dataFeeds)
                {
                    total += val[index];
                }

                // Divide by number of feeds?
                total /= dataFeeds.size();
                *samples++ = total;
            }
        }
        catch (std::exception& ex)
        {
            LOG(INFO) << "Audio Exception: " << ex.what();
        }
        catch (...)
        {
            LOG(INFO) << "Unexpected Exception: ";
        }
    }

    // Update to next time slice
    maud.time = maud.deltaTime * frameCount;

    (void)pInput;
}

void audio_destroy()
{
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    maud_destroy();
}

bool audio_init()
{
    //ma_result result;
    ma_device_config deviceConfig;

    /*
    if (argc < 2) {
        printf("No input file.\n");
        return -1;
    }
    */

    /*result = ma_decoder_init_file(argv[1], NULL, &decoder);
    if (result != MA_SUCCESS) {
        return -2;
    }*/

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format::ma_format_f32; // decoder.outputFormat;
    deviceConfig.playback.channels = 1; //decoder.outputChannels;
    deviceConfig.sampleRate = MA_DEFAULT_SAMPLE_RATE;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = nullptr; // &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS)
    {
        //printf("Failed to open playback device.\n");
        ma_decoder_uninit(&decoder);
        return false;
    }

    if (ma_device_start(&device) != MA_SUCCESS)
    {
        //printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return false;
    }

    maud_init();
    maud_set_rate(device.sampleRate);

    return true;
}

void audio_update()
{
    /*
    if (!maud.note.empty())
    {
        audio_play(maud.note);
        maud.note.clear();
    }
    */
}

void audio_play(const std::string& note)
{
    //for (auto& ch : channels)
    {
        //ch->pInstrument->Play(note_string_to_frequency(note), 1.0f);
    }
}
