#ifndef EngineSystem_h_INCLUDED
#define EngineSystem_h_INCLUDED

#include "../../include/AbstractMainClass.h"
#include "../../include/Sprite.h"
#include "../../include/Image.h"
#include "../../include/Bitmap.h"

class Burning;

class EngineSystem
{
public:

	EngineSystem();
	void manage(const Burning* burning);
	void draw() const;
	bool hasPrecisionMachineBigDamage()   const;
	bool hasPrecisionMachineSmallDamage() const;
	bool hasLatencyMachineBigDamage()   const;
	bool hasLatencyMachineSmallDamage() const;
	~EngineSystem();

private:
	Sprite* m_precisionMachine;
	Sprite* m_latencyMachine;
	Bitmap* m_smallDamage;
	Bitmap* m_bigDamage;
	int m_nbDamagesPrecisionMachine;
	int m_nbDamagesLatencyMachine;
	Sound* m_soundAlert;
};

#endif //EngineSystem_h_INCLUDED
