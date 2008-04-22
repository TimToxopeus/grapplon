#pragma once

#include "Resource.h"
#include <SDL_OpenGL.h>

class CTexture : public IResource
{
protected:
	GLuint m_iGLTexture;
	SDL_Rect size;

public:
	CTexture( std::string szName, int iWidth, int iHeight, GLuint iGLTexture ) : IResource( szName ) { m_eType = RT_TEXTURE; m_iGLTexture = iGLTexture; size.x = size.y = 0; size.w = iWidth; size.h = iHeight; }
	virtual ~CTexture() {}

	GLuint GetTexture() { return m_iGLTexture; }
	SDL_Rect GetSize() { return size; }
};
