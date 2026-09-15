// NewUIGuildInfoWindow.cpp: implementation of the CNewUIGuildInfoWindow class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NewUIMiniMap.h"
#include "NewUISystem.h"
#include "NewUICommonMessageBox.h"
#include "NewUICustomMessageBox.h"
#include "DSPlaySound.h"
#include "wsclientinline.h"
#include "NewUIGuildInfoWindow.h"
#include "NewUIButton.h"
#include "NewUIMyInventory.h"
#include "CSitemOption.h"
#include "MapManager.h"
#include <RISE/WideData.h>
#include <ZzzAI.h>
#include <float.h>
#include <set>
#include <stack>
#include <ZzzLodTerrain.h>

namespace
{
	const int MINIMAP_AUTO_REACH_RANGE = 2;
	const int MINIMAP_AUTO_STEP_STRIDE = 7;
	const int MINIMAP_AUTO_STUCK_LIMIT = 8;

	typedef std::pair<int, int> MiniMapNode;
	typedef std::pair<double, MiniMapNode> MiniMapOpenNode;

	struct MiniMapPathCellInfo
	{
		int parent_i;
		int parent_j;
		double f;
		double g;
		double h;
	};

	bool IsMiniMapPathValidCell(int x, int y)
	{
		return x >= 0 && x < TERRAIN_SIZE && y >= 0 && y < TERRAIN_SIZE;
	}

	bool IsMiniMapPathWalkableCell(int x, int y)
	{
		const WORD att = TerrainWall[TERRAIN_INDEX(x, y)];
		return ((att & TW_NOMOVE) == 0) && ((att & TW_NOGROUND) == 0);
	}

	double CalcMiniMapPathHeuristic(int x, int y, const MiniMapNode& dst)
	{
		const double dx = static_cast<double>(x - dst.first);
		const double dy = static_cast<double>(y - dst.second);
		return sqrt(dx * dx + dy * dy);
	}

	void TraceMiniMapPath(const MiniMapNode& dst, MiniMapPathCellInfo* cellInfo, std::deque<POINT>& outWayPoints)
	{
		std::stack<MiniMapNode> path;
		int x = dst.first;
		int y = dst.second;

		while (!(cellInfo[TERRAIN_INDEX(x, y)].parent_i == x
			&& cellInfo[TERRAIN_INDEX(x, y)].parent_j == y))
		{
			path.push(std::make_pair(x, y));
			const int parentX = cellInfo[TERRAIN_INDEX(x, y)].parent_i;
			const int parentY = cellInfo[TERRAIN_INDEX(x, y)].parent_j;
			x = parentX;
			y = parentY;
		}

		path.push(std::make_pair(x, y));

		int count = 0;
		while (!path.empty())
		{
			const MiniMapNode node = path.top();
			path.pop();

			if (((count++) % MINIMAP_AUTO_STEP_STRIDE) == 0 || path.empty())
			{
				POINT pt = { node.first, node.second };
				outWayPoints.push_back(pt);
			}
		}
	}

	bool BuildMiniMapWayPointsInternal(int sx, int sy, int dx, int dy, std::deque<POINT>& outWayPoints)
	{
		outWayPoints.clear();

		const MiniMapNode src = std::make_pair(sx, sy);
		const MiniMapNode dst = std::make_pair(dx, dy);

		if (!IsMiniMapPathValidCell(src.first, src.second) || !IsMiniMapPathValidCell(dst.first, dst.second))
		{
			return false;
		}

		if (!IsMiniMapPathWalkableCell(src.first, src.second) || !IsMiniMapPathWalkableCell(dst.first, dst.second))
		{
			return false;
		}

		if (src == dst)
		{
			POINT pt = { dx, dy };
			outWayPoints.push_back(pt);
			return true;
		}

		MiniMapPathCellInfo* cellInfo = new MiniMapPathCellInfo[TERRAIN_SIZE * TERRAIN_SIZE];
		bool* closeList = new bool[TERRAIN_SIZE * TERRAIN_SIZE];

		memset(closeList, 0, sizeof(bool) * TERRAIN_SIZE * TERRAIN_SIZE);

		for (int i = 0; i < TERRAIN_SIZE; ++i)
		{
			for (int j = 0; j < TERRAIN_SIZE; ++j)
			{
				MiniMapPathCellInfo& cell = cellInfo[TERRAIN_INDEX(i, j)];
				cell.f = FLT_MAX;
				cell.g = FLT_MAX;
				cell.h = FLT_MAX;
				cell.parent_i = -1;
				cell.parent_j = -1;
			}
		}

		int i = src.first;
		int j = src.second;
		MiniMapPathCellInfo& startCell = cellInfo[TERRAIN_INDEX(i, j)];
		startCell.f = 0.0;
		startCell.g = 0.0;
		startCell.h = 0.0;
		startCell.parent_i = i;
		startCell.parent_j = j;

		std::set<MiniMapOpenNode> openList;
		openList.insert(std::make_pair(0.0, std::make_pair(i, j)));

		bool found = false;

		while (!openList.empty())
		{
			std::set<MiniMapOpenNode>::iterator it = openList.begin();
			i = it->second.first;
			j = it->second.second;
			openList.erase(it);

			closeList[TERRAIN_INDEX(i, j)] = true;

			const int offsets[8][3] =
			{
				{ -1,  0, 1000 }, { 1,  0, 1000 }, { 0,  1, 1000 }, { 0, -1, 1000 },
				{ -1,  1, 1414 }, { -1, -1, 1414 }, { 1,  1, 1414 }, { 1, -1, 1414 }
			};

			for (int index = 0; index < 8; ++index)
			{
				const int nextX = i + offsets[index][0];
				const int nextY = j + offsets[index][1];

				if (!IsMiniMapPathValidCell(nextX, nextY))
				{
					continue;
				}

				if (nextX == dst.first && nextY == dst.second)
				{
					cellInfo[TERRAIN_INDEX(nextX, nextY)].parent_i = i;
					cellInfo[TERRAIN_INDEX(nextX, nextY)].parent_j = j;
					TraceMiniMapPath(dst, cellInfo, outWayPoints);
					found = true;
					break;
				}

				if (closeList[TERRAIN_INDEX(nextX, nextY)] || !IsMiniMapPathWalkableCell(nextX, nextY))
				{
					continue;
				}

				const double gNew = cellInfo[TERRAIN_INDEX(i, j)].g + (static_cast<double>(offsets[index][2]) / 1000.0);
				const double hNew = CalcMiniMapPathHeuristic(nextX, nextY, dst);
				const double fNew = gNew + hNew;
				MiniMapPathCellInfo& nextCell = cellInfo[TERRAIN_INDEX(nextX, nextY)];

				if (nextCell.f == FLT_MAX || nextCell.f > fNew)
				{
					openList.insert(std::make_pair(fNew, std::make_pair(nextX, nextY)));
					nextCell.f = fNew;
					nextCell.g = gNew;
					nextCell.h = hNew;
					nextCell.parent_i = i;
					nextCell.parent_j = j;
				}
			}

			if (found)
			{
				break;
			}
		}

		delete[] closeList;
		delete[] cellInfo;

		return found && !outWayPoints.empty();
	}
}

