// zombieView.cpp : implementation of the CZombieView class
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "resource.h"
#include "world.h"
#include "machine.h"
#include "zombieView.h"
#include <sstream>

struct MachineState;


using namespace Gdiplus;

CZombieView::CZombieView()
: m_BitmapImage(1024, 768)
, m_GraphicsImage(&m_BitmapImage)
{

}

BOOL CZombieView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CZombieView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	DrawGrid();
	DrawCreatures();
	DrawStats();
	
	Graphics graphics(dc);
	graphics.DrawImage(&m_BitmapImage, 0, 0);

	return 0;
}

LRESULT CZombieView::OnMouseOver(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	int mouse_x = GET_X_LPARAM(lParam);
	int mouse_y = GET_Y_LPARAM(lParam);

	auto it = world::get().humans.begin();
	while (it != world::get().humans.end())
	{
		if (mouse_x >= (*it)->x * 30 + 10 && mouse_x <= (*it)->x * 30 + 40)
		{
			if (mouse_y >= (*it)->y * 30 + 10 && mouse_y <= (*it)->y * 30 + 40)
			{
				mem_label = "Human at ("
					+ std::to_string((*it)->x)
					+ ','
					+ std::to_string((*it)->y)
					+ ")\n"
					+ "Memory:"
					+ "\n0: "
					+ std::to_string((*it)->GetMemory(0))
					+ "\n1: "
					+ std::to_string((*it)->GetMemory(1));

			}
		}
		++it;
	}


	return 0;
}

void CZombieView::DrawGrid()
{
	m_GraphicsImage.Clear(Color(255, 255, 255));
	Pen pen(Color(0, 0, 0));
	m_GraphicsImage.DrawRectangle(&pen, 10, 10, 600, 600);
	for (int i = 10; i <= 610; i += 30)
	{
		m_GraphicsImage.DrawLine(&pen, 10, i, 610, i);
	}

	for (int j = 10; j <= 610; j += 30)
	{
		m_GraphicsImage.DrawLine(&pen, j, 10, j, 610);
	}
}

void CZombieView::DrawCreatures()
{
	SolidBrush zBrush(Color(255, 0, 0));
	
	auto it = world::get().zombies.begin();
	while (it != world::get().zombies.end())
	{
		
		int x = (*it)->x;
		int y = (*it)->y;


		MachineState::Facing facing = (*it)->m_Facing;
		Point points[3];
		if (facing == MachineState::RIGHT)
		{
			points[0] = Point(10 + x * 30, 10 + y * 30);
			points[1] = Point(10 + x * 30, 40 + y * 30);
			points[2] = Point(40 + x * 30, 25 + y * 30);
		}

		else if (facing == MachineState::UP)
		{
			points[0] = Point(10 + x * 30, 40 + y * 30);
			points[1] = Point(40 + x * 30, 40 + y * 30);
			points[2] = Point(25 + x * 30, 10 + y * 30);
		}

		else if (facing == MachineState::LEFT)
		{
			
			points[0] = Point(40 + x * 30, 10 + y * 30);
				points[1] = Point(40 + x * 30, 40 + y * 30);
				points[2] = Point(10 + x * 30, 25 + y * 30);
		}
		else
		{
			points[0] = Point(10 + x * 30, 10 + y * 30);
			points[1] = Point(40 + x * 30, 10 + y * 30);
			points[2] = Point(25 + x * 30, 40 + y * 30);
		}

		m_GraphicsImage.FillPolygon(&zBrush, points, 3);
		++it;
	}

	SolidBrush hBrush(Color(0, 0, 255));

	it = world::get().humans.begin();
	while (it != world::get().humans.end())
	{
		int x = (*it)->x;
		int y = (*it)->y;


		MachineState::Facing facing = (*it)->m_Facing;
		Point points[3];
		if (facing == MachineState::RIGHT)
		{
			points[0] = Point(10 + x * 30, 10 + y * 30);
			points[1] = Point(10 + x * 30, 40 + y * 30);
			points[2] = Point(40 + x * 30, 25 + y * 30);
		}

		else if (facing == MachineState::UP)
		{
			points[0] = Point(10 + x * 30, 40 + y * 30);
			points[1] = Point(40 + x * 30, 40 + y * 30);
			points[2] = Point(25 + x * 30, 10 + y * 30);
		}

		else if (facing == MachineState::LEFT)
		{

			points[0] = Point(40 + x * 30, 10 + y * 30);
			points[1] = Point(40 + x * 30, 40 + y * 30);
			points[2] = Point(10 + x * 30, 25 + y * 30);
		}
		else
		{
			points[0] = Point(10 + x * 30, 10 + y * 30);
			points[1] = Point(40 + x * 30, 10 + y * 30);
			points[2] = Point(25 + x * 30, 40 + y * 30);
		}

		m_GraphicsImage.FillPolygon(&hBrush, points, 3);
		++it;
	}
}

void CZombieView::DrawStats()
{
	std::string label1 = "Zombies";
	label1 += ("\nProgram: " + zombie_program);
	label1 += ("\nAlive: " + std::to_string(world::get().zombies.size()));

	std::string label2 = "Humans";
	label2 += ("\nProgram: " + human_program);
	label2 += ("\nAlive: " + std::to_string(world::get().humans.size()));

	std::string label3 = "Month: ";
	label3 += std::to_string(world::get().month);

	Font font(L"Times New Roman", 10);
	SolidBrush brushBlack(Color(0, 0, 0));
	SolidBrush brushRed(Color(255, 0, 0));
	SolidBrush brushBlue(Color(0, 0, 255));




	m_GraphicsImage.DrawString(CA2W(label1.c_str()), -1, &font, PointF(620, 10),
		&brushRed);

	m_GraphicsImage.DrawString(CA2W(label2.c_str()), -1, &font, PointF(620, 110),
		&brushBlue);

	m_GraphicsImage.DrawString(CA2W(label3.c_str()), -1, &font, PointF(620, 220),
		&brushBlack);

	m_GraphicsImage.DrawString(CA2W(mem_label.c_str()), -1, &font, PointF(620, 270),
		&brushBlack);
}
