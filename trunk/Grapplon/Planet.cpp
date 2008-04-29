#include "Planet.h"
#include "ODEManager.h"
#include "ResourceManager.h"
#include "Texture.h"

#pragma warning(disable:4244)

CPlanet::CPlanet(PlanetaryData &data)
{
	this->data = &data;

	std::string image = "media/images/" + data.image;
	m_pImage = (CTexture *)CResourceManager::Instance()->GetResource(image, RT_TEXTURE);

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(m_oPhysicsData, (float)data.radius);

	SetDepth( -2.0f );
	SetMass( this->data->mass );
	SetPosition( this->data->position );

	m_oPhysicsData.m_fGravConst = this->data->gravconst;
	m_oPhysicsData.m_bAffectedByGravity = false;
}

CPlanet::~CPlanet()
{
}


void CPlanet::Render()
{
	CBaseObject::Render();
}

void CPlanet::Update( float fTime )
{
	CBaseObject::Update( fTime );
}
