#pragma once
#include "Common.hpp"

namespace gs2::fun {
    static void drawRect(const std::vector<std::string>& args) {
        if (args.size() < 8) { gs2::fail("draw.rect expects at least 8 args"); return; }
        float x = gs2::numFromString<float>(args[0]);
        float y = gs2::numFromString<float>(args[1]);
        float w = gs2::numFromString<float>(args[2]);
        float h = gs2::numFromString<float>(args[3]);
        float r = gs2::numFromString<float>(args[4]);
        float g = gs2::numFromString<float>(args[5]);
        float b = gs2::numFromString<float>(args[6]);
        float a = gs2::numFromString<float>(args[7]);
        std::string id;
        if (args.size() >= 9) id = args[8];
        auto node = cocos2d::CCDrawNode::create();
        ccColor4F bgcolor = { r, g, b, a };
        cocos2d::CCPoint bl = { x, y };
        cocos2d::CCPoint tr = { x + w, y + h };
        node->drawRect(bl, tr, bgcolor, 0.0f, bgcolor);
        if (!id.empty()) {
            try { auto pid = prefixID(id); node->setID(pid.c_str()); } catch(...){}
        }
        auto root = currentScene();
        if (root) root->addChild(node);
    }

    static void drawCircle(const std::vector<std::string>& args) {
        if (args.size() < 7) { gs2::fail("draw.circle expects at least 7 args"); return; }
        float x = gs2::numFromString<float>(args[0]);
        float y = gs2::numFromString<float>(args[1]);
        float radius = gs2::numFromString<float>(args[2]);
        float r = gs2::numFromString<float>(args[3]);
        float g = gs2::numFromString<float>(args[4]);
        float b = gs2::numFromString<float>(args[5]);
        float a = gs2::numFromString<float>(args[6]);
        std::string id;
        if (args.size() >= 8) id = args[7];
        auto node = cocos2d::CCDrawNode::create();
        ccColor4F col = { r, g, b, a };
        cocos2d::CCPoint pos = { x, y };
        try { node->drawDot(pos, radius, col); } catch(...) {}

    if (!id.empty()) try { auto pid = prefixID(id); node->setID(pid.c_str()); } catch(...){}
        auto root = currentScene(); if (root) root->addChild(node);
    }

    inline void register_draw() {
        gs2::registerCMD("draw.rect", drawRect);
        gs2::registerCMD("draw.circle", drawCircle);
    }
}