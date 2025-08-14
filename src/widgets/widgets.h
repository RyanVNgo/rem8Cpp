/*  @file   widgets.h
 *  @author Ryan V. Ngo
 */

#pragma once

#include <GLFW/glfw3.h>

#include <vector>


class IWidget {
  public:
    virtual void render() = 0;
};

class WidgetRunner {
  public:
    WidgetRunner(GLFWwindow* window);
    ~WidgetRunner();

    void render();

    WidgetRunner(const WidgetRunner& other) = delete;
    WidgetRunner(WidgetRunner&& other) = delete;
    WidgetRunner& operator=(const WidgetRunner& other) = delete;
    WidgetRunner& operator=(WidgetRunner&& other) = delete;

  private:
    std::vector<IWidget*> m_widgets;

};


