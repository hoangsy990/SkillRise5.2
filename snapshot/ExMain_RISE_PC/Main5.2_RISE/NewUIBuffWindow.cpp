// NewUIBuffWindow.cpp: implementation of the CNewUIBuffWindow class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NewUIBuffWindow.h"
#include "ZzzBMD.h"
#include "ZzzCharacter.h"
#include "ZzzTexture.h"
#include "ZzzInventory.h"
#include "UIControls.h"
#include "NewUICommonMessageBox.h"
#include <RISE/CustomIconBuff.h>
#include <RISE/ZzzToolKit.h>
#include <RISE/WideData.h>
#include "RISE/GrowLancerResources.h"
#include "RISE/GrowLancerWrathBuffMetadata.h"
#include "wsctlc.h"
#include "WSclient.h"

namespace {
void RenderWrathReceiptTip(int buff, float x, float y)
{
	const char* name = rise::growlancer::WrathBuffName(buff);
	if (!name) return;
	memset(TextList, 0, sizeof(TextList));
	memset(TextListColor, 0, sizeof(TextListColor));
	memset(TextBold, 0, sizeof(TextBold));
	unicode::_sprintf(TextList[0], "%s", name);
	TextListColor[0] = TEXT_COLOR_YELLOW;
	TextBold[0] = true;
	unicode::_sprintf(TextList[1], "%s", rise::growlancer::kWrathBuffDescription);
	TextListColor[1] = TEXT_COLOR_WHITE;
	int lines = 2;
	const auto* receipt = SocketClient.GrowLancerWrathReceipts().Find(static_cast<unsigned short>(buff));
	if (receipt && receipt->received)
	{
		// Native formatter accepts DWORD seconds without time_t/year wrapping.
		// Reading the countdown never changes authoritative membership.
		std::string time;
		TheBuffTimeControl().GetStringTime(receipt->countdown.Remaining(timeGetTime()), time, true);
		unicode::_sprintf(TextList[lines], GlobalText[2533], time.c_str());
		TextListColor[lines++] = TEXT_COLOR_PURPLE;
	}
	RenderTipTextList(x, y, lines, 0, 3, STRP_TOPCENTER, TRUE, FALSE);
}
}

SEASON3B::CNewUIBuffWindow::CNewUIBuffWindow()
{
	m_pNewUIMng = NULL;

	m_Pos.x = 0;
	m_Pos.y = 0;

	BUFF_IMG_WIDTH = 20.0f;
	BUFF_IMG_HEIGHT = 28.0f;
	BUFF_MAX_LINE_COUNT = 32;
	BUFF_IMG_SPACE = 7;
}

SEASON3B::CNewUIBuffWindow::~CNewUIBuffWindow()
{
	Release();
}

bool SEASON3B::CNewUIBuffWindow::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_BUFF_WINDOW, this);

	SetPos(x, y);

	LoadImages();

	Show(true);

	return true;
}

