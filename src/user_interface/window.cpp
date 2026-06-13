
#include "window.h"


/******************************************************************************/
/* ApplicationWindow */
/******************************************************************************/

ApplicationWindow::ApplicationWindow(
        const char* name,
        std::size_t width,
        std::size_t height
) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window_ = glfwCreateWindow(width, height, name, NULL, NULL);
}

ApplicationWindow::~ApplicationWindow() {
    glfwDestroyWindow(window_);
}

auto ApplicationWindow::valid() const noexcept -> bool {
    if (!window_) {
        return false;
    }
    return true;
}

auto ApplicationWindow::set_vsync(bool mode) noexcept -> void {
    if (mode) {
        glfwSwapInterval(1);
        return;
    }
    glfwSwapInterval(0);
}

auto ApplicationWindow::make_current_context() noexcept -> void {
    glfwMakeContextCurrent(window_);
}

auto ApplicationWindow::swap_buffers() noexcept -> void {
    glfwSwapBuffers(window_);
}

auto ApplicationWindow::should_close() const noexcept -> bool {
    return glfwWindowShouldClose(window_);
}

auto ApplicationWindow::is_key_pressed(int glfw_key) const noexcept -> bool {
    return glfwGetKey(window_, glfw_key) == GLFW_PRESS;
}

auto ApplicationWindow::frame_buff_size(
        std::size_t& width,
        std::size_t& height
) const -> void {
    int w{};
    int h{};
    glfwGetFramebufferSize(window_, &w, &h);
    width = w;
    height = h;
}

[[nodiscard]] auto ApplicationWindow::window() const noexcept -> GLFWwindow* {
    return window_;
}

