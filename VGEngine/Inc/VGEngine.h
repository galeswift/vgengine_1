#ifndef VGENGINE_H
#define VGENGINE_H

#ifdef _DEBUG
#define check( statement ) \
	{\
		assert( statement );\
	}
#else
	#define check( statement )
#endif 

#if 0
#include <windows.h>
#endif

// Path to the default error image
#define ERROR_IMAGE_PATH "Engine\\IMG_Error.png"

// Standard headers
#include <vector>
#include <map>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cassert>

// Any globals will be defined here.
#include "VGGlobals.h"

// SDL Headers
#include "SDL.h"
#include "SDL_Image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "SDL_gfxPrimitives.h"
#include "SDL_gfxPrimitives_font.h"
#include "SDL_rotozoom.h"
#include "SDL_imageFilter.h"
#include "SDL_framerate.h"

// Physics headers
#include "Box2D.h"

// VGEngine headers
#include "VGStorageClasses.h"

// Physics classes
#include "VGPhysics.h"

using namespace std;

/** The base engine class. **/
class SEngine  
{
private:
	/** The last keys that were pressed.  Set to -1 on KeyUp to indicate when we should stop calling the KeyDown function */
	map<int, bool> m_iLastKeyPressed;

	/** Last iteration's tick value **/
	long m_lLastTick;
 
	/** Window width **/
	int m_iWidth;
	/** Window height **/
	int m_iHeight;
 
	/** Has quit been called? **/
	bool m_bQuit;
 
	/** The title of the window **/
	const char* m_czTitle;
 
	/** Screen surface **/
	SDL_Surface* m_pScreen;
 
	/** Is the window minimized? **/
	bool m_bMinimized;
 
	/** Variables to calculate the frame rate **/
	/** Tick counter. **/
	int m_iFPSTickCounter;
 
	/** Frame rate counter. **/
	int m_iFPSCounter;
 
	/** Stores the last calculated frame rate. **/
	int m_iCurrentFPS;
 
	// A batched vector of surfaces to draw
	vector<FBatchedElementInfo*> BatchedElements;
	
	// A vector of currently playing audio files
	vector<FMusicInfo*> BatchedMusic;

	// A vector of currently playing sound files
	vector<FSoundInfo*> BatchedSounds;

	// A cached vector of fonts.  A new font is opened whenever a size is used that doesn't exist in this vector yet.
	vector<FFontInfo*> CachedFonts;

	// Font parameters
	SDL_Color CurrentFontColor;
	string CurrentFontName;
	int CurrentFontSize;

	// Called in Render()
	void RenderBatches( SDL_Surface *Target );

	// Called in Render(), allows us to display error messages to the user
	void RenderDebugBatches( SDL_Surface *Target );

	// Removes from our array, the song that matches the given name
	void RemoveFromMusicBatch( string SongName );

protected:
	void DoThink();
	void DoRender();
 
	void SetSize(const int& iWidth, const int& iHeight);
 
	void HandleInput();

	// Initializes the SDL_Mixer library
	void Init_Audio();
	// Initialize anything to do with the physics engine
	void Init_Physics();

public:
	SEngine();
	virtual ~SEngine();
 
	void Init();
	void Start();
 
	/** OVERLOADED - Data that should be initialized when the application starts. **/
	virtual void Initialized() {}
 
	/** OVERLOADED - All the games calculation and updating. 
		@param iElapsedTime The time in milliseconds elapsed since the function was called last.
	**/
	virtual void Update		( const int& iElapsedTime ) {}
	/** OVERLOADED - All the games rendering. 
		@param pDestSurface The main screen surface.
	**/
	virtual void Render		( SDL_Surface* pDestSurface ) {}
 
	/** OVERLOADED - Allocated data that should be cleaned up. **/
	virtual void End		() {}
 
	/** OVERLOADED - Window is active again. **/
	virtual void WindowActive	() {}
 
	/** OVERLOADED - Window is inactive. **/
	virtual void WindowInactive	() {}
 
 
	/** OVERLOADED - Keyboard key has been released.
		@param iKeyEnum The key number.
	**/
	virtual void KeyUp		(const int& iKeyEnum) {}
 
	/** OVERLOADED - Keyboard key has been pressed.
		@param iKeyEnum The key number.
	**/
	virtual void KeyDown		(const int& iKeyEnum) {}
 
 
	/** OVERLOADED - The mouse has been moved.
		@param iButton	Specifies if a mouse button is pressed.
		@param iX	The mouse position on the X-axis in pixels.
		@param iY	The mouse position on the Y-axis in pixels.
		@param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
		@param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
 
		@bug The iButton variable is always NULL.
	**/
	virtual void MouseMoved		(const int& iButton,
					 const int& iX, 
					 const int& iY, 
					 const int& iRelX, 
					 const int& iRelY) {}
 
