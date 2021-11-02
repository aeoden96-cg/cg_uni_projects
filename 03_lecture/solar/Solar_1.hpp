/*
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 */

// Function prototypes for Solar.cpp

static void KeyPressFunc( unsigned char Key, int x, int y );
static void SpecialKeyFunc( int Key, int x, int y );
static void Key_r(void);
static void Key_s(void);
static void Key_up(void);
static void Key_down(void);
void myDisplay();
void resizeWindow(int w, int h);
bool init_data();
