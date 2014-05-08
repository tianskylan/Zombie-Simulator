#include "stdafx.h"
#include "op.h"
#include "machine.h"
#include "world.h"
#include <iostream>

// Output state information for debugging purposes
void Op::DebugOutput(MachineState& state)
{
	//std::cout << state.m_ProgramCounter << ":" << m_OpName << "," << m_Param << std::endl;
	//std::cout << "position: " << state.x << "." << state.y << " - facing: " << state.m_Facing << std::endl;
}

void OpRotate::Execute(MachineState& state)
{
	DebugOutput(state);
	switch (state.m_Facing)
	{
	case (MachineState::UP) :
		if (m_Param == 0)
		{
			state.m_Facing = MachineState::RIGHT;
		}
		else
		{
			state.m_Facing = MachineState::LEFT;
		}
		break;
	case (MachineState::RIGHT) :
		if (m_Param == 0)
		{
			state.m_Facing = MachineState::DOWN;
		}
		else
		{
			state.m_Facing = MachineState::UP;
		}
		break;
	case (MachineState::DOWN) :
		if (m_Param == 0)
		{
			state.m_Facing = MachineState::LEFT;
		}
		else
		{
			state.m_Facing = MachineState::RIGHT;
		}
		break;
	default:
	case (MachineState::LEFT) :
		if (m_Param == 0)
		{
			state.m_Facing = MachineState::UP;
		}
		else
		{
			state.m_Facing = MachineState::DOWN;
		}
		break;
	}

	state.m_ProgramCounter++;
	state.m_ActionsTaken++;
}

void OpGoto::Execute(MachineState& state)
{
	DebugOutput(state);

	if (m_Param < 1 || m_Param > state.num_ops || m_Param == state.m_ProgramCounter)
	{
		WrongLineNumberError wrongline;
		throw wrongline;
	}

	state.m_ProgramCounter = m_Param;
}

void OpAttack::Execute(MachineState& state)
{
	DebugOutput(state);

	int dest_x, dest_y;

	switch (state.m_Facing)
	{
	case (MachineState::UP) :
		dest_y = state.y - 1;
		dest_x = state.x;
		break;
	case(MachineState::DOWN) :
		dest_y = state.y + 1;
		dest_x = state.x;
		break;
	case(MachineState::LEFT) :
		dest_y = state.y;
		dest_x = state.x - 1;
		break;
	case(MachineState::RIGHT) :
		dest_y = state.y;
		dest_x = state.x + 1;
		break;
	default:
		break;
	}

	// find the creature being attacked
	auto it = world::get().zombies.begin();
	while (it != world::get().zombies.end())
	{
	
		if ((*it)->x == dest_x && (*it)->y == dest_y)
		{
			// Z v Z
			if (state.GetInfect() == true)
			{
				//nothing happens
			}
			//H v Z
			else
			{
				//it = world::get().zombies.erase(it);
				(*it)->m_dead = true;
			}
		}
		++it;
	}


	it = world::get().humans.begin(); 
	while(it != world::get().humans.end())
	{
		if ((*it)->x == dest_x && (*it)->y == dest_y)
		{
			// Z v H
			if (state.GetInfect() == true)
			{
				//it = world::get().humans.erase(it);
				(*it)->m_infected = true;
				(*it)->m_ProgramCounter = 1;
			}
			//H v H
			else
			{
				//it = world::get().humans.erase(it);
				(*it)->m_dead = true;
			}
		}
		
		++it;
	}

	state.m_ProgramCounter++;
	state.m_ActionsTaken++;

}

void OpTestWall::Execute(MachineState& state)
{
	DebugOutput(state);

	state.m_Test = false;

	if (state.m_Facing == MachineState::UP && state.y == 0)
	{
		state.m_Test = true;
	}
	if (state.m_Facing == MachineState::DOWN && state.y == 19)
	{
		state.m_Test = true;
	}
	if (state.m_Facing == MachineState::LEFT && state.x == 0)
	{
		state.m_Test = true;
	}
	if (state.m_Facing == MachineState::RIGHT && state.x == 19)
	{
		state.m_Test = true;
	}

	state.m_ProgramCounter++;
}

void OpJe::Execute(MachineState& state)
{
	DebugOutput(state);

	if (m_Param < 1 || m_Param > state.num_ops || m_Param == state.m_ProgramCounter)
	{
		WrongLineNumberError wrongline;
		throw wrongline;
	}

	if (state.m_Test)
	{
		state.m_ProgramCounter = m_Param;
	}
	else
	{
		state.m_ProgramCounter++;
	}
}

