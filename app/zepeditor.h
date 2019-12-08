#pragma once

#include <mutils/file/file.h>

#include <zep/editor.h>
#include <zep/buffer.h>
#include <zep/tab_window.h>
#include <zep/window.h>
#include <zep/mode.h>
#include <zep/mode_standard.h>
#include <zep/mode_vim.h>
#include <zep/mode_repl.h>
#include <zep/imgui/editor_imgui.h>

#include <functional>

#include <mutils/callback/callback.h>
#include <mutils/chibi/chibi.h>
#include <mutils/string/stringutils.h>

struct ZepWrapper : public Zep::IZepComponent
{
    ZepWrapper(const fs::path& root_path, std::function<void(std::shared_ptr<Zep::ZepMessage>)> fnCommandCB)
        : zepEditor(Zep::ZepPath(root_path.string()))
        , Callback(fnCommandCB)
    {
        zepEditor.RegisterCallback(this);
    }

    virtual Zep::ZepEditor& GetEditor() const override
    {
        return (Zep::ZepEditor&)zepEditor;
    }

    virtual void Notify(std::shared_ptr<Zep::ZepMessage> message) override
    {
        Callback(message);
        
        return;
    }

    virtual void HandleInput()
    {
        zepEditor.HandleInput();
    }

    Zep::ZepEditor_ImGui zepEditor;
    std::function<void(std::shared_ptr<Zep::ZepMessage>)> Callback;
};

