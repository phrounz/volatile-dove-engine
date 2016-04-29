
#include "Burning.h"

#include "EngineSystem.h"

namespace
{
	const int SMALL_DAMAGE_COUNT = 2;
	const int BIG_DAMAGE_COUNT = 4;
}

EngineSystem::EngineSystem():m_nbDamagesPrecisionMachine(0), m_nbDamagesLatencyMachine(0)
{
	m_precisionMachine = new Sprite("data/precision_machine.png");
	m_precisionMachine->setPosition(Int2(1000,100));
	m_latencyMachine = new Sprite("data/latency_machine.png");
	m_latencyMachine->setPosition(Int2(1030,400));

	m_smallDamage = new Bitmap("data/small_damage.png");
	m_bigDamage = new Bitmap("data/big_damage.png");

	m_soundAlert = Engine::instance().getSoundMgr().loadSound("data/sounds/270643__hoerspielwerkstatt-hef__alarm5__mod.wav");
}

void EngineSystem::manage(const Burning* burning)
{
	m_nbDamagesPrecisionMachine = 0;
	m_nbDamagesLatencyMachine = 0;
	const std::list<Fire*>& fires = burning->getFires();
	iterateListConst(fires, Fire*)
	{
		if ((*it)->isColliding(*m_precisionMachine))
		{
			++m_nbDamagesPrecisionMachine;
		}
		else if ((*it)->isColliding(*m_latencyMachine))
		{
			++m_nbDamagesLatencyMachine;
		}
	}

	if (!m_soundAlert->isPlaying() && (this->hasPrecisionMachineBigDamage() || this->hasLatencyMachineBigDamage() || burning->hasCannonDamage()))
	{
		m_soundAlert->play();
	}
}

void EngineSystem::draw() const
{
	m_precisionMachine->draw();
	if (this->hasPrecisionMachineBigDamage())        m_bigDamage->draw(m_precisionMachine->getPosition(), 1.f, Float2(1.f, 1.f));
	else if (this->hasPrecisionMachineSmallDamage()) m_smallDamage->draw(m_precisionMachine->getPosition(), 1.f, Float2(1.f, 1.f));
	
	m_latencyMachine->draw();
	if (this->hasLatencyMachineBigDamage())          m_bigDamage->draw(m_latencyMachine->getPosition(), 1.f, Float2(1.f, 1.f));
	else if (this->hasLatencyMachineSmallDamage())   m_smallDamage->draw(m_latencyMachine->getPosition(), 1.f, Float2(1.f, 1.f));
}

bool EngineSystem::hasPrecisionMachineBigDamage()   const { return m_nbDamagesPrecisionMachine >= BIG_DAMAGE_COUNT; }
bool EngineSystem::hasPrecisionMachineSmallDamage() const { return m_nbDamagesPrecisionMachine >= SMALL_DAMAGE_COUNT; }

bool EngineSystem::hasLatencyMachineBigDamage()   const { return m_nbDamagesLatencyMachine >= BIG_DAMAGE_COUNT; }
bool EngineSystem::hasLatencyMachineSmallDamage() const { return m_nbDamagesLatencyMachine >= SMALL_DAMAGE_COUNT; }

EngineSystem::~EngineSystem()
{
	delete m_precisionMachine;
	delete m_latencyMachine;
	delete m_smallDamage;
	delete m_bigDamage;
	Engine::instance().getSoundMgr().removeSound(m_soundAlert);
}
