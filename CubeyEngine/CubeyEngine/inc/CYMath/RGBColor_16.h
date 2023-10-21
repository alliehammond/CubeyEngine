#pragma once
#include "Terrain/Chunk.h"
namespace CBY
{
    //Uses 4 bits to store each red, green, blue
    //Each color component goes from 0-15
    struct RGBColor_16
    {
        RGBColor_16() : color(0) {}
        RGBColor_16(unsigned char r, unsigned char g, unsigned char b) { setColor(r, g, b); }

        unsigned char getRed() { return color % 16; }
        unsigned char getGreen() { return (color / 16) % 16; }
        unsigned char getBlue() { return (color / (16 * 16)) % 16; }

        void setColor(unsigned char r, unsigned char g, unsigned char b)
        {
            //Cap values at 15
            r = (r < 16) ? r : 15;
            g = (g < 16) ? g : 15;
            b = (b < 16) ? b : 15;

            setColorQuick(r, g, b);
        }
        void setRed(unsigned char r) { setColor(r, getGreen(), getBlue()); }
        void setGreen(unsigned char g) { setColor(getRed(), g, getBlue()); }
        void setBlue(unsigned char b) { setColor(getRed(), getGreen(), b); }

    private:
        //Blue stored as higher bits, red lowest
        unsigned short color;

        inline void setColorQuick(unsigned char r, unsigned char g, unsigned char b)
        {
            color = r + 16 * g + 16 * 16 * b;
        }

        friend class Chunk;
    };

};
