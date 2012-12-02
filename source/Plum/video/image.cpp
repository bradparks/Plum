#include <SDL_opengl.h>

#include "image.h"
#include "canvas.h"
#include "transform.h"

namespace plum
{
    namespace
    {
        int align(int num)
        {
            if (num <= 0)
            {
                return 0;
            }
            else
            {
                num--;
                num |= num >> 1;
                num |= num >> 2;
                num |= num >> 4;
                num |= num >> 8;
                num |= num >> 16;
                num++;

                return num;
            }
        }
    }

    Image::Image(const std::string& filename)
    {
        Canvas* source = Canvas::load(filename);
        init(source);
        delete source;
    }

    Image::Image(Canvas* source)
    {
        init(source);
    }

    Image::~Image()
    {
        glDeleteTextures(1, &textureID);
        delete canvas;
    }

    void Image::init(Canvas* source)
    {
        target = GL_TEXTURE_2D;

        canvas = new Canvas(source->getWidth(), source->getHeight(), align(source->getWidth()), align(source->getHeight()));
        canvas->clear(0);
        source->blit<SoftOpaqueBlender>(0, 0, canvas, SoftOpaqueBlender());
        canvas->setClipRegion(0, 0, source->getWidth() - 1, source->getHeight() - 1);

        glGenTextures(1, &textureID);
        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(target, 0, GL_RGBA8, canvas->getTrueWidth(), canvas->getTrueHeight(),
            0, GL_RGBA, GL_UNSIGNED_BYTE, canvas->getData());
    }

    Canvas* Image::getCanvas() const
    {
        return canvas;
    }

    void Image::refresh()
    {
        bind();
        glTexSubImage2D(target, 0, 0, 0, canvas->getTrueWidth(), canvas->getTrueHeight(),
            GL_RGBA, GL_UNSIGNED_BYTE, canvas->getData());
    }

    void Image::bind()
    {
        glBindTexture(target, textureID); 
    }

    void Image::blit(int x, int y, BlendMode mode, Color tint)
    {
        scaleBlitRegion(0, 0, canvas->getWidth(), canvas->getHeight(), x, y, canvas->getWidth(), canvas->getHeight(), mode, tint);
    }

    void Image::scaleBlit(int x, int y, int width, int height, BlendMode mode, Color tint)
    {
        scaleBlitRegion(0, 0, canvas->getWidth(), canvas->getHeight(), x, y, width, height, mode, tint);
    }

