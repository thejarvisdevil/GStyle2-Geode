#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <mutex>
#include <Geode/Geode.hpp>
#include <ctime>
#include <iomanip>
#include <thread>
#include <cstdlib>

using namespace geode::prelude;

namespace gs2 {
    using CommandFn = std::function<void(const std::vector<std::string>& args)>;

    inline std::unordered_map<std::string, CommandFn> s_commands;
    inline std::mutex s_lock;
    inline cocos2d::CCNode* currently = nullptr;
    inline std::filesystem::path packFolder;
    inline std::string packName;
    inline bool freakout = false;

    inline void fail(const std::string &msg) {
        try { geode::log::error("{}", msg); } catch(...) {}
        freakout = true;
    }

    inline void registerCMD(const std::string& name, CommandFn fn) {
        std::lock_guard<std::mutex> lk(s_lock);
        s_commands[name] = fn;
    }

    inline std::string trim(const std::string& s) {
        size_t a = 0, b = s.size();
        while (a < b && std::isspace((unsigned char)s[a])) ++a;
        while (b > a && std::isspace((unsigned char)s[b-1])) --b;
        return s.substr(a, b - a);
    }

    inline std::vector<std::string> wee(const std::string& line) {
        std::vector<std::string> out;
        std::string cur;
        bool inq = false;
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (c == '\"') {
                if (inq) {
                    out.push_back(cur);
                    cur.clear();
                    inq = false;
                    continue;
                } else {
                    inq = true;
                    cur.clear();
                    continue;
                }
            }
            if (inq) {
                cur.push_back(c);
            } else {
                if (std::isspace((unsigned char)c)) {
                    if (!cur.empty()) { out.push_back(cur); cur.clear(); }
                } else {
                    cur.push_back(c);
                }
            }
        }
        if (!cur.empty()) out.push_back(cur);
        return out;
    }

    namespace fs = std::filesystem;

    template <typename T>
    inline T numFromString(const std::string& s) {
        auto res = geode::utils::numFromString<T>(s);
        if (res) return res.unwrap();
        fail(std::string("invalid number: ") + s);
        return T();
    }

    inline std::string pathToString(const fs::path& p) {
        return geode::utils::string::pathToString(p);
    }

    inline std::vector<std::pair<fs::path, std::string>> getEnabled() {
        try {
            auto cfgDir = geode::Mod::get()->getConfigDir(true);
            auto activa = cfgDir / "active_packs.txt";
            std::vector<std::pair<fs::path, std::string>> out;
            if (fs::exists(activa)) {
                std::ifstream ifs(gs2::pathToString(activa));
                std::string name;
                while (std::getline(ifs, name)) {
                    name = trim(name);
                    if (name.empty()) continue;
                    auto byname = cfgDir / "packs" / name;
                    if (fs::exists(byname) && fs::is_directory(byname)) out.emplace_back(byname, name);
                }
                return out;
            }

            auto packsDir = cfgDir / "packs";
            if (fs::exists(packsDir) && fs::is_directory(packsDir)) {
                for (auto &entry : fs::directory_iterator(packsDir)) {
                    if (entry.is_directory()) { out.emplace_back(entry.path(), gs2::pathToString(entry.path().filename())); }
                }
                if (!out.empty()) return out;
            }
        } catch (...) {}
        fs::path local = fs::current_path() / "packs";
        std::vector<std::pair<fs::path, std::string>> out;
        if (fs::exists(local) && fs::is_directory(local)) {
            for (auto &entry : fs::directory_iterator(local)) if (entry.is_directory()) out.emplace_back(entry.path(), gs2::pathToString(entry.path().filename()));
        }
        return out;
    }

    inline std::pair<fs::path, std::string> currentInfo() {
        auto packs = getEnabled();
        if (!packs.empty()) return packs.front();
        return { fs::path(), std::string() };
    }

    inline std::string double_to_string(double v) {
        std::ostringstream oss;
        oss << v;
        auto s = oss.str();
        if (s.find('.') != std::string::npos) {
            while (!s.empty() && s.back() == '0') s.pop_back();
            if (!s.empty() && s.back() == '.') s.pop_back();
        }
        return s;
    }

    inline std::string gs2_vars(const std::string& key) {
        try {
            if (key == "display.screen" || key == "display") {
                auto win = CCDirector::get()->getWinSize();
                float cs = 1.0f; try { cs = CCDirector::get()->getContentScaleFactor(); } catch(...){ }
                return double_to_string(win.width * cs) + "x" + double_to_string(win.height * cs);
            }
            if (key == "display.width") {
                auto win = CCDirector::get()->getWinSize();
                float cs = 1.0f; try { cs = CCDirector::get()->getContentScaleFactor(); } catch(...){}
                return double_to_string(win.width * cs);
            }
            if (key == "display.height") {
                auto win = CCDirector::get()->getWinSize();
                float cs = 1.0f; try { cs = CCDirector::get()->getContentScaleFactor(); } catch(...){}
                return double_to_string(win.height * cs);
            }
            if (key == "pack.name" || key == "pack") {
                auto info = currentInfo();
                return info.second;
            }
            if (key.rfind("pack.info.", 0) == 0) {
                std::string sub = key.substr(std::string("pack.info.").size());
                auto info = currentInfo();
                auto packdir = info.first;
                if (packdir.empty()) return std::string();
                auto f = packdir / "info.txt";
                if (!fs::exists(f)) return std::string();
                try {
                    std::ifstream ifs(gs2::pathToString(f));
                    std::string line;
                    while (std::getline(ifs, line)) {
                        auto t = trim(line);
                        if (t.empty() || t[0] == '#') continue;
                        auto eq = t.find('=');
                        if (eq == std::string::npos) continue;
                        auto k = trim(t.substr(0, eq));
                        auto v = trim(t.substr(eq + 1));
                        if (k == sub) return v;
                    }
                } catch (...) {}
                return std::string();
            }
        } catch (...) {}
        return std::string();
    }

    struct ExprParser {
        const std::string s;
        size_t i = 0;
        std::string packname;
        int lineno = 0;
        ExprParser(const std::string& src, const std::string& pk, int ln): s(src), i(0), packname(pk), lineno(ln) {}

        void skip() { while (i < s.size() && std::isspace((unsigned char)s[i])) ++i; }

        double nummy() {
            skip();
            size_t start = i;
            bool hasDot = false;
            while (i < s.size() && (std::isdigit((unsigned char)s[i]) || s[i]=='.')) { if (s[i]=='.') hasDot = true; ++i; }
                if (start==i) { fail("expected number at expression"); return 0.0; }
            double val = gs2::numFromString<double>(s.substr(start, i-start));
            skip();
            if (i < s.size() && s[i] == '%') { ++i; return val / 100.0; }
            return val;
        }

        double parseVar() {
            if (i+1 >= s.size() || s[i] != ':' || s[i+1] != ':') { fail("bad variable token"); return 0.0; }
            i += 2;
            size_t start = i;
            while (i+1 < s.size()) {
                if (s[i] == ':' && s[i+1] == ':') break;
                ++i;
            }
                if (i+1 >= s.size()) { fail("unterminated variable token"); return 0.0; }
            std::string name = s.substr(start, i - start);
            i += 2;
            auto valstr = gs2_vars(name);
            if (valstr.empty()) { fail(std::string("unknown variable: ") + name); return 0.0; }
            try {
                    if (valstr.find_first_of("0123456789") == std::string::npos) { fail(std::string("variable not numeric: ") + name); return 0.0; }
                    return numFromString<double>(valstr);
                } catch (...) { fail(std::string("variable not numeric: ") + name); return 0.0; }
        }

        double parseFactor() {
            skip();
            if (i < s.size() && s[i] == '(') { ++i; double v = parseExpr(); skip(); if (i>=s.size() || s[i] != ')') { fail("missing )"); return 0.0; } ++i; return v; }
            if (i < s.size() && s[i] == '-') { ++i; return -parseFactor(); }
            if (i+1 < s.size() && s[i]==':' && s[i+1]==':') return parseVar();
            if (i < s.size() && (std::isdigit((unsigned char)s[i]) || s[i]=='.')) return nummy();
              fail("unexpected token in expression");
              return 0.0;
        }

        double parseTerm() {
            double v = parseFactor();
            while (true) {
                skip();
                if (i < s.size() && (s[i] == '*' || s[i] == '/')) {
                    char op = s[i++];
                    double rhs = parseFactor();
                    if (op == '*') v *= rhs; else if (op == '/') { if (rhs == 0) { fail("division by zero"); return 0.0; } v /= rhs; }
                } else break;
            }
            return v;
        }

        double parseExpr() {
            double v = parseTerm();
            while (true) {
                skip();
                if (i < s.size() && (s[i] == '+' || s[i] == '-')) {
                    char op = s[i++];
                    double rhs = parseTerm();
                    if (op == '+') v += rhs; else v -= rhs;
                } else break;
            }
            return v;
        }
    };

    inline double evalExp(const std::string& expr, const std::string& packname, int lineno) {
        try {
            ExprParser p(expr, packname, lineno);
            double v = p.parseExpr();
            p.skip();
            if (p.i < p.s.size()) { fail("trailing characters in expression"); return 0.0; }
            return v;
        } catch (const std::exception &e) {
                fail(std::string("expression error: ") + e.what());
                return 0.0;
        }
    }

    inline std::string subPlaceholders(const std::string& in, const std::string& packname, int lineno) {
        std::string out;
        size_t pos = 0;
        while (pos < in.size()) {
            auto p = in.find('<', pos);
            if (p == std::string::npos) { out.append(in.substr(pos)); break; }
            out.append(in.substr(pos, p - pos));
            auto q = in.find('>', p+1);
            if (q == std::string::npos) { fail("unterminated placeholder"); return std::string(); }
            auto inner = in.substr(p+1, q-p-1);
            if (inner.size() >= 4 && inner.rfind("::", 0) == 0 && inner.find("::", 2) != std::string::npos) {
                std::string name = inner;
                if (name.rfind("::", name.size()-2) == name.size()-2) {
                    name = name.substr(2, name.size()-4);
                } else if (name.rfind("::", 0) == 0) {
                    name = name.substr(2);
                    if (name.size() >= 2 && name.substr(name.size()-2) == "::") name = name.substr(0, name.size()-2);
                }
                auto val = gs2_vars(name);
                out.append(val);
            } else {
                double val = evalExp(inner, packname, lineno);
                out.append(double_to_string(val));
            }
            pos = q + 1;
        }
        return out;
    }

    inline void runFor(const std::string&, const std::string& target) {
        auto packs = getEnabled();
        if (packs.empty()) return;
        for (auto &info : packs) {
            fs::path packdir = info.first;
            std::string packname = info.second;
            fs::path mainfile = packdir / "main.gstyle2";
            if (!fs::exists(mainfile)) continue;
            std::ifstream in(gs2::pathToString(mainfile));
            if (!in.is_open()) continue;

            std::string line;
            bool in_for = false;
            std::string for_target;
            std::vector<std::pair<std::string,int>> block_lines;
            int lineno = 0;
            bool killed = false;

            auto prevDir = packFolder;
            auto prevName = packName;
            packFolder = packdir;
            packName = packname;

            try {
                while (std::getline(in, line)) {
                    ++lineno;
                    if (killed) break;
                    auto t = trim(line);
                    if (t.empty() || t[0] == '#') continue;
                    if (!in_for) {
                        if (t.rfind("for ", 0) == 0) {
                            auto firstQuote = t.find('"');
                            auto secondQuote = (firstQuote==std::string::npos) ? std::string::npos : t.find('"', firstQuote+1);
                            if (firstQuote!=std::string::npos && secondQuote!=std::string::npos) {
                                for_target = t.substr(firstQuote+1, secondQuote-firstQuote-1);
                                if (t.find('{', secondQuote) != std::string::npos) {
                                    in_for = true;
                                    block_lines.clear();
                                }
                            }
                        }
                    } else {
                        if (t == "}") {
                            if (for_target == target) {
                                for (auto &p : block_lines) {
                                    auto bl_text = p.first;
                                    auto bl_lineno = p.second;
                                    auto tl = trim(bl_text);
                                    if (tl.empty() || tl[0] == '#') continue;
                                    try {
                                        std::string substituted = subPlaceholders(tl, packname, bl_lineno);
                                        auto parts = wee(substituted);
                                        if (parts.empty()) continue;
                                        std::string cmd = parts[0];
                                        std::vector<std::string> args;
                                        if (parts.size() > 1) args.assign(parts.begin() + 1, parts.end());

                                        std::lock_guard<std::mutex> lk(s_lock);
                                        auto it = s_commands.find(cmd);
                                        if (it != s_commands.end()) {
                                            try {
                                                freakout = false;
                                                it->second(args);
                                                if (freakout) {
                                                    geode::log::error("Pack '{}' has been killed at line {}: (check logs)", packname, bl_lineno);
                                                    killed = true; break;
                                                }
                                            } catch (const std::exception &e) {
                                                geode::log::error("Pack '{}' has been killed at line {}: {}", packname, bl_lineno, e.what());
                                                killed = true; break;
                                            } catch (...) {
                                                geode::log::error("Pack '{}' has been killed at line {}", packname, bl_lineno);
                                                killed = true; break;
                                            }
                                        } else {
                                            geode::log::warn("Unknown gs2 command '{}' in pack '{}' at line {}", cmd, packname, bl_lineno);
                                        }
                                    } catch (const std::exception &e) {
                                        geode::log::error("Pack '{}' has been killed at line {}: {}", packname, bl_lineno, e.what());
                                        killed = true; break;
                                    } catch (...) {
                                        geode::log::error("Pack '{}' has been killed at line {}", packname, bl_lineno);
                                        killed = true; break;
                                    }
                                }
                            }
                            in_for = false;
                            for_target.clear();
                            block_lines.clear();
                        } else {
                            block_lines.emplace_back(t, lineno);
                        }
                    }
                }
            } catch (const std::exception &e) {
                geode::log::error("Pack '{}' crashed while running: {}", packname, e.what());
            } catch (...) {
                geode::log::error("Pack '{}' crashed while running (?)", packname);
            }

            packFolder = prevDir;
            packName = prevName;
        }
    }

    template <typename T>
    inline void execute(T* layer, const char* name) {
        cocos2d::CCNode* prev = currently;
        currently = typeinfo_cast<cocos2d::CCNode*>(layer);
        try {
            runFor(std::string(), std::string(name));
        } catch (const std::exception &e) {
            geode::log::error("Pack '{}' crashed while running: {}", name, e.what());
        } catch (...) {
            geode::log::error("Pack '{}' crashed while running (?)", name);
        }
        currently = prev;
    }

}

#include "fun/Log.hpp"
#include "fun/Functions.hpp"

namespace gs2 {
    struct registerr {
        registerr() {
            try { gs2::fun::register_log(); } catch (...) {}
            try { gs2::fun::register_functions(); } catch (...) {}
        }
    } registerrr;
}