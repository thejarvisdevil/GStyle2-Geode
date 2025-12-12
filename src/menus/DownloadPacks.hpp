#pragma once

#include <Geode/Geode.hpp>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <matjson.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/ui/LazySprite.hpp>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <cctype>

using namespace geode::prelude;
using namespace geode::utils;
namespace fs = std::filesystem;


namespace menus {
struct OnlinePack {
	std::string id, version, description, banner, download;
};

class DLPacks : public cocos2d::CCLayer {
public:
	CREATE_FUNC(DLPacks);
	int page = 0;
	bool touchy = false;
	std::vector<OnlinePack> packs;
	std::vector<cocos2d::CCRect> slots, infos;
	cocos2d::CCMenu* information = nullptr;
	EventListener<web::WebTask> fetchy, downloady;
	std::unordered_map<std::string, std::string> banners;
	bool isOnline = true, downloading = false;

	bool init() override {
		if (!CCLayer::init()) return false;
		fetchIt();
		return true;
	}

	void onEnter() override {
		CCLayer::onEnter();
		if (isVisible()) {
			setTouchEnabled(true);
			try { CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true); touchy = true; } catch(...) {}
		}
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

	static int compareIt(const std::string &a, const std::string &b) {
		auto split = [](const std::string &s) {
			std::vector<int> v;
			std::stringstream ss(s);
			std::string tok;
			while (std::getline(ss, tok, '.')) v.push_back(gs2::numFromString<int>(tok));
			return v;
		};
		auto va = split(a), vb = split(b);
		size_t n = std::max(va.size(), vb.size());
		va.resize(n); vb.resize(n);
		for (size_t i = 0; i < n; ++i) {
			if (va[i] < vb[i]) return -1;
			if (va[i] > vb[i]) return 1;
		}
		return 0;
	}

	bool isInstalled(const std::string &id, std::string &ver) {
		try {
			auto p = Mod::get()->getConfigDir(true) / "packs" / id;
			if (!fs::exists(p)) return false;
			auto f = p / "info.txt";
			if (!fs::exists(f)) return true;
				std::ifstream ifs(gs2::pathToString(f));
			std::string line;
			while (std::getline(ifs, line)) {
				line.erase(0, line.find_first_not_of(" \t"));
				line.erase(line.find_last_not_of(" \t") + 1);
				if (line.empty() || line[0] == '#') continue;
				auto eq = line.find('=');
				if (eq == std::string::npos) continue;
				auto k = line.substr(0, eq);
				auto v = line.substr(eq + 1);
				k.erase(k.find_last_not_of(" \t") + 1);
				v.erase(0, v.find_first_not_of(" \t"));
				if (k == "version") { ver = v; return true; }
			}
			return true;
		} catch (...) { return false; }
	}

	std::string getURL(const matjson::Value &v) {
		try {
				if (v.isObject() && v.contains("_url")) {
					auto maybe = v["_url"].asString();
					if (maybe) return maybe.unwrap();
					return std::string();
				}
				auto sres = v.asString();
				if (sres) return sres.unwrap();
				return std::string();
		} catch (...) { return ""; }
	}

	void fetchIt() {
		try {
			fetchy.bind([this] (web::WebTask::Event* e) {
				if (auto res = e->getValue()) {
					try {
						auto resStr = res->string().unwrapOr("");
						auto parsed = matjson::parse(resStr);
						if (!parsed) {
							isOnline = false;
							drawPage();
							return;
						}
						auto val = parsed.unwrap();
						auto arrRes = val.asArray();
						if (!arrRes) {
							isOnline = false;
							drawPage();
							return;
						}
						auto outer = arrRes.unwrap();
						packs.clear();
						banners.clear();
						page = 0;
						for (auto &it : outer) {
							auto innerArrRes = it.asArray();
							if (!innerArrRes) continue;
							auto arr = innerArrRes.unwrap();
							if (arr.size() < 5) continue;
							auto a0 = arr[0].asString(); if (!a0) continue;
							auto a1 = arr[1].asString(); if (!a1) continue;
							auto a2 = arr[2].asString(); if (!a2) continue;
							std::string s0 = a0.unwrap(); std::string s1 = a1.unwrap(); std::string s2 = a2.unwrap();
							std::string url3 = getURL(arr[3]);
							std::string url4 = getURL(arr[4]);
							OnlinePack rp{s0, s1, s2, url3, url4};
							packs.push_back(rp);
							if (!rp.banner.empty()) banners[rp.id] = rp.banner;
						}
						isOnline = true;
					} catch (...) {
						isOnline = false;
					}
					drawPage();
				} else {
					isOnline = false;
					drawPage();
				}
			});
			fetchy.setFilter(web::WebRequest().get("https://jarvisdevil.com/geode_api/gstyle2/v1/packs"));
		} catch (...) {
			isOnline = false;
			drawPage();
		}
	}

