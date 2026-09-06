#pragma once
#include <vector>
#include <memory>
#include <string>
#include "imgui.h"

/**
 * @brief 可渲染文本的抽象接口
 * 
 * 所有文本类都应继承此接口，实现 render() 方法。
 * 配合 TextContent 容器使用，支持多态管理。
 */
struct IRenderableText
{
    virtual ~IRenderableText() = default;
    virtual void render() = 0;
};

/**
 * @brief 文本容器，管理多个可渲染文本的生命周期和渲染
 * 
 * 用法：
 *   TextContent content;
 *   content.add<Text::PlainText>("普通文本");
 *   content.add<Text::ColoredText>("彩色文本", ImVec4(1,0,0,1));
 *   content.render();  // 渲染所有文本
 */
class TextContent
{
private:
    std::vector<std::unique_ptr<IRenderableText>> elements;

public:
    /**
     * @brief 添加一个文本元素到容器中
     * 
     * @tparam T 文本类型（必须继承 IRenderableText）
     * @tparam Args 构造函数参数类型
     * @param args 传递给 T 构造函数的参数
     * 
     * 示例：
     *   content.add<Text::PlainText>("Hello");
     *   content.add<Text::ColoredText>("警告", ImVec4(1,0,0,1));
     */
    template <typename T, typename... Args>
    void add(Args &&...args) { elements.push_back(std::make_unique<T>(std::forward<Args>(args)...)); }

    /** @brief 清空所有文本 */
    void clear() { elements.clear(); }

    /** @brief 渲染所有文本（在主循环中调用） */
    void render()
    {
        for (auto &elem : elements)
        {
            elem->render();
        }
    }

    /** @brief 检查容器是否为空 */
    bool empty() const { return elements.empty(); }

    /** @brief 获取文本数量 */
    size_t size() const { return elements.size(); }
};

namespace Text
{
    /**
     * @brief 纯文本 - 最简单的文本显示
     * 
     * 使用 ImGui::TextUnformatted()，不进行格式化。
     * 适合显示静态文本或变量值。
     * 
     * 示例：
     *   PlainText("Hello World");
     *   PlainText(std::to_string(value).c_str());
     */
    class PlainText : public IRenderableText
    {
        std::string text;

    public:
        /**
         * @brief 构造纯文本
         * @param t 要显示的文本（std::string 或 const char* 均可）
         */
        explicit PlainText(const std::string &t) : text(t) {}
        explicit PlainText(const char *t) : text(t) {}

        void render() override { ImGui::TextUnformatted(text.c_str()); }
    };

    /**
     * @brief 彩色文本 - 带自定义颜色的文本
     * 
     * 使用 ImGui::TextColored()，适合突出显示重要信息。
     * 
     * 示例：
     *   ColoredText("错误！", ImVec4(1,0,0,1));
     */
    class ColoredText : public IRenderableText
    {
        std::string text;
        ImVec4 color;

    public:
        /**
         * @brief 构造彩色文本
         * @param t 文本内容
         * @param c 文本颜色（RGBA，范围 0~1）
         * 
         * 预定义颜色见 Colors 命名空间：
         *   Colors::Red(), Colors::Green(), Colors::Gold() 等
         */
        ColoredText(const std::string &t, const ImVec4 &c) : text(t), color(c) {}

        void render() override { ImGui::TextColored(color, "%s", text.c_str()); }
    };

    /**
     * @brief 标题文本 - 带颜色和分隔线
     * 
     * 默认金色标题，适合用作区块标题。
     * 显示格式：【标题】+ 分隔线
     * 
     * 示例：
     *   TitleText("设置")；
     *   TitleText("信息", Colors::Cyan());
     */
    class TitleText : public IRenderableText
    {
        std::string text;
        ImVec4 color;

    public:
        /**
         * @brief 构造标题文本（默认金色）
         * @param t 标题内容
         */
        explicit TitleText(const std::string &t)
            : text(t), color(1.0f, 0.8f, 0.0f, 1.0f) {}

        /**
         * @brief 构造标题文本（自定义颜色）
         * @param t 标题内容
         * @param c 标题颜色
         */
        TitleText(const std::string &t, const ImVec4 &c) : text(t), color(c) {}

        void render() override
        {
            ImGui::TextColored(color, "【%s】", text.c_str());
            ImGui::Separator();
        }
    };

    /**
     * @brief 分隔线 - 水平分割线
     * 
     * 使用 ImGui::Separator()，用于视觉分隔不同区域。
     */
    class SeparatorText : public IRenderableText
    {
    public:
        void render() override { ImGui::Separator(); }
    };

