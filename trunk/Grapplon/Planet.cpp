#include "Planet.h"
#include "ResourceManager.h"
#include "Texture.h"

#pragma warning(disable:4244)

CPlanet::CPlanet(PlanetaryData &data)
{
	this->data = &data;

	std::string image = "media/images/" + data.image;
	m_pImage = (CTexture *)CResourceManager::Instance()->GetResource(image, RT_TEXTURE);
	SetDepth( 2.0f );

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
/*	if ( data->orbitJoint )
	{
		dJointSetHingeParam( data->orbitJoint, dParamVel, 25000 );
		dJointSetHingeParam( data->orbitJoint, dParamFMax, 250000 );
	}*/
}
