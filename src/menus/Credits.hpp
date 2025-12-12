#pragma once
#include <Geode/Geode.hpp>

namespace menus {
    inline cocos2d::CCNode* make_credits(const cocos2d::CCSize& win) {
        auto node = cocos2d::CCNode::create();
        node->setID("credits-stuff"_spr);

        if (Mod::get()->getSettingValue<bool>("alt_font")) {
            auto label = cocos2d::CCLabelBMFont::create("Credits", "bigFont.fnt");
            label->setPosition({win.width/2.0f, win.height-50.f});
            label->setID("credits-title"_spr);
            node->addChild(label);
        } else {
            auto label = cocos2d::CCLabelBMFont::create("Credits", "BitTrip.fnt"_spr);
            label->setPosition({win.width/2.0f, win.height-50.f});
            label->setID("credits-title"_spr);
            node->addChild(label);
        }

        auto c1 = cocos2d::CCLabelBMFont::create("Background by: Theodoros Tziatzios @ Adobe Community", "Roboto.fnt"_spr);
        c1->setScale(0.7f);
        c1->setPosition({ win.width / 2.0f, win.height - 100.f});
        c1->setID("credit1"_spr);
        node->addChild(c1);

        auto c2 = cocos2d::CCLabelBMFont::create("BGM - \"Slideshow (Scenic)\" by Nintendo [Photo Channel OST]", "Roboto.fnt"_spr);
        c2->setScale(0.7f);
        c2->setPosition({ win.width / 2.0f, win.height - 130.f});
        c2->setID("credit2"_spr);
        node->addChild(c2);

        auto c3 = cocos2d::CCLabelBMFont::create("BitTrip.ttf Font created by Choice Provisions", "Roboto.fnt"_spr);
        c3->setScale(0.8f);
        c3->setPosition({ win.width / 2.0f, win.height - 160.f});
        c3->setID("credit3"_spr);
        node->addChild(c3);

        auto c4 = cocos2d::CCLabelBMFont::create("Other Fonts by Google Fonts", "Roboto.fnt"_spr);
        c4->setPosition({ win.width / 2.0f, win.height - 190.f});
        c4->setID("credit4"_spr);
        node->addChild(c4);

        auto c5 = cocos2d::CCLabelBMFont::create("GStyle2 by jarvisdevil", "Roboto.fnt"_spr);
        c5->setPosition({ win.width / 2.0f, win.height - 220.f});
        c5->setColor({ 0, 255, 255 });
        c5->setID("credit5"_spr);
        node->addChild(c5);

        auto c6 = cocos2d::CCLabelBMFont::create("Old BGM - \"Star Striker\" by Cody O'Quinn", "Roboto.fnt"_spr);
        c6->setScale(0.3f);
        c6->setPosition({ win.width / 2.0f, win.height - 250.f});
        c6->setColor({ 0, 255, 0 });
        c6->setID("credit6"_spr);
        node->addChild(c6);
        
        auto c7 = cocos2d::CCLabelBMFont::create("Beta Testers: 6Yt", "Roboto.fnt"_spr);
        c7->setScale(0.3f);
        c7->setPosition({ win.width / 2.0f, win.height - 260.f});
        c7->setColor({ 0, 255, 0 });
        c7->setID("credit7"_spr);
        node->addChild(c7);

        return node;
    }
}