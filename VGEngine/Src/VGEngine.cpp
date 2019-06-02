#include "VGEngine.h"
//#include <windows.h> // For the WaitMessage() function.
 
/** Default constructor. **/
SEngine::SEngine()
{
	m_lLastTick		= 0;
	m_iWidth		= SCREEN_WIDTH;
	m_iHeight		= SCREEN_HEIGHT;
	m_czTitle		= 0;
 
	m_pScreen		= 0;
 
	m_iFPSTickCounter	= 0;
	m_iFPSCounter		= 0;
	m_iCurrentFPS		= 0;
 
	m_bMinimized		= false;

	// Clear all batched elements so that we can insert into it properly
	BatchedElements.empty();
	BatchedMusic.empty();
	CachedFonts.empty();

	// Assign to white
	CurrentFontColor.b	= 255;
	CurrentFontColor.r	= 255;
	CurrentFontColor.g	= 255;

	CurrentFontName		= "Engine\\DefaultFont.ttf";
	CurrentFontSize		= 12;

	// Seed the randomizer
	srand( (unsigned)time(0) );
}
 
/** Destructor. **/
SEngine::~SEngine()
{
	// Clean up our Batches
	BatchedElements.empty();

	// Shutdown physics
	VGPhysics::Shutdown();

	// Stop all audio from playing
	// Clean up our music
	BatchedMusic.empty();

	// Clean up all of our fonts
	CachedFonts.empty();

	// CLose the audio device
	Mix_CloseAudio();

	//Quit SDL_ttf 
	TTF_Quit();

	// Quit SDL
	SDL_Quit();
}

Uint32 SEngine::GetGameTimeInMilliseconds()
{
	return SDL_GetTicks();
}

double SEngine::GetGameTimeInSeconds()
{
	return SDL_GetTicks()/1000.0f;
}
 
/** Sets the height and width of the window.
	@param iWidth The width of the window
	@param iHeight The height of the window
**/
void SEngine::SetSize(const int& iWidth, const int& iHeight)
{
	m_iWidth  = iWidth;
	m_iHeight = iHeight;
	m_pScreen = SDL_SetVideoMode( iWidth, iHeight, 0, SDL_SWSURFACE );
}
 
/** Initialize SDL, the window and the additional data. **/
void SEngine::Init()
{
	// Register SDL_Quit to be called at exit; makes sure things are cleaned up when we quit.
	atexit( SDL_Quit );
 
	// Initialize SDL's subsystems - in this case, only video.
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) 
	{
		fprintf( stderr, "Unable to init SDL: %s\n", SDL_GetError() );
		exit( 1 );
	}
	
	// Attempt to create a window with the specified height and width.
	SetSize( m_iWidth, m_iHeight );
 
	// If we fail, return error.
	if ( m_pScreen == NULL ) 
	{
		fprintf( stderr, "Unable to set up video: %s\n", SDL_GetError() );
		exit( 1 );
	}

	// Initialize the audio engine
	Init_Audio();

	// Initialize anything to do with the physics engine
	Init_Physics();

	//Initialize SDL_ttf 
	TTF_Init();

	// Call any children's init function
	Initialized();
}

// Initialize anything to do with the physics engine
void SEngine::Init_Physics()
{
	VGPhysics::Instance();
}

// Initialize audio channels, buffers, etc.
void SEngine::Init_Audio()
{
	// set this to any of 512,1024,2048,4096
	// the higher it is, the more FPS shown and CPU needed
	int AudioBufferSize=1024;
	int AudioRate,AudioChannels, Bits;
	Uint16 AudioFormat;
	
	// print out some info on the audio device and stream
	if( Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,AudioBufferSize) < 0 )
	{
		cout<<"Failed to initialize audio device! "<<endl;
	}
	else
	{
		Mix_QuerySpec(&AudioRate, &AudioFormat, &AudioChannels);
		Bits=AudioFormat&0xFF;
		cout<<"Opened audio at "<<AudioRate<<"Hz "<<Bits<<"bit "<<(AudioChannels>1?"stereo":"mono")<<", "<<AudioBufferSize<<"bytes audio buffer"	<<endl;
	}
}
 
/** The main loop. **/
void SEngine::Start()
{
	m_lLastTick = SDL_GetTicks();
	m_bQuit = false;
 
	// Main loop: loop forever.
	while ( !m_bQuit )
	{
		// Handle mouse and keyboard input
		HandleInput();
 
		if ( m_bMinimized ) {
			// Release some system resources if the app. is minimized.
#if 0
			WaitMessage(); // pause the application until focus in regained
#endif
		} else {
			// Do some thinking
			DoThink();
 
			// Render stuff
			DoRender();
		}
	}	
 
	End();
}
 
