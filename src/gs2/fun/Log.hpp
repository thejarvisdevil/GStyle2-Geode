#pragma once
#include "../gs2.hpp"
#include <Geode/Geode.hpp>

namespace gs2::fun {
    inline void register_log() {
        registerCMD("log.info", [](const std::vector<std::string>& args){
            if (args.empty()) return;
            geode::log::info("{}", args[0]);
        });

        registerCMD("log.warn", [](const std::vector<std::string>& args){
            if (args.empty()) return;
            geode::log::warn("{}", args[0]);
        });

        registerCMD("log.error", [](const std::vector<std::string>& args){
            if (args.empty()) return;
            geode::log::error("{}", args[0]);
        });
    }
}