void SEASON3B::CNewUIBuffWindow::Release()
{
	UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIBuffWindow::SetPos(int x, int y)
{
	m_Pos.x = x / 2;
	m_Pos.y = y;
}

void SEASON3B::CNewUIBuffWindow::SetPos(int iScreenWidth)
{
	if (iScreenWidth == 640) 
	{
		SetPos(0, 10);
	}
}

void SEASON3B::CNewUIBuffWindow::BuffSort(std::list<eBuffState>& buffstate)
{
	OBJECT* pHeroObject = &Hero->Object;

	int iBuffSize = g_CharacterBuffSize(pHeroObject);

	for (int i = 0; i < iBuffSize; ++i)
	{
		eBuffState eBuffType = g_CharacterBuff(pHeroObject, i);

		if (SetDisableRenderBuff(eBuffType))	continue;

		if (eBuffType != eBuffNone) {
			eBuffClass eBuffClassType = g_IsBuffClass(eBuffType);

			if (eBuffClassType == eBuffClass_Buff) {
				buffstate.push_front(eBuffType);
			}
			else if (eBuffClassType == eBuffClass_DeBuff) {
				buffstate.push_back(eBuffType);
			}
		}
	}

	for (int i = 0; i < MAX_EFFECT_LIST; i++)
	{
		if (g_pIconBuff->UserBuff[i].Index != 0)
		{
			bool bFound = false;
			for (std::list<eBuffState>::iterator it = buffstate.begin(); it != buffstate.end(); ++it)
			{
				if (*it == (eBuffState)g_pIconBuff->UserBuff[i].Index)
				{
					bFound = true;
					break;
				}
			}

			if (!bFound)
			{
				buffstate.push_front((eBuffState)g_pIconBuff->UserBuff[i].Index);
			}
		}
	}
}

bool SEASON3B::CNewUIBuffWindow::SetDisableRenderBuff(const eBuffState& _BuffState)
{
	switch (_BuffState)
	{
#ifdef PBG_ADD_PKSYSTEM_INGAMESHOP
	case eDeBuff_MoveCommandWin:
#endif //PBG_ADD_PKSYSTEM_INGAMESHOP
	case eDeBuff_FlameStrikeDamage:
	case eDeBuff_GiganticStormDamage:
	case eDeBuff_LightningShockDamage:
	case eDeBuff_Discharge_Stamina:
		return true;
	default:
		return false;
	}
	return false;
}

bool SEASON3B::CNewUIBuffWindow::UpdateMouseEvent()
{
	float x = 0.0f, y = 0.0f;
	int buffwidthcount = 0, buffheightcount = 0;

	std::list<eBuffState> buffstate;
	BuffSort(buffstate);

	std::list<eBuffState>::iterator iter;
	for (iter = buffstate.begin(); iter != buffstate.end(); )
	{
		auto tempiter = iter;
		++iter;
		eBuffState buff = (*tempiter);

		x = m_Pos.x + (buffwidthcount * (BUFF_IMG_WIDTH - BUFF_IMG_SPACE));
		y = m_Pos.y + (buffheightcount * (BUFF_IMG_HEIGHT - BUFF_IMG_SPACE));

		if (SEASON3B::CheckMouseIn(x, y, BUFF_IMG_WIDTH, BUFF_IMG_HEIGHT))
		{
			if (buff == eBuff_InfinityArrow)
			{
				if (SEASON3B::IsRelease(VK_RBUTTON))
				{
					SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CInfinityArrowCancelMsgBoxLayout));
				}
			}
			else if (buff == eBuff_SwellOfMagicPower)
			{
				if (SEASON3B::IsRelease(VK_RBUTTON))
				{
					SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CBuffSwellOfMPCancelMsgBoxLayOut));
				}
			}

			return false;
		}

		if (++buffwidthcount >= BUFF_MAX_LINE_COUNT) {
			buffwidthcount = 0;
			++buffheightcount;
		}
	}
	return true;
}

bool SEASON3B::CNewUIBuffWindow::UpdateKeyEvent()
{
	return true;
}

bool SEASON3B::CNewUIBuffWindow::Update()
{
	return true;
}

bool SEASON3B::CNewUIBuffWindow::Render()
{
	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	RenderBuffStatus(BUFF_RENDER_ICON);

	RenderBuffStatus(BUFF_RENDER_TOOLTIP);

	DisableAlphaBlend();

	return true;
}

void SEASON3B::CNewUIBuffWindow::RenderBuffStatus(BUFF_RENDER renderstate)
{
	OBJECT* pHeroObject = &Hero->Object;

	float SetPosIcon[3] = { 0 };

	int buffwidthcount = 0, buffheightcount = 0;

	std::list<eBuffState> buffstate;
	BuffSort(buffstate);

	std::list<eBuffState>::iterator iter;

	int iCount = (int)buffstate.size();
	if (iCount <= 0)
		return;

	for (int n = 2; n <= iCount; n++)
	{
		SetPosIcon[2] += 6.5f;
	}

	for (iter = buffstate.begin(); iter != buffstate.end(); )
	{
		auto tempiter = iter;
		++iter;
		eBuffState buff = (*tempiter);

		int timeIndex = -1;
		for (int k = 0; k < MAX_EFFECT_LIST; k++)
		{
			if (g_pIconBuff->UserBuff[k].Index == buff)
			{
				timeIndex = k;
				break;
			}
		}
		const bool wrath = rise::growlancer::HasWrathBuffClassification(static_cast<int>(buff));
		if (timeIndex < 0 && !wrath)
			continue;

		SetPosIcon[0] = ( m_Pos.x + 4 + (buffwidthcount * (BUFF_IMG_WIDTH - BUFF_IMG_SPACE)) ) - SetPosIcon[2];
		SetPosIcon[1] = m_Pos.y + (buffheightcount * (BUFF_IMG_HEIGHT - BUFF_IMG_SPACE));

		if (renderstate == BUFF_RENDER_ICON)
		{
			RenderBuffIcon(buff, SetPosIcon[0], SetPosIcon[1], BUFF_IMG_WIDTH, BUFF_IMG_HEIGHT);

			if (!wrath && g_pIconBuff->UserBuff[timeIndex].Index != NULL)
			{
				if (g_pIconBuff->UserBuff[timeIndex].Index != 28)
				{
					float pBuffHp = 100.0f;

					if (g_pIconBuff->UserBuff[timeIndex].MaxTime > 0 && g_pIconBuff->UserBuff[timeIndex].Time > 0)
					{
						pBuffHp = (float)(g_pIconBuff->UserBuff[timeIndex].Time) * 100.0f / (float)(g_pIconBuff->UserBuff[timeIndex].MaxTime);
					}

					EnableAlphaTest();
					glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
					RenderColor(SetPosIcon[0], (SetPosIcon[1] + BUFF_IMG_HEIGHT) - 12, ((BUFF_IMG_WIDTH - 9) * pBuffHp / 100.0f), 2.0f, 0.0f, 0);
					EnableAlphaTest();
					glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				}
			}
		}
		else if (renderstate == BUFF_RENDER_TOOLTIP)
		{
			if (SEASON3B::CheckMouseIn(SetPosIcon[0], SetPosIcon[1], BUFF_IMG_WIDTH - 9, BUFF_IMG_HEIGHT - 10))
			{
				float fTooltip_x = SetPosIcon[0] + (BUFF_IMG_WIDTH / 2);
				float fTooltip_y = SetPosIcon[1] + BUFF_IMG_WIDTH + 2;
				if (wrath) RenderWrathReceiptTip(static_cast<int>(buff), fTooltip_x, fTooltip_y);
				else g_pIconBuff->RenderTipBuff(buff, timeIndex, fTooltip_x, fTooltip_y);
			}
		}

		if (++buffwidthcount >= BUFF_MAX_LINE_COUNT)
		{
			buffwidthcount = 0;
			++buffheightcount;
		}
	}
}


