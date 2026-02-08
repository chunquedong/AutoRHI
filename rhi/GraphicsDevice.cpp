
#include "GraphicsDevice.h"

using namespace mrhi;

GraphicsDevice* g_graphicsDevice = NULL;

GraphicsDevice* GraphicsDevice::cur() {
	return g_graphicsDevice;
}