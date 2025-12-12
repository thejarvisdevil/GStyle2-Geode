#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <filesystem>
#include "menus/MyPacks.hpp"
#include "menus/DownloadPacks.hpp"
#include "menus/Credits.hpp"
#include "menus/Developer.hpp"
#include "gs2/for.hpp"

using namespace geode::prelude;
namespace fs = std::filesystem;

void checkPacks() {
    auto packsDir = Mod::get()->getConfigDir(true) / "packs";

    std::error_code ec;
    if (!fs::exists(packsDir, ec)) {
        fs::create_directory(packsDir, ec);
    }

    size_t count = 0;
    std::error_code bruhmoment;
    for (fs::directory_iterator it(packsDir, bruhmoment); it != fs::directory_iterator(); it.increment(bruhmoment)) {
        if (bruhmoment) { continue; }
        const auto &entry = *it;
        std::error_code bruhmoment2;
        if (!entry.is_directory(bruhmoment2) || bruhmoment2) continue;

        auto filename = gs2::pathToString(entry.path().filename());

        std::error_code f1, f2, f3;
        bool fileone = fs::exists(entry.path() / "main.gstyle2", f1) && !f1;
        bool filetwo = fs::exists(entry.path() / "banner.png", f2) && !f2;
        bool filethethird = fs::exists(entry.path() / "info.txt", f3) && !f3;

        if (fileone && filetwo && filethethird) {
            log::info("Found GStyle2 Pack: '{}'", filename);
            ++count;
        }
    }

    log::info("Total packs installed: {}", count);
}

class GStyleLayer : public cocos2d::CCLayer {
public:
    CREATE_FUNC(GStyleLayer);
    CCNode* packsMenu = nullptr;
    CCNode* dlMenu = nullptr;
    CCNode* creditsMenu = nullptr;
    CCNode* developerMenu = nullptr;

    CCMenuItemSpriteExtra* sideButton(const char* spr, SEL_MenuHandler sel, float px, float py, const char* id, CCMenu* menu) {
        auto s = CCSprite::create(spr);
        if (s) s->setScale(0.4f);
        auto b = CCMenuItemSpriteExtra::create(s, nullptr, this, sel);
        b->setPosition({px, py});
        b->setID(id);
        menu->addChild(b);
        return b;
    }

    bool init() override {
        if (!CCLayer::init()) return false;
        this->setKeypadEnabled(true);
        auto win = CCDirector::get()->getWinSize();

        if (!Mod::get()->getSettingValue<bool>("no_bg")) {
            auto bg = CCSprite::create("background.gif"_spr);
            if (bg) {
                bg->setPosition({win.width / 2.0f, win.height / 3.f});
                bg->setScale(3.f);
                bg->setOpacity(20);
                bg->setID("background"_spr);
                addChild(bg, -100);
            }
        }

        if (!Mod::get()->getSettingValue<bool>("no_bgm")) {
            FMODAudioEngine::sharedEngine()->playMusic("SlideshowScenic.mp3"_spr, true, 1.0f, 0);
        }

        auto discord = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
        if (discord) {
            auto discordBtn = CCMenuItemSpriteExtra::create(discord, nullptr, this, menu_selector(GStyleLayer::onDiscord));
            discordBtn->setPosition({win.width - 40, win.height - 40});
            discordBtn->setID("discord"_spr);
            auto menu = CCMenu::createWithItem(discordBtn);
            menu->setPosition({0,0});
            menu->setID("discord-menu"_spr);
            addChild(menu, 10);
        }

        auto sidebar = CCDrawNode::create();
        if (sidebar) {
            sidebar->drawRect({0,0}, {win.width * 0.10f, win.height}, {0,0,0.1f,1}, 0, {0,0,1,1});
            sidebar->setID("sidebar"_spr);
            addChild(sidebar, -100);
        }

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        menu->setID("sidebar-menu"_spr);
        addChild(menu, 10);

        sideButton("back.png"_spr, menu_selector(GStyleLayer::onBack), 20.f, win.height - 20.f, "back"_spr, menu);
        sideButton("mypacks.png"_spr, menu_selector(GStyleLayer::onMyPacks), 25.f, win.height - 70.f, "mypacks"_spr, menu);
        sideButton("download.png"_spr, menu_selector(GStyleLayer::onDownload), 25.f, win.height - 120.f, "download"_spr, menu);
        sideButton("settings.png"_spr, menu_selector(GStyleLayer::onSettings), 25.f, win.height - 170.f, "settings"_spr, menu);
        sideButton("developer.png"_spr, menu_selector(GStyleLayer::onDeveloper), 25.f, win.height - 220.f, "developer"_spr, menu);
        sideButton("credits.png"_spr, menu_selector(GStyleLayer::onCredits), 25.f, win.height - 270.f, "credits"_spr, menu);

        packsMenu = menus::make_packs(win);
        addChild(packsMenu, 5);
        if (packsMenu) packsMenu->setID("packs-menu"_spr);

        dlMenu = menus::make_downloads(win);
        addChild(dlMenu, 5);
        if (dlMenu) dlMenu->setID("download-menu"_spr);

        creditsMenu = menus::make_credits(win);
        addChild(creditsMenu, 5);
        if (creditsMenu) creditsMenu->setID("credits-menu"_spr);

        developerMenu = menus::make_developer(win);
        addChild(developerMenu, 5);
        if (developerMenu) developerMenu->setID("developer-menu"_spr);

        packsMenu->setVisible(true);
        dlMenu->setVisible(false);
        creditsMenu->setVisible(false);
        developerMenu->setVisible(false);

        Bro(packsMenu, true);
        Bro(dlMenu, false);
        Bro(creditsMenu, false);
        Bro(developerMenu, false);

        if (!Mod::get()->getSettingValue<bool>("no_warn")) {
            runAction(CCSequence::create(CCDelayTime::create(1.0f), CCCallFunc::create(this, callfunc_selector(GStyleLayer::showWarnings)), nullptr));
        }

        return true;
    }

