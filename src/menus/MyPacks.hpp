#pragma once
#include <Geode/Geode.hpp>
#include "../gs2/gs2.hpp"
#include <filesystem>
#include <set>
#include <fstream>

using namespace geode::prelude;
namespace fs = std::filesystem;

namespace menus {
    using namespace geode::prelude;
    namespace fs = std::filesystem;

    class PacksGrid : public cocos2d::CCLayer {
    public:
        CREATE_FUNC(PacksGrid);
        int page = 0;
        std::vector<fs::path> packs;
        std::set<std::string> enabledPacks;
        bool touchy = false;
        cocos2d::CCMenu* information = nullptr;
        std::vector<cocos2d::CCRect> slots;
        std::vector<cocos2d::CCRect> infos;

        bool init() override {
            if (!CCLayer::init()) return false;
            reloadPacks();
            return true;
        }

        void reloadPacks() {
            packs.clear();
            try {
                auto packsDir = Mod::get()->getConfigDir(true) / "packs";
                if (fs::exists(packsDir) && fs::is_directory(packsDir)) {
                    std::error_code ec;
                    for (auto &entry : fs::directory_iterator(packsDir, ec)) {
                        if (ec) break;
                        try {
                            if (entry.is_directory()) {
                                auto p = entry.path();
                                if (fs::exists(p / "banner.png") && fs::exists(p / "main.gstyle2") && fs::exists(p / "info.txt")) {
                                    packs.push_back(p);
                                }
                            }
                        } catch(...) {}
                    }
                }
            } catch (...) {}
            loadActivePack();
            drawPage();
        }

        void onEnter() override {
            CCLayer::onEnter();
            if (isVisible()) {
                setTouchEnabled(true);
                try { CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true); touchy = true; } catch(...) {}
            }
            loadActivePack();
            drawPage();
        }

