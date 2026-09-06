#pragma once
#include <GLFW/glfw3.h>
#include <string>

class Renderer
{
public:
    struct Config
    {
        int width = 1024;
        int height = 768;
        std::string title = "None";
        std::string fontPath = "";
        float fontSize = 18.0f;
    };

    bool Init(const Config &config);
    void Shutdown();

    void BeginFrame();
    void EndFrame();
    bool ShouldClose() const;

    GLFWwindow *GetWindow() const { return m_Window; }

private:
    GLFWwindow *m_Window = nullptr;
};