    void Image::blitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, BlendMode mode, Color tint)
    {
        scaleBlitRegion(sourceX, sourceY, sourceX2, sourceY2, destX, destY,
            std::abs(sourceX2 - sourceX) + 1, std::abs(sourceY2 - sourceY) + 1, mode, tint);
    }

    void Image::scaleBlitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, int scaledWidth, int scaledHeight, BlendMode mode, Color tint)
    {
        ColorChannel r, g, b, a;
        getRGBA(tint, r, g, b, a);

        if(sourceX > sourceX2)
        {
            std::swap(sourceX, sourceX2);
        }
        if(sourceY > sourceY2)
        {
            std::swap(sourceY, sourceY2);
        }
        sourceX = std::min(std::max(0, sourceX), canvas->getWidth() - 1);
        sourceY = std::min(std::max(0, sourceY), canvas->getHeight() - 1);
        sourceX2 = std::min(std::max(0, sourceX2), canvas->getWidth() - 1);
        sourceY2 = std::min(std::max(0, sourceY2), canvas->getHeight() - 1);

        double regionS = ((double) sourceX + 0.5) / canvas->getTrueWidth();
        double regionT = ((double) sourceY + 0.5) / canvas->getTrueHeight();
        double regionS2 = ((double) sourceX2 + 0.5) / canvas->getTrueWidth();
        double regionT2 = ((double) sourceY2 + 0.5) / canvas->getTrueHeight();

        useHardwareBlender(mode);

        glEnable(GL_TEXTURE_2D);
        glPushMatrix();
        bind();

        useHardwareColor(r, g, b, a);
        glTranslated(destX, destY, 0);


        const GLdouble vertexArray[] = {
            0.0, 0.0,
            0.0, scaledHeight,
            scaledWidth, scaledHeight,
            scaledWidth, 0.0,
        };

        const GLdouble textureArray[] = {
            regionS, regionT,
            regionS, regionT2,
            regionS2, regionT2,
            regionS2, regionT,
        };


        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_DOUBLE, 0, vertexArray);
        glTexCoordPointer(2, GL_DOUBLE, 0, textureArray);
        glDrawArrays(GL_QUADS, 0, 4);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        glPopMatrix();
    }

    void Image::rotateBlit(int x, int y, double angle, BlendMode mode, Color tint)
    {
        rotateScaleBlitRegion(0, 0, canvas->getWidth(), canvas->getHeight(), x, y, angle, 1.0, mode, tint);
    }

    void Image::rotateScaleBlit(int x, int y, double angle, double scale, BlendMode mode, Color tint)
    {
        rotateScaleBlitRegion(0, 0, canvas->getWidth(), canvas->getHeight(), x, y, angle, scale, mode, tint);
    }

    void Image::rotateBlitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, double angle, BlendMode mode, Color tint)
    {
        rotateScaleBlitRegion(sourceX, sourceY, sourceX2, sourceY2, destX, destY, angle, 1.0, mode, tint);
    }

    void Image::rotateScaleBlitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, double angle, double scale, BlendMode mode, Color tint)
    {
        ColorChannel r, g, b, a;
        getRGBA(tint, r, g, b, a);

        if(sourceX > sourceX2)
        {
            std::swap(sourceX, sourceX2);
        }
        if(sourceY > sourceY2)
        {
            std::swap(sourceY, sourceY2);
        }
        sourceX = std::min(std::max(0, sourceX), canvas->getWidth() - 1);
        sourceY = std::min(std::max(0, sourceY), canvas->getHeight() - 1);
        sourceX2 = std::min(std::max(0, sourceX2), canvas->getWidth() - 1);
        sourceY2 = std::min(std::max(0, sourceY2), canvas->getHeight() - 1);

        double regionS = ((double) sourceX + 0.5) / canvas->getTrueWidth();
        double regionT = ((double) sourceY + 0.5) / canvas->getTrueHeight();
        double regionS2 = ((double) sourceX2 + 0.5) / canvas->getTrueWidth();
        double regionT2 = ((double) sourceY2 + 0.5) / canvas->getTrueHeight();

        double width = ((double) sourceX2 - sourceX) * scale;
        double height = ((double) sourceY2 - sourceY) * scale;

        useHardwareBlender(mode);

        glEnable(GL_TEXTURE_2D);
        glPushMatrix();
        bind();

        useHardwareColor(r, g, b, a);
        glTranslated(destX + width / 2.0, destY + height / 2.0, 0.0);
        glRotated(angle, 0.0, 0.0, 1.0);
        glTranslated(-width / 2.0, -height / 2.0, 0.0);

        const GLdouble vertexArray[] = {
            0.0, 0.0,
            0.0, height + 1.0,
            width + 1.0, height + 1.0,
            width + 1.0, 0.0,
        };

        const GLdouble textureArray[] = {
            regionS, regionT,
            regionS, regionT2,
            regionS2, regionT2,
            regionS2, regionT,
        };

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_DOUBLE, 0, vertexArray);
        glTexCoordPointer(2, GL_DOUBLE, 0, textureArray);
        glDrawArrays(GL_QUADS, 0, 4);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        glPopMatrix();
    }

    // For when performance really matters, bind the texture and figure out blend modes ahead of time,
    // then call this in your loop. Especially important for tilemaps.
    // "raw" because it does less hand-holding. But could possibly be considered very hand-holdy.
    void Image::rawBlitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, double angle, double scale)
    {
        sourceX = std::min(std::max(0, sourceX), canvas->getWidth() - 1);
        sourceY = std::min(std::max(0, sourceY), canvas->getHeight() - 1);
        sourceX2 = std::min(std::max(0, sourceX2), canvas->getWidth() - 1);
        sourceY2 = std::min(std::max(0, sourceY2), canvas->getHeight() - 1);

        double regionS = ((double) sourceX + 0.5) / canvas->getTrueWidth();
        double regionT = ((double) sourceY + 0.5) / canvas->getTrueHeight();
        double regionS2 = ((double) sourceX2 + 0.5) / canvas->getTrueWidth();
        double regionT2 = ((double) sourceY2 + 0.5) / canvas->getTrueHeight();

        double width = ((double) sourceX2 - sourceX) * scale;
        double height = ((double) sourceY2 - sourceY) * scale;

        glPushMatrix();

        glTranslated(destX + width / 2.0, destY + height / 2.0, 0.0);
        glRotated(angle, 0.0, 0.0, 1.0);
        glTranslated(-width / 2.0, -height / 2.0, 0.0);

        const GLdouble vertexArray[] = {
            0.0, 0.0,
            0.0, height + 1.0,
            width + 1.0, height + 1.0,
            width + 1.0, 0.0,
        };

        const GLdouble textureArray[] = {
            regionS, regionT,
            regionS, regionT2,
            regionS2, regionT2,
            regionS2, regionT,
        };

        glVertexPointer(2, GL_DOUBLE, 0, vertexArray);
        glTexCoordPointer(2, GL_DOUBLE, 0, textureArray);
        glDrawArrays(GL_QUADS, 0, 4);

        glPopMatrix();
    }

    // Draws image, based on a transformation object (saves on complex arg passing)
    void Image::transformBlit(Transform* transform)
    {
        double sourceX, sourceY, sourceX2, sourceY2;
        ColorChannel r, g, b, a;
        getRGBA(transform->tint, r, g, b, a);

        if(transform->clip)
        {
            sourceX = std::min<double>(std::max<double>(0, transform->clip->x), canvas->getWidth() - 1);
            sourceY = std::min<double>(std::max<double>(0, transform->clip->y), canvas->getHeight() - 1);
            sourceX2 = std::min<double>(sourceX + transform->clip->width, canvas->getWidth()) - 1;
            sourceY2 = std::min<double>(sourceY + transform->clip->height, canvas->getHeight()) - 1;
        }
        else
        {
            sourceX = sourceY = 0;
            sourceX2 = canvas->getWidth() - 1;
            sourceY2 = canvas->getHeight() - 1;
        }

        double regionS = ((double) sourceX + 0.5) / canvas->getTrueWidth();
        double regionT = ((double) sourceY + 0.5) / canvas->getTrueHeight();
        double regionS2 = ((double) sourceX2 + 0.5) / canvas->getTrueWidth();
        double regionT2 = ((double) sourceY2 + 0.5) / canvas->getTrueHeight();

        double width = ((double) sourceX2 - sourceX);
        double height = ((double) sourceY2 - sourceY);

        useHardwareBlender(transform->mode);

        glEnable(GL_TEXTURE_2D);
        glPushMatrix();
        bind();

        useHardwareColor(r, g, b, a);
        glTranslated(transform->position->x + transform->pivot->x, transform->position->y + transform->pivot->y, 0.0);
        glScaled(transform->scale->x * (1 - transform->mirror * 2), transform->scale->y, 0.0);
        glRotated(transform->angle, 0.0, 0.0, 1.0);
        glTranslated(-transform->pivot->x, -transform->pivot->y, 0.0);

        const GLdouble vertexArray[] = {
            0.0, 0.0,
            0.0, height + 1.0,
            width + 1.0, height + 1.0,
            width + 1.0, 0.0,
        };

        const GLdouble textureArray[] = {
            regionS, regionT,
            regionS, regionT2,
            regionS2, regionT2,
            regionS2, regionT,
        };

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_DOUBLE, 0, vertexArray);
        glTexCoordPointer(2, GL_DOUBLE, 0, textureArray);
        glDrawArrays(GL_QUADS, 0, 4);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        glPopMatrix();
    }
}
