#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "op.h"
#include "exceptions.h"
#include <stdlib.h>     /* atoi */


// Defines state data accessible by the machine and ops
struct MachineState
{
	// Friend Machine so BindState works
	template <typename MachineTraits>
	friend class Machine;
	
	enum Facing { UP, RIGHT, DOWN, LEFT};
	MachineState()
		: m_ProgramCounter(1)
		, m_ActionsTaken(0)
		, m_Facing(UP)
		, m_Test(false)
		, m_Memory(nullptr)
		, x(0)
		, y(0)
		, m_dead(false)
		, m_infected(false)
		, current_mem(0)
		, num_ops(0)
	{ }

	~MachineState()
	{
		std::cout << "delete memory of a human." << std::endl;
		delete[] m_Memory;
	}

	// Active line number in behavior program
	int m_ProgramCounter;
	// Number of actions taken this turn
	int m_ActionsTaken;
	// Current facing of this character
	Facing m_Facing;
	// Test flag for branches
	bool m_Test;
	int num_ops;

	int x;
	int y;

	int current_mem; // for human only, range : 0, 1
	bool m_dead;
	bool m_infected;
	

	int GetActionsPerTurn() const throw() { return m_ActionsPerTurn; }
	int GetMaxMemory() const throw() {return m_MaxMemory; }
	bool GetInfect() const throw() { return m_InfectOnAttack; }
	int GetMemory(int location)
	{
		// FIXME: Throw exception if out of bounds or memory is not allocated
		return m_Memory[location];
	}
	void SetMemory(int location, int value)
	{
		m_Memory[location] = value;
	}
private:
	// Data which is set by the traits
	int m_ActionsPerTurn;
	int m_MaxMemory;
	bool m_InfectOnAttack;
	int* m_Memory;
};

// Describes the machine which processes ops.
// Different policies dictate behavior possible for machine.
template <typename MachineTraits>
class Machine
{
public:
	// Load in all the ops for this machine from the specified file
	void LoadMachine(std::string& filename);

	// Given the state, binds the trait parameters to it
	void BindState(MachineState& state);

	// Take a turn using this logic for the passed in state
	void TakeTurn(MachineState& state);

	// parse the command string from zom file and return the Operation
	Op* parseCommand(std::string);

private:
	std::vector<Op*> m_Ops;
};


template <typename MachineTraits>
Op* Machine<MachineTraits>::parseCommand(std::string line)
{
	size_t colonfound = line.find(";");
	size_t commafound = line.find(",");
	
	std::string command;
	int param = -1;

	if (colonfound != std::string::npos) //has colon
	{
		line = line.substr(0, colonfound);
	}

	//trim spaces at end
	size_t last_not_space_found = line.find_last_not_of(' ');

	if (last_not_space_found != std::string::npos)
	{
		line.erase(last_not_space_found + 1);
	}

	//trim tabs at end
	last_not_space_found = line.find_last_not_of('\t');

	if (last_not_space_found != std::string::npos)
	{
		line.erase(last_not_space_found + 1);
	}


	if (commafound != std::string::npos) //no colon but has comma
	{
		command = line.substr(0, commafound);
		std::string num_param = line.substr(commafound+1);
		param = atoi(num_param.c_str());
		//char num_param = line[commafound + 1];
		//param = num_param - '0';
	}
	else //no colon and no comma
	{
		command = line;
	}

	// generate the commands and parameters

	/* Action ops */
	if (command == "attack")
	{
		return (new OpAttack(param));
	}
	else if (command == "ranged_attack")
	{
		return (new OpRangedAttack(param));
	}
	else if (command == "rotate")
	{
		return (new OpRotate(param));
	}
	else if (command == "forward")
	{
		return (new OpForward(param));
	}
	else if (command == "endturn")
	{
		return (new OpEndTurn(param));
	}

	/* Other ops*/
	else if (command == "test_human")
	{
		return (new OpTestHuman(param));
	}
	else if (command == "test_wall")
	{
		return (new OpTestWall(param));
	}
	else if (command == "test_zombie")
	{
		return (new OpTestZombie(param));
	}
	else if (command == "test_random")
	{
		return (new OpTestRandom(param));
	}
	else if (command == "test_passable")
	{
		return (new OpTestPassable(param));
	}
	else if (command == "je")
	{
		return (new OpJe(param));
	}
	else if (command == "jne")
	{
		return (new OpJne(param));
	}
	else if (command == "goto")
	{
		return (new OpGoto(param));
	}
	else if (command == "mem")
	{
		return (new OpMem(param));
	}
	else if (command == "set")
	{
		return (new OpSet(param));
	}
	else if (command == "inc")
	{
		return (new OpInc(param));
	}
	else if (command == "dec")
	{
		return (new OpDec(param));
	}
	else if (command == "test_mem")
	{
		return (new OpTestMem(param));
	}
	else if (command == "save_loc")
	{
		return (new OpSaveLoc(param));
	}
	else
	{
		InvalidOp op_error;
		throw  op_error;
	}




	return NULL;


	
}



template <typename MachineTraits>
void Machine<MachineTraits>::LoadMachine(std::string& filename)
{

	std::ifstream fin;
	try
	{
		fin.open(filename);
		if (fin.is_open())
		{
			std::string command;
			m_Ops.clear();

			while (!fin.eof())
			{
				std::getline(fin, command);
				m_Ops.push_back(parseCommand(command));
			}


		}
		else
		{
			FileIOError file_error;
			throw file_error;
		}
	}
	catch (FileIOError& err)
	{
		MessageBox(NULL, err.what(), "ERROR", MB_OK);
	}
	catch (WrongLineNumberError &err)
	{
		MessageBox(NULL, err.what(), "ERROR", MB_OK);
	}
	catch (MemoryAccessError &err)
	{
		MessageBox(NULL, err.what(), "ERROR", MB_OK);
	}
	catch (InvalidOp &err)
	{
		MessageBox(NULL, err.what(), "ERROR", MB_OK);
	}
	
}

template <typename MachineTraits>
void Machine<MachineTraits>::BindState(MachineState& state)
{
	state.m_ActionsPerTurn = MachineTraits::ACTIONS_PER_TURN;
	state.m_MaxMemory = MachineTraits::MEMORY_LOCATIONS;
	state.m_InfectOnAttack = MachineTraits::INFECT_ON_ATTACK;
	state.current_mem = -1;
	state.num_ops = m_Ops.size();
	delete state.m_Memory;
	state.m_Memory = new int[state.m_MaxMemory];
	for (int i = 0; i < state.m_MaxMemory; i++)
	{
		state.m_Memory[i] = 0;
	}
}

template <typename MachineTraits>
void Machine<MachineTraits>::TakeTurn(MachineState& state)
{
	if (state.m_infected || state.m_dead)
	{
		return;
	}

	state.m_ActionsTaken = 0;
	while (state.m_ActionsTaken < MachineTraits::ACTIONS_PER_TURN)
	{
		m_Ops.at(state.m_ProgramCounter - 1)->Execute(state);
	} 
}