SEASON3B::CNewUIMiniMap::CNewUIMiniMap()
{
	m_pNewUIMng			= NULL;
	IndexIMGMap			= -1;
	ViTriDiChuyen.x		= 0;
	ViTriDiChuyen.y		= 0;
	ViTriChon.x			= 0;
	ViTriChon.y			= 0;

	m_bSuccess			= true;

	pWidthForm			= 350;
	pHeighForm			= 300;

	m_MapPos.x			= 0;
	m_MapPos.y			= 0;
	
	Movement			= false;
	CacheMapNumber		= -1;
	XNPC				= 0;
	YNPC				= 0;
	m_LastAutoMovePos.x = -1;
	m_LastAutoMovePos.y = -1;
	m_AutoMoveStuckCount = 0;
	m_AutoMoveMap = -1;
	m_MiniOverlayCenterX = 0.f;
	m_MiniOverlayCenterY = 0.f;
	m_MiniOverlayLastTick = 0;
	m_MiniOverlayMap = -1;
	m_bMiniOverlayCenterInitialized = false;
}

SEASON3B::CNewUIMiniMap::~CNewUIMiniMap()
{
	Release();
}

bool SEASON3B::CNewUIMiniMap::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_MINI_MAP, this);
	
	SetPos(x, y);
	
	CreateButton();

	LoadImagesBitMap();

	return true;
}

void SEASON3B::CNewUIMiniMap::SetPos(int x, int y)
{
	m_MapPos.x = x / 2;
	m_MapPos.y = (y / 2) - 30;

	m_BtnExit.ChangeButtonInfo(m_MapPos.x + pWidthForm - 22, m_MapPos.y, 18, 14);
}

void SEASON3B::CNewUIMiniMap::ClosingProcess()
{
	SendExitInventory();
}

float SEASON3B::CNewUIMiniMap::GetLayerDepth()
{
	return 8.1f;
}

void SEASON3B::CNewUIMiniMap::CreateButton()
{
	m_BtnExit.ChangeButtonImgState(true, IMAGE_MINIMAP_INTERFACE + 6, true);
	m_BtnExit.ChangeButtonInfo(m_MapPos.x + pWidthForm - 22, m_MapPos.y - 22, 18, 14);
	m_BtnExit.ChangeToolTipText(GlobalText[1002], true);
}

