#include "Planet.h"
#include "ResourceManager.h"
#include "Texture.h"

#pragma warning(disable:4244)

CPlanet::CPlanet(const float& fMass)
{
	m_pImage = (CTexture *)CResourceManager::Instance()->GetResource("media/images/white_radius.png", RT_TEXTURE);
	SetDepth( 2.0f );

	// Update naar veel zwaardere mass
	SetMass( fMass );
}

CPlanet::~CPlanet()
{
}


void CPlanet::Render()
{

	//SDL_Color colour;
	//colour.r = colour.g = colour.b = 128;

	CBaseObject::Render();
}

void CPlanet::Update( float fTime )
{

}
