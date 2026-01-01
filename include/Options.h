#pragma once
#include <string>

struct Options {
    float volume = 100.f;
    bool muted = false;
    int width = 1280;
    int height = 720;
};

Options load_options(const std::string& filename);
void save_options(const Options& opts, const std::string& filename);
