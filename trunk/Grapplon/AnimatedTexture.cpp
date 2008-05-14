#include "AnimatedTexture.h"
#include "Tokenizer.h"
#include "LogManager.h"
#include "ResourceManager.h"

CAnimatedTexture::CAnimatedTexture( std::string name )
{
	m_szScriptFile = name;
	LoadTextureData();

	m_iCurAnim = m_iCurFrame = 0;
	m_fTimeFrameChange = 0.0f;

	// 30 FPS
	m_fDesiredFramesPerSecond = 1.0f / 30.0f;
}

void CAnimatedTexture::LoadTextureData()
{
	std::vector<std::string> szTokens;
	CTokenizer tokenizer;

	pFile = fopen( m_szScriptFile.c_str(), "rt" );
	if ( !pFile )
		return;

	m_bHeaderRead = false;

	std::string in;
	in = ReadLine();
	while ( !feof( pFile ) )
	{
		if ( in == "[object]" )
			ReadHeader();
		else if ( m_bHeaderRead )
			ReadAnimation(in);
		in = ReadLine();
	}

	fclose( pFile );

	SetAnimation( 0 );
}

void CAnimatedTexture::UpdateFrame(float fTime)
{
	m_fTimeFrameChange += fTime;

	float step = m_fTimeFrameChange / m_fDesiredFramesPerSecond;
	if ( step >= 1.0f )
	{
		int div = (int)(step / 1.0f);
		m_iCurFrame += div;
		if ( m_iCurFrame >= m_vAnimations[m_iCurAnim].m_iFrames )
			m_iCurFrame -= m_vAnimations[m_iCurAnim].m_iFrames;

		m_fTimeFrameChange -= (div * m_fDesiredFramesPerSecond);
	}
}

Coords CAnimatedTexture::GetTextureCoords()
{
	Coords coords;

	coords.w = m_vAnimations[m_iCurAnim].m_fXStep;
	coords.h = 1;

	coords.x = coords.w * m_iCurFrame;
	coords.y = 0;

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

void CAnimatedTexture::ReadHeader()
{
	std::string in = "[object]";
	CTokenizer tokenizer;
	std::vector<std::string> tokens;

	size.x = size.y = size.w = size.h = 0;

	in = ReadLine();
	while ( !feof( pFile ) && in != "" )
	{
		tokens = tokenizer.GetTokens( in );

		if ( tokens[0] == "width" )
		{
			size.w = atoi(tokens[2].c_str());
		}
		else if ( tokens[0] == "height" )
		{
			size.h = atoi(tokens[2].c_str());
		}
		else if ( tokens[0] == "animations" )
		{
			int animCount = (int)tokens.size() - 2;
			for ( int i = 0; i<animCount; i++ )
			{
				Animation anim;
				anim.m_szName = tokens[2 + i];
				anim.m_pTexture = NULL;
				m_vAnimations.push_back( anim );
			}
		}

		in = ReadLine();
	}

	m_bHeaderRead = true;
}

void CAnimatedTexture::ReadAnimation(std::string anim)
{
	std::string in = anim;
	CTokenizer tokenizer;
	std::vector<std::string> tokens;

	tokens = tokenizer.GetTokens(anim);
	int index = -1;
	for ( unsigned int i = 0; i<m_vAnimations.size(); i++ )
	{
		if ( m_vAnimations[i].m_szName == tokens[0] )
		{
			index = i;
			break;
		}
	}

	// Animation name not found, read until end of animation info block, then exit function
	if ( index == -1 )
	{
		CLogManager::Instance()->LogMessage("Incorrect animation name!");
		while ( !feof( pFile ) && in != "" )
			in = ReadLine();
		return;
	}

	// Animation info valid, read data
	in = ReadLine();
	while ( !feof( pFile ) && in != "" )
	{
		tokens = tokenizer.GetTokens( in, " ,;[]:\"" );
		if ( tokens[0] == "file" )
		{
			m_vAnimations[index].m_pTexture = (CTexture *)CResourceManager::Instance()->GetResource(tokens[2], RT_TEXTURE);
		}
		else if ( tokens[0] == "frames" )
		{
			m_vAnimations[index].m_iFrames = atoi(tokens[2].c_str());
			if ( m_vAnimations[index].m_iFrames != 0 )
				m_vAnimations[index].m_fXStep = 1.0f / (float)m_vAnimations[index].m_iFrames;
			else
				m_vAnimations[index].m_fXStep = 1.0f;
		}
		else if ( tokens[0] == "speed" )
		{
			m_vAnimations[index].m_iSpeed = atoi(tokens[2].c_str());
		}
		in = ReadLine();
	}
}

std::string CAnimatedTexture::ReadLine()
{
	if ( !pFile || feof(pFile) )
		return "";

	char input[1024];
	fgets( input, 1024, pFile );
	if ( feof(pFile) )
		return "";
	int len = strlen(input);
	if ( len > 0 )
		input[len - 1] = 0; // Cut off the \n
	return std::string(input);
}

void CAnimatedTexture::SetAnimation( int iAnimation )
{
	if ( iAnimation < 0 || iAnimation >= (int)m_vAnimations.size() )
		return;
	
	m_iCurAnim = iAnimation;
	m_iCurFrame = 0;

	if ( m_vAnimations[iAnimation].m_iSpeed != 0.0f )
		m_fDesiredFramesPerSecond = 1.0f / (float)m_vAnimations[iAnimation].m_iSpeed;
	else
		m_fDesiredFramesPerSecond = 1.0f;
}

void CAnimatedTexture::Scale(float fScale)
{
	size.w = (Uint16)((float)size.w * fScale);
	size.h = (Uint16)((float)size.h * fScale);
}

void CAnimatedTexture::SetFrame( unsigned int iFrame )
{
	if ( iFrame > m_vAnimations[m_iCurAnim].m_iFrames )
		return;

	m_iCurFrame = iFrame;
}