void SEASON3B::CNewUIMiniMap::LoadImagesBitMap()
{
	LoadBitmapFile("Interface\\mini_map_ui_corner.tga",		IMAGE_MINIMAP_INTERFACE + 1, GL_LINEAR);
	LoadBitmapFile("Interface\\mini_map_ui_line.jpg",		IMAGE_MINIMAP_INTERFACE + 2, GL_LINEAR);
	LoadBitmapFile("Interface\\mini_map_ui_cha.tga",		IMAGE_MINIMAP_INTERFACE + 3, GL_LINEAR);
	LoadBitmapFile("Interface\\mini_map_ui_portal.tga",		IMAGE_MINIMAP_INTERFACE + 4, GL_LINEAR);
	LoadBitmapFile("Interface\\mini_map_ui_npc.tga",		IMAGE_MINIMAP_INTERFACE + 5, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\mini_map_ui_cancel.tga",  IMAGE_MINIMAP_INTERFACE + 6, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\map_radar.tga",			IMAGE_MINIMAP_INTERFACE + 7, GL_LINEAR);

	LoadBitmapFile("RISE\\FaceMap\\World_00.tga", START_WORLD, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_01.tga", START_WORLD + 1, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_02.tga", START_WORLD + 2, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_03.tga", START_WORLD + 3, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_04.tga", START_WORLD + 4, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_06.tga", START_WORLD + 6, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_07.tga", START_WORLD + 7, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_08.tga", START_WORLD + 8, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_09.tga", START_WORLD + 9, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_10.tga", START_WORLD + 10, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_18.tga", START_WORLD + 18, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_11.tga", START_WORLD + 11, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_24.tga", START_WORLD + 24, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_30.tga", START_WORLD + 30, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_31.tga", START_WORLD + 31, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_33.tga", START_WORLD + 33, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_34.tga", START_WORLD + 34, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_37.tga", START_WORLD + 37, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_38.tga", START_WORLD + 38, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_41.tga", START_WORLD + 41, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_42.tga", START_WORLD + 42, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_51.tga", START_WORLD + 51, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_56.tga", START_WORLD + 56, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_57.tga", START_WORLD + 57, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_63.tga", START_WORLD + 63, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_79.tga", START_WORLD + 79, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_80.tga", START_WORLD + 80, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_81.tga", START_WORLD + 81, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_82.tga", START_WORLD + 82, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_83.tga", START_WORLD + 83, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_84.tga", START_WORLD + 84, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_85.tga", START_WORLD + 85, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_86.tga", START_WORLD + 86, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_87.rise", START_WORLD + 87, GL_LINEAR);
	LoadBitmapFile("RISE\\FaceMap\\World_88.rise", START_WORLD + 88, GL_LINEAR);
}

void SEASON3B::CNewUIMiniMap::Release()
{
	UnloadImages();

	for (int i = 1; i < 8; i++)
	{
		DeleteBitmap(IMAGE_MINIMAP_INTERFACE + i);
	}

	for (int i = START_WORLD; i < END_WORLD; ++i)
	{
		DeleteBitmap(i);
	}

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIMiniMap::SetBtnPos(int Num, float x, float y, float nx, float ny)
{
	m_Btn_Loc[Num][0] = x;
	m_Btn_Loc[Num][1] = y;
	m_Btn_Loc[Num][2] = nx;
	m_Btn_Loc[Num][3] = ny;
}

bool SEASON3B::CNewUIMiniMap::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MINI_MAP))
	{
		if (IsPress(VK_ESCAPE) == true || IsPress(VK_TAB) == true)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_MINI_MAP);
			PlayBuffer(SOUND_CLICK01);
			return false;
		}
	}
	return true;
}

void SEASON3B::CNewUIMiniMap::CreateMap()
{
	float x, y;

	x = m_MapPos.x;
	y = m_MapPos.y;

	float DIRPointer;

	g_pUIForm->RenderLineForm(IndexIMGMap, x - 3, y - 18, pWidthForm + 6, pHeighForm + 36);

	int i;
	for (i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
	{
		CHARACTER* c = &CharactersClient[i];
		OBJECT* o = &c->Object;

		if (!c || !o->Live)
			continue;

		XNPC = (float)(x + (c->PositionX * (pWidthForm / 256.f)));
		YNPC = (float)(y + ((255 - c->PositionY) * (pHeighForm / 256.f)));
		DIRPointer = (o->Angle[2]) + 180.0;

		if (o->Kind == KIND_PLAYER)
		{
			RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, XNPC + 3, YNPC - 3, 12.0, 12.0, DIRPointer, 0.0, 0.0, 0.1030000001, 0.05200000107);
		}
		else if (o->Kind == KIND_MONSTER)
		{
			RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, XNPC, YNPC, 10.0, 10.0, 0, 0.1349999905, 0.0, 0.1030000001, 0.05200000107);
		}
		else if (o->Kind == KIND_NPC)
		{
			if (c->Class == 251)
			{
				RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, XNPC, YNPC - 0.5, 10.0, 10.0, 0, 0.009999999776, 0.1319999844, 0.1030000001, 0.05200000107);
			}
			else if (c->Class == 255 || c->Class == 253)
			{
				RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, XNPC, YNPC - 0.5, 10.0, 10.0, 0, 0.1349999905, 0.1319999844, 0.1030000001, 0.05200000107);
			}
			else if (c->Class == 240) //-- Baul
			{
				RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, XNPC, YNPC, 10.0, 10.0, 0, 0.009999999776, 0.1920999587, 0.1030000001, 0.05200000107);
			}
			else if (c->Class == 229 || c->Class == 235 || c->Class == 233 || c->Class == 237) //-- Quest
			{
				RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, XNPC, YNPC, 10.0, 10.0, 0, 0.0, 0.06599999219, 0.1030000001, 0.05200000107);
			}
			else
			{
				RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, XNPC, YNPC, 10.0, 10.0, 0, 0.1349999905, 0.06599999219, 0.1030000001, 0.05200000107);
			}
		}
	}

	if (Movement)
	{
		XNPC = (float)(m_MapPos.x + (ViTriDiChuyen.x * (pWidthForm / 256.f)));
		YNPC = (float)(m_MapPos.y + ((255 - ViTriDiChuyen.y) * (pHeighForm / 256.f)));

		RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, XNPC, YNPC, 10.0, 10.0, 0, 0.1349999905, 0.06599999219 * 3, 0.1030000001, 0.05200000107);
	}
}

