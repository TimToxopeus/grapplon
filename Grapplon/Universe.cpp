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
	sun.orbitSpeed = 0;
	sun.orbitJoint = 0;
	sun.planetType = SUN;
	sun.orbitStyle = STATIC;
	sun.rotation = 0;
	sun.scale = 1.0f;
	sun.emitter = "";

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
		else if ( tokens[0] == "pos" )
		{
			sun.position[0] = (float)atof(tokens[2].c_str());
			sun.position[1] = (float)atof(tokens[3].c_str());
		}
		else if ( tokens[0] == "rotation" )
			sun.rotation = atoi(tokens[2].c_str());
		else if ( tokens[0] == "scale" )
			sun.scale = (float)atof(tokens[2].c_str());
		else if ( tokens[0] == "emitter" )
		{
			sun.emitter = tokens[2];
			sun.bNear = (tokens[3] == "near" ? true : false);
			sun.offsetForward = atoi(tokens[4].c_str());
			sun.offsetRight = atoi(tokens[5].c_str());
		}

		in = ReadLine();
	}

	m_vUniverse.push_back( sun );
}

void CUniverse::ReadPlanet()
{
	PlanetaryData planet;
	planet.radius = 64;
	planet.orbitJoint = 0;
	planet.planetType = NORMAL;
	planet.orbitStyle = STATIC;
	planet.rotation = 0;
	planet.scale = 1.0f;
	planet.emitter = "";

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
			planet.orbitSpeed = (float)atof(tokens[4].c_str());
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
		else if ( tokens[0] == "planet_type" )
		{
			if ( tokens[2] == "normal" )
				planet.planetType = NORMAL;
			else if ( tokens[2] == "ice" )
				planet.planetType = ICE;
			else if ( tokens[2] == "broken" )
				planet.planetType = BROKEN;
		}
		else if ( tokens[0] == "orbit_style" )
		{
			if ( tokens[2] == "static" )
				planet.orbitStyle = STATIC;
			else if ( tokens[2] == "circle" )
				planet.orbitStyle = CIRCLE;
			else if ( tokens[2] == "ellipse" )
				planet.orbitStyle = ELLIPSE;
		}
		else if ( tokens[0] == "pos" )
		{
			planet.position[0] = (float)atof(tokens[2].c_str());
			planet.position[1] = (float)atof(tokens[3].c_str());
		}
		else if ( tokens[0] == "rotation" )
			planet.rotation = atoi(tokens[2].c_str());
		else if ( tokens[0] == "scale" )
			planet.scale = (float)atof(tokens[2].c_str());
		else if ( tokens[0] == "emitter" )
		{
			planet.emitter = tokens[2];
			planet.bNear = (tokens[3] == "near" ? true : false);
			planet.offsetForward = atoi(tokens[4].c_str());
			planet.offsetRight = atoi(tokens[5].c_str());
		}

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
	return;
	int index1, index2;
	PlanetaryData data1, data2;
	CPlanet *pOrbittee, *pOrbitted;

	if ( orbitted == "" )
		return;

	index1 = IndexByName( orbittee );
	index2 = IndexByName( orbitted );

	data1 = m_vUniverse[index1];
	data2 = m_vUniverse[index2];

	pOrbittee = m_vPlanets[index1];
	pOrbitted = m_vPlanets[index2];

	// Create joint
	//dJointID joint = CODEManager::Instance()->CreateJoint( pOrbitted->GetBody(), pOrbittee->GetBody(), data2.position[0], data2.position[1] );
	dJointID joint = CODEManager::Instance()->createHingeJoint();
	dJointAttach( joint, pOrbitted->GetBody(), pOrbittee->GetBody() );
	dJointSetHingeAnchor(joint, data2.position[0], data2.position[1], 0.0f);
	pOrbittee->SetOrbitJoint( joint );
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
