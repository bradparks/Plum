#ifndef PLUM_IMAGE_H
#define PLUM_IMAGE_H

#include <string>
#include "color.h"
#include "video.h"
#include "canvas.h"
#include "blending.h"

namespace plum
{
    class Canvas;
    struct Transform;
    class Image
    {
        public:
            Image(const Canvas& source);
            ~Image();

            Canvas& canvas();
            void refresh();

            void bind();

            void blit(int x, int y, BlendMode mode);
            void scaleBlit(int x, int y, int width, int height, BlendMode mode);
            void rotateBlit(int x, int y, double angle, BlendMode mode);
            void rotateScaleBlit(int x, int y, double angle, double scale, BlendMode mode);
            void blitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, BlendMode mode);
            void scaleBlitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, int scaledWidth, int scaledHeight, BlendMode mode);
            void rotateBlitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, double angle, BlendMode mode);
            void rotateScaleBlitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, double angle, double scale, BlendMode mode);
            void rawBlitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, double angle, double scale);
            void transformBlit(Transform* transform);

        private:
            // A backend software canvas that this image's raw texture copies.
            // Useful if the textures need to be refreshed later.
            Canvas canvas_;
            // The GL target type
            int target; 
            // The GL texture ID
            unsigned int textureID;
    };
}

#endif