void OpForward::Execute(MachineState& state)
{
	DebugOutput(state);

	int dest_x = -1;
	int dest_y = -1;


	switch (state.m_Facing)
	{
	case (MachineState::UP) :
		dest_y = state.y - 1;
		dest_x = state.x;
		break;
	case(MachineState::DOWN) :
		dest_y = state.y + 1;
		dest_x = state.x;
		break;
	case(MachineState::LEFT) :
		dest_y = state.y;
		dest_x = state.x - 1;
		break;
	case(MachineState::RIGHT) :
		dest_y = state.y;
		dest_x = state.x + 1;
		break;
	default:
		break;
	}

	if (world::get().isPassable(dest_x, dest_y))
	{
		state.x = dest_x;
		state.y = dest_y;
	}

	state.m_ProgramCounter++;
	state.m_ActionsTaken++;
}

void OpTestRandom::Execute(MachineState& state)
{
	DebugOutput(state);
	int flag = std::rand() % 2;
	std::cout << flag << std::endl;

	if (flag == 1)
	{
		state.m_Test = true;
	}
	else
	{
		state.m_Test = false;
	}

	state.m_ProgramCounter++;
}

void OpRangedAttack::Execute(MachineState& state)
{
	DebugOutput(state);

	int dest_x, dest_y;

	switch (state.m_Facing)
	{
	case (MachineState::UP) :
		dest_y = state.y - 2;
		dest_x = state.x;
		break;
	case(MachineState::DOWN) :
		dest_y = state.y + 2;
		dest_x = state.x;
		break;
	case(MachineState::LEFT) :
		dest_y = state.y;
		dest_x = state.x - 2;
		break;
	case(MachineState::RIGHT) :
		dest_y = state.y;
		dest_x = state.x + 2;
		break;
	default:
		break;
	}

	// find the creature being attacked

	
	auto it = world::get().zombies.begin();
	while (it != world::get().zombies.end())
	{
		if ((*it)->x == dest_x && (*it)->y == dest_y)
		{
			//it = world::get().zombies.erase(it);
			(*it)->m_dead = true;
		}
		++it;
	}

	it = world::get().humans.begin();
	while (it != world::get().humans.end())
	{
		if ((*it)->x == dest_x && (*it)->y == dest_y)
		{
			//it = world::get().humans.erase(it);
			(*it)->m_dead = true;
		}

		++it;
	}

	state.m_ProgramCounter++;
	state.m_ActionsTaken++;
}

void OpEndTurn::Execute(MachineState& state)
{
	DebugOutput(state);
	state.m_ActionsTaken += 100; //this definitely ends the turn
	state.m_ProgramCounter++;
}

void OpTestHuman::Execute(MachineState& state)
{
	DebugOutput(state);

	int dest_x, dest_y;

	switch (state.m_Facing)
	{
	case (MachineState::UP) :
		dest_y = state.y - m_Param;
		dest_x = state.x;
		break;
	case(MachineState::DOWN) :
		dest_y = state.y + m_Param;
		dest_x = state.x;
		break;
	case(MachineState::LEFT) :
		dest_y = state.y;
		dest_x = state.x - m_Param;
		break;
	case(MachineState::RIGHT) :
		dest_y = state.y;
		dest_x = state.x + m_Param;
		break;
	default:
		break;
	}

	state.m_Test = false;

	auto it = world::get().humans.begin();
	while (it != world::get().humans.end())
	{
		if ((*it)->x == dest_x && (*it)->y == dest_y)
		{
			state.m_Test = true;
		}

		++it;
	}
	/*
	for (auto it = world::get().humans.begin(); it != world::get().humans.end(); ++it)
	{
		if ((*it)->x == dest_x && (*it)->y == dest_y)
		{
			state.m_Test = true;
		}
	}
	*/

	state.m_ProgramCounter++;
}

