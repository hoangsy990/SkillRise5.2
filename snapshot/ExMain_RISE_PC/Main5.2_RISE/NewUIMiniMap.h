//////////////////////////////////////////////////////////////////////
// NewUIGuildInfoWindow.h: interface for the CNewUIGuildInfoWindow class.
//////////////////////////////////////////////////////////////////////

#pragma once

#include "NewUIBase.h"
#include "NewUIManager.h"
#include "NewUIMainFrameWindow.h"
#include "NewUIChatLogWindow.h"
#include "NewUIMyInventory.h"
#include <deque>
#include <vector>

namespace SEASON3B
{
	class CNewUIMiniMap : public CNewUIObj
	{
	public:
		enum IMAGE_LIST
		{
			IMAGE_MINIMAP_INTERFACE = BITMAP_MINI_MAP_BEGIN,
		};

		private:
		CNewUIManager*			m_pNewUIMng;
		CNewUIButton			m_BtnExit;
		MINI_MAP				m_Mini_Map_Data[MAX_MINI_MAP_DATA];
		float					m_Btn_Loc[MAX_MINI_MAP_DATA][4];

		public:
		
		CNewUIMiniMap();
		virtual ~CNewUIMiniMap();

		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();

		void SetPos(int x, int y);
		void SetBtnPos(int Num, float x, float y, float nx, float ny);

		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();
		void RenderMiniOverlay();
		bool IsMiniOverlayVisible() const;
		bool UpdateMiniOverlayMouseEvent();
		void ReceiveMapMarkers(const BYTE* data, int size);
		void ResetMapMarkers() { m_MapMarkers.clear(); m_MarkerMap = -1; m_MarkerRequestTick = 0;
#ifdef RISE_SLAYER_PORT
            ResetSlayerDetection();
#endif
        }
#ifdef RISE_SLAYER_PORT
        void BeginSlayerDetection(int map, int durationSeconds);
        void ResetSlayerDetection();
#endif
		void CreateMap();
		void CreateText();
		void CreateButton();
		float GetLayerDepth();

		void ClosingProcess();

		CNewUIButton m_BtnToolTip;
		void LoadImages(const char* Filename);
		
		POINT	ViTriChon;
		POINT	ViTriDiChuyen;
		bool	Movement;
		bool	m_bSuccess;
		int		IndexIMGMap;
		int		CacheMapNumber;

		void MiniMapLoad();
		bool ReCheckMap(int Map);
		void UnloadImages();
		void LoadImagesBitMap();
		void SendAutoMover();

		float pWidthForm;
		float pHeighForm;
		float XNPC;
		float YNPC;

		private:
		std::deque<POINT> m_AutoMoveWayPoints;
		struct MapMarker { BYTE kind, x, y; WORD monsterClass; char name[32]; };
		std::vector<MapMarker> m_MapMarkers;
		int m_MarkerMap = -1;
		DWORD m_MarkerRequestTick = 0;
		void RenderMapMarkers();
#ifdef RISE_SLAYER_PORT
        DWORD m_SlayerDetectionStartTick = 0;
        DWORD m_SlayerDetectionDurationMs = 0;
        int m_SlayerDetectionMap = -1;
        void RenderSlayerDetection();
#endif
		POINT m_LastAutoMovePos;
		int m_AutoMoveStuckCount;
		int m_AutoMoveMap;
		float m_MiniOverlayCenterX;
		float m_MiniOverlayCenterY;
		DWORD m_MiniOverlayLastTick;
		DWORD m_MiniOverlayClickTick = 0;
		int m_MiniOverlayMap;
		bool m_bMiniOverlayCenterInitialized;

		bool RequestMouse(int mx, int my);
		bool BuildAutoMoveWayPoints(int sx, int sy, int dx, int dy);
		bool StartNextAutoMoveStep();
		bool IsNearAutoMovePoint(const POINT& pt, int range = 1) const;
		void StopAutoMove(bool bStopHero);
		POINT m_MapPos;
	};
}


