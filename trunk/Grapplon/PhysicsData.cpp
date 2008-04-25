#include "PhysicsData.h"

void PhysicsData::ToggleIgnore( PhysicsData *pData )
{
	for ( int i = 0; i<m_vIgnoreCollisions.size(); i++ )
	{
		if ( m_vIgnoreCollisions[i] == pData )
		{
			m_vIgnoreCollisions.erase( m_vIgnoreCollisions.begin() + i );
			return;
		}
	}
	m_vIgnoreCollisions.push_back( pData );
}

bool PhysicsData::CollidesWith(PhysicsData *pData)
{
	for ( int i = 0; i<m_vIgnoreCollisions.size(); i++ )
	{
		if ( m_vIgnoreCollisions[i] == pData )
		{
			return false;
		}
	}
	return true;
}
