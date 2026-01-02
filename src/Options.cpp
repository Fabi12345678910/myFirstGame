#include "Options.h"
#include <fstream>
#include <nlohmann/json.hpp>

Options load_options(const std::string& filename) {
    Options opts;
    std::ifstream in(filename);
    if (in) {
        nlohmann::json j;
        in >> j;
        opts.music_volume = j.value("music_volume", 100.f);
        opts.music_muted = j.value("music_muted", false);
        auto res = j.value("resolution", std::vector<int>{1280, 720});
        if (res.size() == 2) {
            opts.width = res[0];
            opts.height = res[1];
        }
        opts.fullscreen = j.value("fullscreen", false);
    }
    return opts;
}

void save_options(const Options& opts, const std::string& filename) {
    nlohmann::json j;
    j["music_volume"] = opts.music_volume;
    j["music_muted"] = opts.music_muted;
    j["resolution"] = {opts.width, opts.height};
    j["fullscreen"] = opts.fullscreen;
    std::ofstream out(filename);
    out << j.dump(4);
}
