#include "Universe.h"
#include "Tokenizer.h"
#include "Planet.h"
#include "ODEManager.h"

extern std::string itoa2(const int x);

CUniverse::CUniverse()
{
}

CUniverse::~CUniverse()
{
	CleanUp();
}

void CUniverse::CleanUp()
{
	if ( m_vPlanets.size() != 0 )
	{
		for ( unsigned int i = 0; i<m_vPlanets.size(); i++ )
			delete m_vPlanets[i];
		m_vPlanets.clear();
	}
	m_vUniverse.clear();
}

bool CUniverse::Load( std::string file )
{
	CleanUp();

	CTokenizer tokenizer;
	pTokenizer = &tokenizer;

	pFile = fopen( file.c_str(), "rt+" );
	if ( pFile )
	{
		while ( !feof(pFile) )
		{
			std::string in = ReadLine();
			if ( in == "[sun]" )
				ReadSun();
			if ( in == "[planet]" )
				ReadPlanet();
		}
		fclose( pFile );

		// Calculate positions
		for ( unsigned int i = 0; i<m_vUniverse.size(); i++ )
		{
			PlanetaryData d = m_vUniverse[i];
			if ( d.name == "sun" ) continue; // Sun is exempt from position, always at 0,0!

			if ( d.orbit != "" )
			{
				int orbitted = IndexByName( d.orbit );
				m_vUniverse[i].position = m_vUniverse[orbitted].position + Vector( (float)d.range, 0, 0 );
			}
		}

		// Create objects and set up orbits
		for ( unsigned int i = 0; i<m_vUniverse.size(); i++ )
		{
			CPlanet *pPlanet = new CPlanet( m_vUniverse[i] );
			m_vPlanets.push_back( pPlanet );
		}
		SetUpOrbits();
		return true;
	}
	return false;
}

void CUniverse::ReadSun()
{
	PlanetaryData sun;
	sun.asteroidcount = 0;
	sun.orbit = "";
	sun.range = 0;
	sun.radius = 64;
	sun.revolutiontime = 0;
	sun.orbitJoint = 0;

	std::string in = ReadLine();
	while ( !feof(pFile) && in != "" )
	{
		std::vector<std::string> tokens = pTokenizer->GetTokens( in );

		if ( tokens[0] == "name" )
			sun.name = tokens[2];
		else if ( tokens[0] == "mass" )
			sun.mass = atoi(tokens[2].c_str());
		else if ( tokens[0] == "grav" )
			sun.gravconst = (float)atof(tokens[2].c_str());
		else if ( tokens[0] == "image" )
			sun.image = tokens[2];
		else if ( tokens[0] == "angle" )
			sun.start_angle = atoi(tokens[2].c_str());
		else if ( tokens[0] == "radius" )
			sun.radius = atoi(tokens[2].c_str());

		in = ReadLine();
	}

	m_vUniverse.push_back( sun );
}

void CUniverse::ReadPlanet()
{
	PlanetaryData planet;
	planet.radius = 64;
	planet.orbitJoint = 0;

	std::string in = ReadLine();
	while ( !feof(pFile) && in != "" )
	{
		std::vector<std::string> tokens = pTokenizer->GetTokens( in );

		if ( tokens[0] == "name" )
			planet.name = tokens[2];
		else if ( tokens[0] == "orbit" )
		{
			planet.orbit = tokens[2];
			planet.range = atoi(tokens[3].c_str());
			planet.revolutiontime = atoi(tokens[4].c_str());
		}
		else if ( tokens[0] == "mass" )
			planet.mass = atoi(tokens[2].c_str());
		else if ( tokens[0] == "grav" )
			planet.gravconst = (float)atof(tokens[2].c_str());
		else if ( tokens[0] == "image" )
			planet.image = tokens[2];
		else if ( tokens[0] == "asteroids" )
			planet.asteroidcount = atoi(tokens[2].c_str());
		else if ( tokens[0] == "angle" )
			planet.start_angle = atoi(tokens[2].c_str());
		else if ( tokens[0] == "radius" )
			planet.radius = atoi(tokens[2].c_str());

		in = ReadLine();
	}

	m_vUniverse.push_back( planet );
}

int CUniverse::IndexByName( std::string name )
{
	for ( unsigned int i = 0; i<m_vUniverse.size(); i++ )
	{
		if ( m_vUniverse[i].name == name )
			return i;
	}
	return -1;
}

// object1 orbits around object2
void CUniverse::SetUpOrbit( std::string orbittee, std::string orbitted )
{
	int index1, index2;
	PlanetaryData data1, data2;
	CPlanet *pOrbittee, *pOrbitted;

	if ( orbitted == "" )
	{
		return;
		// Special static connection
		pOrbittee = m_vPlanets[IndexByName(orbittee)];
		dJointID joint = CODEManager::Instance()->CreateJoint( pOrbittee->GetBody(), 0 );
		dJointSetHingeAnchor( joint, 0.0f, 0.0f, 0.0f );
		pOrbittee->SetOrbitJoint( joint );
		return;
	}

	index1 = IndexByName( orbittee );
	index2 = IndexByName( orbitted );

	data1 = m_vUniverse[index1];
	data2 = m_vUniverse[index2];

	pOrbittee = m_vPlanets[index1];
	pOrbitted = m_vPlanets[index2];

	// Create joint
	dJointID joint = CODEManager::Instance()->CreateJoint( pOrbitted->GetBody(), pOrbittee->GetBody() );
	//dJointSetHingeAnchor( joint, pOrbitted->GetX(), pOrbitted->GetY(), 0.0f );
//	dJointSetAMotorParam( joint, dParamVel3, 25000 );
//	dJointSetAMotorParam( joint, dParamFMax3, 250000 );
	pOrbittee->SetOrbitJoint( joint );
	dBodySetLinearVel( pOrbittee->GetBody(), 0, -250, 0 );
}

std::string CUniverse::ReadLine()
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

void CUniverse::SetUpOrbits()
{
	for ( unsigned int i = 0; i<m_vUniverse.size(); i++ )
	{
		SetUpOrbit( m_vUniverse[i].name, m_vUniverse[i].orbit );
	}
}

void CUniverse::Update( float fTime )
{
}