void SEASON3B::CNewUIMiniMap::CreateText()
{
	if ((Hero->CtlCode == CTLCODE_20OPERATOR) || (Hero->CtlCode == CTLCODE_08OPERATOR))
	{
		pKit->ThisFont(g_hFont, true, m_MapPos.x + 10, m_MapPos.y - 20, 0xFFFF00FF, 0, pWidthForm, 0, 0, "World: %s [%d / %d] Map: %d", gMapManager.GetMapName(gMapManager.WorldActive), Hero->PositionX, Hero->PositionY, gMapManager.WorldActive);
	}
	else
	{
		pKit->ThisFont(g_hFont, true, m_MapPos.x + 10, m_MapPos.y - 20, 0xFFFF00FF, 0, pWidthForm, 0, 0, GlobalText[2478], gMapManager.GetMapName(gMapManager.WorldActive), Hero->PositionX, Hero->PositionY);
	}


	if (MouseX > m_MapPos.x && MouseY > m_MapPos.y && MouseX < m_MapPos.x + pWidthForm && MouseY < m_MapPos.y + pHeighForm)
	{
		int bx = (MouseX - m_MapPos.x) + 1;
		int by = (MouseY - m_MapPos.y) + 1;

		ViTriChon.x = bx / (pWidthForm / 256.f);
		ViTriChon.y = 256.0 - (by / (pHeighForm / 256.f));

		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			pKit->ThisFont(g_hFont, true, MouseX - 12, MouseY - 10, 0xFF9100FF, 0, 30, 0, 3, "%d ~ %d", ViTriChon.x, ViTriChon.y);
		}
	}
	else
	{
		ViTriChon.x = 0;
		ViTriChon.y = 0;
	}

	if (Movement)
	{
		XNPC = (float)(m_MapPos.x + (ViTriDiChuyen.x * (pWidthForm / 256.f)));
		YNPC = (float)(m_MapPos.y + ((255 - ViTriDiChuyen.y) * (pHeighForm / 256.f)));

		pKit->ThisFont(g_hFont, true, XNPC - 24, YNPC - 12, 0x00FFFFFF, 0, 50, 16, 3, "%03d ~ %03d", ViTriDiChuyen.x, ViTriDiChuyen.y);
	}

	for (int i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
	{
		CHARACTER* c = &CharactersClient[i];
		OBJECT* o = &c->Object;

		if (!c || !o->Live)
			continue;

		XNPC = (float)(m_MapPos.x + (c->PositionX * (pWidthForm / 256.f)));
		YNPC = (float)(m_MapPos.y + ((255 - c->PositionY) * (pHeighForm / 256.f)));
		
		if (CheckMouseIn((int)XNPC - 2, (int)YNPC - 7, (int)10, (int)10))
		{
			pKit->ThisFont(g_hFont, true, (int)XNPC, (int)YNPC - 10, 0x00FFFFFF, 180, 0, 0, 3, c->ID);
		}
	}
}

bool SEASON3B::CNewUIMiniMap::IsMiniOverlayVisible() const
{
	return pKit->m_MiniMapOn && IndexIMGMap >= 0 && m_bSuccess && Hero != NULL &&
		g_pNewUISystem->CanShowGameplayOverlay() &&
		!g_pNewUISystem->IsVisible(INTERFACE_CHATINPUTBOX);
}

bool SEASON3B::CNewUIMiniMap::UpdateMiniOverlayMouseEvent()
{
	if (!IsMiniOverlayVisible())
	{
		m_MiniOverlayClickTick = 0;
		return true;
	}
	if (!CheckMouseIn(2, 2, 99.6f, 99.6f))
		return true;
	if (IsPress(VK_LBUTTON))
	{
		const DWORD now = GetTickCount();
		const bool doubleClick = m_MiniOverlayClickTick != 0 &&
			now - m_MiniOverlayClickTick <= GetDoubleClickTime();
		m_MiniOverlayClickTick = doubleClick ? 0 : now;
		MouseLButton = false;
		MouseLButtonPush = false;
		MouseLButtonPop = false;
		if (doubleClick)
			g_pNewUISystem->Show(INTERFACE_MINI_MAP);
	}
	return false;
}

