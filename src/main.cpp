#include <cmath>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_opengl.h>
#include <ctime>
#include <cstdlib>
#include <dirent.h>
#include <string>
#include <algorithm>
#include <vector>
#include "ken_burns_image.h"
#include "log.h"


ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_TIMER *image_timer = NULL;
ALLEGRO_FONT *font = NULL;
Log *myLog;
ken_burns_image *image1 = NULL;
const float FPS = 60;
const float image_time = 10;
ken_burns_image *image2 = NULL;
ALLEGRO_BITMAP *logo;
bool isLogo;
bool caption = true;
int current_image;
bool fade;
float fadePercent;
bool new_image;
bool texture_loaded;
int screen_width, screen_height;
std::vector<std::string> fileList;
std::vector<std::string>::iterator fileIterator;


// Draws the supplied image to screen.
void draw_image(ken_burns_image *theImage)
{
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, al_get_opengl_texture(theImage->texture));
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0, theImage->texy);
    glVertex2f(theImage->xB, theImage->yB);
    glTexCoord2f(theImage->texx, theImage->texy);
    glVertex2f(theImage->xE, theImage->yB);
    glTexCoord2f(theImage->texx, 1.0 );
    glVertex2f(theImage->xE, theImage->yE);
    glTexCoord2f(0, 1.0);
    glVertex2f(theImage->xB, theImage->yE);
    glEnd();
    glPopMatrix();
}

// The main render loop.  Loads images if required, blends, scales, and translates
void Render()
{
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    if(fade)   //Are we in the middle of cross fading?
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable( GL_BLEND );
        if(!texture_loaded)
        {
            if(current_image==1)
                image2->waitForImageLoad();
            else
                image1->waitForImageLoad();
            texture_loaded = true;
        }
        glColor4f(1.0,1.0,1.0,fadePercent);
        if(current_image==1)
        {
            glPushMatrix();
            glScalef(image1->zoomx, image1->zoomy, 1.0);
            glTranslatef(image1->current_panx, image1->current_pany, 0.0);
            draw_image(image1);
            glPopMatrix();
            glColor4f(1.0,1.0,1.0,1.0-fadePercent);
            draw_image(image2);
        }
        else
        {
            glPushMatrix();
            glScalef(image2->zoomx, image2->zoomy, 1.0);
            glTranslatef(image2->current_panx, image2->current_pany, 0.0);
            draw_image(image2);
            glPopMatrix();
            glColor4f(1.0,1.0,1.0,1.0-fadePercent);
            draw_image(image1);
        }
        fadePercent = fadePercent - 0.01;
    }
    else  //No cross fade, so only one image to display
    {
        if(current_image==1)
        {
            glPushMatrix();
            glScalef(image1->zoomx, image1->zoomy, 1.0);
            image1->zoom();
            glTranslatef(image1->current_panx, image1->current_pany, 0.0);
            image1->pan();
            draw_image(image1);
            glPopMatrix();
        }
        else
        {
            glPushMatrix();
            glScalef(image2->zoomx, image2->zoomy, 1.0);
            image2->zoom();
            glTranslatef(image2->current_panx, image2->current_pany, 0.0);
            image2->pan();
            draw_image(image2);
            glPopMatrix();
        }
    }

    //Display the logo
    if(isLogo)
    {
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
        al_draw_tinted_bitmap(logo, al_map_rgba_f(1.0, 1.0, 1.0, 0.75), 5, 5, 0);
    }
    if(current_image==1 && image1->hasCaption && !fade)
    {
        al_draw_bitmap(image1->caption, screen_width - 20 - image1->captionw, screen_height - 20 - image1->captionh, 0);
    }
    else if(current_image==2 && image2->hasCaption && !fade)
    {
        al_draw_bitmap(image2->caption, screen_width - 20 - image2->captionw, screen_height - 20 - image2->captionh, 0);
    }

    if(fade && fadePercent < 0.02) //End fade
        {
            fadePercent = 1.0;
            glDisable(GL_BLEND);
            fade = false;
            new_image = true;
            al_start_timer(image_timer);
        }
    glPopMatrix();
}

//Load in a list of .jpg files.  A file named logo.jpg will be used for the logo in the corner
//Requires at least 2 files for a slideshow to happen
void getFiles(void)
{
    dirent* dp;
    //DIR* dirp = opendir(".");
    DIR* dirp = opendir(".");
    int len;
    std::string filename;
    std::string extension;
    std::string start;
    myLog->Write("Clearing file list");
    fileList.clear();
    myLog->Write("File list cleared");
    while ((dp = readdir(dirp)) != NULL)
    {
        myLog->Write("Finding new file");
        filename = dp->d_name;
        myLog->Write("File found: %s", filename.c_str());
        len = filename.length();
        myLog->Write("Lowercasing filename");
        std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
        myLog->Write("Filename lowercased");
        if(len > 4)
        {
            myLog->Write("Filename long enough");
            extension = filename.substr(len - 4, 4);
            myLog->Write("File extension: %s", extension.c_str());
            start = filename.substr(0, 5);
            if(strcmp(".jpg", extension.c_str())==0)
            {
                if(strcmp("logo.", start.c_str())==0)
                {
                    isLogo = true;
                    myLog->Write("Found logo");
                    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
                    logo = al_load_bitmap(filename.c_str());
                }
                else
                {
                    myLog->Write("Adding file to list");
                    fileList.push_back(dp->d_name);
                }
            }
        }
        myLog->Write("Trying next file");
    }
    closedir(dirp);
    if(fileList.size()>1)
    {
        myLog->Write("Shuffling file list");
        random_shuffle ( fileList.begin(), fileList.end() );
        myLog->Write("File list shuffled");
        fileIterator = fileList.begin();
        myLog->Write("File iterator begun");
    }
    else
    {
        myLog->Write("Not enough files");
        throw 10;
    }
}

