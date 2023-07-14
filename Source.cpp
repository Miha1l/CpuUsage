#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <Windows.h>
#include "Cpu.h"
#include "Core.h"

bool isRender = true;

GLuint createShaderProgram(std::string vertexPath, std::string fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vertexFile(vertexPath);
    std::ifstream fragmentFile(fragmentPath);

    std::stringstream vertexStream, fragmentStream;
    vertexStream << vertexFile.rdbuf();
    fragmentStream << fragmentFile.rdbuf();

    vertexFile.close();
    fragmentFile.close();

    vertexCode = vertexStream.str();
    fragmentCode = fragmentStream.str();

    const char* vertexShaderCode = vertexCode.c_str();
    const char* fragmentShaderCode = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

DWORD WINAPI updateCpuUsageValues(LPVOID p) {
    Worker* worker = (Worker*)p;
    while (isRender) {
        worker->calculateUsage();
        Sleep(1000);
    }
    return 0;
}

void resizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(void)
{
    int coresCount = std::thread::hardware_concurrency();
    int countElements = coresCount + 1;

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 800, "TestTask", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint shaderProgram = createShaderProgram("./VertexShader.hlsl", "./FragmentShader.hlsl");

    std::vector<Worker*> workers;
    std::vector<HANDLE> threads;

    for (int i = 0; i < coresCount; i++) {
        workers.push_back(new Core(i));
        threads.push_back(CreateThread(NULL, 0, updateCpuUsageValues, workers.back(), 0, NULL));
    }
    workers.push_back(new Cpu());
    threads.push_back(CreateThread(NULL, 0, updateCpuUsageValues, workers.back(), 0, NULL));

    std::vector<GLuint> vboGraphArray(countElements);
    std::vector<GLuint> vaoGraphArray(countElements);

    glGenVertexArrays(countElements, vaoGraphArray.data());
    glGenBuffers(countElements, vboGraphArray.data());
    for (int i = 0; i < countElements; i++) {
        glBindVertexArray(vaoGraphArray[i]);

        glBindBuffer(GL_ARRAY_BUFFER, vboGraphArray[i]);
        glBufferData(GL_ARRAY_BUFFER, workers[i]->getUsageValues().getBufferSize(), NULL, GL_STREAM_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(2 * sizeof(GL_FLOAT)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    std::vector<GLuint> vboFrameArray(countElements);
    std::vector<GLuint> vaoFrameArray(countElements);

    glGenVertexArrays(countElements, vboFrameArray.data());
    glGenBuffers(countElements, vaoFrameArray.data());
    for (int i = 0; i < countElements; i++) {
        glBindVertexArray(vaoFrameArray[i]);

        glBindBuffer(GL_ARRAY_BUFFER, vboFrameArray[i]);
        glBufferData(GL_ARRAY_BUFFER, workers[i]->getFramePoints().getBufferSize(), workers[i]->getFramePoints().getBufferPointer(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(2 * sizeof(GL_FLOAT)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glUseProgram(shaderProgram);

        for (int i = 0; i < countElements; i++) {
            glBindVertexArray(vaoGraphArray[i]);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, vboGraphArray[i]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, workers[i]->getUsageValues().getElementsCount() * sizeof(Point), workers[i]->getUsageValues().getBufferPointer());
            glDrawArrays(GL_LINE_STRIP, 0, workers[i]->getUsageValues().getElementsCount());
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(0);
            glBindVertexArray(0);

            glBindVertexArray(vaoFrameArray[i]);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, vboFrameArray[i]);
            glDrawArrays(GL_LINE_LOOP, 0, workers[i]->getFramePoints().getElementsCount());
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(0);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(countElements, vaoGraphArray.data());
    glDeleteBuffers(countElements, vboGraphArray.data());

    glDeleteVertexArrays(countElements, vaoFrameArray.data());
    glDeleteBuffers(countElements, vboFrameArray.data());

    isRender = false;
    WaitForMultipleObjects(countElements, threads.data(), TRUE, INFINITE);
    for (int i = 0; i < countElements; i++) {
        CloseHandle(threads[i]);
    }

    for (int i = 0; i < countElements; i++) {
        delete workers[i];
    }

    glfwTerminate();
    return 0;
}