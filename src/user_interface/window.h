
#pragma once

#include <GLFW/glfw3.h>

#include <cstddef>


/******************************************************************************/
/* ApplicationWindow */
/******************************************************************************/

class ApplicationWindow {
    public:
        explicit ApplicationWindow(
                const char* name,
                std::size_t width,
                std::size_t height
        );

        ~ApplicationWindow();

        [[nodiscard]] auto valid() const noexcept -> bool;

        auto set_vsync(bool mode) noexcept -> void;

        auto make_current_context() noexcept -> void;

        auto swap_buffers() noexcept -> void;

        [[nodiscard]] auto should_close() const noexcept -> bool;

        [[nodiscard]] auto is_key_pressed(int glfw_key) const noexcept -> bool;

        auto frame_buff_size(
                std::size_t& width,
                std::size_t& height
        ) const -> void;

        [[nodiscard]] auto window() const noexcept -> GLFWwindow*;

        ApplicationWindow(const ApplicationWindow& other) = delete;
        ApplicationWindow(ApplicationWindow&& other) = delete;
        ApplicationWindow& operator=(const ApplicationWindow& other) = delete;
        ApplicationWindow& operator=(ApplicationWindow&& other) = delete;

    private:
        GLFWwindow* window_;

};

