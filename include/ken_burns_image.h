#ifndef KEN_BURNS_IMAGE_H
#define KEN_BURNS_IMAGE_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <string>
#include <stdio.h>
#include <cstdlib>

class ken_burns_image;

struct ThreadParameter
{
    std::string filename;
    ken_burns_image *kbi;
};

class ken_burns_image
{
    public:
        float xB, yB, xE, yE;
        float zoomx, zoomy, zoomz;
        float zoomstep;
        float panx, pany;
        int captionw, captionh;
        float texx, texy;
        float current_panx, current_pany;
        float offsetx, offsety;
        std::string fileName;
        ALLEGRO_BITMAP *texture, *texture2, *caption;
        bool hasCaption;
        int framecount;
        bool isFading;
        float height, width;
        bool textureLoaded;
        bool threadRunning;
        ALLEGRO_DISPLAY *display;
        ALLEGRO_THREAD *t;
        ThreadParameter *params;
        ken_burns_image(ALLEGRO_DISPLAY *display, std::string filename);
        void calcTextureParams(void);
        ALLEGRO_BITMAP *load_texture(std::string);
        void waitForImageLoad();
        void zoom();
        void pan();
        void generateCaption(ALLEGRO_FONT* font);
        virtual ~ken_burns_image();
    protected:
    private:

};

#endif // KEN_BURNS_IMAGE_H
