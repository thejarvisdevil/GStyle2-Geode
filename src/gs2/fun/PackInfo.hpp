#pragma once
#include "Common.hpp"

namespace gs2::fun {
    static void packInfo(const std::vector<std::string>& args) {
        if (args.empty()) { gs2::fail("pack.info expects a key"); return; }
        std::string key = args[0];
        auto dir = getPackDir(); if (dir.empty()) { gs2::fail("no pack dir"); return; }
        auto info = getPackInfo(dir);
        auto it = info.find(key);
        if (it == info.end()) { gs2::fail(std::string("pack.info: key not found: ") + key); return; }
        geode::log::info("pack.info {} = {}", key, it->second);
    }

    inline void register_packinfo() {
        gs2::registerCMD("pack.info", packInfo);
    }
}