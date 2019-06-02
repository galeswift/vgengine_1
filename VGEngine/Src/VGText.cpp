#include "VGEngine.h"

void SEngine::String_Draw(const string String, int X, int Y)
{
	String_Draw( String.c_str(), X, Y );
}
void SEngine::String_Draw(const char *String, int X, int Y)
{
	BatchedElements.push_back( new FBatchedElementInfo() );

	static_cast<FBatchedElementInfo*>(BatchedElements[ BatchedElements.size()-1 ])->SetSurface( TTF_RenderText_Solid( Font_GetFont(CurrentFontName, CurrentFontSize), String, CurrentFontColor ) );
	static_cast<FBatchedElementInfo*>(BatchedElements[ BatchedElements.size()-1 ])->SetPos(X,Y);
}


// Returns a TTF_Font matching the name and size.  This function may create a new font if there is not one that exists that matches the name
TTF_Font* SEngine::Font_GetFont( string FontName, int Size )
{
	TTF_Font *Result = NULL;

	vector<FFontInfo*>::iterator it;

	for( it = CachedFonts.begin(); it != CachedFonts.end() ; ++it )
	{
		if( (*it)->GetFontName() == FontName && (*it)->GetFontSize() == Size )
		{
			Result = (*it)->GetFont();
			break;
		}
	}

	if( Result == NULL )
	{
		FFontInfo *NewEntry = new FFontInfo( FontName, Size );
		CachedFonts.push_back( NewEntry );
		
		if( CachedFonts.size() > 0 )
		{
			Result = CachedFonts[ CachedFonts.size() - 1]->GetFont();
		}
	}

	return Result;
}

void SEngine::Font_SetSize( int NewSize )
{
	CurrentFontSize = NewSize;
}

void SEngine::Font_SetFontName( string FontName )
{
	CurrentFontName = FontName;
}

void SEngine::Font_SetFontColor( Uint8 R, Uint8 G, Uint8 B )
{
	CurrentFontColor.r = R;
	CurrentFontColor.g = G;
	CurrentFontColor.b = B;
}
