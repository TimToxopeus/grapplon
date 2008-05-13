#include "ParticleBehaviour.h"

CParticleBehaviour::CParticleBehaviour( std::string szName, float fEffect, float fVelocity )
{
	m_szName = szName;
	m_fVelocity = fVelocity;
	m_fEffect = fEffect;
}

Vector CParticleBehaviour::ComputeDirection( Vector direction )
{
	return direction.Rotate( m_fEffect ) * m_fVelocity;
}
