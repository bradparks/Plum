#include <iostream>

#include "engine.h"

#ifdef _WIN32
#include <GLFW/glfw3native.h>
#endif

namespace plum
{
    const char* const VertexCompatHeader =
        // VERSION 1.2 compatiblity.
        "#version 120\n"
        "#define in attribute\n"
        "#define out varying\n";
    const char* const VertexCoreHeader =
        "#version 150\n";
    const char* const VertexShader =
        "uniform mat4 projection;\n"
        "uniform vec2 origin;\n"
        "uniform vec2 pivot;\n"
        "uniform vec2 scale;\n"
        "uniform float angle;\n"
        "in vec2 xy;\n"
        "in vec2 uv;\n"
        "out vec2 fragmentUV;\n"
        "void main()\n"
        "{\n"
        // /1 0 0 x + p\   /s 0 0 0\   /+cos(a) -sin(a) 0 0\   /1 0 0 -p\
        // |0 1 0 y + q| * |0 t 0 0| * |+sin(a) +cos(a) 0 0| * |0 1 0 -q|
        // |0 0 1 0    |   |0 0 1 0|   |0       0       1 0|   |0 0 1 0 |
        // \0 0 0 1    /   \0 0 0 1/   \0       0       0 1/   \0 0 0 1 /
        // a = angle, (s, t) = scale, (p, q) = pivot, (x, y) = position
        "   gl_Position = projection * mat4(\n"
        "       scale.x * cos(angle), scale.y * sin(angle), 0, 0,\n"
        "       -scale.x * sin(angle), scale.y * cos(angle), 0, 0,\n"
        "       0, 0, 1, 0,\n"
        "       -scale.x * cos(angle) * pivot.x + pivot.x + origin.x + pivot.y * scale.x * sin(angle), -scale.y * cos(angle) * pivot.y + pivot.y + origin.y - pivot.x * scale.y * sin(angle), 0, 1\n"
        "   ) * vec4(xy, 0, 1);\n"
        "   fragmentUV = uv;\n"
        "}\n";

    const char* const FragmentCompatHeader =
        // VERSION 1.2 compatiblity.
        "#version 120\n"
        "#define in varying\n"
        "#define outColor gl_FragColor\n"
        "#define texture texture2D\n";
    const char* const FragmentCoreHeader =
        "#version 150\n";
    const char* const FragmentShader =
        "uniform vec4 color;\n"
        "uniform sampler2D image;\n"
        "uniform float hasImage;\n"
        "in vec2 fragmentUV;\n"
        "#ifndef outColor\n"
        "out vec4 outColor;\n"
        "#endif\n"
        "void main()\n"
        "{\n"
        "    outColor = hasImage * texture(image, fragmentUV) * color + (1 - hasImage) * color;\n"
        "}\n";

    Engine::Impl::Impl()
    {
        if(!glfwInit())
        {
            quit("Failed to initialize glfw.");
        }
        glfwSetTime(0.0);

        bool core = true;
        const char* vertexShaderSource[2] = {VertexCoreHeader, VertexShader};
        const char* fragmentShaderSource[2] = {FragmentCoreHeader, FragmentShader};

        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
        root = glfwCreateWindow(1, 1, "", nullptr, nullptr);
        if(!root)
        {
            fprintf(stderr, "* Failed to create a OpenGL 3.2 context, falling back on 2.1...\n\n");

            glfwDefaultWindowHints();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
            root = glfwCreateWindow(1, 1, "", nullptr, nullptr);

            vertexShaderSource[0] = VertexCompatHeader;
            fragmentShaderSource[0] = FragmentCompatHeader;
            core = false;

            if(!root)
            {
                quit("Failed to initialize graphics context.");
            }
        }

        glfwMakeContextCurrent(root);
        glewInit();

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 2, vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        {
            GLint status;
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
            if(!status)
            {
                glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &status);
                std::vector<char> error(status);
                glGetShaderInfoLog(vertexShader, error.size(), nullptr, error.data());
                quit(std::string("Error found in vertex shader:\r\n") + error.data());
            }
        }

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 2, fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        {
            GLint status;
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
            if(!status)
            {
                glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &status);
                std::vector<char> error(status);
                glGetShaderInfoLog(fragmentShader, error.size(), nullptr, error.data());
                quit(std::string("Error found in fragment shader:\r\n") + error.data());
            }
        }

        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        if(core)
        {
            glBindFragDataLocation(fragmentShader, 0, "outColor");
        }
        glLinkProgram(program);
        {
            GLint status;
            glGetProgramiv(program, GL_LINK_STATUS, &status);
            if(!status)
            {
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &status);
                std::vector<char> error(status);
                glGetProgramInfoLog(program, error.size(), nullptr, error.data());
                quit(std::string("Error found during shader linking:\r\n") + error.data());
            }
        }
        glUseProgram(program);

        projectionUniform = glGetUniformLocation(program, "projection");
        originUniform = glGetUniformLocation(program, "origin");
        pivotUniform = glGetUniformLocation(program, "pivot");
        scaleUniform = glGetUniformLocation(program, "scale");
        angleUniform = glGetUniformLocation(program, "angle");
        colorUniform = glGetUniformLocation(program, "color");
        hasImageUniform = glGetUniformLocation(program, "hasImage");
        xyAttribute = glGetAttribLocation(program, "xy");
        uvAttribute = glGetAttribLocation(program, "uv");
    }

    Engine::Impl::~Impl()
    {
        glfwDestroyWindow(root);
        glfwTerminate();
    }

    void Engine::Impl::quit(const std::string& message)
    {
        if(message.length())
        {
            fprintf(stderr, "Exit Requested: %s\n", message.c_str());
#ifdef _WIN32
            MessageBoxA(nullptr, message.c_str(), "Exit Requested", MB_SYSTEMMODAL);
#endif
            throw SystemExit(1);
        }
        throw SystemExit(0);
    }

    void Engine::Impl::refresh()
    {
        windowless = true;
        for(const auto h : updateHooks)
        {
            if(auto f = h.lock())
            {
                (*f)();
            }
        }
        updateHooks.cleanup();
        if(windowless)
        {
            quit("Attempt to refresh with no screens open.");
        }
    }

    Engine::Engine()
        : impl(new Impl())
    {
    }

    Engine::~Engine()
    {
    }

    void Engine::refresh()
    {
        impl->refresh();
    }

    void Engine::quit(const std::string& message)
    {
        impl->quit(message);
    }

    std::shared_ptr<Engine::UpdateHook> Engine::addUpdateHook(const UpdateHook& hook)
    {
        auto ptr = std::make_shared<Engine::UpdateHook>(hook);
        impl->updateHooks.append(ptr);
        return ptr;
    }
}