	void downloadPack(const std::string &url, const std::string &id) {
		if (downloading) return;
		downloading = true;
		FLAlertLayer::create("Download", "Started download. <cr>Please be patient.</c>", "OK")->show();
		downloady.bind([this, id, url] (web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				if (!res->ok()) {
					FLAlertLayer::create("Download failed", "Server returned error.", "OK")->show();
					downloading = false;
					return;
				}
				try {
					auto packsDir = Mod::get()->getConfigDir(true) / "packs";
					auto mkdirRes = file::createDirectory(packsDir);
					auto zipPath = packsDir / (id + ".zip");
					if (!file::writeBinarySafe(zipPath, res->data())) {
						FLAlertLayer::create("Download failed", "Failed to write zip.", "OK")->show();
						downloading = false;
						return;
					}
					auto destDir = packsDir / id;
					try { if (fs::exists(destDir)) fs::remove_all(destDir); } catch (...) {}
					if (file::Unzip::intoDir(zipPath, destDir, false)) {
						FLAlertLayer::create("Success", "Pack installed.", "OK")->show();
						drawPage();
					} else {
						FLAlertLayer::create("Extract failed", "Failed to extract pack.", "OK")->show();
					}
					try { fs::remove(zipPath); } catch(...) {}
				} catch (...) {
					FLAlertLayer::create("Download failed", "Exception.", "OK")->show();
				}
				downloading = false;
			} else if (e->isCancelled()) {
				downloading = false;
			}
		});
		downloady.setFilter(web::WebRequest().timeout(std::chrono::seconds(120)).get(url));
	}

	void changePage(int delta) {
		int pages = std::max(1, (int)std::ceil((double)packs.size() / 4));
		page = std::clamp(page + delta, 0, pages - 1);
		drawPage();
	}

	bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override {
		auto loc = convertToNodeSpace(touch->getLocation());
		for (auto &r : infos) if (r.containsPoint(loc)) return false;
		for (size_t i = 0; i < slots.size(); ++i) {
			if (slots[i].containsPoint(loc)) {
				int idx = page * 4 + i;
				if (idx >= 0 && idx < (int)packs.size()) {
					if (packs[idx].download.empty()) {
						FLAlertLayer::create("Download", "No URL found. Most likely isn't approved for downloading yet, try again later.", "Aww")->show();
					} else {
						downloadPack(packs[idx].download, packs[idx].id);
					}
					return true;
				}
			}
		}
		return false;
	}

