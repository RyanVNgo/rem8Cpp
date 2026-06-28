
#pragma once

#include <GL/glew.h>


/******************************************************************************/
/* Texture */
/******************************************************************************/

class Texture {
    public:
        explicit Texture(std::size_t width, std::size_t height);

        ~Texture();

        auto update(
                std::size_t x_offset,
                std::size_t y_offset,
                std::size_t width,
                std::size_t height,
                const uint8_t* data
        ) noexcept -> void;

        auto bind() const noexcept -> void;

        auto unbind() const noexcept -> void;

        Texture(const Texture& other) = delete;
        Texture(Texture&& other) = delete;
        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) = delete;

    private:
        GLuint id_{0};

};


/******************************************************************************/
/* General GL Calls */
/******************************************************************************/

auto update_viewport(std::size_t width, std::size_t height) noexcept -> void;

auto clear() noexcept -> void;

auto draw_texture(const Texture& tex) noexcept -> void;