void OpTestZombie::Execute(MachineState& state)
{
	DebugOutput(state);

	int dest_x, dest_y;

	switch (state.m_Facing)
	{
	case (MachineState::UP) :
		dest_y = state.y - m_Param;
		dest_x = state.x;
		break;
	case(MachineState::DOWN) :
		dest_y = state.y + m_Param;
		dest_x = state.x;
		break;
	case(MachineState::LEFT) :
		dest_y = state.y;
		dest_x = state.x - m_Param;
		break;
	case(MachineState::RIGHT) :
		dest_y = state.y;
		dest_x = state.x + m_Param;
		break;
	default:
		break;
	}

	state.m_Test = false;

	auto it = world::get().zombies.begin();
	while (it != world::get().zombies.end())
	{
		if ((*it)->x == dest_x && (*it)->y == dest_y)
		{
			state.m_Test = true;
		}

		++it;
	}
	/*
	for (auto it = world::get().zombies.begin(); it != world::get().zombies.end(); ++it)
	{
		if ((*it)->x == dest_x && (*it)->y == dest_y)
		{
			state.m_Test = true;
		}
	}
	*/

	state.m_ProgramCounter++;
}

void OpTestPassable::Execute(MachineState& state)
{
	DebugOutput(state);

	int dest_x, dest_y;

	switch (state.m_Facing)
	{
	case (MachineState::UP) :
		dest_y = state.y - 1;
		dest_x = state.x;
		break;
	case(MachineState::DOWN) :
		dest_y = state.y + 1;
		dest_x = state.x;
		break;
	case(MachineState::LEFT) :
		dest_y = state.y;
		dest_x = state.x - 1;
		break;
	case(MachineState::RIGHT) :
		dest_y = state.y;
		dest_x = state.x + 1;
		break;
	default:
		break;
	}

	if (world::get().isPassable(dest_x, dest_y))
	{
		state.m_Test = true;
	}
	else
	{
		state.m_Test = false;
	}

	state.m_ProgramCounter++;
}

void OpJne::Execute(MachineState& state)
{
	DebugOutput(state);

	if (m_Param < 1 || m_Param > state.num_ops || m_Param == state.m_ProgramCounter)
	{
		WrongLineNumberError wrongline;
		throw wrongline;
	}

	if (state.m_Test == false)
	{
		state.m_ProgramCounter = m_Param;
	}
	else 
	{
		state.m_ProgramCounter++;
	}
}

void OpMem::Execute(MachineState& state)
{
	DebugOutput(state);

	if (m_Param < 0 || m_Param > state.GetMaxMemory()-1)
	{
		MemoryAccessError mem_error;
		throw mem_error;
	}


	state.current_mem = m_Param;
	state.m_ProgramCounter++;
}

void OpSet::Execute(MachineState& state)
{
	DebugOutput(state);

	if (state.current_mem < 0 || state.current_mem > state.GetMaxMemory() - 1)
	{
		MemoryAccessError mem_error;
		throw mem_error;
	}

	state.SetMemory(state.current_mem, m_Param);
	state.m_ProgramCounter++;
}

void OpInc::Execute(MachineState& state)
{
	DebugOutput(state);

	if (state.current_mem < 0 || state.current_mem > state.GetMaxMemory() - 1)
	{
		MemoryAccessError mem_error;
		throw mem_error;
	}

	int mem_val = state.GetMemory(state.current_mem);
	state.SetMemory(state.current_mem, mem_val + 1);

	state.m_ProgramCounter++;
}

void OpDec::Execute(MachineState& state)
{
	DebugOutput(state);

	if (state.current_mem < 0 || state.current_mem > state.GetMaxMemory() - 1)
	{
		MemoryAccessError mem_error;
		throw mem_error;
	}

	int mem_val = state.GetMemory(state.current_mem);
	state.SetMemory(state.current_mem, mem_val - 1);

	state.m_ProgramCounter++;
}

void OpTestMem::Execute(MachineState& state)
{
	DebugOutput(state);

	if (state.current_mem < 0 || state.current_mem > state.GetMaxMemory() - 1)
	{
		MemoryAccessError mem_error;
		throw mem_error;
	}

	int mem_val = state.GetMemory(state.current_mem);
	if (mem_val == m_Param)
	{
		state.m_Test = true;
	}
	else
	{
		state.m_Test = false;
	}

	state.m_ProgramCounter++;
}

void OpSaveLoc::Execute(MachineState& state)
{
	if (state.current_mem < 0 || state.current_mem > state.GetMaxMemory() - 1)
	{
		MemoryAccessError mem_error;
		throw mem_error;
	}
	DebugOutput(state);
	state.SetMemory(0, state.x);
	state.SetMemory(1, state.y);

	state.m_ProgramCounter++;
}