/** Handles all controller inputs.
	@remark This function is called once per frame.
**/
void SEngine::HandleInput()
{
	// Poll for events, and handle the ones we care about.
	SDL_Event event;
	
	for( map<int, bool>::iterator i = m_iLastKeyPressed.begin() ; i != m_iLastKeyPressed.end() ; ++i )
	{
		if( i->second )
		{
			KeyDown( i->first );
		}
	}

	while ( SDL_PollEvent( &event ) ) 
	{
		switch ( event.type ) 
		{
		case SDL_KEYDOWN:
			// For repeating keyboard input for multiple keys
			if( m_iLastKeyPressed[event.key.keysym.sym] == false )
			{
				m_iLastKeyPressed[event.key.keysym.sym] = true;
			}

			// If escape is pressed set the Quit-flag
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				m_bQuit = true;
				break;
			}
 
			KeyDown( event.key.keysym.sym );
			break;
 
		case SDL_KEYUP:
			// For repeating keyboard input for multiple keys
			if( m_iLastKeyPressed[event.key.keysym.sym] == true )
			{
				m_iLastKeyPressed[event.key.keysym.sym] = false;
			}
			KeyUp( event.key.keysym.sym );
			break;
 
		case SDL_QUIT:
			m_bQuit = true;
			break;
 
		case SDL_MOUSEMOTION:
			MouseMoved(
				event.button.button, 
				event.motion.x, 
				event.motion.y, 
				event.motion.xrel, 
				event.motion.yrel);
			break;
 
		case SDL_MOUSEBUTTONUP:
			MouseButtonUp(
				event.button.button, 
				event.motion.x, 
				event.motion.y, 
				event.motion.xrel, 
				event.motion.yrel);
			break;
 
		case SDL_MOUSEBUTTONDOWN:
			MouseButtonDown(
				event.button.button, 
				event.motion.x, 
				event.motion.y, 
				event.motion.xrel, 
				event.motion.yrel);
			break;
 
		case SDL_ACTIVEEVENT:
			if ( event.active.state & SDL_APPACTIVE ) {
				if ( event.active.gain ) {
					m_bMinimized = false;
					WindowActive();
				} else {
					m_bMinimized = true;
					WindowInactive();
				}
			}
			break;
		} // switch
	} // while (handling input)
}
 
/** Handles the updating routine. **/
void SEngine::DoThink() 
{
	long iElapsedTicks = SDL_GetTicks() - m_lLastTick;
	m_lLastTick = SDL_GetTicks();
 
	// Update physics simulation before updating the game.
	VGPhysics::Instance()->Update( iElapsedTicks );

	Update( iElapsedTicks );
 
	m_iFPSTickCounter += iElapsedTicks;
}
 
/** Handles the rendering and FPS calculations. **/
void SEngine::DoRender()
{
	++m_iFPSCounter;
	if ( m_iFPSTickCounter >= 1000 )
	{
		m_iCurrentFPS = m_iFPSCounter;
		m_iFPSCounter = 0;
		m_iFPSTickCounter = 0;
	}
 
	SDL_FillRect( m_pScreen, 0, SDL_MapRGB( m_pScreen->format, 70, 70, 70 ) );
 
	// Lock surface if needed
	if ( SDL_MUSTLOCK( m_pScreen ) )
		if ( SDL_LockSurface( m_pScreen ) < 0 )
			return;

	// Call the child's render function
	Render( GetScreen() );
 
	// Render any text, polygons, etc.
	RenderBatches( GetScreen() );

	// Unlock if needed
	if ( SDL_MUSTLOCK( m_pScreen ) ) 
		SDL_UnlockSurface( m_pScreen );
 
	// Tell SDL to update the whole gScreen
	SDL_Flip( m_pScreen );
}

void SEngine::RenderBatches( SDL_Surface *Target )
{
	vector<FBatchedElementInfo*>::iterator it;

	for( it = BatchedElements.begin(); it != BatchedElements.end() ; ++it )
	{
		if( (*it)->GetSurface() != NULL )
		{
			Image_Draw( (*it)->GetSurface(),(*it)->GetXPos(),(*it)->GetYPos());
			delete *it;
		}
	}
	
	BatchedElements.clear();
	
}
 
/** Sets the title of the window 
	@param czTitle A character array that contains the text that the window title should be set to.
**/
void SEngine::SetTitle(const char* czTitle)
{
	m_czTitle = czTitle;
	SDL_WM_SetCaption( czTitle, 0 );
}
 
/** Retrieve the title of the application window.
	@return The last set windows title as a character array.
	@remark Only the last set title is returned. If another application has changed the window title, then that title won't be returned.
**/
const char* SEngine::GetTitle()
{
	return m_czTitle;
}
 
/** Retrieve the main screen surface.
	@return A pointer to the SDL_Surface surface
	@remark The surface is not validated internally.
**/
SDL_Surface* SEngine::GetScreen()
{ 
	return m_pScreen;
}
 
/** Get the current FPS.
	@return The number of drawn frames in the last second.
	@remark The FPS is only updated once each second.
**/
int SEngine::GetFPS()
{
	return m_iCurrentFPS;
}
SDL_Surface* SEngine::Image_Load( string FileName )
{
    //The image that's loaded
    SDL_Surface* LoadedImage = NULL;
    
    //The optimized image that will be used
    SDL_Surface* OptimizedImage = NULL;
    
    //Load the image using SDL_image
    LoadedImage = IMG_Load( FileName.c_str() );
    
	if( LoadedImage == NULL )
	{
		LoadedImage = IMG_Load( ERROR_IMAGE_PATH );
	}

    //If the image loaded
    if( LoadedImage != NULL )
    {
        //Create an optimized image
        OptimizedImage = SDL_DisplayFormatAlpha( LoadedImage );
        
        //Free the old image
        SDL_FreeSurface( LoadedImage );
    }
    
    //Return the optimized image
    return OptimizedImage;
}

