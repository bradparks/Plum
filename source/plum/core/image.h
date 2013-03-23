#ifndef PLUM_IMAGE_H
#define PLUM_IMAGE_H

#include <string>
#include "color.h"
#include "screen.h"
#include "canvas.h"
#include "blending.h"

namespace plum
{
    class Canvas;
    class Sheet;
    struct Transform;

    class Image
    {
        public:
            Image(const Canvas& source);
            ~Image();

            Canvas& canvas();
            const Canvas& canvas() const;
            void refresh();

            void startBatch(BlendMode mode, Color tint);
            void endBatch();

            void draw(int x, int y);
            void draw(int x, int y, const Transform& transform);
            void drawFrame(const Sheet& sheet, int f, int x, int y);
            void drawFrame(const Sheet& sheet, int f, int x, int y, const Transform& transform);
            void drawRaw(int x, int y);
            void drawFrameRaw(const Sheet& sheet, int f, int x, int y);

            class Impl;
            std::shared_ptr<Impl> impl;
    };
}

#endif