void SEASON3B::CNewUIBuffWindow::RenderBuffIcon(eBuffState& eBuffType, float x, float y, float width, float height)
{
	using namespace rise::growlancer;
	WrathBuffIconRect rect;
	if (GetWrathBuffIconRect(static_cast<int>(eBuffType), rect))
	{
		if (EnsureWrathBuffAtlas())
			RenderBitmap(kWrathBuffAtlasBitmap, x, y, width / 1.8f, height / 1.8f,
				float(rect.x) / kWrathBuffAtlasWidth, float(rect.y) / kWrathBuffAtlasHeight,
				float(rect.width) / kWrathBuffAtlasWidth, float(rect.height) / kWrathBuffAtlasHeight);
		// Never fall through to invalid legacy atlas coordinates on load failure.
		return;
	}
	int iWidthIndex, iHeightIndex;
	float u, v;

	float Chia = 1.8f;
	if (eBuffType < 81) // eBuff_Berserker
	{
		iWidthIndex = (eBuffType - 1) % 10;
		iHeightIndex = (eBuffType - 1) / 10;

		u = iWidthIndex * width / 256.f;
		v = iHeightIndex * height / 256.f;
		// RISE Buff Icon
		RenderBitmap(IMAGE_BUFF_STATUS, x, y, width / Chia, height / Chia, u, v, width / 256.f, height / 256.f);
	}
	else if (eBuffType < 161)
	{
		// eBuff_Santa
		iWidthIndex = (eBuffType - 81) % 10; // eBuff_Berserker
		iHeightIndex = (eBuffType - 81) / 10; // eBuff_Berserker

		u = iWidthIndex * width / 256.f;
		v = iHeightIndex * height / 256.f;
		// RISE Buff Icon
		RenderBitmap(IMAGE_BUFF_STATUS2, x, y, width / Chia, height / Chia, u, v, width / 256.f, height / 256.f);
	}
	else
	{
		iWidthIndex = (eBuffType - 161) % 10;
		iHeightIndex = (eBuffType - 161) / 10;

		u = iWidthIndex * width / 256.f;
		v = iHeightIndex * height / 256.f;
		// RISE Buff Icon
		RenderBitmap(IMAGE_BUFF_STATUS3, x, y, width / Chia, height / Chia, u, v, width / 256.f, height / 256.f);
	}
}

float SEASON3B::CNewUIBuffWindow::GetLayerDepth()	//. 5.3f
{
	return 0.95f;
}

void SEASON3B::CNewUIBuffWindow::OpenningProcess()
{

}

void SEASON3B::CNewUIBuffWindow::ClosingProcess()
{

}

void SEASON3B::CNewUIBuffWindow::LoadImages()
{
	LoadBitmapFile("Interface\\newui_statusicon.jpg", IMAGE_BUFF_STATUS, GL_LINEAR);
	LoadBitmapFile("Interface\\newui_statusicon2.jpg", IMAGE_BUFF_STATUS2, GL_LINEAR);
	LoadBitmapFile("Interface\\newui_statusicon3.jpg", IMAGE_BUFF_STATUS3, GL_LINEAR);
}

void SEASON3B::CNewUIBuffWindow::UnloadImages()
{
	rise::growlancer::ReleaseWrathBuffAtlas();
	DeleteBitmap(IMAGE_BUFF_STATUS3);
	DeleteBitmap(IMAGE_BUFF_STATUS2);
	DeleteBitmap(IMAGE_BUFF_STATUS);
}