    /**
     * @brief 缩进文本 - 带缩进的文本
     * 
     * 适合层级结构显示，如树形菜单、大纲等。
     * 
     * 示例：
     *   IndentText("子项目1", 20.0f);
     *   IndentText("子项目2", 20.0f);
     */
    class IndentText : public IRenderableText
    {
        std::string text;
        float indentSize = 20.0f;

    public:
        /**
         * @brief 构造缩进文本（默认缩进 20px）
         * @param t 文本内容
         */
        explicit IndentText(const std::string &t) : text(t) {}

        /**
         * @brief 构造缩进文本（自定义缩进）
         * @param t 文本内容
         * @param indent 缩进量（像素）
         */
        IndentText(const std::string &t, float indent) : text(t), indentSize(indent) {}

        void render() override
        {
            ImGui::Indent(indentSize);
            ImGui::TextUnformatted(text.c_str());
            ImGui::Unindent(indentSize);
        }
    };

    /**
     * @brief 状态文本 - 根据数值百分比自动变色
     * 
     * 显示格式："标签: 值 / 最大值"
     * 颜色规则：
     *   - 比例 > 60%：绿色
     *   - 比例 30%~60%：黄色
     *   - 比例 < 30%：红色
     * 
     * 适合显示血量、进度、健康度等。
     * 
     * 示例：
     *   StatusText("血量", 75, 100);  // 显示绿色
     *   StatusText("电量", 20, 100);  // 显示红色
     */
    class StatusText : public IRenderableText
    {
        std::string label;
        int value;
        int maxValue;
        ImVec4 color;

    public:
        /**
         * @brief 构造状态文本
         * @param l 标签名称
         * @param val 当前值
         * @param maxVal 最大值
         */
        StatusText(const std::string &l, int val, int maxVal)
            : label(l), value(val), maxValue(maxVal)
        {
            float ratio = (float)val / maxVal;
            if (ratio > 0.6f)
                color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            else if (ratio > 0.3f)
                color = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
            else
                color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
        }

        void render() override
        {
            ImGui::TextColored(color, "%s: %d / %d", label.c_str(), value, maxValue);
        }
    };

    /**
     * @brief 对话文本 - 带说话人和自动换行
     * 
     * 显示格式："说话人: 内容"（内容自动换行）
     * 适合显示 NPC 对话、聊天记录等。
     * 
     * 示例：
     *   DialogueText("NPC", "欢迎来到我的商店！");
     *   DialogueText("玩家", "我想买点东西。", Colors::Cyan());
     */
    class DialogueText : public IRenderableText
    {
        std::string speaker;
        std::string content;
        ImVec4 speakerColor = ImVec4(0.6f, 0.8f, 1.0f, 1.0f);

    public:
        /**
         * @brief 构造对话文本（默认说话人颜色：淡蓝色）
         * @param spk 说话人名称
         * @param cnt 对话内容
         */
        DialogueText(const std::string &spk, const std::string &cnt)
            : speaker(spk), content(cnt) {}

        /**
         * @brief 构造对话文本（自定义说话人颜色）
         * @param spk 说话人名称
         * @param cnt 对话内容
         * @param color 说话人颜色
         */
        DialogueText(const std::string &spk, const std::string &cnt, const ImVec4 &color)
            : speaker(spk), content(cnt), speakerColor(color) {}

        void render() override
        {
            ImGui::TextColored(speakerColor, "%s: ", speaker.c_str());
            ImGui::SameLine();
            ImGui::TextWrapped("%s", content.c_str());
        }
    };

    /**
     * @brief 列表项文本 - 带前缀符号的列表项
     * 
     * 显示格式："前缀 + 文本"
     * 默认前缀为 "• "，适合用作无序列表。
     * 
     * 示例：
     *   ListItemText("苹果");           // "• 苹果"
     *   ListItemText("完成", "✓ ");    // "✓ 完成"
     *   ListItemText("错误", "✗ ", Colors::Red()); // 红色 "✗ 错误"
     */
    class ListItemText : public IRenderableText
    {
        std::string text;
        std::string prefix;
        ImVec4 color;

    public:
        /**
         * @brief 构造列表项（默认前缀 "• "，白色）
         * @param t 文本内容
         */
        ListItemText(const std::string &t)
            : text(t), prefix("• "), color(1.0f, 1.0f, 1.0f, 1.0f) {}

        /**
         * @brief 构造列表项（自定义前缀，白色）
         * @param t 文本内容
         * @param pre 前缀字符串（如 "• ", "✓ ", "✗ "）
         */
        ListItemText(const std::string &t, const std::string &pre)
            : text(t), prefix(pre), color(1.0f, 1.0f, 1.0f, 1.0f) {}

