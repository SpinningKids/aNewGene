
/************************************
  Code By: Pan/SpinningKids
  Revised on 2002/07/02 12:40:36 PM
  Cleaned up on 2021/01/08 00:49:18 AM
  Comments: Creation
 ************************************/

#include "GenTex.h"
#include "noise.h"

#include <array>
#include <cstring>

using namespace std;

template<typename PixelFunction>
static unique_ptr<GLTexture> radialSymmetryTextureHelper(int logsize, PixelFunction&& pixelFunction) {
    auto texture = make_unique<GLTexture>(logsize);
    const int size = texture->getSize();
    float* tex = texture->getImage();
    const float m = 2.0f / (size - 1);
    for (int i = 0; i < size; i++) {
        const float x = m * i - 1.f;
        for (int j = 0; j < size; j++) {
            const float y = m * j - 1.f;
            const float r = x * x + y * y;
            array<float, 4> value = pixelFunction(x, y, r);
            const size_t index = ((i << logsize) + j) * 4;
            memcpy(tex + index, value.data(), sizeof(value));
        }
    }
    texture->update();
    return texture;
}

unique_ptr<GLTexture> circle(int logsize) {
    return radialSymmetryTextureHelper(logsize, [](float x, float y, float r) {
        const float col = (r > 1.f) ? 0.f : (1.f - r) * (1.f - r);
        const float channel = 1.f - col * 0.25f;
        return array{ channel, channel, channel, col };
    });
}

unique_ptr<GLTexture> smoke(int logsize) {
    return radialSymmetryTextureHelper(logsize, [](float x, float y, float r) {
        const float col = 0.3f * (2 + 0.5f * (vnoise(5 * r, 10 * x, 10 * y) + vnoise(10 * r, 20 * x, 20 * y) + vnoise(20 * r, 40 * x, 40 * y) + vnoise(40 * r, 80 * x, 80 * y))) * (1 - r);
        return array{ 1.f, 1.f, 1.f, col };
    });
}

unique_ptr<GLTexture> circle2(int logsize) {
    return radialSymmetryTextureHelper(logsize, [](float x, float y, float r) {
        const float col = (r > 1) ? 0.f : ((r <= 0.95f) ? 1 : (1 - r) / 0.05f) * (1 - 0.5f * (1 - r) * (1 - r));
        return array{ 1.f, 1.f, 1.f, col };
    });
}
