#pragma once

#include "Texture.h"
#define MAX_ANIMS 8

struct Coords
{
	float x, y, w, h;
};

class CAnimatedTexture
{
private:
	CTexture *m_pTexture;
	SDL_Rect size;

	unsigned char m_iCurAnim, m_iMaxAnims;
	unsigned char m_iCurFrame;
	unsigned char m_iAnimFrames[MAX_ANIMS];
	unsigned char m_iLargestFrameCount;
	float m_fTimeFrameChange, m_fDesiredFramesPerSecond;
	float x_step, y_step;

public:
	CAnimatedTexture( std::string name );

	void LoadTextureData();
	void UpdateFrame( float fTime );

	Coords GetTextureCoords();
	GLuint GetTexture() { return m_pTexture->GetTexture(); }
	SDL_Rect GetSize();
	void SetFramerate( unsigned int FramesPerSecond );
};
