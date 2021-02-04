#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"
#include "Line.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imfilebrowser.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
int* pStart = new int[2]{ 0, 0 };
int* pFinal = new int[2]{ 0, 0 };
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
    //glfwSetMouseButtonCallback(window, mouse_callback);

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
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // Our initial imGUI state
    float clear_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float line_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float point_size = 1.0f;
    /*int pStart[2] = { 0, 0 };
    int pFinal[2] = { 0, 0 };*/
    int numOfPixels = 0;
    int lineWidth = 1;
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

    Line* newLine = new Line();

    glEnable(GL_PROGRAM_POINT_SIZE); // enable this to manipulate pixel size

    // create a file browser instance
    ImGui::FileBrowser saveFileDialog;
    ImGui::FileBrowser loadFileDialog;

    // (optional) set browser properties
    saveFileDialog.SetTypeFilters({ ".json" });
    saveFileDialog.SetTitle("Save file");
    loadFileDialog.SetTypeFilters({ ".json" });
    loadFileDialog.SetTitle("Load file");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


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

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, numOfPixels);
        glBindVertexArray(0);

        // Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            ImGui::SliderFloat("Pixel size", &point_size, 1.0f, 50.0f);
            ImGui::ColorEdit3("Background color", clear_color); // RGB
            ImGui::ColorEdit4("Line color", line_color); // RGBA

            ImGui::Text("These settings are applied on the next draw");
            ImGui::InputInt2("Starting point", pStart);
            ImGui::InputInt2("Final point", pFinal);

            ImGui::SliderInt("Line width", &lineWidth, 1, 5);

            ImGui::Combo("Pixel Spacing", &spacing_current, spacing, IM_ARRAYSIZE(spacing));
            if (ImGui::Button("Draw line"))
            {
                *newLine = Line(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT);
                std::vector<float> points = newLine->createPoints(pattern[spacing_current], lineWidth); // this is 1d vector!!
                numOfPixels = points.size() / 3; // each pixel vertex within the std::vector has 3 components

                unsigned int tempVBO;
                glGenBuffers(1, &tempVBO);
                // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
                glBindVertexArray(VAO);

                glBindBuffer(GL_ARRAY_BUFFER, tempVBO);

                try
                {
                    // position attribute (only x, y and z component)
                    // Known bug: Access violation reading location, hard to reproduce, but should pop up if you redraw the line often enough
                    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(points[0]), points.data(), GL_STATIC_DRAW);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL); // last argument is the pointer to the offset(could be 0 or null as well) to the vertex attribute
                    glEnableVertexAttribArray(0);
                }
                catch ( ... ) {
                    // this is really dangerous, idk what will happen, just print the error message, and clear the screen and hope for the best
                    std::cout << "that exception caught" << std::endl;
                    numOfPixels = 0;
                }

                // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
                // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
                glBindVertexArray(0);

                glDeleteBuffers(1, &tempVBO);
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear screen"))
            {
                numOfPixels = 0;
            }
            if (ImGui::Button("Save file"))
            {
                saveFileDialog.Open();
            }
            ImGui::SameLine();
            if (ImGui::Button("Open file"))
            {
                loadFileDialog.Open();
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

            saveFileDialog.Display();
            loadFileDialog.Display();

            if (saveFileDialog.HasSelected())
            {
                std::cout << "Creating file: " << saveFileDialog.GetSelected().string() << std::endl;
                *newLine = Line(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT);
                newLine->writeJSON(saveFileDialog.GetSelected().string(), point_size, spacing_current, clear_color, line_color, lineWidth);

                saveFileDialog.ClearSelected();
            }

            if (loadFileDialog.HasSelected())
            {
                std::cout << "Selected file: " << loadFileDialog.GetSelected().string() << std::endl;

                // this is really ugly
                *newLine = Line(pStart, pFinal, SCR_WIDTH, SCR_HEIGHT);
                if (newLine->readJSON(loadFileDialog.GetSelected().string(), pStart, pFinal, &point_size, &spacing_current, clear_color, line_color, &lineWidth))
                {
                    std::vector<float> points = newLine->createPoints(pattern[spacing_current], lineWidth); // this is 1d vector!!
                    numOfPixels = points.size() / 3; // each pixel vertex within the std::vector has 3 components

                    //float* vertices = &points[0]; // "convert" the std::vector into traditional array

                    unsigned int tempVBO;
                    glGenBuffers(1, &tempVBO);
                    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
                    glBindVertexArray(VAO);

                    glBindBuffer(GL_ARRAY_BUFFER, tempVBO);

                    try
                    {
                        // position attribute (only x, y and z component)
                        // Known bug: Access violation reading location, hard to reproduce, but should pop up if you redraw the line often enough
                        //glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vertices), vertices, GL_STATIC_DRAW);
                        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(points[0]), points.data(), GL_STATIC_DRAW);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL); // last argument is the pointer to the offset (could be 0 or null as well) to the vertex attribute
                        glEnableVertexAttribArray(0);
                    }
                    catch (...) {
                        // this is really dangerous, idk what will happen, just print the error message, and clear the screen and hope for the best
                        std::cout << "that exception caught" << std::endl;
                        numOfPixels = 0;
                    }

                    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
                    glBindBuffer(GL_ARRAY_BUFFER, 0);

                    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
                    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
                    glBindVertexArray(0);

                    glDeleteBuffers(1, &tempVBO);
                }
                loadFileDialog.ClearSelected();
            }
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
    glDeleteProgram(ourShader.ID);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete newLine;

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

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        double x;
        double y;
        glfwGetCursorPos(window, &x, &y);

        // opengl set the coordinate from bottom left while glfw is from top left smh
        pStart[1] = -1 * (floor(y)) + SCR_HEIGHT;
        pStart[0] = floor(x);
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        double x;
        double y;
        glfwGetCursorPos(window, &x, &y);

        // opengl set the coordinate from bottom left while glfw is from top left smh
        pFinal[1] = -1.0 * (floor(y)) + SCR_HEIGHT;
        pFinal[0] = floor(x);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

//void mouse_callback(GLFWwindow* window, int button, int action, int mods)
//{
//    // if i use mouse button, clicking anything on imgui window will trigger it as well
//    if (button == GLFW_MOUSE_BUTTON_LEFT) {
//        double x;
//        double y;
//        glfwGetCursorPos(window, &x, &y);
//
//        // opengl set the coordinate from bottom left while glfw is from top left smh
//        pStart[1] = -1 * (floor(y)) + SCR_HEIGHT;
//        pStart[0] = floor(x);
//    }
//
//    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
//        double x;
//        double y;
//        glfwGetCursorPos(window, &x, &y);
//
//        // opengl set the coordinate from bottom left while glfw is from top left smh
//        pFinal[1] = -1 * (floor(y)) + SCR_HEIGHT;
//        pFinal[0] = floor(x);
//    }
//}