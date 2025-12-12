#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/MDTextArea.hpp>

namespace menus {
    inline cocos2d::CCNode* make_developer(const cocos2d::CCSize& win) {
        auto node = cocos2d::CCNode::create();
        node->setID("developer-stuff"_spr);

        if (Mod::get()->getSettingValue<bool>("alt_font")) {
            auto label = cocos2d::CCLabelBMFont::create("Developers", "bigFont.fnt");
            label->setPosition({win.width/2.0f, win.height-50.f});
            label->setID("developer-title"_spr);
            node->addChild(label);
        } else {
            auto label = cocos2d::CCLabelBMFont::create("Developers", "BitTrip.fnt"_spr);
            label->setPosition({win.width/2.0f, win.height-50.f});
            label->setID("developer-title"_spr);
            node->addChild(label);
        }

        auto infoText = MDTextArea::create(
            "## Hello kind person!\n"
            "If you are interested in building GStyle2 Packs, you can check out the [GStyle2 Development Guide](https://gs2.jarvisdevil.com/).  \n\n"
            "You can find your packs folder by going to the folder containing Geometry Dash then navigating to: `geode / config / jarvisdevil.gstyle2 / packs` \n\n"
            "<cy>GStyle2 is still a work in progress and will get better on each update.</c> Happy pack building!\n\n",
            { win.width * 0.7f, win.height * 0.7f }
        );
        infoText->setPosition({ win.width / 2.0f, win.height / 2.0f - 20.f });
        infoText->setID("developer-textarea"_spr);
        node->addChild(infoText);

        return node;
    }
}