#include <mutils/ui/sdl_imgui_starter.h>

#include "audio.h"
#include "config_app.h"
#include "maudio/maudio.h"
#include "maudio/scheme/scheme.h"

#include "nodegraph/model/graph.h"
#include "nodegraph/view/graphview.h"
#include "zepeditor.h"
#include "SDL.h"

using namespace MUtils;
using namespace NodeGraph;
using namespace Zep;

ZepRepl repl;
std::shared_ptr<ZepWrapper> spZep;

void zep_init()
{
    // Initialize the editor and watch for changes
    spZep = std::make_shared<ZepWrapper>(MAUDIO_ROOT, [](std::shared_ptr<ZepMessage> spMessage) -> void {
        if (spMessage->messageId == Zep::Msg::Buffer)
        {
            auto spBufferMsg = std::static_pointer_cast<BufferMessage>(spMessage);
            if (spBufferMsg->type == BufferMessageType::TextAdded || spBufferMsg->type == BufferMessageType::TextDeleted || spBufferMsg->type == BufferMessageType::TextChanged)
            {
                auto path = spBufferMsg->pBuffer->GetFilePath();
            }
        }
        else if (spMessage->messageId == Msg::HandleCommand)
        {
            if (spMessage->str == ":repl")
            {
                spZep->GetEditor().GetActiveTabWindow()->GetActiveWindow()->GetBuffer().SetReplProvider(&repl);
                spZep->GetEditor().AddRepl();
                spMessage->handled = true;
                return;
            }
        }
    });
    spZep->GetEditor().AddTabWindow()->AddWindow(spZep->GetEditor().GetEmptyBuffer("Scratch.lsp"), nullptr, false);
    
    // Provide a parser
    repl.fnParser = [&](const std::string& str) -> std::string {
        auto ret = chibi_repl(scheme_get_chibi(), NULL, str);

        // Trim off excess carriage return or spaces
        ret = MUtils::string_right_trim(ret);
        return ret;
    };
    
    repl.fnIsFormComplete = [&](const std::string& str, int& indent)
    {
        int count = 0;
        for (auto& ch : str)
        {
            if (ch == '(')
                count++;
            if (ch == ')')
                count--;
        }

        if (count < 0)
        {
            indent = -1;
            return false;
        }
        else if (count == 0)
        {
            return true;
        }

        int count2 = 0;
        indent = 1;
        for (auto& ch : str)
        {
            if (ch == '(')
                count2++;
            if (ch == ')')
                count2--;
            if (count2 == count)
            {
                break;
            }
            indent++;
        }
        return false;
    };
}

void zep_show(const glm::ivec2& displaySize)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13f, 0.1f, 0.12f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));

    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

    uint32_t flags = ImGuiCond_FirstUseEver;

    ImGui::SetNextWindowSize(ImVec2(displaySize.x * .5f, displaySize.y * .75f), flags);

    bool show = true;
    if (!ImGui::Begin("Zep", &show, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar))
    {
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);
        ImGui::End();
        return;
    }

    // Simple menu options for switching mode and splitting
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Timings"))
        {
            for (uint32_t index = 0; index < MUtils::globalProfiler.timerData.size(); index++)
            {
                auto& p = MUtils::globalProfiler.timerData[index];
                auto& name = MUtils::globalProfiler.IdToName[index];

                //fmt::format_to(jorvik.format_buffer, "{} : {:.2f} ms\n", name, p.average / 1000.0);
            }

            //jorvik.format_buffer.push_back(0);
            //ImGui::MenuItem(jorvik.format_buffer.data());
            //jorvik.format_buffer.clear();

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    auto min = ImGui::GetCursorScreenPos();
    auto max = ImGui::GetContentRegionAvail();
    if (max.x <= 0)
        max.x = 1;
    if (max.y <= 0)
        max.y = 1;
    ImGui::InvisibleButton("ZepContainer", max);

    // Fill the window
    max.x = min.x + max.x;
    max.y = min.y + max.y;

    spZep->zepEditor.SetDisplayRegion(NVec2f(min.x, min.y), NVec2f(max.x, max.y));
    spZep->zepEditor.Display();
    bool zep_focused = ImGui::IsWindowFocused();
    if (zep_focused)
    {
        spZep->zepEditor.HandleInput();
    }

    // Make the zep window focused on start of the demo - just so the user doesn't start typing without it;
    // not sure why I need to do it twice; something else is stealing the focus the first time round
    static int focus_count = 0;
    if (focus_count++ < 2)
    {
        ImGui::SetWindowFocus();
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

class App : public IAppStarterClient
{
public:
    App()
        //: graphView(graph)
    {
        settings.flags |= AppStarterFlags::DockingEnable;
        settings.startSize = glm::ivec2(1680, 1000);
    }

    // Inherited via IAppStarterClient
    virtual fs::path GetRootPath() const override
    {
        return fs::path(MAUDIO_ROOT);
    }

    virtual void Init() override
    {
        audio_init();
        scheme_init(SDL_GetBasePath());
        zep_init();
        //graphView.BuildNodes();
    }

    virtual void Update(float time, const glm::ivec2& displaySize) override
    {
        audio_update();
    }

    virtual void Destroy() override
    {
        audio_destroy();
    }

    virtual void Draw(const glm::ivec2& displaySize) override
    {
        /* Missing */
    }

    virtual void DrawGUI(const glm::ivec2& displaySize) override
    {
        //graphView.Show();
        zep_show(displaySize);
    }

    virtual AppStarterSettings& GetSettings() override
    {
        return settings;
    }

private:
    //NodeGraph::GraphView graphView;
    AppStarterSettings settings;
};

App theApp;

// Main code
int main(int args, char** ppArgs)
{
    return sdl_imgui_start(args, ppArgs, &theApp);
}
