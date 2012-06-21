#include "ken_burns_image.h"

static void *loadImageThread(ALLEGRO_THREAD *me, void* arg);

//Load an image from disk.  Scales to a 2048x2048 texture
ALLEGRO_BITMAP *ken_burns_image::load_texture(std::string filename)
{
   ALLEGRO_BITMAP *texture = NULL;
   ALLEGRO_BITMAP *small_texture = NULL;
   int max_size = al_get_display_option(display, ALLEGRO_MAX_BITMAP_SIZE);
   max_size = 2048;  //Maximum texture size.  May have to be dropped to 1024 for older video cards.
   al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
   texture = al_load_bitmap(filename.c_str());

      int new_height;
      int new_width;
      float aspect;
      height = al_get_bitmap_height(texture);
      width = al_get_bitmap_width(texture);

          if(height < width)
          {
              aspect = (float)height / (float)width;
              new_height = max_size * aspect;
              new_width = max_size;
              texx = 1.0;
              texy = 1.0 - aspect;
          }
          else
          {
              aspect = (float)width / (float)height;
              new_width = max_size * aspect;
              new_height = max_size;
              texx = aspect;
              texy = 0.0;
          }
          small_texture = al_create_bitmap(max_size, max_size);

      al_set_target_bitmap(small_texture);
      al_draw_scaled_bitmap(texture, 0, 0, width, height, 0, 0, new_width, new_height, 0);
      al_set_target_bitmap(al_get_backbuffer(display));
      al_destroy_bitmap(texture);
      texture = small_texture;
      small_texture = NULL;
      height = new_height;
      width = new_width;
   return texture;
}


//Threaded image loader
static void *loadImageThread(ALLEGRO_THREAD *me, void* arg)
{
    ThreadParameter *params = (ThreadParameter *)arg;
    params->kbi->texture2 = params->kbi->load_texture(params->filename);
    params->kbi->calcTextureParams();
    return NULL;
}

//Pause while waiting for the image loader thread to complete
void ken_burns_image::waitForImageLoad()
{
    al_join_thread(t, NULL);
    al_set_new_bitmap_flags(ALLEGRO_MIPMAP | ALLEGRO_MIN_LINEAR | ALLEGRO_VIDEO_BITMAP);
    texture = al_clone_bitmap(texture2);
    al_destroy_bitmap(texture2);
    threadRunning = false;
    al_destroy_thread(t);
    delete params;
}


ken_burns_image::ken_burns_image(ALLEGRO_DISPLAY *thedisplay, std::string filename)
{
    textureLoaded = false;
    display = thedisplay;
    fileName = filename;
    int pandir = rand() % 5;  //The direction the ken burns effect will move
    switch(pandir)
    {
    case 0:
        panx = 0.00025;
        break;
    case 1:
        panx = 0.000125;
        break;
    case 2:
        panx = -0.00025;
        break;
    case 3:
        panx = -0.000125;
        break;
    case 4:
        panx = 0.0;
        break;
    }
    current_panx = 0.0;
    current_pany = 0.0;
    pandir = rand() % 5;
    switch(pandir)
    {
    case 0:
        pany = 0.00025;
        break;
    case 1:
        pany = 0.000125;
        break;
    case 2:
        pany = -0.00025;
        break;
    case 3:
        pany = -0.000125;
        break;
    case 4:
        pany = 0.0;
        break;
    }
    params = new ThreadParameter();
    params->filename = filename;
    hasCaption = false;
    params->kbi = this;
    t = al_create_thread(loadImageThread, params);
    threadRunning = true;
    al_start_thread(t);
}

ken_burns_image::~ken_burns_image()
{
    if(threadRunning)
        waitForImageLoad();
    al_destroy_bitmap(texture);
    if(hasCaption)
        al_destroy_bitmap(caption);
}


//Zoom into the image every frame
void ken_burns_image::zoom()
{
    zoomx = zoomx + 0.0005;
    zoomy = zoomy + 0.0005;
}

//Ken burns style pan
void ken_burns_image::pan()
{
    current_panx = current_panx + panx;
    current_pany = current_pany + pany;
}

//Take the filename, strip off the extension, and use it to build a caption
void ken_burns_image::generateCaption(ALLEGRO_FONT* font)
{
    int bbx, bby, bbw, bbh;
    if(strncmp(fileName.c_str(), "_", 1)!=0)
    {
        std::string fileNameNoExt;
        fileNameNoExt = fileName.substr(0, fileName.length()-4);
        al_get_text_dimensions(font, fileNameNoExt.c_str(), &bbx, &bby, &bbw, &bbh);
        al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
        caption = al_create_bitmap(bbw, bbh);
        captionw = bbw;
        captionh = bbh;
        al_set_target_bitmap(caption);
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_text(font, al_map_rgb_f(1.0, 1.0, 1.0), 0, 0, ALLEGRO_ALIGN_LEFT, fileNameNoExt.c_str());
        al_set_target_bitmap(al_get_backbuffer(display));
        hasCaption = true;
    }
}

//Calculate various parameters used to make the image fill the screen
void ken_burns_image::calcTextureParams()
{
    float screenRatio = (float)al_get_display_width(display)/(float)al_get_display_height(display);
    float imageRatio = (float)width / (float)height;
    zoomx = 1.0;
    zoomy = 1.0;
    zoomz = 1.0;
    if (screenRatio > imageRatio)
    {
        float b = (float) (1.0 - imageRatio / screenRatio);
        xB = (float) (-1.0 + b);
        xE = (float) (1.0 - b);
        yB = (float) (-1.0);
        yE = (float) 1.0;
    }
    else
    {
        float b = (float) (1.0 - screenRatio/imageRatio);
        yB = (float) (-1.0 + b);
        yE = (float) (1.0 - b);
        xB = (float) -1.0;
        xE = (float) 1.0;
    }
}
