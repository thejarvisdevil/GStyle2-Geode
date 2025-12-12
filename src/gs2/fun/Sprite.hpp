#pragma once
#include "Common.hpp"

namespace gs2::fun {
    static void spriteCreate(const std::vector<std::string>& args) {
        if (args.size() < 5) { gs2::fail("sprite.create expects 5 args"); return; }
        std::string image = args[0];
        float scale = gs2::numFromString<float>(args[1]);
        float x = gs2::numFromString<float>(args[2]);
        float y = gs2::numFromString<float>(args[3]);
        std::string id = args[4];
        auto packdir = getPackDir();
        if (packdir.empty()) { gs2::fail("no pack directory"); return; }
        fs::path img = packdir / "sprites" / image;
        if (!fs::exists(img)) { gs2::fail(std::string("sprite file not found: ") + gs2::pathToString(img)); return; }
        cocos2d::CCSprite* spr = nullptr;
        cocos2d::CCNode* node = nullptr;
        try {
            auto g = CCSprite::create(gs2::pathToString(img).c_str());
            g->setScale(scale);
            g->setPosition({x,y});
            if (g) { node = g; }
        } catch(...) {}
        if (!node) {
            spr = cocos2d::CCSprite::create(gs2::pathToString(img).c_str());
            if (!spr) { gs2::fail(std::string("failed to create sprite: ") + gs2::pathToString(img)); return; }
            spr->setScale(scale);
            spr->setPosition({x,y});
            node = spr;
        }

    if (!id.empty()) try { auto pid = prefixID(id); node->setID(pid.c_str()); } catch(...){ }
        auto root = currentScene(); if (root) root->addChild(node);
    }

    inline void register_sprite() {
        gs2::registerCMD("sprite.create", spriteCreate);
    }
}