#include "WinManager.h"
#include "GameManager.h"

void ConsoleWin::render()
{
    if (!show)
        return;
    ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
    ImGui::Begin(windowTitle.c_str(), &show, flags);
    ImGui::BeginChild("ConsoleText",
                      ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
                      ImGuiChildFlags_None,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_HorizontalScrollbar);

    for (auto &str : history)
    {
        ImGui::TextUnformatted(str.c_str());
    }

    if (needAutoScroll && isAutoScroll)
    {
        ImGui::SetScrollY(ImGui::GetScrollMaxY() + 10);
        needAutoScroll = false;
    }

    ImGui::SetItemDefaultFocus();
    if (focus)
    {
        ImGui::SetKeyboardFocusHere();
        focus = false;
    }
    executeCmd();
    ImGui::EndChild();
    ImGui::End();
}

bool ConsoleWin::executeCmd()
{
    if (ImGui::InputText("Input", &inputbuf, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        focus = true;
        strings strs = split(inputbuf);
        if (strs.empty())
        {
            inputbuf.clear();
            return 1;
        }

        auto it = commands.find(strs[0]);
        if (it != commands.end())
        {
            it->second(strs);
            inputbuf.clear();
            return 0;
        }
        else
        {
            addHistory("[error]what is: " + strs[0]);
            inputbuf.clear();
            return 1;
        }
    }
    return 0;
}

void ConsoleWin::registerCmd(std::string name, CommandFunc func)
{
    commands[name] = func;
}

ConsoleWin::strings ConsoleWin::split(std::string str)
{
    strings tokens;
    std::stringstream ss(str);
    std::string word;
    while (ss >> word)
    {
        tokens.push_back(word);
    }
    return tokens;
}

void TextWindow::render()
{
    if (!show)
        return;
    ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
    ImGui::Begin(windowTitle.c_str(), &show, flags);
    content.render();
    ImGui::End();
}

void MainWindow::render()
{
    if (!show)
        return;
    ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
    ImGui::Begin(windowTitle.c_str(), &show, flags);
    ImGui::BeginChild("ConsoleText",
                      ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
                      ImGuiChildFlags_None,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_HorizontalScrollbar);
    textcontent.render();
    buttoncontent.render();
    if (needAutoScroll && isAutoScroll)
    {
        ImGui::SetScrollY(ImGui::GetScrollMaxY());
        needAutoScroll = false;
    }
    ImGui::EndChild();
    ImGui::End();
}

bool WinManager::removeWindow(WindowBase *window)
{
    auto it = std::find_if(windows.begin(), windows.end(),
                           [window](const std::unique_ptr<WindowBase> &ptr)
                           {
                               return ptr.get() == window;
                           });
    if (it != windows.end())
    {
        windows.erase(it);
        return true;
    }
    return false;
}

void WinManager::renderAll()
{
    for (auto &i : windows)
    {
        i->render();
    }
    if (ImGui::BeginPopupContextVoid("WindowManagerMenu"))
    {
        if (ImGui::BeginMenu("窗口显示"))
        {
            for (auto &i : windows)
            {
                if (ImGui::MenuItem(i->getTitle().c_str(), nullptr, &i->show))
                {
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
}