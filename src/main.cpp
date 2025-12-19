#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/cocos.hpp>
#include <filesystem>
#include "menus/MyPacks.hpp"
#include "menus/DownloadPacks.hpp"
#include "menus/Credits.hpp"
#include "menus/Developer.hpp"
#include "gs2/for.hpp"
#include "gs2/gs2.hpp"

// wip:
// fix certain textures such as loading textures from not loading gs2 pack textures/
// make it refresh textures if needed when going outside of gs2 menu

using namespace geode::prelude;
namespace fs = std::filesystem;

void checkPacks() {
    auto packsDir = Mod::get()->getConfigDir(true) / "packs";

    std::error_code ec;
    if (!fs::exists(packsDir, ec)) {
        fs::create_directory(packsDir, ec);
    }

    auto enabled = gs2::getEnabled();
    std::vector<std::string> yippeePacks;
    for (auto &p : enabled) yippeePacks.push_back(p.second);
    auto kanye = Mod::get()->getConfigDir(true) / "active_packs.txt";
    if (fs::exists(kanye) && yippeePacks.empty()) {
        return;
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
            try {
                if (std::find(yippeePacks.begin(), yippeePacks.end(), filename) == yippeePacks.end()) continue;

                auto texdir = entry.path() / "textures";
                std::error_code texec;
                if (fs::exists(texdir, texec) && fs::is_directory(texdir, texec) && !texec) {
                    CCFileUtils::get()->addTexturePack(CCTexturePack{
                        .m_id = filename,
                        .m_paths = { gs2::pathToString(texdir) }
                    });
                    log::info("Applied custom textures from pack: '{}'", filename);
                }
            } catch (const std::exception &e) {
                log::warn("Failed to apply textures for pack '{}': {}", filename, e.what());
            } catch (...) {
                log::warn("Failed to apply textures for pack '{}': unknown error", filename);
            }
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
            CCSprite* bg = nullptr;

            if (Mod::get()->getSettingValue<bool>("custom_bg")) {
                auto bgpng = Mod::get()->getConfigDir(true) / "background.png";
                auto bggif = Mod::get()->getConfigDir(true) / "background.gif";

                if (std::filesystem::exists(bgpng)) {
                    bg = CCSprite::create(geode::utils::string::pathToString(bgpng).c_str());
                    if (bg) {
                        bg->setPosition({ win.width / 2.f, win.height / 2.f });
                        bg->setScale(3.f);
                        bg->setID("background"_spr);
                        addChild(bg, -100);
                    }
                } else if (std::filesystem::exists(bggif)) {
                    bg = CCSprite::create(geode::utils::string::pathToString(bggif).c_str());
                    if (bg) {
                        bg->setPosition({ win.width / 2.f, win.height / 2.f });
                        bg->setScale(3.f);
                        bg->setID("background"_spr);
                        addChild(bg, -100);
                    }
                } else {
                    bg = CCSprite::create("background.gif"_spr);
                    if (bg) {
                        bg->setPosition({ win.width / 2.f, win.height / 3.f });
                        bg->setScale(3.f);
                        bg->setOpacity(20);
                        bg->setID("background"_spr);
                        addChild(bg, -100);
                    }
                }
            } else {
                bg = CCSprite::create("background.gif"_spr);
                if (bg) {
                    bg->setPosition({ win.width / 2.f, win.height / 3.f });
                    bg->setScale(3.f);
                    bg->setOpacity(20);
                    bg->setID("background"_spr);
                    addChild(bg, -100);
                }
            }
        }

        if (!Mod::get()->getSettingValue<bool>("no_bgm")) {
            if (Mod::get()->getSettingValue<bool>("custom_bgm")) {
                if (std::filesystem::exists(Mod::get()->getConfigDir(true) / "music.mp3")) {
                    FMODAudioEngine::sharedEngine()->playMusic(geode::utils::string::pathToString(Mod::get()->getConfigDir(true) / "music.mp3"), true, 1.0f, 0);
                }
            } else if (Mod::get()->getSettingValue<bool>("old_bgm")) {
                FMODAudioEngine::sharedEngine()->playMusic("StarStriker.mp3"_spr, true, 1.0f, 0);
            } else {
                FMODAudioEngine::sharedEngine()->playMusic("SlideshowScenic.mp3"_spr, true, 1.0f, 0);
            }
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
            auto color = Mod::get()->getSettingValue<cocos2d::ccColor4B>("sidebar_color");
            if (color.r != 0 || color.g != 0 || color.b != 26 || color.a != 255) {
                sidebar->drawRect({0,0}, {win.width * 0.10f, win.height}, {color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f}, 0, {0,0,1,1});
            } else {
                sidebar->drawRect({0,0}, {win.width * 0.10f, win.height}, {0,0,0.1f,1}, 0, {0,0,1,1});
            }
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