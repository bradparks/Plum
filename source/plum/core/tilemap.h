#ifndef PLUM_TILEMAP_H
#define PLUM_TILEMAP_H

namespace plum
{
    class Sheet;
    class Image;
    class Screen;
    class Tilemap
    {
        public:
            static const unsigned int InvalidTile = (unsigned int)(-1);

            Tilemap(int width, int height);
            ~Tilemap();

            int getWidth() const;
            int getHeight() const;

            void clear(unsigned int tileIndex);

            unsigned int get(int tx, int ty) const;
            void dot(int tx, int ty, unsigned int tileIndex);
            void rect(int tx, int ty, int tx2, int ty2, unsigned int tileIndex);
            void fillRect(int tx, int ty, int tx2, int ty2, unsigned int tileIndex);
            void line(int tx, int ty, int tx2, int ty2, unsigned int tileIndex);
            void stamp(int tx, int ty, Tilemap* dest);
            void draw(Image& img, const Sheet& sheet, int sourceX, int sourceY, int destX, int destY, int tilesWide, int tilesHigh, Screen& dest);

        private:
            int width, height;
            unsigned int* data;
    };
}

#endif