int main()
{
    #if DEBUG
        myLog = new Log("debug.log", true);
    #else
        myLog = new Log("debug.log", false);
    #endif
    bool redraw = true;
    new_image = false;
    texture_loaded = false;
    current_image = 1;
    al_init();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    timer = al_create_timer(1.0 / FPS);  //Timer for refresh loop
    if(!timer)
    {
        myLog->Write("failed to create timer!");
        return -1;
    }
    image_timer = al_create_timer(image_time);  //Timer for image display on screen
    if(!image_timer)
    {
        myLog->Write("failed to create timer!");
        return -1;
    }

    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        myLog->Write("failed to create event_queue!");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    srand( time( NULL ) );
    isLogo = false;
    ALLEGRO_MONITOR_INFO mon_info;
    al_get_monitor_info(0, &mon_info);
    //If compiled in debug mode, runs in a window
    #if DEBUG
    al_set_new_display_flags(ALLEGRO_WINDOWED /*ALLEGRO_FULLSCREEN*/ | ALLEGRO_OPENGL);
    display = al_create_display( 640, 480 );
    screen_width = 640;
    screen_height = 480;
    #else
    al_set_new_display_flags(ALLEGRO_FULLSCREEN | ALLEGRO_OPENGL);
    display = al_create_display( mon_info.x2, mon_info.y2 );
    screen_width = mon_info.x2;
    screen_height = mon_info.y2;
    #endif
    myLog->Write("Width: %i Height: %i", mon_info.x2, mon_info.y2);
    font = al_load_font("font.ttf", 24, 0);
    al_hide_mouse_cursor(display);
    try
    {
        getFiles();
    }
    catch (int e)
    {
        myLog->Write("Not enough files");
        return 10;
    }

    bool quit = false;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    image1 = new ken_burns_image(display, (*fileIterator).c_str());
    fileIterator++;
    image2 = new ken_burns_image(display, (*fileIterator).c_str());
    image1->waitForImageLoad();
    image2->generateCaption(font);
    image1->generateCaption(font);
    myLog->Write("Thread joined");
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_timer_event_source(image_timer));
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);
    al_start_timer(image_timer);
    while(!quit)  //Main program loop, runs until alt+f4
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            if(ev.timer.source == timer)
                redraw = true;
            if(ev.timer.source == image_timer)
            {
                fade = true;
                fadePercent = 1.0;
                al_stop_timer(image_timer);
                al_set_timer_count(image_timer, 0);
            }
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }

        if(new_image)
        {
            if(current_image == 1)
            {
                current_image = 2;
                new_image = false;
                delete image1;
                fileIterator++;
                myLog->Write("Next file");
                if(fileIterator == fileList.end())
                {
                    myLog->Write("Loading new file list");
                    getFiles();
                    myLog->Write("New file list loaded");

                }
                myLog->Write("Trying to load %s", (*fileIterator).c_str());
                image1 = new ken_burns_image(display, (*fileIterator).c_str());
                image1->generateCaption(font);
                myLog->Write("Image loading");
                texture_loaded = false;
            }
            else
            {
                current_image = 1;
                new_image = false;
                delete image2;
                fileIterator++;
                myLog->Write("Next file");
                if(fileIterator == fileList.end())
                {
                    myLog->Write("Loading new file list");
                    getFiles();
                    myLog->Write("New file list loaded");
                }
                myLog->Write("Trying to load %s", (*fileIterator).c_str());
                image2 = new ken_burns_image(display, (*fileIterator).c_str());
                image2->generateCaption(font);
                myLog->Write("Image loading");
                texture_loaded = false;
            }
            al_start_timer(image_timer);

        }
        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;
            al_clear_to_color(al_map_rgb(0,0,0));

            Render();
            al_flip_display();
        }
    }
    //Time to quit
    al_destroy_display( display );
    al_destroy_timer(timer);
    al_destroy_timer(image_timer);
    al_destroy_event_queue(event_queue);
    if(isLogo)
        al_destroy_bitmap(logo);
    delete image1;
    image1 = NULL;
    delete image2;
    image2 = NULL;
    return 0;
}
