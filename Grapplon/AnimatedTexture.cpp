#include "AnimatedTexture.h"
#include "Tokenizer.h"
#include "LogManager.h"
#include "ResourceManager.h"

CAnimatedTexture::CAnimatedTexture( std::string name )
{
	m_pTexture = (CTexture *)CResourceManager::Instance()->GetResource(name, RT_TEXTURE);
	size = m_pTexture->GetSize();

	m_iCurAnim = m_iCurFrame = 0;
	m_iMaxAnims = 1;
	m_iAnimFrames[0] = 1;
	m_iLargestFrameCount = 1;
	m_fTimeFrameChange = 0.0f;
	x_step = y_step = 1.0f;

	// 30 FPS
	m_fDesiredFramesPerSecond = 1.0f / 30.0f;

	LoadTextureData();
}

void CAnimatedTexture::LoadTextureData()
{
	std::string szFile = "media/scripts/";
	std::vector<std::string> szTokens;
	CTokenizer tokenizer;
	szTokens = tokenizer.GetTokens( m_pTexture->GetName(), "/." );
	szFile += szTokens[2] + ".txt";

	FILE *pFile = fopen( szFile.c_str(), "rt" );
	if ( !pFile )
		return;

	m_iLargestFrameCount = 1;

	char read[256];
	while ( !feof( pFile ) )
	{
		fgets(read, 256, pFile);
		std::string in = read;

		szTokens = tokenizer.GetTokens(in, " ={,}\n");
		if ( szTokens[0] == "animcount" )
		{
			m_iMaxAnims = atoi(szTokens[1].c_str());
			if ( m_iMaxAnims > MAX_ANIMS )
			{
				CLogManager::Instance()->LogMessage( "Texture script error: Too many animations listed in animcount [" + szFile + "]" );
				return;
			}
		}
		if ( szTokens[0] == "animframes" )
		{
			if ( szTokens.size() - 1 > m_iMaxAnims )
			{
				CLogManager::Instance()->LogMessage( "Texture script error: Too many frames listed in animframes [" + szFile + "]" );
				continue;
			}
			for ( unsigned int i = 1; i<szTokens.size(); i++ )
			{
				m_iAnimFrames[i - 1] = atoi(szTokens[i].c_str());
				if ( m_iAnimFrames[i - 1] > m_iLargestFrameCount )
					m_iLargestFrameCount = m_iAnimFrames[i - 1];
			}
		}
	}

	x_step = 1.0f / m_iLargestFrameCount;
	y_step = 1.0f / m_iMaxAnims;

	size = m_pTexture->GetSize();
	size.w /= m_iLargestFrameCount;
	size.h /= m_iMaxAnims;

	fclose( pFile );
}

void CAnimatedTexture::UpdateFrame(float fTime)
{
	m_fTimeFrameChange += fTime;

	float step = m_fTimeFrameChange / m_fDesiredFramesPerSecond;
	if ( step >= 1.0f )
	{
		int div = (int)(step / 1.0f);
		m_iCurFrame += div;
		if ( m_iCurFrame >= m_iAnimFrames[m_iCurAnim] )
			m_iCurFrame -= m_iAnimFrames[m_iCurAnim];

		m_fTimeFrameChange -= (div * m_fDesiredFramesPerSecond);
	}
}

Coords CAnimatedTexture::GetTextureCoords()
{
	Coords coords;

	coords.x = x_step * m_iCurFrame;
	coords.y = y_step * m_iCurAnim;

	coords.w = x_step;
	coords.h = y_step;

	return coords;
}


SDL_Rect CAnimatedTexture::GetSize()
{
	return size;
}

void CAnimatedTexture::SetFramerate( unsigned int FramesPerSecond )
{
	if ( FramesPerSecond > 0 )
		m_fDesiredFramesPerSecond = (1.0f / (float)FramesPerSecond);
}
