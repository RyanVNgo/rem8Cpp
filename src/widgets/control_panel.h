
#pragma once

#include <filesystem>
#include <chrono>

#include "imgui.h"

#include "widgets.h"
#include "emulator.h"
#include "file_explorer.h"


class ControlPanel : public IWidget {
    public:
        explicit ControlPanel(rem8Cpp& emulator);

        auto render() noexcept -> void override;

        [[nodiscard]] auto pause() const noexcept -> bool;

        [[nodiscard]] auto load_addr() const noexcept -> uint16_t;

        [[nodiscard]] auto start_addr() const noexcept -> uint16_t;

        [[nodiscard]] auto clock_rate() const noexcept -> int;

        [[nodiscard]] auto reload() const noexcept -> bool;

        [[nodiscard]] auto get_selected_rom() const noexcept -> std::filesystem::path;

        auto unset_reload() noexcept -> void;

    private:
        rem8Cpp& emulator_;
        ImGuiIO& io_;
        FileExplorer file_explorer_;
        std::chrono::high_resolution_clock::time_point time_last_;
        std::chrono::high_resolution_clock::time_point time_curr_;
        float framerate_;
        bool pause_;
        std::filesystem::path selected_rom_;
        uint16_t load_addr_;
        uint16_t start_addr_;
        int clock_rate_;

        bool reload_;

};


