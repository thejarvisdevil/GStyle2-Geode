#pragma once
#include "Common.hpp"
#include <Geode/binding/FMODAudioEngine.hpp>

namespace gs2::fun {
    static void audioMusic(const std::vector<std::string>& args) {
        if (args.size() < 4) { gs2::fail("audio.music expects 4 args"); return; }
        std::string audio = args[0];
        bool loop = (args[1] == "true" || args[1] == "1");
        float fadeIn = gs2::numFromString<float>(args[2]);
        int channel = gs2::numFromString<int>(args[3]);
        auto packdir = getPackDir(); if (packdir.empty()) { gs2::fail("no pack dir"); return; }
        fs::path p = packdir / "audio" / audio;
        if (!fs::exists(p)) { gs2::fail(std::string("audio file not found: ") + gs2::pathToString(p)); return; }
        auto pstr = gs2::pathToString(p);
        FMODAudioEngine::sharedEngine()->playMusic(pstr.c_str(), loop, fadeIn, channel);
    }

    static void audioEffect(const std::vector<std::string>& args) {
        if (args.size() < 1) { gs2::fail("audio.effect expects 1 arg"); return; }
        std::string audio = args[0];
        auto packdir = getPackDir(); if (packdir.empty()) { gs2::fail("no pack dir"); return; }
        fs::path p = packdir / "audio" / audio;
        if (!fs::exists(p)) { gs2::fail(std::string("audio file not found: ") + gs2::pathToString(p)); return; }
        FMODAudioEngine::sharedEngine()->playEffect(gs2::pathToString(p).c_str());
    }

    static void audioStopMusic(const std::vector<std::string>& args) {
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    }

    static void audioStopEffect(const std::vector<std::string>& args) {
        FMODAudioEngine::sharedEngine()->stopAllEffects();
    }

    inline void register_audio() {
        gs2::registerCMD("audio.music", audioMusic);
        gs2::registerCMD("audio.effect", audioEffect);
        gs2::registerCMD("audio.stopMusic", audioStopMusic);
        gs2::registerCMD("audio.stopEffect", audioStopEffect);
    }
}