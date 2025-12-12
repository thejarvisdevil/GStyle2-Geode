#pragma once
#include "Common.hpp"

namespace gs2::fun {
    static void setScale(const std::vector<std::string>& args) {
        if (args.size() < 2) { gs2::fail("set.scale expects 2 args"); return; }
        float scale = gs2::numFromString<float>(args[0]);
        std::string path = args[1];
        auto node = findNode(path); if (!node) { gs2::fail(std::string("node not found: ") + path); return; }
        node->setScale(scale);
    }

    static void setPosition(const std::vector<std::string>& args) {
        if (args.size() < 3) { gs2::fail("set.position expects 3 args"); return; }
        float x = gs2::numFromString<float>(args[0]); float y = gs2::numFromString<float>(args[1]); std::string path = args[2];
        auto node = findNode(path); if (!node) { gs2::fail(std::string("node not found: ") + path); return; }
        node->setPosition({x,y});
    }

    static void setX(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.x expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setPositionX(gs2::numFromString<float>(args[0])); }
    static void setY(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.y expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setPositionY(gs2::numFromString<float>(args[0])); }
    static void setScaleX(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.scaleX expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setScaleX(gs2::numFromString<float>(args[0])); }
    static void setScaleY(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.scaleY expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setScaleY(gs2::numFromString<float>(args[0])); }
    static void setRotation(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.rotation expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setRotation(gs2::numFromString<float>(args[0])); }
    static void setSkewX(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.skewX expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setSkewX(gs2::numFromString<float>(args[0])); }
    static void setSkewY(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.skewY expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setSkewY(gs2::numFromString<float>(args[0])); }
    static void setAnchorX(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.anchorX expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setAnchorPoint({gs2::numFromString<float>(args[0]), node->getAnchorPoint().y}); }
    static void setAnchorY(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.anchorY expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setAnchorPoint({node->getAnchorPoint().x, gs2::numFromString<float>(args[0])}); }
    static void setZLayer(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.z expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setZOrder((int)gs2::numFromString<int>(args[0])); }
    static void setVisible(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.visible expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setVisible(args[0]=="true" || args[0]=="1"); }
    static void setContentWidth(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.contentWidth expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setContentSize({gs2::numFromString<float>(args[0]), node->getContentSize().height}); }
    static void setContentHeight(const std::vector<std::string>& args) { if (args.size()<2) { gs2::fail("set.contentHeight expects 2 args"); return; } auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; } node->setContentSize({node->getContentSize().width, gs2::numFromString<float>(args[0])}); }

    static void setColor(const std::vector<std::string>& args) {
        if (args.size()<4) { gs2::fail("set.color expects 4 args"); return; }
        auto node = findNode(args[3]); if(!node) { gs2::fail("node not found"); return; }
        GLubyte r = (GLubyte)gs2::numFromString<int>(args[0]); GLubyte g = (GLubyte)gs2::numFromString<int>(args[1]); GLubyte b = (GLubyte)gs2::numFromString<int>(args[2]);
        if (auto spr = typeinfo_cast<cocos2d::CCSprite*>(node)) {
            spr->setColor({r,g,b});
            return;
        }
        if (auto lbl = typeinfo_cast<cocos2d::CCLabelBMFont*>(node)) {
            lbl->setColor({r,g,b});
            return;
        }
        gs2::fail("node does not support color");
        return;
    }

    static void setOpacity(const std::vector<std::string>& args) {
        if (args.size()<2) { gs2::fail("set.opacity expects 2 args"); return; }
        auto node = findNode(args[1]); if(!node) { gs2::fail("node not found"); return; }
        GLubyte o = (GLubyte)gs2::numFromString<int>(args[0]);
        if (auto spr = typeinfo_cast<cocos2d::CCSprite*>(node)) { spr->setOpacity(o); return; }
        if (auto lbl = typeinfo_cast<cocos2d::CCLabelBMFont*>(node)) { lbl->setOpacity(o); return; }
        gs2::fail("node does not support opacity");
        return;
    }

    inline void register_set() {
        gs2::registerCMD("set.scale", setScale);
        gs2::registerCMD("set.position", setPosition);
        gs2::registerCMD("set.x", setX);
        gs2::registerCMD("set.y", setY);
        gs2::registerCMD("set.scaleX", setScaleX);
        gs2::registerCMD("set.scaleY", setScaleY);
        gs2::registerCMD("set.rotation", setRotation);
        gs2::registerCMD("set.skewX", setSkewX);
        gs2::registerCMD("set.skewY", setSkewY);
        gs2::registerCMD("set.anchorX", setAnchorX);
        gs2::registerCMD("set.anchorY", setAnchorY);
        gs2::registerCMD("set.z", setZLayer);
        gs2::registerCMD("set.visible", setVisible);
        gs2::registerCMD("set.contentWidth", setContentWidth);
        gs2::registerCMD("set.contentHeight", setContentHeight);
        gs2::registerCMD("set.color", setColor);
        gs2::registerCMD("set.opacity", setOpacity);
    }
}