void SEASON3B::CNewUIMiniMap::RenderMiniOverlay()
{
	if (!IsMiniOverlayVisible())
	{
		m_bMiniOverlayCenterInitialized = false;
		m_MiniOverlayClickTick = 0;
		return;
	}

	const float mapW = 99.6f;
	const float mapH = 99.6f;
	const float x = 2.f;
	const float y = 2.f;
	const float viewTiles = 48.f;
	const float halfView = viewTiles * 0.5f;
	const float targetCenterX = static_cast<float>(Hero->PositionX);
	const float targetCenterY = 255.f - static_cast<float>(Hero->PositionY);
	const DWORD currentTick = GetTickCount();
	const float jumpX = fabsf(targetCenterX - m_MiniOverlayCenterX);
	const float jumpY = fabsf(targetCenterY - m_MiniOverlayCenterY);

	if (!m_bMiniOverlayCenterInitialized ||
		m_MiniOverlayMap != gMapManager.WorldActive ||
		jumpX > 8.f || jumpY > 8.f)
	{
		m_MiniOverlayCenterX = targetCenterX;
		m_MiniOverlayCenterY = targetCenterY;
		m_bMiniOverlayCenterInitialized = true;
	}
	else
	{
		DWORD elapsedMs = currentTick - m_MiniOverlayLastTick;
		if (elapsedMs > 50)
		{
			elapsedMs = 50;
		}

		const float maxStep = 2.5f * (static_cast<float>(elapsedMs) / 1000.f);
		const float deltaX = targetCenterX - m_MiniOverlayCenterX;
		const float deltaY = targetCenterY - m_MiniOverlayCenterY;

		if (fabsf(deltaX) <= maxStep)
			m_MiniOverlayCenterX = targetCenterX;
		else if (deltaX != 0.f)
			m_MiniOverlayCenterX += (deltaX > 0.f) ? maxStep : -maxStep;

		if (fabsf(deltaY) <= maxStep)
			m_MiniOverlayCenterY = targetCenterY;
		else if (deltaY != 0.f)
			m_MiniOverlayCenterY += (deltaY > 0.f) ? maxStep : -maxStep;
	}

	m_MiniOverlayLastTick = currentTick;
	m_MiniOverlayMap = gMapManager.WorldActive;

	float viewX = m_MiniOverlayCenterX - halfView;
	float viewY = m_MiniOverlayCenterY - halfView;

	if (viewX < 0.f) viewX = 0.f;
	if (viewY < 0.f) viewY = 0.f;
	if (viewX > 256.f - viewTiles) viewX = 256.f - viewTiles;
	if (viewY > 256.f - viewTiles) viewY = 256.f - viewTiles;

	const float texU = viewX / 256.f;
	const float texV = viewY / 256.f;
	const float texSize = viewTiles / 256.f;
	const float scaleX = mapW / viewTiles;
	const float scaleY = mapH / viewTiles;

	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);
	g_pUIForm->RenderToolTip(x, y, mapW, mapH);
	DisableAlphaBlend();
	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);
	RenderBitmap(IndexIMGMap, x, y, mapW, mapH, texU, texV, texSize, texSize);
	glColor4f(1.f, 1.f, 1.f, 1.f);

	for (int i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
	{
		CHARACTER* c = &CharactersClient[i];
		OBJECT* o = &c->Object;
		if (!o->Live || !o->Visible)
		{
			continue;
		}

		float worldX = (c == Hero) ? m_MiniOverlayCenterX :
			static_cast<float>(c->PositionX);
		float worldY = (c == Hero) ? m_MiniOverlayCenterY :
			255.f - static_cast<float>(c->PositionY);

		if (o->Kind == KIND_MONSTER)
		{
			// Object.Position is interpolated every frame, while PositionX/Y
			// changes by whole terrain cells and makes the red marker jump.
			worldX = (o->Position[0] / TERRAIN_SCALE) - 0.5f;
			worldY = 255.f - ((o->Position[1] / TERRAIN_SCALE) - 0.5f);
		}
		if (worldX < viewX || worldX > viewX + viewTiles ||
			worldY < viewY || worldY > viewY + viewTiles)
		{
			continue;
		}

		const float iconX = x + ((worldX - viewX) * scaleX);
		const float iconY = y + ((worldY - viewY) * scaleY);

		if (o->Kind == KIND_PLAYER)
		{
			const float direction = o->Angle[2] + 180.f;
			RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, iconX + 1.f, iconY - 1.f, 9.f, 9.f,
				direction, 0.f, 0.f, 0.1030000001f, 0.05200000107f);
		}
		else if (o->Kind == KIND_MONSTER)
		{
			RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, iconX, iconY, 7.f, 7.f,
				0.f, 0.1349999905f, 0.f, 0.1030000001f, 0.05200000107f);
		}
		else if (o->Kind == KIND_NPC)
		{
			RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, iconX, iconY, 7.5f, 7.5f,
				0.f, 0.1349999905f, 0.06599999219f, 0.1030000001f, 0.05200000107f);
		}
	}

	if (Movement)
	{
		const float targetWorldX = static_cast<float>(ViTriDiChuyen.x);
		const float targetWorldY = 255.f - static_cast<float>(ViTriDiChuyen.y);
		if (targetWorldX >= viewX && targetWorldX <= viewX + viewTiles &&
			targetWorldY >= viewY && targetWorldY <= viewY + viewTiles)
		{
			const float targetX = x + ((targetWorldX - viewX) * scaleX);
			const float targetY = y + ((targetWorldY - viewY) * scaleY);
			RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, targetX, targetY, 7.5f, 7.5f,
				0.f, 0.1349999905f, 0.06599999219f * 3.f, 0.1030000001f, 0.05200000107f);
		}
	}
}
void SEASON3B::CNewUIMiniMap::ReceiveMapMarkers(const BYTE* data, int size)
{
	if (!data || size < 10 || data[0] != 0xC2 || data[3] != 0xF3 || data[4] != 0xE7 ||
		data[5] != 1 || data[6] > 1 || data[9] > 64 || size != 10 + data[9] * 37 ||
		size != ((int)data[1] << 8 | data[2])) return;
	const int map = data[7] | ((int)data[8] << 8);
	if (SceneFlag != MAIN_SCENE || map != gMapManager.WorldActive ||
		(!data[6] && m_MarkerMap != map)) return;
	std::vector<MapMarker> chunk;
	for (int i = 0; i < data[9]; ++i)
	{
		const BYTE* p = data + 10 + i * 37;
		if (p[0] > 2) return;
		MapMarker marker = {};
		marker.kind = p[0]; marker.x = p[1]; marker.y = p[2];
		marker.monsterClass = p[3] | ((WORD)p[4] << 8);
		memcpy(marker.name, p + 5, 31);
		for (int n = 0; n < 31 && marker.name[n]; ++n)
			if ((unsigned char)marker.name[n] < 32) marker.name[n] = ' ';
		chunk.push_back(marker);
	}
	if ((data[6] ? 0 : m_MapMarkers.size()) + chunk.size() > 512) return;
	if (data[6]) m_MapMarkers.clear();
	m_MarkerMap = map;
	m_MapMarkers.insert(m_MapMarkers.end(), chunk.begin(), chunk.end());
}

