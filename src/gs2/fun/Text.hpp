#pragma once
#include "Common.hpp"

namespace gs2::fun {
    static void textCreate(const std::vector<std::string>& args) {
        if (args.size() < 6) { gs2::fail("text.create expects 6 args"); return; }
        std::string text = args[0];
        float scale = gs2::numFromString<float>(args[1]);
        float x = gs2::numFromString<float>(args[2]);
        float y = gs2::numFromString<float>(args[3]);
        std::string font = args[4];
        std::string id = args[5];
        std::string fontFile = "bigFont.fnt";
        if (font == "ArchivoBlack") fontFile = "ArchivoBlack"_spr;
        else if (font == "BitcountGridDouble") fontFile = "BitcountGridDouble"_spr;
        else if (font == "BitTrip") fontFile = "BitTrip"_spr;
        else if (font == "Oswald") fontFile = "Oswald"_spr;
        else if (font == "Roboto") fontFile = "Roboto"_spr;
        else if (font == "ZalandoSansExpanded") fontFile = "ZalandoSansExpanded"_spr;
        else if (font == "bigFont") fontFile = "bigFont";
        auto label = cocos2d::CCLabelBMFont::create(text.c_str(), (fontFile + ".fnt").c_str());
        if (!label) label = cocos2d::CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
        label->setScale(scale);
        label->setPosition({x,y});
    if (!id.empty()) try { auto pid = prefixID(id); label->setID(pid.c_str()); } catch(...){ }
        auto root = currentScene(); if (root) root->addChild(label);
    }

    inline void register_text() {
        gs2::registerCMD("text.create", textCreate);
    }
}