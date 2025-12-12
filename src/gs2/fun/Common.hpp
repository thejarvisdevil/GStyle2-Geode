#pragma once
#include "../gs2.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;
namespace fs = std::filesystem;

namespace gs2::fun {
    static std::unordered_map<std::string, std::unordered_map<std::string,std::string>> packInformation;

    static std::unordered_map<std::string,std::string> getPackInfo(const fs::path& packdir) {
        std::unordered_map<std::string,std::string> out;
        try {
            auto f = packdir / "info.txt";
            if (!fs::exists(f)) return out;
            std::ifstream ifs(gs2::pathToString(f));
            std::string line;
            while (std::getline(ifs, line)) {
                auto t = gs2::trim(line);
                if (t.empty() || t[0]=='#') continue;
                auto eq = t.find('=');
                if (eq == std::string::npos) continue;
                auto k = gs2::trim(t.substr(0, eq));
                auto v = gs2::trim(t.substr(eq+1));
                out[k] = v;
            }
        } catch (...) {}
        return out;
    }

    static fs::path getPackDir() {
        if (!gs2::packFolder.empty()) return gs2::packFolder;
        auto packs = gs2::getEnabled();
        if (!packs.empty()) return packs.front().first;
        return fs::path();
    }

    static cocos2d::CCNode* currentScene() {
        if (gs2::currently) return gs2::currently;
        auto s = CCDirector::get()->getRunningScene();
        if (!s) s = CCDirector::get()->getRunningScene();
        return s;
    }

    static cocos2d::CCNode* findByID(cocos2d::CCNode* parent, const std::string& id) {
        if (!parent) return nullptr;
        try {
            auto child = parent->getChildByID(id.c_str());
            if (child) return child;
        } catch (...) {
        }
        auto children = parent->getChildren();
        if (!children) return nullptr;
        for (size_t i=0; i<children->count(); ++i) {
            auto c = typeinfo_cast<cocos2d::CCNode*>(children->objectAtIndex(i));
            try {
                auto cidVal = c->getID();
                std::string cidstr;
                try { cidstr = cidVal; } catch(...) { cidstr.clear(); }
                if (!cidstr.empty() && cidstr == id) return c;
            } catch (...) {}
        }
        return nullptr;
    }

    static cocos2d::CCNode* findNode(const std::string& path) {
        auto root = currentScene();
        if (!root) return nullptr;
        cocos2d::CCNode* cur = root;
        size_t i = 0;
        while (i < path.size()) {
            if (path[i] == '/') { ++i; continue; }
            size_t j = i;
            while (j < path.size() && path[j] != '/' && path[j] != '"') ++j;
            std::string token = gs2::trim(path.substr(i, j - i));
            std::string id;
            if (j < path.size() && path[j] == '"') {
                ++j;
                size_t k = j;
                while (k < path.size() && path[k] != '"') ++k;
                id = path.substr(j, k - j);
                j = k + 1;
            }
            cocos2d::CCNode* next = nullptr;
            if (!id.empty()) next = findByID(cur, id);
            if (!next && !token.empty()) next = findByID(cur, token);
            if (!next) {
                auto children = cur->getChildren();
                if (!children) return nullptr;
                for (size_t ci=0; ci<children->count(); ++ci) {
                    auto c = typeinfo_cast<cocos2d::CCNode*>(children->objectAtIndex(ci));
                    try {
                        auto desc = std::string(c->description());
                        if (!token.empty() && desc.find(token) != std::string::npos) { next = c; break; }
                    } catch(...) { }
                }
            }
            if (!next) return nullptr;
            cur = next;
            i = j;
        }
        return cur;
    }

    static std::string cleanMe(const std::string& in) {
        std::string out;
        for (char c : in) {
            unsigned char uc = (unsigned char)c;
            if (std::isalnum(uc) || c == '_') {
                out.push_back((char)std::tolower(uc));
            } else if (std::isspace(uc) || c == '-') {
                out.push_back('_');
            } else {
                out.push_back('_');
            }
        }
        return out;
    }

    static std::string prefixID(const std::string& id) {
        if (id.empty()) return id;
        auto dir = getPackDir();
        std::string packname;
        if (!gs2::packName.empty()) packname = gs2::packName;
        else if (!dir.empty()) packname = gs2::pathToString(dir.filename());
        if (packname.empty()) packname = "unknown";
        auto info = getPackInfo(dir);
        std::string author;
        auto it = info.find("author");
        if (it != info.end() && !it->second.empty()) author = it->second; else author = packname;
        std::string a = cleanMe(author);
        std::string p = cleanMe(packname);
        std::string mid = cleanMe(id);
        std::string out = a + "-" + p + "-" + mid;
        return out;
    }
}