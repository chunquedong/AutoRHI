
#include "GraphicsDevice.h"

using namespace arhi;

GraphicsDevice* g_graphicsDevice = NULL;

GraphicsDevice* GraphicsDevice::cur() {
	return g_graphicsDevice;
}