#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"
#include "Line.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_impl_glfw.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void recalculateVertices(unsigned int* VAO, unsigned int* VBO, int pStart[2], int pFinal[2], int* numOfPixels, const unsigned int _pattern);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char* glsl_version = "#version 150";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // required for apple machine
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Draw Line", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Shader ourShader("shaders/vertexGLSL.vs", "shaders/fragmentGLSL.fs");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // set up buffer(s)
    // ----------------------------------
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Our initial state
    float clear_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float line_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float point_size = 5.0f;
    int pStart[2] = { 0, 0 };
    int pFinal[2] = { 0, 0 };
    int numOfPixels = 0;
    int spacing_current = 0;
    const char* spacing[] = { // 2^24
        "0 pixel",
        "4 pixels",
        "8 pixel",
        "12 pixel",
        "16 pixel",
        "20 pixel",
    };

    const unsigned int pattern[] = {
        0xffffff,
        0x0fffff,
        0x00ffff,
        0x000fff,
        0x0000ff,
        0x00000f,
    };
    glEnable(GL_PROGRAM_POINT_SIZE); // enable this to manipulate pixel size

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        // be sure to activate the shader
        ourShader.use();

        // set the customizable attribute with uniform
        ourShader.setFloat("pointSize", point_size);
        ourShader.setVec4("lineColor", line_color);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_POINTS, 0, numOfPixels);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

            ImGui::SliderFloat("Point size", &point_size, 0.0f, 50.0f);
            ImGui::ColorEdit3("Background color", clear_color); // RGB
            ImGui::ColorEdit4("Line color", line_color); // RGBA

            ImGui::InputInt2("Starting point", pStart);
            ImGui::InputInt2("Final point", pFinal);

            ImGui::Combo("Pixel Spacing", &spacing_current, spacing, IM_ARRAYSIZE(spacing));
            if (ImGui::Button("Draw line"))
            {
                recalculateVertices(&VAO, &VBO, pStart, pFinal, &numOfPixels, pattern[spacing_current]);
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear screen"))
            {
                // TODO: find the actual solution for this band-aid
                int p0[] = { -100, -100 };
                int p1[] = { -100, -100 };
                numOfPixels = 0;
                recalculateVertices(&VAO, &VBO, p0, p1, &numOfPixels, 0x000000);
            }
            ImGui::Text("Relative to the source directory");
            if (ImGui::Button("Save file"))
            {
                Line newLine(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT);
                newLine.writeJSON("filename.json", point_size, spacing_current, clear_color, line_color);
            }
            ImGui::SameLine();
            if (ImGui::Button("Open file"))
            {
                // this is really ugly
                Line newLine(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT);
                newLine.readJSON("filename.json", pStart, pFinal, &point_size, &spacing_current, clear_color, line_color);

                recalculateVertices(&VAO, &VBO, pStart, pFinal, &numOfPixels, pattern[spacing_current]);
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(ourShader.ID);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void recalculateVertices(unsigned int* VAO, unsigned int* VBO, int pStart[2], int pFinal[2], int* numOfPixels, const unsigned int _pattern)
{
    Line newLine(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT);
    std::vector<float> points = newLine.createPoints(_pattern); // this is 1d vector!!
    *numOfPixels = points.size() / 3; // each pixel vertex within the std::vector has 3 components

    float* vertices = &points[0]; // "convert" the std::vector into traditional array

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // position attribute (only x, y and z component)
    // Known bug: Access violation reading location, hard to reproduce, but should pop up if you redraw the line often enough
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}