void SEASON3B::CNewUIMiniMap::RenderMapMarkers()
{
	const DWORD now = GetTickCount();
	if (m_MarkerMap != gMapManager.WorldActive)
	{
		m_MapMarkers.clear();
		m_MarkerMap = gMapManager.WorldActive;
		m_MarkerRequestTick = 0;
	}
	if (!m_MarkerRequestTick || now - m_MarkerRequestTick >= 5000)
	{
		SystemDataSendTwo(0xF3, 0xE7);
		m_MarkerRequestTick = now;
	}
	std::vector<RECT> occupied;
	const int order[] = { 2, 0, 1 };
	for (int category : order)
	for (const MapMarker& marker : m_MapMarkers)
	{
		if (marker.kind != category) continue;
		const float x = m_MapPos.x + marker.x * pWidthForm / 256.f;
		const float y = m_MapPos.y + (255 - marker.y) * pHeighForm / 256.f;
		const bool boss = marker.kind == 2;
		const float iconSize = boss ? 14.f : 10.f;
		const float u = 0.135f;
		const float v = marker.kind == 0 ? 0.066f : (boss ? 0.198f : 0.f);
		RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, x, y, iconSize, iconSize, 0, u, v, 0.103f, 0.052f);
		char label[96];
		_snprintf_s(label, sizeof(label), _TRUNCATE, "[%s] %s", boss ? "Boss" : (marker.kind == 0 ? "NPC" : "Spot"), marker.name);
		const DWORD color = boss ? 0xFF6060FF : (marker.kind == 0 ? 0x80FF80FF : 0xFFD070FF);
		// Place labels without covering one another; dense spots keep a hover label.
		for (int attempt = 0; attempt < 4; ++attempt)
		{
			const int width = 100, height = 12;
			const int left = (int)x + (attempt % 2 == 0 ? 8 : -width - 8);
			const int top = (int)y + (attempt < 2 ? -height : 3);
			RECT rect = { left, top, left + width, top + height };
			if (rect.left < m_MapPos.x || rect.right > m_MapPos.x + pWidthForm ||
				rect.top < m_MapPos.y || rect.bottom > m_MapPos.y + pHeighForm) continue;
			bool overlap = false;
			for (const RECT& other : occupied)
				if (rect.left < other.right && rect.right > other.left && rect.top < other.bottom && rect.bottom > other.top) { overlap = true; break; }
			if (overlap) continue;
			pKit->ThisFont(g_hFont, true, rect.left, rect.top, color, 0, width, height, 1, "%s", label);
			occupied.push_back(rect);
			break;
		}
		if (CheckMouseIn(x - 7, y - 7, 14, 14))
			RenderTipText((int)x, (int)y - 20, label);
	}
}

#ifdef RISE_SLAYER_PORT
void SEASON3B::CNewUIMiniMap::ResetSlayerDetection()
{
    m_SlayerDetectionStartTick = 0;
    m_SlayerDetectionDurationMs = 0;
    m_SlayerDetectionMap = -1;
}

void SEASON3B::CNewUIMiniMap::BeginSlayerDetection(int map,
    int durationSeconds)
{
    // Only the GS buff-start signal can enter here. A local cast graph does
    // not start or extend the sonar result.
    if (SceneFlag != MAIN_SCENE || !Hero || !Hero->Object.Live ||
        map != gMapManager.WorldActive || durationSeconds != 60)
        return;
    m_SlayerDetectionStartTick = GetTickCount();
    m_SlayerDetectionDurationMs = durationSeconds * 1000;
    m_SlayerDetectionMap = map;
}

void SEASON3B::CNewUIMiniMap::RenderSlayerDetection()
{
    if (!m_SlayerDetectionStartTick)
        return;
    if (!Hero || !Hero->Object.Live || Hero->Dead ||
        m_SlayerDetectionMap != gMapManager.WorldActive ||
        GetTickCount() - m_SlayerDetectionStartTick >=
            m_SlayerDetectionDurationMs)
    {
        ResetSlayerDetection();
        return;
    }
    // The existing 5.2 character pool is the local life-form source; unlike
    // spawn pins, these positions move with the actors. S21's exact reveal
    // radius is not present in the client skill-handler dump.
    for (int i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
    {
        CHARACTER* character = &CharactersClient[i];
        OBJECT* object = &character->Object;
        if (character == Hero || !object->Live || character->Dead ||
            (object->Kind != KIND_MONSTER && object->Kind != KIND_PLAYER))
            continue;
        float tileX = static_cast<float>(character->PositionX);
        float tileY = static_cast<float>(character->PositionY);
        if (object->Kind == KIND_MONSTER)
        {
            tileX = object->Position[0] / TERRAIN_SCALE - 0.5f;
            tileY = object->Position[1] / TERRAIN_SCALE - 0.5f;
        }
        if (tileX < 0.f || tileX > 255.f || tileY < 0.f || tileY > 255.f)
            continue;
        const float x = m_MapPos.x + tileX * pWidthForm / 256.f;
        const float y = m_MapPos.y + (255.f - tileY) * pHeighForm / 256.f;
        glColor4f(0.45f, 0.85f, 1.f, 1.f);
        RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 7, x, y, 9.f, 9.f,
            0.f, 0.135f, 0.f, 0.103f, 0.052f);
    }
    glColor4f(1.f, 1.f, 1.f, 1.f);
}
#endif

bool SEASON3B::CNewUIMiniMap::Render()
{
	EnableAlphaTest();
	glColor4f(1.0, 1.0, 1.0, 1.0);

	CreateMap();
	CreateText();
	RenderMapMarkers();
#ifdef RISE_SLAYER_PORT
    RenderSlayerDetection();
#endif

	m_BtnExit.Render();

	DisableAlphaBlend();
	return true;
}

bool SEASON3B::CNewUIMiniMap::Update()
{
	return true;
}

void SEASON3B::CNewUIMiniMap::UnloadImages()
{
	DeleteBitmap(IMAGE_MINIMAP_INTERFACE);
}

