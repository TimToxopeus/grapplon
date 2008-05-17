#include "GameSettings.h"
#include "Tokenizer.h"

#include <vector>

CGameSettings *CGameSettings::m_pInstance = NULL;

CGameSettings::CGameSettings()
{
	m_fThrowForce = 500000.0f;
	m_fMaxTurnSpeed = 70.0f;
	m_fTurnAcceleration = 2.0f;
	m_fEjectForce = 6000000.0f;
	m_fRetractForce = 10000.0f;

	Init();
}

CGameSettings::~CGameSettings()
{
}

bool CGameSettings::Init()
{
	pFile = fopen( "media/scripts/settings.txt", "rt" );
	if ( !pFile )
		return false;

	CTokenizer tokenizer;

	std::string in = ReadLine();
	std::vector<std::string> tokens;
	while ( in != "" )
	{
		tokens = tokenizer.GetTokens( in );
		if ( tokens.size() == 0 )
			continue;

		if ( tokens[0] == "ThrowForce" )
		{
			m_fThrowForce = (float)atof( tokens[2].c_str() );
		}
		else if ( tokens[0] == "MaxTurnSpeed" )
		{
			m_fMaxTurnSpeed = (float)atof( tokens[2].c_str() );
		}
		else if ( tokens[0] == "TurnAcceleration" )
		{
			m_fTurnAcceleration = (float)atof( tokens[2].c_str() );
		}
		else if ( tokens[0] == "EjectForce" )
		{
			m_fEjectForce = (float)atof( tokens[2].c_str() );
		}
		else if ( tokens[0] == "RetractForce" )
		{
			m_fRetractForce = (float)atof( tokens[2].c_str() );
		}
	}

	fclose( pFile );
	return true;
}

std::string CGameSettings::ReadLine()
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
