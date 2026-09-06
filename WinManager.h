#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include "imgui.h"
#include "imgui_stdlib.h"

#include "TextRenderer.h"
#include "ButtonRenderer.h"

class WindowBase
{
protected:
    std::string windowTitle;
    ImGuiWindowFlags flags = ImGuiWindowFlags_None;
    ImVec2 size = ImVec2(600, 400);

public:
    bool show = true;

public:
    virtual void render() = 0;

    explicit WindowBase(const std::string &title) : windowTitle(title) {}
    explicit WindowBase(const std::string &title, const ImVec2 &s) : windowTitle(title), size(s) {}
    virtual ~WindowBase() = default;

    void setFlags(ImGuiWindowFlags f) { flags = f; }
    void setTitle(const std::string &title) { windowTitle = title; }
    std::string getTitle() { return windowTitle; }
    void showWindow(bool s) { show = s; }
    bool isVisible() const { return show; }
};

class TextWindow : public WindowBase
{
private:
    TextContent content;

public:
    explicit TextWindow(const std::string &title) : WindowBase(title) {}
    explicit TextWindow(const std::string &title, const ImVec2 &s) : WindowBase(title, s) {}
    template <typename T, typename... Args>
    void add(Args &&...args) { content.add<T>(std::forward<Args>(args)...); }
    void clear() { content.clear(); }

    void render();
};

class ConsoleWin : public WindowBase
{
public:
    using strings = std::vector<std::string>;
    using CommandFunc = std::function<void(const strings &args)>;

private:
    std::string inputbuf;
    std::unordered_map<std::string, CommandFunc> commands;
    bool focus = false;
    bool needAutoScroll = false;

public:
    strings history;
    bool isAutoScroll = true;

public:
    explicit ConsoleWin(const std::string &title) : WindowBase(title) {}
    explicit ConsoleWin(const std::string &title, const ImVec2 &s) : WindowBase(title, s) {}
    void render();
    bool executeCmd();
    void registerCmd(std::string name, CommandFunc func);
    strings split(std::string str);
    void clear() { history.clear(); };
    void addHistory(const std::string &msg)
    {
        history.push_back(msg);
        needAutoScroll = true;
    };
};

class MainWindow : public WindowBase
{
private:
    TextContent textcontent;
    ButtonContent buttoncontent;
    bool needAutoScroll = false;

public:
    bool isAutoScroll = true;

public:
    explicit MainWindow(const std::string &title) : WindowBase(title) {}
    explicit MainWindow(const std::string &title, const ImVec2 &s) : WindowBase(title, s) {}
    template <typename T, typename... Args>
    void addText(Args &&...args)
    {
        textcontent.add<T>(std::forward<Args>(args)...);
        needAutoScroll = true;
    }
    template <typename T, typename... Args>
    void addButton(Args &&...args)
    {
        buttoncontent.add<T>(std::forward<Args>(args)...);
        needAutoScroll = true;
    }
    void clear()
    {
        textcontent.clear();
        buttoncontent.clear();
    }

    void render();
};

class WinManager
{
    std::vector<std::unique_ptr<WindowBase>> windows;

public:
    template <typename T>
    T *bindWindow(std::unique_ptr<T> window)
    {
        T *ptr = window.get();
        windows.push_back(std::move(window));
        return ptr;
    }
    bool removeWindow(WindowBase *window);
    void clearAll() { windows.clear(); }
    void renderAll();
};