bool SEASON3B::CNewUIMiniMap::UpdateMouseEvent()
{
	bool pRetMouse = true;

	if (m_BtnExit.UpdateMouseEvent() == true)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_MINI_MAP);
		return true;
	}

	if (IsPress(VK_LBUTTON))
	{
		pRetMouse = RequestMouse(MouseX, MouseY);
		if (pRetMouse == false)
		{
			PlayBuffer(SOUND_CLICK01);
		}
	}

	if (CheckMouseIn(m_MapPos.x, m_MapPos.y, pWidthForm, pHeighForm))
	{
		return false;
	}

	return pRetMouse;
}

bool SEASON3B::CNewUIMiniMap::RequestMouse(int mx, int my)
{
	if (MouseX > m_MapPos.x && MouseY > m_MapPos.y && MouseX < m_MapPos.x + pWidthForm && MouseY < m_MapPos.y + pHeighForm)
	{
		if (ViTriChon.x > 0 && ViTriChon.y > 0)
		{
			ViTriDiChuyen = ViTriChon;
			Movement = true;
			m_AutoMoveMap = gMapManager.WorldActive;
			m_AutoMoveStuckCount = 0;
			m_LastAutoMovePos.x = Hero->PositionX;
			m_LastAutoMovePos.y = Hero->PositionY;
			if (!BuildAutoMoveWayPoints(Hero->PositionX, Hero->PositionY, ViTriDiChuyen.x, ViTriDiChuyen.y))
			{
				StopAutoMove(false);
				CGAutoMove(0);
				return true;
			}
			CGAutoMove(1);
			if (!Hero->Movement)
			{
				StartNextAutoMoveStep();
			}
		}
	}
	return true;
}

void SEASON3B::CNewUIMiniMap::LoadImages(const char* Filename)
{
	char Fname[300];
	int i = 0;

	if (IndexIMGMap != -1)
		m_bSuccess = true;

	sprintf(Fname, "Data\\RISE\\Minimap\\Minimap_%s_%s.bmd", Filename, g_strSelectedML.c_str());

	for (i = 0; i < MAX_MINI_MAP_DATA; i++)
	{
		m_Mini_Map_Data[i].Kind = 0;
	}

	FILE* fp = fopen(Fname, "rb");

	if (fp != NULL)
	{
		int Size = sizeof(MINI_MAP);
		BYTE* Buffer = new BYTE[Size * MAX_MINI_MAP_DATA + 45];
		fread(Buffer, (Size * MAX_MINI_MAP_DATA) + 45, 1, fp);

		DWORD dwCheckSum;
		fread(&dwCheckSum, sizeof(DWORD), 1, fp);
		fclose(fp);

		if (dwCheckSum != GenerateCheckSum2(Buffer, (Size * MAX_MINI_MAP_DATA) + 45, 0x2BC1))
		{
			char Text[256];
			sprintf(Text, "%s - File corrupted.", Fname);
			MessageBox(g_hWnd, Text, NULL, MB_OK);
			SendMessage(g_hWnd, WM_DESTROY, 0, 0);
		}
		else
		{
			BYTE* pSeek = Buffer;

			for (i = 0; i < MAX_MINI_MAP_DATA; i++)
			{
				BuxConvert(pSeek, Size);
				memcpy(&(m_Mini_Map_Data[i]), pSeek, Size);

				pSeek += Size;
			}
		}

		delete[] Buffer;
	}
}

bool SEASON3B::CNewUIMiniMap::ReCheckMap(int Map) // OK
{
	if (Map - 1 == 30) { return 0; }

	char Path[64];

	wsprintf(Path, "Data\\RISE\\FaceMap\\World_%02d.ozt", Map);
	FILE* fp = fopen(Path, "rb");

	if (fp != NULL)
	{
		fclose(fp);
		return 1;
	}
	else
	{
		return 0;
	}
}
void SEASON3B::CNewUIMiniMap::MiniMapLoad()
{
	SendAutoMover();

	if (gMapManager.WorldActive != CacheMapNumber)// && gMapManager.WorldActive != 30)
	{
		switch (gMapManager.WorldActive)
		{
			case 0:  IndexIMGMap = START_WORLD + 0;  break;
			case 1:  IndexIMGMap = START_WORLD + 1;  break;
			case 2:  IndexIMGMap = START_WORLD + 2;  break;
			case 3:  IndexIMGMap = START_WORLD + 3;  break;
			case 4:  IndexIMGMap = START_WORLD + 4;  break;
			case 6:  IndexIMGMap = START_WORLD + 6;  break;
			case 7:  IndexIMGMap = START_WORLD + 7;  break;
			case 8:  IndexIMGMap = START_WORLD + 8;  break;
			case 9:  IndexIMGMap = START_WORLD + 9;  break;
			case 10: IndexIMGMap = START_WORLD + 10; break;

			case 11: 
			case 12: 
			case 13: 
			case 14: 
			case 15: 
			case 16: 
			case 17: 
			case 52: IndexIMGMap = START_WORLD + 11; break;

			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 53: IndexIMGMap = START_WORLD + 18; break;

			case 29: IndexIMGMap = START_WORLD + 24; break;
			case 30: IndexIMGMap = START_WORLD + 30; break;
			case 31: IndexIMGMap = START_WORLD + 31; break;
			case 33: IndexIMGMap = START_WORLD + 33; break;
			case 34: IndexIMGMap = START_WORLD + 34; break;
			case 37: IndexIMGMap = START_WORLD + 37; break;
			case 38: IndexIMGMap = START_WORLD + 38; break;
			case 41: IndexIMGMap = START_WORLD + 41; break;
			case 42: IndexIMGMap = START_WORLD + 42; break;
			case 51: IndexIMGMap = START_WORLD + 51; break;
			case 56: IndexIMGMap = START_WORLD + 56; break;
			case 57: IndexIMGMap = START_WORLD + 57; break;
			case 63: IndexIMGMap = START_WORLD + 63; break;
			case 79: IndexIMGMap = START_WORLD + 79; break;
			case 80: IndexIMGMap = START_WORLD + 80; break;
			case 81: IndexIMGMap = START_WORLD + 81; break;
			case 82: IndexIMGMap = START_WORLD + 82; break;
			case 83: IndexIMGMap = START_WORLD + 83; break;
			case 84: IndexIMGMap = START_WORLD + 84; break;
			case 85: IndexIMGMap = START_WORLD + 85; break;
			case 86: IndexIMGMap = START_WORLD + 86; break;
			case 87: IndexIMGMap = START_WORLD + 87; break;
			case 88: IndexIMGMap = START_WORLD + 88; break;

			default: IndexIMGMap = -1; break;
		}
	}

	CacheMapNumber = gMapManager.WorldActive;
}


