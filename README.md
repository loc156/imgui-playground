#imgui-playground
基于 Dear ImGui + GLFW 的轻量级 UI 封装框架。
> 这是一个个人学习/实验项目，来自日常开发中为了方便而写的简单 ImGui 封装库。代码随意使用修改，采用 [MIT](LICENSE) 协议开源。

项目经由cmake构建。
相关前置：
c++，
Dear ImGui 1.x（含 backends/imgui_impl_glfw.h + imgui_impl_opengl3.h），
GLFW 3.x，

## 功能
- 窗口管理（ConsoleWin / TextWindow / MainWindow）
- 命令控制台（注册命令 + 历史滚动）
- 文本容器（纯文本、彩色、标题、状态条、对话等）
- 按钮容器（状态绑定、回调函数）

AI使用提醒：
renderer.h(/.cpp)文件来自ai的生成，为完全黑盒子。作为底层渲染封装直接使用。
其余文件的注释大部分由 AI 辅助补充，已做简单检查，但可还未修改（懒惰发力了），请以代码实际行为为准。
来自ai的使用案例：

License
MIT
https://github.com/loc156/imgui-playground/blob/main/LICENSE

## 快速开始

```cpp
#include "Renderer.h"
#include "WinManager.h"

int main()
{
    // 1. 初始化渲染器
    Renderer renderer;
    Renderer::Config cfg;
    cfg.width = 1280;
    cfg.height = 720;
    cfg.title = "My ImGui App";
    cfg.fontPath = "C:/Windows/Fonts/msyh.ttc";  // 可选，支持中文
    cfg.fontSize = 18.0f;
    if (!renderer.Init(cfg))
        return -1;

    // 2. 创建窗口管理器
    WinManager wm;

    // 3. 创建控制台窗口（带命令输入）
    auto* console = wm.bindWindow(std::make_unique<ConsoleWin>("控制台", ImVec2(600, 400)));

    console->registerCmd("hello", [&](const ConsoleWin::strings& args) {
        if (args.size() > 1)
            console->addHistory("Hello, " + args[1] + "!");
        else
            console->addHistory("Hello, World!");
    });

    console->registerCmd("clear", [&](const ConsoleWin::strings&) {
        console->clear();
        console->addHistory("[系统] 已清屏");
    });

    // 4. 创建主窗口（带文本 + 按钮）
    auto* mainWin = wm.bindWindow(std::make_unique<MainWindow>("主界面", ImVec2(700, 500)));

    static bool btnPressed = false;
    static int counter = 0;

    mainWin->addText<Text::TitleText>("系统状态");
    mainWin->addText<Text::StatusText>("运行状态", 75, 100);
    mainWin->addText<Text::EmptyLine>();

    mainWin->addText<Text::TitleText>("操作面板");
    mainWin->addButton<Button::Button>("点我计数", &btnPressed);
    mainWin->addText<Text::PlainText>("按钮状态: " + std::string(btnPressed ? "按下" : "松开"));

    mainWin->addButton<Button::CallBackButton>("重置", [&counter]() {
        counter = 0;
    });

    mainWin->addText<Text::ColoredText>("计数器: " + std::to_string(counter), Colors::Gold());

    // 5. 创建只读信息窗口
    auto* infoWin = wm.bindWindow(std::make_unique<TextWindow>("信息面板", ImVec2(400, 300)));
    infoWin->add<Text::PlainText>("这是一个只读信息窗口");
    infoWin->add<Text::ColoredText>("支持彩色文本", Colors::Cyan());
    infoWin->add<Text::ListItemText>("列表项 1");
    infoWin->add<Text::ListItemText>("列表项 2", "✓ ");
    infoWin->add<Text::SeparatorText>();
    infoWin->add<Text::DialogueText>("系统", "欢迎使用此框架");

    // 6. 主循环
    while (!renderer.ShouldClose())
    {
        renderer.BeginFrame();
        wm.renderAll();
        renderer.EndFrame();
    }

    renderer.Shutdown();
    return 0;
}