        void onExit() override {
            setTouchEnabled(false);
            try { if (touchy) CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this); } catch(...) {}
            CCLayer::onExit();
        }

        void setVisible(bool visible) override {
            CCLayer::setVisible(visible);
            try {
                if (visible && !touchy) {
                    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
                    touchy = true;
                    setTouchEnabled(true);
                } else if (!visible && touchy) {
                    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
                    touchy = false;
                    setTouchEnabled(false);
                }
            } catch(...) {}
        }

        void setActiveLayer(bool active) {
            try {
                if (active && !touchy) {
                    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
                    touchy = true;
                    setTouchEnabled(true);
                } else if (!active && touchy) {
                    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
                    touchy = false;
                    setTouchEnabled(false);
                }
            } catch(...) {}
        }

        std::string getPackInfo(const fs::path &pack, const char* key) {
            try {
                auto f = pack / "info.txt";
                if (!fs::exists(f)) return "";
                std::error_code lol;
                if (!fs::exists(f, lol) || lol) return "";
                std::ifstream ifs(gs2::pathToString(f));
                if (!ifs.is_open()) return "";
                std::string line;
                while (std::getline(ifs, line)) {
                    auto t = line;
                    t.erase(0, t.find_first_not_of(" \t"));
                    t.erase(t.find_last_not_of(" \t") + 1);
                    if (t.empty() || t[0] == '#') continue;
                    auto eq = t.find('=');
                    if (eq == std::string::npos) continue;
                    auto k = t.substr(0, eq);
                    auto v = t.substr(eq + 1);
                    k.erase(k.find_last_not_of(" \t") + 1);
                    v.erase(0, v.find_first_not_of(" \t"));
                    if (k == key) return v;
                }
            } catch (...) {}
            return "";
        }

        bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override {
            auto loc = convertToNodeSpace(touch->getLocation());
            for (auto &r : infos) if (r.containsPoint(loc)) return false;
            for (size_t i = 0; i < slots.size(); ++i) {
                if (slots[i].containsPoint(loc)) {
                    int idx = page * 4 + i;
                    if (idx >= 0 && idx < (int)packs.size()) {
                        auto name = gs2::pathToString(packs[idx].filename());
                        try {
                            if (fs::exists(packs[idx] / "textures") && fs::is_directory(packs[idx] / "textures")) {
                                FLAlertLayer::create("Woah There!", "<cy>This GStyle2 pack contains textures.</c> <cl>You will have to restart Geometry Dash for it to fully take effect.</c>", "OK")->show();
                            }
                        } catch(...) {}
                        if (enabledPacks.count(name)) enabledPacks.erase(name); else enabledPacks.insert(name);
                        activaCam();
                        drawPage();
                        return true;
                    }
                }
            }
            return false;
        }

        void activaCam() {
            try {
                auto cfg = Mod::get()->getConfigDir(true) / "active_packs.txt";
                std::ofstream ofs(gs2::pathToString(cfg), std::ios::trunc);
                if (ofs.is_open()) {
                    for (auto &n : enabledPacks) ofs << n << "\n";
                    ofs.close();
                }
            } catch (...) {}
        }

        void loadActivePack() {
            try {
                auto cfg = Mod::get()->getConfigDir(true) / "active_packs.txt";
                if (!fs::exists(cfg)) {
                    std::ofstream ofs(gs2::pathToString(cfg), std::ios::trunc);
                    if (ofs.is_open()) ofs.close();
                } else {
                    std::ifstream ifs(gs2::pathToString(cfg));
                    std::string line;
                    while (std::getline(ifs, line)) {
                        line.erase(0, line.find_first_not_of(" \t"));
                        line.erase(line.find_last_not_of(" \t") + 1);
                        if (!line.empty() && line[0] != '#') enabledPacks.insert(line);
                    }
                    ifs.close();
                }
            } catch (...) {}
        }

        void onInfo(CCObject* sender) {
            auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
            if (!item) return;
            std::string id;
            try { id = item->getID(); } catch(...) {}
            if (id.empty()) return;
            fs::path found;
            for (auto &p : packs) if (gs2::pathToString(p.filename()) == id) { found = p; break; }
            if (found.empty()) return;
            std::ostringstream oss;
            oss << "Name: " << getPackInfo(found, "name");
            auto author = getPackInfo(found, "author");
            auto version = getPackInfo(found, "version");
            auto desc = getPackInfo(found, "description");
            if (!author.empty()) oss << "\nAuthor: " << author;
            if (!version.empty()) oss << "\nVersion: " << version;
            if (!desc.empty()) oss << "\n\n" << desc;
            FLAlertLayer::create("Pack Info", oss.str(), "OK")->show();
        }

        void onDelete(CCObject* sender) {
            auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
            if (!item) return;
            std::string id;
            try { id = item->getID(); } catch(...) {}
            if (id.empty()) return;
            fs::path found;
            for (auto &p : packs) if (gs2::pathToString(p.filename()) == id) { found = p; break; }
            if (found.empty()) return;
            try { fs::remove_all(found); } catch (...) {}
            for (size_t i = 0; i < packs.size(); ++i) if (gs2::pathToString(packs[i].filename()) == id) { packs.erase(packs.begin() + i); break; }
            enabledPacks.erase(id);
            activaCam();
            drawPage();
            FLAlertLayer::create("Deleted Pack", ("Pack <cy>" + id + "</c> has been deleted").c_str(), "OK")->show();
        }

        void drawSlot(const cocos2d::CCSize& win, const cocos2d::CCPoint& origin, const fs::path &pack, float w, float h) {
            auto draw = cocos2d::CCDrawNode::create();
            bool active = enabledPacks.count(gs2::pathToString(pack.filename())) > 0;
            ccColor4F outline = active ? ccColor4F{0,1,0,1} : ccColor4F{0,0,1,1};
            cocos2d::CCPoint bl = origin, tr = { origin.x + w, origin.y + h };
            draw->drawRect(bl, tr, {0,0,0,1}, 1.0f, outline);
            addChild(draw);
            slots.push_back({bl.x, bl.y, w, h});

            auto hate = gs2::pathToString(pack / "banner.png");
            auto banner = CCSprite::create(hate.c_str());
            if (banner && banner->getTexture()) {
                float s = std::min(w / banner->getTexture()->getContentSize().width, h / banner->getTexture()->getContentSize().height);
                banner->setScale(s);
                banner->setPosition({ bl.x + w/2, bl.y + h/2 });
                addChild(banner);
            } else {
                auto ph = cocos2d::CCDrawNode::create();
                ph->drawRect(bl, tr, {1,1,1,1}, 0.0f, {0,0,0,1});
                addChild(ph);
                auto qm = cocos2d::CCLabelBMFont::create("?", "bigFont.fnt");
                qm->setPosition({ bl.x + w/2, bl.y + h/2 });
                addChild(qm);
            }

            auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
            if (infoSpr) {
                infoSpr->setScale(0.6f);
                auto infBtn = CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(PacksGrid::onInfo));
                try { infBtn->setID(gs2::pathToString(pack.filename()).c_str()); } catch(...) {}
                if (!information) { information = CCMenu::create(); information->setPosition({0,0}); addChild(information, 1000); }
                infBtn->setAnchorPoint({1,0});
                cocos2d::CCPoint desired = { tr.x + 8.0f, tr.y - 8.0f };
                auto menuPos = information->getPosition();
                infBtn->setPosition({ desired.x - menuPos.x, desired.y - menuPos.y });
                information->addChild(infBtn);
            }

            auto delSpr = CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
            if (delSpr) {
                delSpr->setScale(0.4f);
                auto delBtn = CCMenuItemSpriteExtra::create(delSpr, this, menu_selector(PacksGrid::onDelete));
                try { delBtn->setID(gs2::pathToString(pack.filename()).c_str()); } catch(...) {}
                if (!information) { information = CCMenu::create(); information->setPosition({0,0}); addChild(information, 1000); }
                delBtn->setAnchorPoint({0,1});
                cocos2d::CCPoint desired = { bl.x - 8.0f, tr.y + 8.0f };
                auto menuPos = information->getPosition();
                delBtn->setPosition({ desired.x - menuPos.x, desired.y - menuPos.y });
                information->addChild(delBtn);
            }
        }

        void drawPage() {
            removeAllChildrenWithCleanup(true);
            slots.clear();
            infos.clear();
            information = nullptr;
            auto win = CCDirector::get()->getWinSize();

            if (Mod::get()->getSettingValue<bool>("alt_font")) {
                auto title = cocos2d::CCLabelBMFont::create("My Packs", "bigFont.fnt");
                title->setPosition({win.width/2, win.height-50});
                title->setID("packs-title"_spr);
                addChild(title);
            } else {
                auto title = cocos2d::CCLabelBMFont::create("My Packs", "BitTrip.fnt"_spr);
                title->setPosition({win.width/2, win.height-50});
                title->setID("packs-title"_spr);
                addChild(title);
            }

            const float w = 1200.f, h = 400.f, spacing = 20.f;
            const int cols = 2, rows = 2;
            float scale = std::min((win.width * 0.7f) / (cols * w + (cols - 1) * spacing), (win.height * 0.5f) / (rows * h + (rows - 1) * spacing));
            float startX = (win.width - cols * (w*scale + spacing)) / 2 + win.width * 0.02f;
            float startY = (win.height - rows * (h*scale + spacing)) / 2 + 10.f;

            int remaining = (int)packs.size() - page * 4;
            for (int i = 0; i < std::min(4, remaining); ++i) {
                int r = i / cols;
                int c = i % cols;
                drawSlot(win, {startX + c * (w*scale + spacing), startY + (rows - 1 - r) * (h*scale + spacing)}, packs[page*4+i], w*scale, h*scale);
            }

            if (!information) { information = CCMenu::create(); information->setPosition({0,0}); addChild(information, 1000); }
            auto leftS = CCSprite::create("back.png"_spr);
            auto rightS = CCSprite::create("back.png"_spr);
            if (leftS && rightS) {
                leftS->setScale(0.45f);
                rightS->setScale(0.45f);
                rightS->setFlipX(true);
                float cy = startY - 50, cx = startX + cols * w * scale / 2;
                auto leftB = CCMenuItemSpriteExtra::create(leftS, this, menu_selector(PacksGrid::onPrev));
                auto rightB = CCMenuItemSpriteExtra::create(rightS, this, menu_selector(PacksGrid::onNext));
                leftB->setID("prev-btn"_spr);
                rightB->setID("next-btn"_spr);
                leftB->setPosition({cx - 70, cy});
                rightB->setPosition({cx + 70, cy});
                information->addChild(leftB);
                information->addChild(rightB);
            }
        }

        void onPrev(CCObject*) { if (page > 0) { page--; drawPage(); } }
        void onNext(CCObject*) { int maxPages = (packs.size() + 3) / 4; if (page + 1 < maxPages) { page++; drawPage(); } }
    };

    inline cocos2d::CCNode* make_packs(const cocos2d::CCSize& win) {
        auto node = cocos2d::CCNode::create();
        node->setID("packs-grid-node"_spr);
        auto grid = PacksGrid::create();
        grid->setID("packs-grid"_spr);
        node->addChild(grid);
        return node;
    }
}