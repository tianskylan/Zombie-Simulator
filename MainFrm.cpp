// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "zombieView.h"
#include "MainFrm.h"
#include "world.h"
#include <ctime>

void makeTestZombie()
{
	std::srand((unsigned int)time(0));
	MachineState *zbst = new MachineState();

	int x, y;

	do
	{
		x = std::rand() % 20;
		y = std::rand() % 20;
	} while (!world::get().isPassable(x, y));

	zbst->x = x;
	zbst->y = y;
	zbst->m_Facing = (MachineState::Facing)(std::rand() % 4);

	world::get().zombieMachine.BindState(*zbst);

	world::get().zombies.push_back(zbst);
}

void makeTestHuman()
{
	MachineState *hmst = new MachineState();

	int x, y;

	do
	{
		x = std::rand() % 20;
		y = std::rand() % 20;
	} while (!world::get().isPassable(x, y));

	hmst->x = x;
	hmst->y = y;
	hmst->m_Facing = (MachineState::Facing)(std::rand() % 4);


	world::get().humanMachine.BindState(*hmst);

	world::get().humans.push_back(hmst);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	CreateSimpleStatusBar();

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	// TEMP CODE: Initialize zombie test machine
	//world::get().zombieMachine.LoadMachine(std::string("search.zom"));
	//world::get().humanMachine.LoadMachine(std::string("the_governor.zom"));


	// END TEMP CODE
	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (world::get().zombies.size() == 0)
	{
		std::string human_win = "Humans Win!";

		KillTimer(1);
		world::get().simulating = false;
		MessageBox(human_win.c_str(), "Game Over", MB_OK);

	}

	if (world::get().humans.size() == 0)
	{
		std::string zombie_win = "Zombies Win!";

		KillTimer(1);
		world::get().simulating = false;
		MessageBox(zombie_win.c_str(), "Game Over", MB_OK);
	}

	// Check if this is the turn timer
	if (wParam == 1)
	{
		// TEMP CODE: Take turn for zombie machine

		auto it = world::get().zombies.begin();
		while (it != world::get().zombies.end())
		{
			world::get().zombieMachine.TakeTurn(**it);
			++it;
		}
		
		it = world::get().humans.begin();
		while (it != world::get().humans.end())
		{
			world::get().humanMachine.TakeTurn(**it);
			++it;
		}
		
		world::get().month++;


		// erasing dead creatures
		it = world::get().zombies.begin();
		while (it != world::get().zombies.end())
		{
			if ((*it)->m_dead)
			{
				it = world::get().zombies.erase(it);
			}

			else
			{
				++it;
			}

		}

		it = world::get().humans.begin();
		while (it != world::get().humans.end())
		{
			if ((*it)->m_dead)
			{
				it = world::get().humans.erase(it);
			}

			else if ((*it)->m_infected)
			{
				(*it)->m_infected = false;
				world::get().zombieMachine.BindState(**it);
				world::get().zombies.push_back(*it);
				it = world::get().humans.erase(it);
			}

			else
			{
				++it;
			}
			
		}

		m_view.RedrawWindow();

	}

	return 0;
}

LRESULT CMainFrame::OnSimStart(WORD , WORD , HWND , BOOL& )
{
	// Add timer to run once per second
	if (!world::get().simulating)
	{
		SetTimer(1, 1000);
		world::get().simulating = true;
	}
	else
	{
		KillTimer(1);
		world::get().simulating = false;
	}

	return 0;
}

LRESULT CMainFrame::OnSimClear(WORD, WORD, HWND, BOOL&)
{
	KillTimer(1);
	world::get().simulating = false;
	world::get().month = 0;

	world::get().humans.clear();
	world::get().zombies.clear();

	m_view.RedrawWindow();

	return 0;
}



LRESULT CMainFrame::OnLoadZombie(WORD, WORD, HWND, BOOL&)
{
	std::string name;

	// opens a dialog for save file, REQUIRE file exists
	CFileDialog dialog(true);

	if (IDOK == dialog.DoModal())
	{
		name = dialog.m_szFileName;
		world::get().zombieMachine.LoadMachine(name);
		zombie_loaded = true;

		size_t found = name.find_last_of('\\');
		
		if (found != std::string::npos)
		{
			name = name.substr(found+1);
		}

		m_view.zombie_program = name;
		m_view.RedrawWindow();
	}

	if (zombie_loaded && human_loaded)
	{
		EnableMenuItem(GetMenu(), ID_SIMULATION_RANDOMIZE, MF_ENABLED);
	}

	return 0;
}



LRESULT CMainFrame::OnLoadSurvivor(WORD, WORD, HWND, BOOL&)
{
	std::string name;

	// opens a dialog for save file, REQUIRE file exists
	CFileDialog dialog(true);

	if (IDOK == dialog.DoModal())
	{
		name = dialog.m_szFileName;
		world::get().humanMachine.LoadMachine(name);
		human_loaded = true;
		m_view.human_program = name;

		size_t found = name.find_last_of('\\');

		if (found != std::string::npos)
		{
			name = name.substr(found + 1);
		}
		m_view.human_program = name;
		m_view.RedrawWindow();

	}

	if (zombie_loaded && human_loaded)
	{
		EnableMenuItem(GetMenu(), ID_SIMULATION_RANDOMIZE, MF_ENABLED);
	}

	return 0;
}
LRESULT CMainFrame::OnSimRand(WORD, WORD, HWND, BOOL&)
{
	for (int i = 0; i < 20; i++)
	{
		if (i % 2 == 0)
		{
			makeTestHuman();
		}
		makeTestZombie();
	}

	m_view.RedrawWindow();

	return 0;
}