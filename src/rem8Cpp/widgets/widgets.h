
#pragma once

#include <GLFW/glfw3.h>

#include <vector>


class IWidget {
    public:
        virtual auto render() noexcept -> void= 0;
};

class WidgetRunner {
    public:
        explicit WidgetRunner(GLFWwindow* window);

        ~WidgetRunner();

        auto add_widget(IWidget* widget) noexcept -> void;

        auto render() noexcept -> void;

        WidgetRunner(const WidgetRunner& other) = delete;
        WidgetRunner(WidgetRunner&& other) = delete;
        WidgetRunner& operator=(const WidgetRunner& other) = delete;
        WidgetRunner& operator=(WidgetRunner&& other) = delete;

    private:
        std::vector<IWidget*> widgets_;

};