        /**
         * @brief 构造列表项（自定义前缀和颜色）
         * @param t 文本内容
         * @param pre 前缀字符串
         * @param c 文本颜色
         */
        ListItemText(const std::string &t, const std::string &pre, const ImVec4 &c)
            : text(t), prefix(pre), color(c) {}

        void render() override
        {
            ImGui::TextColored(color, "%s%s", prefix.c_str(), text.c_str());
        }
    };

    /**
     * @brief 空行 - 垂直间距占位
     * 
     * 默认使用 ImGui::Spacing()（小间距），
     * 也可以自定义高度使用 ImGui::Dummy()。
     * 
     * 示例：
     *   EmptyLine();        // 默认间距
     *   EmptyLine(10.0f);   // 10px 高度空行
     */
    class EmptyLine : public IRenderableText
    {
        float height = 0.0f;

    public:
        EmptyLine() = default;

        /**
         * @brief 构造指定高度的空行
         * @param h 高度（像素），传 0 表示使用默认间距
         */
        explicit EmptyLine(float h) : height(h) {}

        void render() override
        {
            if (height > 0.0f)
                ImGui::Dummy(ImVec2(0.0f, height));
            else
                ImGui::Spacing();
        }
    };

    /**
     * @brief 高亮文本 - 带背景色的文本
     * 
     * 适合用来突出显示重要信息、警告、提示等。
     * 
     * 示例：
     *   HighlightText("重要提示！", Colors::Gold());
     *   HighlightText("错误信息", Colors::Red(), Colors::White());
     */
    class HighlightText : public IRenderableText
    {
        std::string text;
        ImVec4 bgColor;
        ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    public:
        /**
         * @brief 构造高亮文本（白色文字）
         * @param t 文本内容
         * @param bg 背景颜色
         */
        HighlightText(const std::string &t, const ImVec4 &bg)
            : text(t), bgColor(bg) {}

        /**
         * @brief 构造高亮文本（自定义文字颜色）
         * @param t 文本内容
         * @param bg 背景颜色
         * @param fg 文字颜色
         */
        HighlightText(const std::string &t, const ImVec4 &bg, const ImVec4 &fg)
            : text(t), bgColor(bg), textColor(fg) {}

        void render() override
        {
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, bgColor);
            ImGui::TextWrapped("%s", text.c_str());
            ImGui::PopStyleColor(2);
        }
    };

    /**
     * @brief 自动换行文本 - 宽度超出自动换行
     * 
     * 使用 ImGui::TextWrapped()，适合显示长文本。
     * 
     * 示例：
     *   WrappedText("这是一段很长的文字，会自动换行显示...");
     */
    class WrappedText : public IRenderableText
    {
        std::string text;

    public:
        explicit WrappedText(const std::string &t) : text(t) {}

        void render() override { ImGui::TextWrapped("%s", text.c_str()); }
    };
}

/**
 * @brief 预定义颜色常量
 * 
 * 使用方式：
 *   ImVec4 red = Colors::Red();
 *   ColoredText("警告", Colors::Gold());
 * 
 * 所有颜色返回值均为 ImVec4，范围 0~1。
 */
namespace Colors
{
    constexpr ImVec4 White() { return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); }
    constexpr ImVec4 Gold() { return ImVec4(1.0f, 0.8f, 0.0f, 1.0f); }
    constexpr ImVec4 Red() { return ImVec4(1.0f, 0.2f, 0.2f, 1.0f); }
    constexpr ImVec4 Green() { return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); }
    constexpr ImVec4 Blue() { return ImVec4(0.2f, 0.5f, 1.0f, 1.0f); }
    constexpr ImVec4 Yellow() { return ImVec4(1.0f, 0.9f, 0.0f, 1.0f); }
    constexpr ImVec4 Orange() { return ImVec4(1.0f, 0.6f, 0.0f, 1.0f); }
    constexpr ImVec4 Purple() { return ImVec4(0.6f, 0.2f, 1.0f, 1.0f); }
    constexpr ImVec4 Gray() { return ImVec4(0.5f, 0.5f, 0.5f, 1.0f); }
    constexpr ImVec4 Dark() { return ImVec4(0.2f, 0.2f, 0.2f, 1.0f); }
    constexpr ImVec4 Cyan() { return ImVec4(0.0f, 1.0f, 1.0f, 1.0f); }
    constexpr ImVec4 Pink() { return ImVec4(1.0f, 0.4f, 0.7f, 1.0f); }
};