#pragma once
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include "imgui.h"

/**
 * @brief 可渲染按钮的抽象接口
 *
 * 所有按钮类都应继承此接口，实现 render() 方法。
 * 配合 ButtonContent 容器使用，支持多态管理。
 */
struct IRenderableButton
{
    virtual ~IRenderableButton() = default;
    virtual void render() = 0;
};

/**
 * @brief 按钮容器，管理多个可渲染按钮的生命周期和渲染
 *
 * 用法：
 *   ButtonContent content;
 *   content.add<Button::Button>("点击", &flag);
 *   content.add<Button::StateButton>("状态", &flag);
 *   content.render();  // 渲染所有按钮
 */
class ButtonContent
{
private:
    std::vector<std::unique_ptr<IRenderableButton>> elements;

public:
    /**
     * @brief 添加一个按钮到容器中
     *
     * @tparam T 按钮类型（必须继承 IRenderableButton）
     * @tparam Args 构造函数参数类型
     * @param args 传递给 T 构造函数的参数
     *
     * 示例：
     *   content.add<Button::Button>("确定", &flag);
     *   content.add<Button::StateButton>("状态", &flag, ImVec2(100,30));
     */
    template <typename T, typename... Args>
    void add(Args &&...args) { elements.push_back(std::make_unique<T>(std::forward<Args>(args)...)); }

    /** @brief 清空所有按钮 */
    void clear() { elements.clear(); }

    /** @brief 渲染所有按钮（在主循环中调用） */
    void render()
    {
        for (auto &elem : elements)
        {
            elem->render();
        }
    }

    /** @brief 检查容器是否为空 */
    bool empty() const { return elements.empty(); }

    /** @brief 获取按钮数量 */
    size_t size() const { return elements.size(); }
};

namespace Button
{
    /**
     * @brief 状态按钮 - 点击时修改绑定的 bool 变量
     *
     * 点击后绑定的 bool 变为 true，松开后变为 false。
     * 适合用于表示"按钮是否正在被按下"的状态。
     *
     * 示例：
     *   bool flag = false;
     *   StateButton btn("点击", &flag);  // flag 会跟随按钮状态变化
     */
    class StateButton : public IRenderableButton
    {
        std::string text;
        ImVec2 size = ImVec2(0, 0);
        bool *ispressed = nullptr;

    public:
        /**
         * @brief 构造一个状态按钮（默认大小）
         * @param t 按钮显示文本
         * @param e 指向外部 bool 变量的指针，按钮按下时为 true，松开时为 false
         */
        StateButton(const std::string &t, bool *e) : text(t), ispressed(e) {}

        /**
         * @brief 构造一个状态按钮（自定义大小）
         * @param t 按钮显示文本
         * @param e 指向外部 bool 变量的指针
         * @param s 按钮大小（ImVec2(宽度, 高度)），传 ImVec2(0,0) 使用默认大小
         */
        StateButton(const std::string &t, bool *e, const ImVec2 &s) : text(t), ispressed(e), size(s) {}

        void render() override
        {
            if (ImGui::Button(text.c_str(), size))
                *ispressed = true;
            else
                *ispressed = false;
        }
    };

    /**
     * @brief 小状态按钮 - StateButton 的 SmallButton 版本
     *
     * 使用 ImGui::SmallButton()，内边距更小，适合紧凑布局。
     * 行为和 StateButton 完全一致。
     */
    class StateSmallButton : public IRenderableButton
    {
        std::string text;
        bool *ispressed = nullptr;

    public:
        /**
         * @brief 构造一个小状态按钮
         * @param t 按钮显示文本
         * @param e 指向外部 bool 变量的指针，按钮按下时为 true，松开时为 false
         */
        StateSmallButton(const std::string &t, bool *e) : text(t), ispressed(e) {}

        void render() override
        {
            if (ImGui::SmallButton(text.c_str()))
                *ispressed = true;
            else
                *ispressed = false;
        }
    };

    /**
     * @brief 小回调按钮 - 点击时执行指定的回调函数
     *
     * 适合需要执行自定义逻辑的场景（如打开弹窗、切换状态等）。
     * 注意：这是 SmallButton 版本，内边距更小。
     */
    class CallBackSmallButton : public IRenderableButton
    {
        std::function<void()> callback;
        std::string text;

    public:
        /**
         * @brief 构造一个小回调按钮
         * @param t 按钮显示文本
         * @param cb 点击时执行的回调函数（无参数，无返回值）
         *
         * 示例：
         *   CallBackSmallButton btn("删除", []() { deleteFile(); });
         */
        CallBackSmallButton(const std::string &t, std::function<void()> cb)
            : text(t), callback(std::move(cb)) {}

        void render() override
        {
            if (ImGui::SmallButton(text.c_str()))
                if (callback)
                    callback();
        }
    };

