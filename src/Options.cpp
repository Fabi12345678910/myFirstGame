#include "Options.h"
#include <fstream>
#include <nlohmann/json.hpp>

Options load_options(const std::string& filename) {
    Options opts;
    std::ifstream in(filename);
    if (in) {
        nlohmann::json j;
        in >> j;
        opts.volume = j.value("volume", 100.f);
        opts.muted = j.value("muted", false);
        auto res = j.value("resolution", std::vector<int>{1280, 720});
        if (res.size() == 2) {
            opts.width = res[0];
            opts.height = res[1];
        }
    }
    return opts;
}

void save_options(const Options& opts, const std::string& filename) {
    nlohmann::json j;
    j["volume"] = opts.volume;
    j["muted"] = opts.muted;
    j["resolution"] = {opts.width, opts.height};
    std::ofstream out(filename);
    out << j.dump(4);
}
