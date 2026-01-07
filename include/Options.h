#pragma once
#include <string>

struct Options {
    float music_volume = 100.f;
    bool music_muted = false;
    int width = 1280;
    int height = 720;
    bool fullscreen = false;
};

Options load_options(const std::string& filename);
void save_options(const Options& opts, const std::string& filename);