	/** OVERLOADED - A mouse button has been released.
		@param iButton	Specifies if a mouse button is pressed.
		@param iX	The mouse position on the X-axis in pixels.
		@param iY	The mouse position on the Y-axis in pixels.
		@param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
		@param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
	**/
 
	virtual void MouseButtonUp	(const int& iButton, 
					 const int& iX, 
					 const int& iY, 
					 const int& iRelX, 
					 const int& iRelY) {}
 
	/** OVERLOADED - A mouse button has been pressed.
		@param iButton	Specifies if a mouse button is pressed.
		@param iX	The mouse position on the X-axis in pixels.
		@param iY	The mouse position on the Y-axis in pixels.
		@param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
		@param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
	**/
	virtual void MouseButtonDown	(const int& iButton, 
					 const int& iX, 
					 const int& iY, 
					 const int& iRelX, 
					 const int& iRelY) {}
 
	void		SetTitle	(const char* czTitle);
	const char* 	GetTitle	();
 
	SDL_Surface* 	GetScreen	();
 
	int 		GetFPS		();
	// Returns how long the game has been running in milliseconds
	Uint32		GetGameTimeInMilliseconds();
	// Returns how long the game has been running in seconds
	double		GetGameTimeInSeconds();

	// Loads a JPG, BMP, or PNG
	SDL_Surface* Image_Load( string FileName );

	// Apply an SDL_Surface to another SDL_Surface.
	void Image_Draw( SDL_Surface* Source, int X, int Y );
	// Apply an SDL_Surface to another SDL_Surface.  Allows you to adjust the Height/Width of the image as well.
	void Image_Draw( SDL_Surface* Source, int X, int Y, int Height, int Width, bool bSmoothScale = true );
	// Apply an SDL_Surface to another SDL_Surface.  Allows you to adjust the rotation/scale of the image as well.
	void Image_Draw( SDL_Surface* Source,  int X, int Y, double RotationRadians, float Scale, bool bSmoothScale = true );

	// Takes an image, scales it, and returns the result.
	SDL_Surface* Image_Scale( SDL_Surface* Image, double Scale );

	// Takes an image, rotates it, and returns the result.
	SDL_Surface* Image_Rotate( SDL_Surface* Image, double Radians );

	// Draws text on screen.  
	void String_Draw( const char *String, int X, int Y);	
	void String_Draw( const string String, int X, int Y);	

	// Sets the font size.  Must be called before calling DrawString to take effect.
	void Font_SetSize( int NewSize );

	// Sets the font name.  Must be called before calling DrawString to take effect.
	void Font_SetFontName( string FontName );

	// Sets the font color.  Must be called before calling DrawString to take effect.
	void Font_SetFontColor( Uint8 R, Uint8 G, Uint8 B);

	// Returns a TTF_Font matching the name and size.  This function may create a new font if there is not one that exists that matches the name
	TTF_Font* Font_GetFont( string FontName, int Size );

	// Load a Sound file
	void Sound_Load( string FileName, string SoundName );

	// Play that Sound file
	void Sound_Play( string SoundName, int Loops = 0);

	// Stop a Sound file
	void Sound_Stop( string SoundName );

	// Returns the Mix_Chunk file correlated to the song name passed in 
	Mix_Chunk* Sound_GetSound( string SoundName );

	// Load a music file
	void Music_Load( string FileName, string SongName );

	// Play that music file
	void Music_Play( string SongName, int Loops = 1, bool bRestart = false);

	// Fade in a music file
	void Music_FadeIn( string SongName, int MillisecondsToFadeOver = 2000, int Loops = 1, bool bRestart = false);

	// Play that music file
	void Music_SetVolume( int Volume );

	// Stop a music file
	void Music_Stop( string SongName );

	// Return 1 if we're playing a song, 0 otherwise
	int Music_IsPlaying();

	// Returns the Mix_Music file correlated to the song name passed in 
	FMusicInfo* Music_GetSong( string SongName );

	// Physics interface
	void	Phys_DrawBody( b2Body *InBody);

};

// Utility functions

// Convert a value to a string
template <class T> string String_Convert( T InValue, int DecimalPlaces = 2 )
{

stringstream SS;
string Result;

SS<< std::fixed<<std::setprecision( DecimalPlaces ) << InValue;
Result = SS.str();
return Result;
}

// returns a number from Min to max.  Including min and max as possible values
static int RandRange( int Min, int Max )
{
	return ( rand() % ( (Max+1) - Min ) + Min );
}
 
#endif // SIMPLEENGINE_H 