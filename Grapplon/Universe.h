#pragma once

#include <stdio.h>
#include <string>
#include <vector>

#include "ActiveObject.h"
#include "PlanetaryData.h"

class CTokenizer;
class CPlanet;

class CUniverse
{
private:
	void CleanUp();
	void ReadPlanet(ObjectType planetType);
	void ReadUniverse();

	FILE *pFile;
	CTokenizer *pTokenizer;
	std::vector<PlanetaryData> m_vUniverse;

	std::vector<CPlanet *> m_vPlanets;

	void SetUpOrbit( CPlanet* orbittee, CPlanet* orbitted );
	int IndexByName( std::string name );
	std::string ReadLine();

public:
	CUniverse();
	~CUniverse();

	bool Load( std::string file );
	void Update( float fTime );
};
