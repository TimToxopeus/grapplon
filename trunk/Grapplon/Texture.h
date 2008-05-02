#pragma once

#include "Resource.h"
#include <SDL.h>
#include <SDL_OpenGL.h>

#define MAX_ANIMS 8

struct Coords
{
	float x, y, w, h;
};

class CTexture : public IResource
{
protected:
	GLuint m_iGLTexture;
	SDL_Rect size;

	unsigned char m_iCurAnim, m_iMaxAnims;
	unsigned char m_iCurFrame;
	unsigned char m_iAnimFrames[MAX_ANIMS];
	unsigned char m_iLargestFrameCount;
	float m_fTimeFrameChange, m_fDesiredFramesPerSecond;
	float x_step, y_step;

public:
	CTexture( std::string szName, int iWidth, int iHeight, GLuint iGLTexture );
	virtual ~CTexture() {}

	void LoadTextureData( std::string szFile );
	void UpdateFrame( float fTime );

	GLuint GetTexture() { return m_iGLTexture; }
	SDL_Rect GetSize();
	Coords GetTextureCoords();
};
