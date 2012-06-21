# Reception slideshow #

Simple software for displaying a slideshow of images with Ken Burns style zoom on each one, and cross fades between images.  A company logo can be displayed in the top left corner if desired, and optional picture captions in the bottom right.

## Usage ##
Any .jpg files in the same folder will be displayed in random order, for 10 seconds per image.  A file named logo.jpg will be displayed in the top left corner.  A font named font.ttf will be used to generate captions from the image filenames.  If you do not want a caption on a file, begin the filename with an underscore e.g. _DSC1234.jpg
There is very little in the way of error checking.  It runs 8 hours a day in reception here, but things such as excessively long captions, or inadequate video card will cause crashes.

## Hardware requirements ##
A reasonable OpenGL capable card.  I am using an Nvidia MX440, so it doesn't need to be modern.  As long as your video card supports a 2048x2048 texture size you should be fine.  If your video card only supports a smaller one, you can modify the texture size in ken_burns_image.cpp.

## Software requirements ##
The main requirement is the [Allegro](http://alleg.sourceforge.net/) library.  I built it using MingW 4.6.2 and Allegro 5.1.1
Linked libraries:
* liballegro-5.1.1-static-mt.a 
* liballegro_image-5.1.1-static-mt.a 
* liballegro_font-5.1.1-static-mt.a 
* liballegro_ttf-5.1.1-static-mt.a 
* libfreetype-2.4.4-static-mt.a 
* MinGW\lib\libadvapi32.a 
* MinGW\lib\libcomdlg32.a 
* MinGW\lib\libgdi32.a 
* MinGW\lib\libgdiplus.a 
* MinGW\lib\libglu32.a 
* MinGW\lib\libkernel32.a 
* MinGW\lib\libole32.a 
* MinGW\lib\libopengl32.a 
* MinGW\lib\libpsapi.a 
* MinGW\lib\libshell32.a 
* MinGW\lib\libshlwapi.a 
* MinGW\lib\libuser32.a 
* MinGW\lib\libuuid.a 
* MinGW\lib\libwinmm.a 
* MinGW\lib\libws2_32.a