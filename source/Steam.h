#pragma once
#ifdef USES_STEAM_INTEGRATION

namespace Steam
{
	void init(); ///< init
	void runStep();
	void deinit(); ///< deinit
}

#endif //USES_STEAM_INTEGRATION
