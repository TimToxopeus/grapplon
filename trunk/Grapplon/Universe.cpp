#include "Universe.h"
#include "Tokenizer.h"
#include "Planet.h"
#include "ODEManager.h"
#include "GameSettings.h"
#include "Sun.h"
#include "OrdinaryPlanet.h"
#include "Asteroid.h"


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
			if		( in == "[sun]" ) 		{	ReadPlanet(SUN);	}
			else if ( in == "[ordinary]" )	{	ReadPlanet(ORDINARY);	}
			else if ( in == "[asteroid]" )	{	ReadPlanet(ASTEROID);	}
			else if ( in == "[ice]" )		{	ReadPlanet(ICE);	}
			else if ( in == "[broken]" )	{	ReadPlanet(BROKEN);	}
			else if ( in == "[universe]")	{	ReadUniverse();	}
		}
		fclose( pFile );

		for ( unsigned int i = 0; i<m_vUniverse.size(); i++ )
		{
			if		( m_vUniverse[i].planetType == SUN )		{	m_vPlanets.push_back( new CSun( m_vUniverse[i] ) );				}
			else if ( m_vUniverse[i].planetType == ASTEROID )	{	m_vPlanets.push_back( new CAsteroid( m_vUniverse[i] ) );		}
			else if ( m_vUniverse[i].planetType == ICE )		{	m_vPlanets.push_back( new COrdinaryPlanet( m_vUniverse[i] ) );	}
			else if ( m_vUniverse[i].planetType == BROKEN )		{	m_vPlanets.push_back( new COrdinaryPlanet( m_vUniverse[i] ) );	}
			else if ( m_vUniverse[i].planetType == ORDINARY )	{	m_vPlanets.push_back( new COrdinaryPlanet( m_vUniverse[i] ) );	}
		}

		// Calculate positions
		for ( unsigned int i = 0; i<m_vUniverse.size(); i++ )
		{
			PlanetaryData& d = m_vUniverse[i];
			if ( d.planetType == SUN || d.orbit == "") continue;

			CPlanet* orbitted = m_vPlanets[IndexByName( d.orbit )];
			SetUpOrbit(m_vPlanets[i], orbitted);
		}

		// Create objects and set up orbits
		return true;
	}
	return false;
}

void CUniverse::ReadUniverse()
{
	std::string in = ReadLine();
	while ( !feof(pFile) && in != "" )
	{
		std::vector<std::string> tokens = pTokenizer->GetTokens( in, " ,;[]:\t" );

		if ( tokens[0]		== "width" )			CODEManager::Instance()->m_iWidth	= atoi(tokens[2].c_str());
		if ( tokens[0]		== "height" )			CODEManager::Instance()->m_iHeight	= atoi(tokens[2].c_str());
		else if ( tokens[0] == "boundaryforce" )	CODEManager::Instance()->m_iBoundaryForce = atoi(tokens[2].c_str());

		in = ReadLine();
	}


}

void CUniverse::ReadPlanet(ObjectType planType)
{
	PlanetaryData planetData;
	planetData.imageOrbit = "";
	planetData.imageGlow = "";

	planetData.orbitLength = 0;
	planetData.orbitSpeed = 0;
	planetData.asteroidcount = 0;
	planetData.orbitJoint = 0;
	planetData.rotation = 0;
	planetData.scale = 1.0f;
	planetData.emitter = "";
	planetData.orbit = "";
	planetData.position = Vector(0, 0, 0);

	planetData.planetType = planType;

	std::string in = ReadLine();

	while ( !feof(pFile) && in != "" )
	{
		std::vector<std::string> tokens = pTokenizer->GetTokens( in );

		if		( tokens[0] == "name" )				planetData.name				= tokens[2];
		else if ( tokens[0] == "orbit" )			{
													planetData.orbit			= tokens[2];
													planetData.orbitLength		= atoi(tokens[3].c_str());
													planetData.orbitSpeed		= (float)atof(tokens[4].c_str());
													}
		else if ( tokens[0] == "mass" )				planetData.mass				= atoi(tokens[2].c_str());
		else if ( tokens[0] == "grav" )				planetData.gravconst		= (float)atof(tokens[2].c_str());
		else if ( tokens[0] == "image" )			planetData.image			= tokens[2];
		else if ( tokens[0] == "imageorbit" )		planetData.imageOrbit		= tokens[2];
		else if ( tokens[0] == "imageglow" )		planetData.imageGlow		= tokens[2];
		else if ( tokens[0] == "asteroids" )		planetData.asteroidcount	= atoi(tokens[2].c_str());
		else if ( tokens[0] == "angle" )			planetData.orbitAngle		= atoi(tokens[2].c_str());
		else if ( tokens[0] == "radius" )			planetData.radius			= atoi(tokens[2].c_str());
		else if ( tokens[0] == "pos" )	
		{
			planetData.position[0]												= (float)atof(tokens[2].c_str());
			planetData.position[1]												= (float)atof(tokens[3].c_str());
		}
		else if ( tokens[0] == "rotation" )	planetData.rotation					= atoi(tokens[2].c_str());
		else if ( tokens[0] == "scale" )	planetData.scale					= (float)atof(tokens[2].c_str());
		else if ( tokens[0] == "emitter" )	
		{
			planetData.emitter													= tokens[2];
			planetData.bNear													= (tokens[3] == "near" ? true : false);
			planetData.offsetForward											= atoi(tokens[4].c_str());
			planetData.offsetRight												= atoi(tokens[5].c_str());
		}

		in = ReadLine();
	}

	m_vUniverse.push_back( planetData );
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
void CUniverse::SetUpOrbit( CPlanet* orbitter, CPlanet* orbitted )
{
	orbitter->orbitOwner = orbitted;
	orbitter->SetPosition( orbitted->GetPosition() + Vector::FromAngleLength(orbitter->m_fOrbitLength, orbitter->m_fOrbitAngle) );

	Vector hingePos = orbitted->GetPosition();
	// Create joint
	dJointID joint = CODEManager::Instance()->createHingeJoint("Orbit Joint");
	dJointAttach( joint, orbitted->GetBody(), orbitter->GetBody() );
	dJointSetHingeAnchor(joint, hingePos[0], hingePos[1], 0.0f);
	orbitter->SetOrbitJoint( joint );
	orbitter->GetPhysicsData()->planetData->bIsOrbitting = true;
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

void CUniverse::Update( float fTime )
{
}
