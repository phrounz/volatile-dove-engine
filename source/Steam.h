#pragma once
#ifdef USES_STEAM_INTEGRATION

namespace Steam
{
	struct AchievementInfo
	{
		AchievementInfo(std::string pId, std::string pName, std::string pDescription) : id(pId), name(pName), description(pDescription) {}
		std::string id, name, description;
	};
	void init(const std::vector<AchievementInfo>& achievementInfos); ///< init
	void runStep();///< manage SteamWorks stuff
	void unlockAchievement(const std::string& str);//< unlock the achievement with id str
	void clearAchievement(const std::string& str);///< de-unlock the achievement with id str (mostly for development purpose)
	void deinit(); ///< deinit
}

#endif //USES_STEAM_INTEGRATION
