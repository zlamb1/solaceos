#pragma once

#include "gfx/font.hpp"
#include "option.hpp"

namespace Gfx {

Option<BitmapFont> LoadPSFFont(void *header, usize len);

}