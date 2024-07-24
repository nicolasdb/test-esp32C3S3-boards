## Export a bitmap
using GIMP to draw the layout and then converting it to a bitmap for Arduino is a smart and efficient way to define the "Type" or behavior type on the matrix. It's much more intuitive than manually creating complex arrays.  

From 
```
/* XPM */
static char * testGIEP8x8_xpm[] = {
"8 8 4 1",
" 	c #FFFFFF",
".	c #00FF00",
"+	c #000000",
"@	c #FFFF00",
"        ",
" .   .  ",
"    ++  ",
" @  +@  ",
" ++@+   ",
"++++++++",
"+...+..+",
"++++++++"};
```
to 
```
const uint32_t BITMAP_ARRAY[64] = {
  0xFFFFFF, 0x00FF00, 0xFFFFFF, 0xFFFFFF, 0x00FF00, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
  0xFFFFFF, 0xFFFF00, 0xFFFFFF, 0xFFFFFF, 0x000000, 0xFFFF00, 0xFFFFFF, 0xFFFFFF,
  0xFFFFFF, 0x000000, 0x000000, 0xFFFF00, 0x000000, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
  0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
  0x000000, 0x00FF00, 0x00FF00, 0x00FF00, 0x000000, 0x00FF00, 0x00FF00, 0x000000,
  0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
};
```
note: extra colors are due to anti

**To use this:**  
1. Design your layout in GIMP, using specific colors for each type of pixel (rain, button, sewer, basin).  
1. Export the XPM and convert it to the BITMAP_ARRAY format (you can ask to a chatBOT, or use a script to automate this process if you'll be doing it frequently).