	void drawSlot(const cocos2d::CCSize& win, const cocos2d::CCPoint& pos, const OnlinePack &pack, float w, float h) {
		auto draw = cocos2d::CCDrawNode::create();
		std::string localVer;
		bool inst = isInstalled(pack.id, localVer);
		int cmp = (inst && !localVer.empty()) ? compareIt(localVer, pack.version) : 0;
		ccColor4F outline = inst ? (cmp < 0 ? ccColor4F{1,0,0,1} : ccColor4F{0,1,0,1}) : ccColor4F{0,0,1,1};
		cocos2d::CCPoint bl = pos, tr = {pos.x + w, pos.y + h};
		draw->drawRect(bl, tr, {0,0,0,1}, 1.0f, outline);
		addChild(draw);
		slots.emplace_back(bl.x, bl.y, w, h);

		if (banners.count(pack.id) && !banners[pack.id].empty()) {
			auto url = banners[pack.id];
			auto lazy = LazySprite::create({w, h}, true);
			lazy->setAutoResize(true);
			lazy->setLoadCallback([url](Result<> res) {
				if (!res) geode::log::error("Failed to load banner: {}", url);
			});
			if (url.rfind("http", 0) == 0 || url.find("://") != std::string::npos) {
				lazy->loadFromUrl(url);
			} else if (fs::exists(url)) {
				lazy->loadFromFile(url);
			} else {
				auto ph = cocos2d::CCDrawNode::create();
				ph->drawRect(bl, tr, {1,1,1,1}, 0.0f, {0,0,0,1});
				addChild(ph);
				return;
			}
			lazy->setPosition({bl.x + w/2, bl.y + h/2});
			addChild(lazy);
		} else {
			auto ph = cocos2d::CCDrawNode::create();
			ph->drawRect(bl, tr, {1,1,1,1}, 0.0f, {0,0,0,1});
			addChild(ph);
		}

		auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
		if (infoSpr) {
			infoSpr->setScale(0.6f);
			auto infBtn = CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(DLPacks::onInfo));
			try { infBtn->setID(pack.id.c_str()); } catch(...) {}
			if (!information) { information = CCMenu::create(); information->setPosition({0,0}); addChild(information, 1000); }
			infBtn->setAnchorPoint({1,0});
			cocos2d::CCPoint desired = { tr.x + 8.0f, tr.y - 8.0f };
			auto menuPos = information->getPosition();
			infBtn->setPosition({ desired.x - menuPos.x, desired.y - menuPos.y });
			information->addChild(infBtn);
		}
	}

	void drawPage() {
		removeAllChildrenWithCleanup(true);
		slots.clear();
		infos.clear();
		information = nullptr;
		auto win = CCDirector::get()->getWinSize();

		if (!isOnline) {
			auto offline = cocos2d::CCLabelBMFont::create("No internet connection.", "Roboto.fnt"_spr);
			offline->setPosition({win.width/2, win.height/2});
			offline->setAnchorPoint({0.5, 0.5});
			offline->setScale(0.75f);
			offline->setID("offline-label"_spr);
			addChild(offline);
			return;
		}

		if (Mod::get()->getSettingValue<bool>("alt_font")) {
			auto title = cocos2d::CCLabelBMFont::create("Download Packs", "bigFont.fnt");
			title->setPosition({win.width/2, win.height-50});
			title->setID("download-title"_spr);
			addChild(title);
		} else {
			auto title = cocos2d::CCLabelBMFont::create("Download Packs", "BitTrip.fnt"_spr);
			title->setPosition({win.width/2, win.height-50});
			title->setID("download-title"_spr);
			addChild(title);
		}

		const float w = 1200.f, h = 400.f, spacing = 20.f;
		const int cols = 2, rows = 2;
		float scale = std::min((win.width*0.7f) / (cols*w + (cols-1)*spacing), (win.height*0.5f) / (rows*h + (rows-1)*spacing));
		float startX = (win.width - cols*(w*scale+spacing)) / 2 + win.width*0.02f;
		float startY = (win.height - rows*(h*scale+spacing)) / 2 + 10.f;

		int remaining = (int)packs.size() - page * 4;
		for (int i = 0; i < std::min(4, remaining); ++i) {
			int r = i / cols;
			int c = i % cols;
			drawSlot(win, {startX + c*(w*scale+spacing), startY + (rows-1-r)*(h*scale+spacing)}, packs[page*4+i], w*scale, h*scale);
		}

		if (packs.size() > 4) {
			if (!information) { information = CCMenu::create(); information->setPosition({0,0}); addChild(information, 1000); }
            auto leftS = CCSprite::create("back.png"_spr);
            auto rightS = CCSprite::create("back.png"_spr);
            if (leftS && rightS) {
                leftS->setScale(0.45f);
                rightS->setScale(0.45f);
                rightS->setFlipX(true);
                float cy = startY - 50, cx = startX + cols * w * scale / 2;
                auto leftB = CCMenuItemSpriteExtra::create(leftS, this, menu_selector(DLPacks::onPrev));
                auto rightB = CCMenuItemSpriteExtra::create(rightS, this, menu_selector(DLPacks::onNext));
                leftB->setID("prev-btn"_spr);
                rightB->setID("next-btn"_spr);
                leftB->setPosition({cx - 70, cy});
                rightB->setPosition({cx + 70, cy});
                information->addChild(leftB);
                information->addChild(rightB);
            }
		}
	}

	void onInfo(cocos2d::CCObject* sender) {
		auto it = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
		if (!it) return;
		std::string id;
		try { id = it->getID(); } catch(...) {}
		for (auto &p : packs) {
			if (p.id == id) {
				std::ostringstream oss;
				oss << "Pack ID: " << p.id << "\nVersion: " << p.version << "\n\n" << p.description;
				FLAlertLayer::create("Pack Info", oss.str(), "OK")->show();
				break;
			}
		}
	}

	void onPrev(cocos2d::CCObject*) { changePage(-1); }
	void onNext(cocos2d::CCObject*) { changePage(1); }
};

inline cocos2d::CCNode* make_downloads(const cocos2d::CCSize&) {
	auto node = cocos2d::CCNode::create();
	node->setID("downloads-grid-node"_spr);
	auto grid = DLPacks::create();
	grid->setID("downloads-grid"_spr);
	node->addChild(grid);
	return node;
}
}