#pragma once
#include "Common.hpp"
#include "Draw.hpp"
#include "Text.hpp"
#include "Sprite.hpp"
#include "Audio.hpp"
#include "Set.hpp"
#include "PackInfo.hpp"

namespace gs2::fun {
    inline void register_functions() {
        register_draw();
        register_text();
        register_sprite();
        register_audio();
        register_set();
        register_packinfo();
    }
}