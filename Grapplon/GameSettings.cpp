#include "GameSettings.h"
#include "Tokenizer.h"

#include <vector>

CGameSettings *CGameSettings::m_pInstance = NULL;

CGameSettings::CGameSettings()
{

/*
	THROW_FORCE			= 500000.0f;
	MAX_TURN_SPEED		= 70.0f;
	TURN_ACCEL			= 2.0f;
	EJECT_FORCE			= 6000000.0f;
	RETRACT_FORCE		= 10000.0f;
	PITCH_ACCEL_OUT		= 40.0f;
	PITCH_ACCEL_IN		= 0.0f;
	LINK_LENGTH			= 10.0f;
	LINK_AMOUNT			= 8;
	LINK_GRASP_CON		= 8;
	CENT_DIST_HOOK		= -18;
	CFM					= 0.001f;
	ERP					= 0.8f;
	HOOK_AIR_DRAG		= 2.0f;
	HOOK_MASS			= 1.0f;
	AUTO_AIM_ANGLE		= 10.0f;
*/

	THROW_FORCE				= -1;
	MAX_TURN_SPEED			= -1;
	TURN_ACCEL				= -1;
	EJECT_FORCE				= -1;
	RETRACT_FORCE			= -1;
	PITCH_ACCEL_OUT			= -1;
	PITCH_ACCEL_IN			= -1;
	LINK_LENGTH				= -1;
	LINK_AMOUNT				= -1;
	LINK_GRASP_CON			= -1;
	CENT_DIST_HOOK			= -1;
	CFM						= -1;
	ERP						= -1;
	HOOK_AIR_DRAG			= -1;
	HOOK_MASS				= -1;
	AUTO_AIM_ANGLE			= -1;
	WALL_BOUNCES			= -1;
	W_BOUNCE_TOGGLE_TIME	= -1;
	PLAYERS					= -1;
	DAMAGE_MULT				= -1;

	BUFFERS					= 2;
	BUFFER_SIZE				= 49600;

	MIN_ZOOM				= 2.0f;
	MAX_ZOOM				= 4.0f;

	MATCH_TIME				= 180.0f;
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

		if (	  tokens[0] == "ThrowForce" )			{ THROW_FORCE			= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "MaxTurnSpeed" )			{ MAX_TURN_SPEED		= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "TurnAcceleration" )		{ TURN_ACCEL			= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "EjectForce" )			{ EJECT_FORCE			= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "RetractForce" )    		{ RETRACT_FORCE			= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "PitchAccelOut" )		{ PITCH_ACCEL_OUT		= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "PitchAccelIn" )			{ PITCH_ACCEL_IN		= (float)atof( tokens[2].c_str() ); }
		else if ( tokens[0] == "MinAccelForProcess" )	{ MIN_ACCEL_FOR_PROCESS	= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "LinkLength" )			{ LINK_LENGTH			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "LinkAmount" )			{ LINK_AMOUNT			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "LinkGraspCon" )			{ LINK_GRASP_CON		=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "CentDistHook" )			{ CENT_DIST_HOOK		= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "CFM" )					{ CFM					= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "ERP" )					{ ERP					= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "HookAirDrag" )			{ HOOK_AIR_DRAG			= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "HookMass" )				{ HOOK_MASS				= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "AutoAimAngle" )			{ AUTO_AIM_ANGLE		= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "Level" )				{ LEVEL					=			   tokens[2]		  ;	}
		else if ( tokens[0] == "AsterWallBounces" )		{ WALL_BOUNCES			=	     atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "WBounceToggleTime" )    { W_BOUNCE_TOGGLE_TIME  = (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "Players" )				{ PLAYERS				=	     atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "DamageMult" )			{ DAMAGE_MULT			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "BufferSize" )			{ BUFFER_SIZE			=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "Buffers" )				{ BUFFERS				=		 atoi( tokens[2].c_str() );	}
		else if ( tokens[0] == "MinZoom" )				{ MIN_ZOOM				= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "MaxZoom" )				{ MAX_ZOOM				= (float)atof( tokens[2].c_str() );	}
		else if ( tokens[0] == "MatchTime" )			{ MATCH_TIME			= (float)atof( tokens[2].c_str() );	}
		in = ReadLine();


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