void SEASON3B::CNewUIMiniMap::SendAutoMover()
{
	if (!Movement)
		return;

	if (SEASON3B::IsPress(VK_ESCAPE))
	{
		StopAutoMove(true);
		CGAutoMove(0);
		return;
	}


	pKit->ThisFont(g_hFont, true, m_MapPos.x, 34, 0xFFB300FF, 180, pWidthForm, 0, RT3_SORT_CENTER, GlobalText[2477], ViTriDiChuyen.x, ViTriDiChuyen.y);

	if (Hero->Dead != 0 || Hero->Appear != 0 || gMapManager.WorldActive != m_AutoMoveMap)
	{
		StopAutoMove(true);
		return;
	}

	const POINT finalTarget = { ViTriDiChuyen.x, ViTriDiChuyen.y };
	if (IsNearAutoMovePoint(finalTarget, MINIMAP_AUTO_REACH_RANGE))
	{
		StopAutoMove(false);
		CGAutoMove(0);
		return;
	}

	while (!m_AutoMoveWayPoints.empty() && IsNearAutoMovePoint(m_AutoMoveWayPoints.front(), MINIMAP_AUTO_REACH_RANGE))
	{
		m_AutoMoveWayPoints.pop_front();
	}

	if (m_AutoMoveWayPoints.empty())
	{
		if (!BuildAutoMoveWayPoints(Hero->PositionX, Hero->PositionY, ViTriDiChuyen.x, ViTriDiChuyen.y))
		{
			StopAutoMove(true);
			CGAutoMove(0);
			return;
		}
	}

	if (Hero->Movement)
	{
		m_LastAutoMovePos.x = Hero->PositionX;
		m_LastAutoMovePos.y = Hero->PositionY;
		m_AutoMoveStuckCount = 0;
		return;
	}

	if (m_LastAutoMovePos.x == Hero->PositionX && m_LastAutoMovePos.y == Hero->PositionY)
	{
		++m_AutoMoveStuckCount;
	}
	else
	{
		m_LastAutoMovePos.x = Hero->PositionX;
		m_LastAutoMovePos.y = Hero->PositionY;
		m_AutoMoveStuckCount = 0;
	}

	if (m_AutoMoveStuckCount >= MINIMAP_AUTO_STUCK_LIMIT)
	{
		m_AutoMoveStuckCount = 0;
		if (!BuildAutoMoveWayPoints(Hero->PositionX, Hero->PositionY, ViTriDiChuyen.x, ViTriDiChuyen.y))
		{
			StopAutoMove(true);
			CGAutoMove(0);
			return;
		}
	}

	if (!StartNextAutoMoveStep())
	{
		if (!BuildAutoMoveWayPoints(Hero->PositionX, Hero->PositionY, ViTriDiChuyen.x, ViTriDiChuyen.y)
			|| !StartNextAutoMoveStep())
		{
			StopAutoMove(true);
			CGAutoMove(0);
		}
	}
}

bool SEASON3B::CNewUIMiniMap::BuildAutoMoveWayPoints(int sx, int sy, int dx, int dy)
{
	return BuildMiniMapWayPointsInternal(sx, sy, dx, dy, m_AutoMoveWayPoints);
}

bool SEASON3B::CNewUIMiniMap::StartNextAutoMoveStep()
{
	while (!m_AutoMoveWayPoints.empty() && IsNearAutoMovePoint(m_AutoMoveWayPoints.front(), MINIMAP_AUTO_REACH_RANGE))
	{
		m_AutoMoveWayPoints.pop_front();
	}

	if (m_AutoMoveWayPoints.empty())
	{
		return false;
	}

	const POINT nextPoint = m_AutoMoveWayPoints.front();

	if (!PathFinding2(Hero->PositionX, Hero->PositionY, nextPoint.x, nextPoint.y, &Hero->Path, 0.0f, TW_NOMOVE))
	{
		return false;
	}

	Hero->MovementType = MOVEMENT_MOVE;
	Hero->Movement = true;
	SendMove(Hero, &Hero->Object);
	return true;
}

bool SEASON3B::CNewUIMiniMap::IsNearAutoMovePoint(const POINT& pt, int range) const
{
	return abs(Hero->PositionX - pt.x) <= range && abs(Hero->PositionY - pt.y) <= range;
}

void SEASON3B::CNewUIMiniMap::StopAutoMove(bool bStopHero)
{
	Movement = false;
	m_AutoMoveWayPoints.clear();
	m_AutoMoveStuckCount = 0;
	m_AutoMoveMap = -1;

	if (bStopHero && Hero->Movement)
	{
		Hero->Movement = false;
		SetPlayerStop(Hero);
	}
}