    /**
     * @brief 回调按钮 - 点击时执行指定的回调函数（支持自定义大小）
     *
     * 注意：当前实现使用的是 SmallButton，如需使用普通 Button，
     * 请使用统一 Button 类或自行修改。
     */
    class CallBackButton : public IRenderableButton
    {
        std::function<void()> callback;
        std::string text;
        ImVec2 size = ImVec2(0, 0);

    public:
        /**
         * @brief 构造一个回调按钮（默认大小）
         * @param t 按钮显示文本
         * @param cb 点击时执行的回调函数
         */
        CallBackButton(const std::string &t, std::function<void()> cb)
            : text(t), callback(std::move(cb)) {}

        /**
         * @brief 构造一个回调按钮（自定义大小）
         * @param t 按钮显示文本
         * @param s 按钮大小
         * @param cb 点击时执行的回调函数
         */
        CallBackButton(const std::string &t, const ImVec2 &s, std::function<void()> cb)
            : text(t), size(s), callback(std::move(cb)) {}

        void render() override
        {
            if (ImGui::SmallButton(text.c_str()))
                if (callback)
                    callback();
        }
    };

    /**
     * @brief 通用按钮 - 整合了状态绑定和回调功能
     *
     * 这是最推荐的按钮类，同时支持：
     *   - 绑定外部 bool 变量（按下=true，松开=false）
     *   - 执行回调函数
     *   - 普通按钮 / SmallButton 切换
     *   - 自定义大小
     *
     * 四种使用方式：
     *   1. Button("文本", &flag)              - 状态绑定
     *   2. Button("文本", &flag, ImVec2(w,h)) - 状态绑定 + 自定义大小
     *   3. Button("文本", [](){})             - 回调函数
     *   4. Button("文本", ImVec2(w,h), [](){}) - 回调函数 + 自定义大小
     *   5. Button::makeSmall("文本", &flag)    - 小按钮 + 状态绑定
     *   6. Button::makeSmall("文本", [](){})   - 小按钮 + 回调函数
     *
     * 示例：
     *   bool flag = false;
     *   Button btn1("点击", &flag);                    // 状态按钮
     *   Button btn2("回调", []() { doSomething(); });  // 回调按钮
     *   auto btn3 = Button::makeSmall("小按钮", &flag); // 小状态按钮
     */
    class Button : public IRenderableButton
    {
    private:
        std::string text;
        ImVec2 size = ImVec2(0, 0);

        bool *boundBool = nullptr;      // 绑定的外部 bool 变量
        std::function<void()> callback; // 点击回调函数

        bool isSmall = false; // 是否使用 SmallButton

    public:
        /**
         * @brief 构造一个状态绑定按钮（默认大小）
         * @param t 按钮显示文本
         * @param bound 指向外部 bool 变量的指针，按下=true，松开=false
         */
        Button(const std::string &t, bool *bound)
            : text(t), boundBool(bound) {}

        /**
         * @brief 构造一个状态绑定按钮（自定义大小）
         * @param t 按钮显示文本
         * @param bound 指向外部 bool 变量的指针
         * @param s 按钮大小
         */
        Button(const std::string &t, bool *bound, const ImVec2 &s)
            : text(t), boundBool(bound), size(s) {}

        /**
         * @brief 构造一个回调按钮（默认大小）
         * @param t 按钮显示文本
         * @param cb 点击时执行的回调函数
         */
        Button(const std::string &t, std::function<void()> cb)
            : text(t), callback(std::move(cb)) {}

        /**
         * @brief 构造一个回调按钮（自定义大小）
         * @param t 按钮显示文本
         * @param s 按钮大小
         * @param cb 点击时执行的回调函数
         */
        Button(const std::string &t, const ImVec2 &s, std::function<void()> cb)
            : text(t), size(s), callback(std::move(cb)) {}

        /**
         * @brief 创建一个小状态按钮（工厂方法）
         * @param t 按钮显示文本
         * @param bound 指向外部 bool 变量的指针
         * @return Button 对象（isSmall = true）
         */
        static Button makeSmall(const std::string &t, bool *bound)
        {
            Button btn(t, bound);
            btn.isSmall = true;
            return btn;
        }

        /**
         * @brief 创建一个小回调按钮（工厂方法）
         * @param t 按钮显示文本
         * @param cb 点击时执行的回调函数
         * @return Button 对象（isSmall = true）
         */
        static Button makeSmall(const std::string &t, std::function<void()> cb)
        {
            Button btn(t, std::move(cb));
            btn.isSmall = true;
            return btn;
        }

        void render() override
        {
            bool pressed = false;
            if (isSmall)
                pressed = ImGui::SmallButton(text.c_str());
            else
                pressed = ImGui::Button(text.c_str(), size);
            if (pressed)
            {
                if (boundBool)
                    *boundBool = true;
                if (callback)
                    callback();
            }
            else if (boundBool)
                *boundBool = false;
        }
    };
};