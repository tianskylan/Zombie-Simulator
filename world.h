#pragma once

struct MachineState;

#include "singleton.h"
#include "machine.h"
#include "traits.h"
#include <list>


#define DECLARE_SINGLETON(SingletonClass) friend class Singleton<SingletonClass>;

class world :
	public Singleton<world>
{
public:
	world();
	~world();
	std::list<MachineState*> humans;
	std::list<MachineState*> zombies;

	Machine<ZombieTraits> zombieMachine;
	Machine<HumanTraits> humanMachine;

	int month = 0;

	bool simulating = false;

	bool isPassable(int x, int y)
	{
		bool retval = true;

		if (x < 0 || x > 19)
		{
			retval = false;
		}

		if (y < 0 || y > 19)
		{
			retval = false;
		}

		auto it = world::get().humans.begin();
		while (it != world::get().humans.end())
		{
			if ((*it)->x == x && (*it)->y == y)
			{
				retval = false;
			}
			++it;
		}

		it = world::get().zombies.begin();
		while (it != world::get().zombies.end())
		{
			if ((*it)->x == x && (*it)->y == y)
			{
				retval = false;
			}
			++it;
		}

		return retval;

	}

};

