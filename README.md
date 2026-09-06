#imgui-playground
基于 Dear ImGui + GLFW 的轻量级 UI 封装框架。
这是一个个人学习项目，或者说实验项目，来自我个人学习过程中因为需要或者使用而编写的简单imgui封装库。
代码随意使用修改。
符合开源协议MIT

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
renderer.h(/.cpp)文件来自ai的生成，为完全黑盒子。
其他文件的相关使用注释基本均来自AI，我稍微检查了一下，有些错误，但没有修改，请自己斟酌。

来自ai的使用案例：
#include "Renderer.h"
#include "WinManager.h"
#include "GameManager.h"  // 假设你有这个

int main()
{
    // 1. 初始化渲染器
    Renderer renderer;
    Renderer::Config cfg;
    cfg.width = 1280;
    cfg.height = 720;
    cfg.title = "My ImGui App";
    cfg.fontPath = "C:/Windows/Fonts/msyh.ttc";  // 微软雅黑，支持中文
    cfg.fontSize = 18.0f;
    if (!renderer.Init(cfg))
        return -1;

    // 2. 创建窗口管理器
    WinManager wm;

    // 3. 创建一个控制台窗口（带命令输入）
    auto* console = wm.bindWindow(std::make_unique<ConsoleWin>("控制台", ImVec2(600, 400)));

    // 注册命令
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

    console->registerCmd("status", [&](const ConsoleWin::strings&) {
        console->addHistory("[系统] 运行正常，窗口数: " + std::to_string(2));
    });

    // 4. 创建一个主窗口（带文本 + 按钮）
    auto* mainWin = wm.bindWindow(std::make_unique<MainWindow>("主界面", ImVec2(700, 500)));

    static bool btnPressed = false;
    static int counter = 0;

    mainWin->addText<Text::TitleText>("系统状态");
    mainWin->addText<Text::StatusText>("运行时间", 120, 100);  // 120% 显示绿色
    mainWin->addText<Text::EmptyLine>();

    mainWin->addText<Text::TitleText>("操作面板");
    mainWin->addButton<Button::Button>("点我计数", &btnPressed);
    mainWin->addText<Text::PlainText>("按钮状态: " + std::string(btnPressed ? "按下" : "松开"));

    mainWin->addButton<Button::CallBackButton>("重置", []() {
        counter = 0;
        // 注意：这里无法直接更新 UI，需要通过 GameManager 或消息机制
    });

    mainWin->addText<Text::ColoredText>("计数器: " + std::to_string(counter), Colors::Gold());

    // 5. 创建普通文本窗口
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

        // 渲染所有窗口
        wm.renderAll();

        // 如果你的游戏有更新逻辑
        // GameManager::getInstance().update();

        renderer.EndFrame();
    }

    // 7. 清理

    
    renderer.Shutdown();
    return 0;
}