void SEngine::Image_Draw(  SDL_Surface *Source, int X, int Y )
{
	if( Source != NULL )
	{
		//Rectangle to hold the offsets
		SDL_Rect Offset;
	    
		//Get offsets
		Offset.x = X;
		Offset.y = Y;
	
		//Blit the surface
		SDL_BlitSurface( Source, NULL, GetScreen(), &Offset );
	}
}

void SEngine::Image_Draw( SDL_Surface* Source, int X, int Y, int Height, int Width, bool bSmoothScale )
{
	if( Source != NULL )
	{
		double ZoomX, ZoomY;
		ZoomX = Height/(double)Source->h;
		ZoomY = Width/(double)Source->w;
		Image_Draw( zoomSurface(Source, ZoomX, ZoomY, bSmoothScale), X, Y );
	}
}

// Apply an SDL_Surface to another SDL_Surface.  Allows you to adjust the rotation of the image as well.
void SEngine::Image_Draw( SDL_Surface* Source, int X, int Y, double RotationRadians, float Scale, bool bSmoothScale )
{
	if( Source != NULL )
	{
		Image_Draw( Image_Scale( Image_Rotate(Source, RotationRadians), Scale ), X, Y);
	}
}


// Takes an image, scales it, and returns the result.
SDL_Surface* SEngine::Image_Scale( SDL_Surface* Image, double Scale )
{
	return zoomSurface( Image, abs(Scale), abs(Scale), 1);
}

SDL_Surface* SEngine::Image_Rotate( SDL_Surface* Image, double Radians )
{
	return rotozoomSurface(Image, -1 * Radians *(180/ M_PI), 1.0, 1);
}

// Physics interface

void SEngine::Phys_DrawBody( b2Body *InBody )
{
	if( InBody != NULL)
	{ 
		for (b2Shape* s = InBody->GetShapeList(); s; s = s->GetNext())
		{
			b2PolygonShape* data = NULL;
			data = static_cast<b2PolygonShape*>(s);
			b2CircleShape* circleData = NULL;
			circleData = static_cast<b2CircleShape*>(s);

			if( s->GetType() == e_polygonShape )
			{
				Sint16 *px = new Sint16[data->GetVertexCount()];
				Sint16 *py = new Sint16[data->GetVertexCount()];

				Sint16 *px_s = new Sint16[data->GetVertexCount()];
				Sint16 *py_s = new Sint16[data->GetVertexCount()];

				const b2Vec2 *verts = data->GetVertices();
				for( int i=0  ; i<data->GetVertexCount() ; i++ )
				{
					px[i] = (int)SCALE_BOX2D_VG * (int)InBody->GetWorldPoint( verts[i] ).x;
					py[i] = (int)SCALE_BOX2D_VG * (int)InBody->GetWorldPoint( verts[i] ).y;

					px_s[i] = (int)SCALE_BOX2D_VG * (int)InBody->GetWorldPoint( verts[i] ).x+20;
					py_s[i] = (int)SCALE_BOX2D_VG * (int)InBody->GetWorldPoint( verts[i] ).y+20;
				}
				
				if( InBody->IsStatic() )
				{
					filledPolygonRGBA( GetScreen(),px_s, py_s, data->GetVertexCount(), 20, 20, 20, 255 );
				}

				filledPolygonRGBA( GetScreen(),px, py, data->GetVertexCount(), 255, 255, 255, 255 );
								
				if( InBody->IsStatic() )
				{
					aapolygonRGBA( GetScreen(),px, py, data->GetVertexCount(), 255, 0, 0, 255 );
				}				
				else
				{
					aapolygonRGBA( GetScreen(),px, py, data->GetVertexCount(), 0, 255, 0, 255 );
				}

				// when done, delete it.
				delete px;
				delete py;
				delete px_s;
				delete py_s;
			}
			else if( s->GetType() == e_circleShape )
			{
				filledCircleRGBA( GetScreen(), int(SCALE_BOX2D_VG * InBody->GetWorldPoint(circleData->GetLocalPosition()).x), int(SCALE_BOX2D_VG * InBody->GetWorldPoint(circleData->GetLocalPosition()).y), int(circleData->GetRadius() * SCALE_BOX2D_VG), 255, 255, 255, 255 );
				filledCircleRGBA( GetScreen(), int(SCALE_BOX2D_VG * InBody->GetWorldPoint(circleData->GetLocalPosition()).x), int(SCALE_BOX2D_VG * InBody->GetWorldPoint(circleData->GetLocalPosition()).y), int(circleData->GetRadius() * SCALE_BOX2D_VG- 3), 0, 255, 0, 255 );
		
			}
		}
	}
}