    void onEnter() override {
        CCLayer::onEnter();
        runAction(CCSequence::create(CCDelayTime::create(0.05f), CCCallFunc::create(this, callfunc_selector(GStyleLayer::Broo)), nullptr));
    }

    void Broo() {
        Bro(packsMenu, packsMenu && packsMenu->isVisible());
        Bro(dlMenu, dlMenu && dlMenu->isVisible());
        Bro(creditsMenu, creditsMenu && creditsMenu->isVisible());
        Bro(developerMenu, developerMenu && developerMenu->isVisible());
    }

    void Bro(CCNode* node, bool active) {
        if (!node) return;
        auto children = node->getChildren();
        if (!children) return;
        CCObject* obj = nullptr;
        CCARRAY_FOREACH(children, obj) {
            auto child = typeinfo_cast<CCNode*>(obj);
            if (!child) continue;
            if (auto pg = typeinfo_cast<menus::PacksGrid*>(child)) {
                if (pg) pg->setActiveLayer(active);
            }
            if (auto dg = typeinfo_cast<menus::DLPacks*>(child)) {
                if (dg) dg->setActiveLayer(active);
            }
            Bro(child, active);
        }
    }

    void onMyPacks(CCObject*) {
        packsMenu->setVisible(true);
        dlMenu->setVisible(false);
        creditsMenu->setVisible(false);
        developerMenu->setVisible(false);
        Bro(packsMenu, true);
        Bro(dlMenu, false);
        Bro(creditsMenu, false);
        Bro(developerMenu, false);
    }

    void onDownload(CCObject*) {
        packsMenu->setVisible(false);
        dlMenu->setVisible(true);
        creditsMenu->setVisible(false);
        developerMenu->setVisible(false);
        Bro(packsMenu, false);
        Bro(dlMenu, true);
        Bro(creditsMenu, false);
        Bro(developerMenu, false);
    }

    void onCredits(CCObject*) {
        packsMenu->setVisible(false);
        dlMenu->setVisible(false);
        creditsMenu->setVisible(true);
        developerMenu->setVisible(false);
        Bro(packsMenu, false);
        Bro(dlMenu, false);
        Bro(creditsMenu, true);
        Bro(developerMenu, false);
    }

    void onDeveloper(CCObject*) {
        packsMenu->setVisible(false);
        dlMenu->setVisible(false);
        creditsMenu->setVisible(false);
        developerMenu->setVisible(true);
        Bro(packsMenu, false);
        Bro(dlMenu, false);
        Bro(creditsMenu, false);
        Bro(developerMenu, true);
    }

    void onBack(CCObject*) {
        GameManager::get()->fadeInMenuMusic();
        CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }

    void keyBackClicked() { onBack(nullptr); }

    void onDiscord(CCObject*) {
        geode::utils::web::openLinkInBrowser("https://dsc.gg/devlin");
    }

    void onSettings(CCObject*) {
        geode::openSettingsPopup(Mod::get(), true);
    }

    void showWarnings() {
        geode::createQuickPopup("Warning (2/2)", "Geode / GStyle2 <cr>IS NOT</c> responsible for any issues. Do not ask for support on Geode's #help.\nIf you have issues with a pack, manually delete it and restart or ask for help on my Discord.\n<cb>Hide this by toggling \"I Understand\" in settings</c>", "OK", nullptr, 400.f, nullptr, true, true);
        geode::createQuickPopup("Warning (1/2)", "This mod modifies and creates many things.\nPacks here are community-made and verified.\nPlease <cr>DO NOT</c> download packs not listed here as they might cause issues.\n", "OK", nullptr, 400.f, nullptr, true, true);
    }

    static CCScene* scene() {
        auto s = CCScene::create();
        s->addChild(GStyleLayer::create());
        return s;
    }
};

class $modify(GSMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        auto menu = getChildByID("bottom-menu");
        auto spr = CCSprite::create("circle.png"_spr);
        spr->setScale(0.8f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(GSMenuLayer::onGStyle));
        btn->setID("circle"_spr);
        menu->addChild(btn);
        menu->updateLayout();
        return true;
    }

    void onGStyle(CCObject*) {
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, GStyleLayer::scene()));
    }
};

$execute {
    try {
        checkPacks();
    } catch (const std::exception &e) {
        geode::log::error("checkPacks sucks: {}", e.what());
    } catch (...) {
        geode::log::error("checkPacks sucks idk");
    }
}