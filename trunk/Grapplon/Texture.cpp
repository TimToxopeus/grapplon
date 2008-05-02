#include "Texture.h"

CTexture::CTexture( std::string szName, int iWidth, int iHeight, GLuint iGLTexture ) : IResource( szName )
{
	m_eType = RT_TEXTURE;
	m_iGLTexture = iGLTexture;
	size.x = size.y = 0;
	size.w = iWidth; size.h = iHeight;

	m_iCurAnim = m_iCurFrame = 0;
	m_iMaxAnims = 1;
	m_iAnimFrames[0] = 1;
	m_iLargestFrameCount = 1;
	m_fTimeFrameChange = 0.0f;
	x_step = y_step = 1.0f;

	// 30 FPS
	m_fDesiredFramesPerSecond = 1.0f / 30.0f;
}

SDL_Rect CTexture::GetSize()
{
	return size;
}

void CTexture::LoadTextureData( std::string szFile )
{
	if ( szFile == "" )
		return;

	FILE *pFile = fopen( szFile, "rt" );
	if ( !pFile )
		return;

	x_step = 1.0f / m_iLargestFrameCount;
	y_step = 1.0f / m_iMaxAnims;

	fclose( pFile );
}

void CTexture::UpdateFrame(float fTime)
{
	m_fTimeFrameChange += fTime;

	float step = m_fTimeFrameChange / m_fDesiredFramesPerSecond;
	if ( step >= 1.0f )
	{
		int div = (int)(step / 1.0f);
		m_iCurFrame += div;
		if ( m_iCurFrame > m_iAnimFrames[m_iCurAnim] )
			m_iCurFrame -= m_iAnimFrames[m_iCurAnim];
	}
}

Coords CTexture::GetTextureCoords()
{
	Coords coords;

	coords.x = x_step * m_iCurFrame;
	coords.y = y_step * m_iCurAnim;

	coords.w = x_step;
	coords.h = y_step;

	return coords;
}
