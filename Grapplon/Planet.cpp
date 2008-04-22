#include "Planet.h"
#include "ResourceManager.h"
#include "Texture.h"

CPlanet::CPlanet()
{
	m_pImage = (CTexture *)CResourceManager::Instance()->GetResource("media/images/Player.png", RT_TEXTURE);
}
