#include "stdafx.h"
#include "EffectManager.h"
#include "Attack.h"
#include "Fruit.h"
#include "IllusionTemple.h"
#include "ItemManager.h"
#include "Map.h"
#include "MemScript.h"
#include "ObjectManager.h"
#include "Party.h"
#include "Util.h"
#include "Viewport.h"
#include "ServerInfo.h"
#include "Path.h"
#include "../../Addon/DualColor.h"
#include "../../Shared/PegasusHighSkillCatalogData.h"
#include <resource.h>
#define IDC_EFFECT_EDIT 9307
CEffectManager gEffectManager;
CEffectManagerUI gEffectManagerUI;
void SetListViewItemTextW(HWND hList, int nItem, int nSubItem, std::string utf8Text)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(), -1, NULL, 0);
	if (len > 0)
	{
		std::vector<WCHAR> buf(len);
		MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(), -1, &buf[0], len);
		LVITEMW lvi;
		memset(&lvi, 0, sizeof(lvi));
		lvi.iSubItem = nSubItem;
		lvi.pszText = &buf[0];
		SendMessageW(hList, LVM_SETITEMTEXTW, nItem, (LPARAM)&lvi);
	}
}
CEffectManagerUI::CEffectManagerUI()
{
	this->m_Data.clear();
}
void CEffectManagerUI::Load(HWND hList)
{
	this->m_Data.clear();
	ListView_DeleteAllItems(hList);
	char* path = "..\\Data\\Effect.txt";
	CMemScript* lpMemScript = new CMemScript;
	if (lpMemScript->SetBuffer(path) == 0)
	{
		LogAdd(LOG_RED, "[EffectManagerUI] Error: %s not found", path);
		delete lpMemScript;
		return;
	}
	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}
			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}
			EFFECT_DB_INFO info;
			info.Index = lpMemScript->GetNumber();
			info.Group = lpMemScript->GetAsString();
			info.ItemIndex = lpMemScript->GetAsString();
			info.Name = lpMemScript->GetAsString();
			info.Save = lpMemScript->GetAsString();
			info.Type = lpMemScript->GetAsString();
			info.Flag = lpMemScript->GetAsString();
			info.Count = lpMemScript->GetAsString();
			info.Value1 = lpMemScript->GetAsString();
			info.Value2 = lpMemScript->GetAsString();
			info.Value3 = lpMemScript->GetAsString();
			info.Value4 = lpMemScript->GetAsString();
			if (info.ItemIndex == "*")
			{
				info.Comment = "--";
			}
			else
			{
				char* name = gItemManager.GetItemName(atoi(info.ItemIndex.c_str()));
				if (name == NULL || strcmp(name, "No Name") == 0)
				{
					info.Comment = "Item does not exist";
				}
				else
				{
					info.Comment = name;
				}
			}
			this->m_Data.push_back(info);
		}
	}
	catch (...)
	{
		LogAdd(LOG_RED, "[EffectManagerUI] Error parsing %s", path);
	}
	delete lpMemScript;
	this->Refresh(hList);
}
void CEffectManagerUI::Refresh(HWND hList)
{
	ListView_DeleteAllItems(hList);
	for (int n = 0; n < (int)this->m_Data.size(); n++)
	{
		LVITEMW lvi;
		memset(&lvi, 0, sizeof(lvi));
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = n;
		lvi.iSubItem = 0;
		WCHAR szIndex[16];
		swprintf(szIndex, L"%d", this->m_Data[n].Index);
		lvi.pszText = szIndex;
		lvi.lParam = n;
		SendMessageW(hList, LVM_INSERTITEMW, n, (LPARAM)&lvi);
		SetListViewItemTextW(hList, n, 1, this->m_Data[n].Group);
		SetListViewItemTextW(hList, n, 2, this->m_Data[n].ItemIndex);
		SetListViewItemTextW(hList, n, 3, this->m_Data[n].Name);
		SetListViewItemTextW(hList, n, 4, this->m_Data[n].Save);
		SetListViewItemTextW(hList, n, 5, this->m_Data[n].Type);
		SetListViewItemTextW(hList, n, 6, this->m_Data[n].Flag);
		SetListViewItemTextW(hList, n, 7, this->m_Data[n].Count);
		SetListViewItemTextW(hList, n, 8, this->m_Data[n].Value1);
		SetListViewItemTextW(hList, n, 9, this->m_Data[n].Value2);
		SetListViewItemTextW(hList, n, 10, this->m_Data[n].Value3);
		SetListViewItemTextW(hList, n, 11, this->m_Data[n].Value4);
		SetListViewItemTextW(hList, n, 12, this->m_Data[n].Comment);
	}
}
void CEffectManagerUI::Save(HWND hList)
{
	char* path = gPath.GetFullPath("Effect.txt");
	FILE* file;
	if (fopen_s(&file, path, "w") != 0)
	{
		MessageBox(NULL, "Could not open Effect.txt for writing!", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	char line[1024];
	auto WriteField = [](char* b, const char* val, int width) {
		int len = (int)strlen(val);
		strcat(b, val);
		for (int i = 0; i < width - len; i++) strcat(b, " ");
	};
	line[0] = 0;
	WriteField(line, "//Index", 8);
	WriteField(line, "Group", 8);
	WriteField(line, "ItemIndex", 12);
	WriteField(line, "Name", 32);
	WriteField(line, "Save", 8);
	WriteField(line, "Type", 8);
	WriteField(line, "Flag", 8);
	WriteField(line, "Count", 8);
	WriteField(line, "Value1", 10);
	WriteField(line, "Value2", 10);
	WriteField(line, "Value3", 10);
	WriteField(line, "Value4", 10);
	WriteField(line, "//Comment", 32);
	fprintf(file, "%s\n", line);
	for (auto const& info : this->m_Data)
	{
		line[0] = 0;
		char buf[64];
		sprintf_s(buf, "%d", info.Index); WriteField(line, buf, 8);
		WriteField(line, info.Group.c_str(), 8);
		WriteField(line, info.ItemIndex.c_str(), 12);
		sprintf_s(buf, "\"%s\"", info.Name.c_str()); WriteField(line, buf, 32);
		WriteField(line, info.Save.c_str(), 8);
		WriteField(line, info.Type.c_str(), 8);
		WriteField(line, info.Flag.c_str(), 8);
		WriteField(line, info.Count.c_str(), 8);
		WriteField(line, info.Value1.c_str(), 10);
		WriteField(line, info.Value2.c_str(), 10);
		WriteField(line, info.Value3.c_str(), 10);
		WriteField(line, info.Value4.c_str(), 10);
		if (!info.Comment.empty() && info.Comment != "--")
		{
			sprintf_s(buf, "// %s", info.Comment.c_str()); 
			WriteField(line, buf, 32);
		}
		fprintf(file, "%s\n", line);
	}
	fprintf(file, "end");
	fclose(file);
	gEffectManager.Load(gPath.GetFullPath("Effect.txt"));
	MessageBoxW(NULL, MSGBOX_03, L"Effect Manager", MB_OK | MB_ICONINFORMATION);
	LogAdd(LOG_BLUE, "[EffectManagerUI] Settings saved and reloaded!");
}
static WNDPROC g_OldListProc;
static HWND hEdit;
static int g_Row = -1;
static int g_Col = -1;
LRESULT CALLBACK EffectListSubclass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_MOUSEWHEEL:
	{
		ShowWindow(hEdit, SW_HIDE);
		break;
	}
	}
	return CallWindowProc(g_OldListProc, hWnd, message, wParam, lParam);
}
INT_PTR CALLBACK CEffectManagerUI::EffectProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		HWND hList = GetDlgItem(hDlg, IDC_EFFECT_LIST);
		SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		LVCOLUMN lvCol;
		memset(&lvCol, 0, sizeof(lvCol));
		lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		lvCol.pszText = "Index"; lvCol.cx = 50; ListView_InsertColumn(hList, 0, &lvCol);
		lvCol.pszText = "Group"; lvCol.cx = 50; ListView_InsertColumn(hList, 1, &lvCol);
		lvCol.pszText = "ItemIndex"; lvCol.cx = 70; ListView_InsertColumn(hList, 2, &lvCol);
		lvCol.pszText = "Name"; lvCol.cx = 250; ListView_InsertColumn(hList, 3, &lvCol);
		lvCol.pszText = "Save"; lvCol.cx = 50; ListView_InsertColumn(hList, 4, &lvCol);
		lvCol.pszText = "Type"; lvCol.cx = 50; ListView_InsertColumn(hList, 5, &lvCol);
		lvCol.pszText = "Flag"; lvCol.cx = 50; ListView_InsertColumn(hList, 6, &lvCol);
		lvCol.pszText = "Count"; lvCol.cx = 50; ListView_InsertColumn(hList, 7, &lvCol);
		lvCol.pszText = "Value1"; lvCol.cx = 60; ListView_InsertColumn(hList, 8, &lvCol);
		lvCol.pszText = "Value2"; lvCol.cx = 60; ListView_InsertColumn(hList, 9, &lvCol);
		lvCol.pszText = "Value3"; lvCol.cx = 60; ListView_InsertColumn(hList, 10, &lvCol);
		lvCol.pszText = "Value4"; lvCol.cx = 60; ListView_InsertColumn(hList, 11, &lvCol);
		lvCol.pszText = "Comment"; lvCol.cx = 150; ListView_InsertColumn(hList, 12, &lvCol);
		g_OldListProc = (WNDPROC)SetWindowLongPtr(hList, GWLP_WNDPROC, (LONG_PTR)EffectListSubclass);
		hEdit = GetDlgItem(hDlg, IDC_EFFECT_EDIT);
		HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Roboto Condensed");
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
		gEffectManagerUI.Load(hList);
		RECT rc, rcOwner, rcDlg;
		GetWindowRect(GetDesktopWindow(), &rcOwner);
		GetWindowRect(hDlg, &rcDlg);
		CopyRect(&rc, &rcOwner);
		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);
		SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE);
		return (INT_PTR)TRUE;
	}
	case WM_NOTIFY:
	{
		LPNMHDR nm = (LPNMHDR)lParam;
		if (nm->idFrom == IDC_EFFECT_LIST)
		{
			if (nm->code == NM_DBLCLK)
			{
				LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)lParam;
				g_Row = lpnmlv->iItem;
				g_Col = lpnmlv->iSubItem;
				if (g_Row != -1)
				{
					RECT rect;
					ListView_GetSubItemRect(nm->hwndFrom, g_Row, g_Col, LVIR_BOUNDS, &rect);
					if (g_Col == 0)
					{
						rect.right = rect.left + ListView_GetColumnWidth(nm->hwndFrom, 0);
					}
					MapWindowPoints(nm->hwndFrom, hDlg, (LPPOINT)&rect, 2);
					WCHAR szText[256];
					LVITEMW lvi;
					memset(&lvi, 0, sizeof(lvi));
					lvi.iSubItem = g_Col;
					lvi.pszText = szText;
					lvi.cchTextMax = 256;
					SendMessageW(nm->hwndFrom, LVM_GETITEMTEXTW, g_Row, (LPARAM)&lvi);
					SetWindowPos(hEdit, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
					SetWindowTextW(hEdit, szText);
					SetFocus(hEdit);
					SendMessage(hEdit, EM_SETSEL, 0, -1);
				}
			}
			else if (nm->code == NM_RCLICK)
			{
				LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)lParam;
				g_Row = lpnmlv->iItem;
				HMENU hMenu = CreatePopupMenu();
				AppendMenu(hMenu, MF_STRING, IDM_EFFECT_INSERT_ROW, "Insert Row");
				if (g_Row != -1)
				{
					AppendMenu(hMenu, MF_STRING, IDM_EFFECT_DELETE_THIS, "Delete This Row");
				}
				AppendMenu(hMenu, MF_STRING, IDM_EFFECT_DELETE_ALL, "Delete All Rows");
				POINT pt;
				GetCursorPos(&pt);
				TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hDlg, NULL);
				DestroyMenu(hMenu);
			}
		}
		break;
	}
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDC_EFFECT_EDIT && HIWORD(wParam) == EN_KILLFOCUS)
		{
			WCHAR szWideText[256];
			GetWindowTextW(hEdit, szWideText, 256);
			ShowWindow(hEdit, SW_HIDE);
			char szText[256];
			WideCharToMultiByte(CP_UTF8, 0, szWideText, -1, szText, 256, NULL, NULL);
			if (g_Row != -1)
			{
				HWND hList = GetDlgItem(hDlg, IDC_EFFECT_LIST);
				SetListViewItemTextW(hList, g_Row, g_Col, szText);
				if (g_Row >= 0 && g_Row < (int)gEffectManagerUI.m_Data.size())
				{
					switch (g_Col)
					{
					case 0: gEffectManagerUI.m_Data[g_Row].Index = atoi(szText); break;
					case 1: gEffectManagerUI.m_Data[g_Row].Group = szText; break;
					case 2:
						gEffectManagerUI.m_Data[g_Row].ItemIndex = szText;
						if (gEffectManagerUI.m_Data[g_Row].ItemIndex == "*")
						{
							gEffectManagerUI.m_Data[g_Row].Comment = "--";
						}
						else
						{
							char* name = gItemManager.GetItemName(atoi(szText));
							if (name == NULL || strcmp(name, "No Name") == 0)
							{
								gEffectManagerUI.m_Data[g_Row].Comment = "Item does not exist";
							}
							else
							{
								gEffectManagerUI.m_Data[g_Row].Comment = name;
							}
						}
						SetListViewItemTextW(hList, g_Row, 12, gEffectManagerUI.m_Data[g_Row].Comment);
						break;
					case 3: gEffectManagerUI.m_Data[g_Row].Name = szText; break;
					case 4: gEffectManagerUI.m_Data[g_Row].Save = szText; break;
					case 5: gEffectManagerUI.m_Data[g_Row].Type = szText; break;
					case 6: gEffectManagerUI.m_Data[g_Row].Flag = szText; break;
					case 7: gEffectManagerUI.m_Data[g_Row].Count = szText; break;
					case 8: gEffectManagerUI.m_Data[g_Row].Value1 = szText; break;
					case 9: gEffectManagerUI.m_Data[g_Row].Value2 = szText; break;
					case 10: gEffectManagerUI.m_Data[g_Row].Value3 = szText; break;
					case 11: gEffectManagerUI.m_Data[g_Row].Value4 = szText; break;
					case 12: gEffectManagerUI.m_Data[g_Row].Comment = szText; break;
					}
				}
			}
		}
		switch (LOWORD(wParam))
		{
		case IDC_EFFECT_SAVE:
			gEffectManagerUI.Save(GetDlgItem(hDlg, IDC_EFFECT_LIST));
			break;
		case IDM_EFFECT_INSERT_ROW:
		{
			EFFECT_DB_INFO info;
			info.Index = 0;
			info.Group = "0";
			info.ItemIndex = "*";
			info.Name = "New Effect";
			info.Save = "0";
			info.Type = "0";
			info.Flag = "0";
			info.Count = "*";
			info.Value1 = "*";
			info.Value2 = "*";
			info.Value3 = "*";
			info.Value4 = "*";
			info.Comment = "--";
			if (g_Row != -1 && g_Row < (int)gEffectManagerUI.m_Data.size())
			{
				gEffectManagerUI.m_Data.insert(gEffectManagerUI.m_Data.begin() + g_Row + 1, info);
			}
			else
			{
				gEffectManagerUI.m_Data.push_back(info);
			}
			gEffectManagerUI.Refresh(GetDlgItem(hDlg, IDC_EFFECT_LIST));
			break;
		}
		case IDM_EFFECT_DELETE_THIS:
		{
			if (g_Row != -1 && g_Row < (int)gEffectManagerUI.m_Data.size())
			{
				gEffectManagerUI.m_Data.erase(gEffectManagerUI.m_Data.begin() + g_Row);
				gEffectManagerUI.Refresh(GetDlgItem(hDlg, IDC_EFFECT_LIST));
			}
			break;
		}
		case IDM_EFFECT_DELETE_ALL:
		{
			if (MessageBox(hDlg, "Are you sure you want to delete all effects?", "Warning", MB_YESNO | MB_ICONWARNING) == IDYES)
			{
				gEffectManagerUI.m_Data.clear();
				gEffectManagerUI.Refresh(GetDlgItem(hDlg, IDC_EFFECT_LIST));
			}
			break;
		}
		}
		break;
	}
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}
CEffectManager::CEffectManager()
{
	this->Init();
}
void CEffectManager::Init()
{
	for (int n = 0; n < MAX_EFFECT; n++)
	{
		this->m_EffectInfo[n].Index = -1;
	}
}
void CEffectManager::Load(char* path)
{
	CMemScript* lpMemScript = new CMemScript;
	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}
	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}
	this->Init();
	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}
			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}
			EFFECT_INFO info;
			info.Index = lpMemScript->GetNumber();
			info.Group = lpMemScript->GetAsNumber();
			info.ItemIndex = lpMemScript->GetAsNumber();
			strcpy_s(info.Name, lpMemScript->GetAsString());
			info.Save = lpMemScript->GetAsNumber();
			info.Type = lpMemScript->GetAsNumber();
			info.Flag = lpMemScript->GetAsNumber();
			info.Count = lpMemScript->GetAsNumber();
			info.Value[0] = lpMemScript->GetAsNumber();
			info.Value[1] = lpMemScript->GetAsNumber();
			info.Value[2] = lpMemScript->GetAsNumber();
			info.Value[3] = lpMemScript->GetAsNumber();
			this->SetInfo(info);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}
	EFFECT_INFO swordWrath = {};
	swordWrath.Index = rise::pegasus::kSwordWrathNativeEffect;
	swordWrath.Group = rise::pegasus::kSwordWrathNativeEffect;
	swordWrath.ItemIndex = -1;
	strcpy_s(swordWrath.Name, "Pegasus Sword Wrath");
	swordWrath.Save = 0;
	swordWrath.Type = 0;
	swordWrath.Flag = 0;
	swordWrath.Count = -1;
	for (int n = 0; n < 4; ++n) swordWrath.Value[n] = -1;
	if (this->GetInfo(swordWrath.Index) == 0)
	{
		this->SetInfo(swordWrath);
	}
	else
	{
		LogAdd(LOG_RED, "[Pegasus] Effect %d is already configured; Sword Wrath is disabled until the collision is resolved", swordWrath.Index);
	}
	struct PegasusEffectSeed { int index; const char* name; };
	const PegasusEffectSeed pegasusEffects[] = {
		{rise::pegasus::kDarknessNativeEffect, "Pegasus Darkness"},
		{rise::pegasus::kSpiralChargeNativeEffect, "Pegasus Spiral Charge"},
		{rise::pegasus::kCrusherChargeNativeEffect, "Pegasus Crusher Charge"},
		{rise::pegasus::kElementalChargeNativeEffect, "Pegasus Elemental Charge"},
		{rise::pegasus::kDexBoosterNativeEffect, "Pegasus Dex Booster"},
	};
	for (const PegasusEffectSeed& seed : pegasusEffects)
	{
		EFFECT_INFO info = {};
		info.Index = seed.index;
		info.Group = seed.index;
		info.ItemIndex = -1;
		strcpy_s(info.Name, seed.name);
		info.Save = 0;
		info.Type = 0;
		info.Flag = 0;
		info.Count = -1;
		for (int n = 0; n < 4; ++n) info.Value[n] = -1;
		if (this->GetInfo(info.Index) == 0)
		{
			this->SetInfo(info);
		}
		else
		{
			LogAdd(LOG_RED, "[Pegasus] Effect %d is already configured; %s is disabled until the collision is resolved", info.Index, seed.name);
		}
	}
	delete lpMemScript;
}
void CEffectManager::SetInfo(EFFECT_INFO info)
{
	if (EFFECT_RANGE(info.Index) == 0)
	{
		return;
	}
	this->m_EffectInfo[info.Index] = info;
}
EFFECT_INFO* CEffectManager::GetInfo(int index)
{
	if (EFFECT_RANGE(index) == 0)
	{
		return 0;
	}
	if (this->m_EffectInfo[index].Index != index)
	{
		return 0;
	}
	return &this->m_EffectInfo[index];
}
EFFECT_INFO* CEffectManager::GetInfoByItem(int ItemIndex)
{
	for (int n = 0; n < MAX_EFFECT; n++)
	{
		EFFECT_INFO* lpInfo = this->GetInfo(n);
		if (lpInfo == 0)
		{
			continue;
		}
		if (lpInfo->ItemIndex == ItemIndex)
		{
			return lpInfo;
		}
	}
	return 0;
}
void CEffectManager::MainProc()
{
	for (int n = 0; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnected(n) == 0)
		{
			continue;
		}
		LPOBJ lpObj = &gObj[n];
		for (int i = 0; i < MAX_EFFECT_LIST; i++)
		{
			CEffect* lpEffect = &lpObj->Effect[i];
			if (lpEffect->IsEffect() == 0)
			{
				continue;
			}
			if (((lpEffect->m_count == 0) ? 0 : (lpEffect->m_count--)) > 0)
			{
				this->PeriodicEffect(lpObj, lpEffect);
				if (lpEffect->m_count == 0)
				{
					this->RemoveEffect(lpObj, lpEffect);
					this->GCPeriodicEffectSend(lpObj, 0, lpEffect->m_group, 1, 0, lpEffect->m_index);
					lpEffect->Clear();
				}
			}
		}
	}
}
bool CEffectManager::AddEffect(LPOBJ lpObj, bool type, int index, int count, WORD value1, WORD value2, WORD value3, WORD value4)
{
	EFFECT_INFO* lpInfo = this->GetInfo(index);
	if (lpInfo == 0)
	{
		return 0;
	}
	if (type == 0 && lpInfo->Count != -1)
	{
		count = ((lpInfo->Type == 2) ? ((int)time(0) + lpInfo->Count) : lpInfo->Count);
	}
	if (lpInfo->Value[0] != -1)
	{
		value1 = lpInfo->Value[0];
	}
	if (lpInfo->Value[1] != -1)
	{
		value2 = lpInfo->Value[1];
	}
	if (lpInfo->Value[2] != -1)
	{
		value3 = lpInfo->Value[2];
	}
	if (lpInfo->Value[3] != -1)
	{
		value4 = lpInfo->Value[3];
	}
	CEffect* lpEffect = this->GetEffectByGroup(lpObj, lpInfo->Group);
	if (lpEffect == 0)
	{
		for (int n = 0; n < MAX_EFFECT_LIST; n++)
		{
			lpEffect = &lpObj->Effect[n];
			if (lpEffect->Set(index, lpInfo->Group, lpInfo->Save, lpInfo->Type, lpInfo->Flag, 0, count, value1, value2, value3, value4) != 0)
			{
				this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 0, lpEffect->m_count, lpEffect->m_index);
				this->InsertEffect(lpObj, lpEffect);
				return 1;
			}
		}
	}
	else
	{
		if (index != lpEffect->m_index || (value1 >= lpEffect->m_value[0] && value2 >= lpEffect->m_value[1] && value3 >= lpEffect->m_value[2] && value4 >= lpEffect->m_value[3]))
		{
			this->RemoveEffect(lpObj, lpEffect);
			this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 2, 0, lpEffect->m_index);
			lpEffect->Clear();
			if (lpEffect->Set(index, lpInfo->Group, lpInfo->Save, lpInfo->Type, lpInfo->Flag, 0, count, value1, value2, value3, value4) != 0)
			{
				this->GCPeriodicEffectSend(lpObj, lpEffect->m_group, 0, 0, lpEffect->m_count, lpEffect->m_index);
				this->InsertEffect(lpObj, lpEffect);
				return 1;
			}
		}
	}
	return 0;
}
bool CEffectManager::DelEffect(LPOBJ lpObj, int index)
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];
		if (lpEffect->IsEffect() == 0)
		{
			continue;
		}
		if (lpEffect->m_index == index)
		{
			this->RemoveEffect(lpObj, lpEffect);
			this->GCPeriodicEffectSend(lpObj, 0, lpEffect->m_group, 1, 0, lpEffect->m_index);
			lpEffect->Clear();
			return 1;
		}
	}
	return 0;
}
bool CEffectManager::DelEffectByGroup(LPOBJ lpObj, int group)
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];
		if (lpEffect->IsEffect() == 0)
		{
			continue;
		}
		if (lpEffect->m_group == group)
		{
			this->RemoveEffect(lpObj, lpEffect);
			this->GCPeriodicEffectSend(lpObj, 0, lpEffect->m_group, 1, 0, lpEffect->m_index);
			lpEffect->Clear();
			return 1;
		}
	}
	return 0;
}
CEffect* CEffectManager::GetEffect(LPOBJ lpObj, int index)
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];
		if (lpEffect->IsEffect() == 0)
		{
			continue;
		}
		if (lpEffect->m_index == index)
		{
			return lpEffect;
		}
	}
	return 0;
}
CEffect* CEffectManager::GetEffectByGroup(LPOBJ lpObj, int group)
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];
		if (lpEffect->IsEffect() == 0)
		{
			continue;
		}
		if (lpEffect->m_group == group)
		{
			return lpEffect;
		}
	}
	return 0;
}
bool CEffectManager::CheckEffect(LPOBJ lpObj, int index)
{
	if (this->GetEffect(lpObj, index) != 0)
	{
		return 1;
	}
	return 0;
}
bool CEffectManager::CheckEffectByGroup(LPOBJ lpObj, int group)
{
	if (this->GetEffectByGroup(lpObj, group) != 0)
	{
		return 1;
	}
	return 0;
}
void CEffectManager::InsertEffect(LPOBJ lpObj, CEffect* lpEffect)
{
	switch (lpEffect->m_index)
	{
	case EFFECT_GREATER_DAMAGE:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage += lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_DEFENSE:
		lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		break;
	case EFFECT_ELF_BUFFER:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense += lpEffect->m_value[1];
		break;
	case EFFECT_MANA_SHIELD:
		lpObj->EffectOption.AddDamageReduction += lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE:
		lpObj->EffectOption.AddCriticalDamage += lpEffect->m_value[0];
		break;
	case EFFECT_INFINITY_ARROW:
		break;
	case EFFECT_BP_RECOVERY:
		break;
	case EFFECT_GREATER_LIFE:
		lpObj->EffectOption.MulMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_MANA:
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_BLESS_POTION:
		break;
	case EFFECT_SOUL_POTION:
		lpObj->EffectOption.AddPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddBPRecovery += lpEffect->m_value[1];
		GCItemUseSpecialTimeSend(lpObj->Index, 2, 60);
		break;
	case EFFECT_DISABLE_MAGIC:
		break;
	case EFFECT_CASTLE_GATE_STATE:
		break;
	case EFFECT_GUILD_STATE1:
		break;
	case EFFECT_GUILD_STATE2:
		break;
	case EFFECT_GUILD_STATE3:
		break;
	case EFFECT_GUILD_STATE4:
		break;
	case EFFECT_INVISIBILITY:
		break;
	case EFFECT_GUILD_STATE5:
		break;
	case EFFECT_CASTLE_CROWN_STATE:
		break;
	case EFFECT_CRYWOLF_STATE1:
		break;
	case EFFECT_CRYWOLF_STATE2:
		break;
	case EFFECT_CRYWOLF_STATE3:
		break;
	case EFFECT_CRYWOLF_STATE4:
		break;
	case EFFECT_CRYWOLF_STATE5:
		break;
	case EFFECT_CRYWOLF_STATE6:
		break;
	case EFFECT_CRYWOLF_STATE7:
		break;
	case EFFECT_GAME_MASTER:
		break;
	case EFFECT_SEAL_OF_ASCENSION1:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_WEALTH1:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[1];
		break;
	case EFFECT_SEAL_OF_SUSTENANCE1:
		break;
	case EFFECT_ORDER_OF_SPEED:
		break;
	case EFFECT_ORDER_OF_SUBLIMATION:
		break;
	case EFFECT_ORDER_OF_PROTECTION:
		lpObj->EffectOption.AddDamageReduction += lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN1:
		lpObj->EffectOption.AddPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed += lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN2:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN3:
		lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN4:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN5:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_ASCENSION2:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_WEALTH2:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[1];
		break;
	case EFFECT_SEAL_OF_SUSTENANCE2:
		break;
	case EFFECT_SEAL_OF_MOVEMENT:
		break;
	case EFFECT_SCROLL_OF_QUICK:
		lpObj->EffectOption.AddPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed += lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_DEFENSE:
		lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_DAMAGE:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_MAGIC_DAMAGE:
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_LIFE:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_MANA:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_STRENGTH:
		lpObj->EffectOption.AddStrength += lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_DEXTERITY:
		lpObj->EffectOption.AddDexterity += lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_VITALITY:
		lpObj->EffectOption.AddVitality += lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_ENERGY:
		lpObj->EffectOption.AddEnergy += lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_LEADERSHIP:
		lpObj->EffectOption.AddLeadership += lpEffect->m_value[0];
		break;
	case EFFECT_POISON:
		break;
	case EFFECT_ICE:
		lpObj->DelayLevel = 1;
		lpObj->DelayActionTime = 800;
		break;
	case EFFECT_ICE_ARROW:
		break;
	case EFFECT_FIRE_SLASH:
		lpObj->EffectOption.DivDefense += lpEffect->m_value[0];
		break;
	case EFFECT_PHYSI_DAMAGE_IMMUNITY:
		break;
	case EFFECT_MAGIC_DAMAGE_IMMUNITY:
		break;
	case EFFECT_STERN:
		break;
	case EFFECT_MAGIC_DEFENSE:
		break;
	case EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY:
		break;
	case EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY:
		break;
	case EFFECT_ORDER_OF_RESTRAINT:
		break;
	case EFFECT_CRYWOLF_STATE8:
		break;
	case EFFECT_CRYWOLF_STATE9:
		break;
	case EFFECT_CRYWOLF_STATE10:
		break;
	case EFFECT_CRYWOLF_STATE11:
		break;
	case EFFECT_CRYWOLF_STATE12:
		break;
	case EFFECT_DAMAGE_REFLECT:
		lpObj->EffectOption.AddDamageReflect += lpEffect->m_value[0];
		break;
	case EFFECT_SLEEP:
		break;
	case EFFECT_BLIND:
		lpObj->EffectOption.DivAttackSuccessRate += lpEffect->m_value[0];
		break;
	case EFFECT_NEIL:
		break;
	case EFFECT_SAHAMUTT:
		break;
	case EFFECT_LESSER_DAMAGE:
		lpObj->EffectOption.DivPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.DivCurseDamage += lpEffect->m_value[0];
		break;
	case EFFECT_LESSER_DEFENSE:
		lpObj->EffectOption.DivDefense += lpEffect->m_value[0];
		break;
	case EFFECT_CHERRY_BLOSSOM1:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_CHERRY_BLOSSOM2:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_CHERRY_BLOSSOM3:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		break;
	case EFFECT_SWORD_POWER:
		lpObj->EffectOption.AddSwordPowerDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate += lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP += lpEffect->m_value[3];
		break;
	case EFFECT_MAGIC_CIRCLE:
		lpObj->EffectOption.AddMinMagicDamage += lpEffect->m_value[0];
		break;
	case EFFECT_SWORD_SLASH:
		break;
	case EFFECT_LIGHTNING_STORM:
		break;
	case EFFECT_RED_STORM:
		break;
	case EFFECT_FROZEN_STAB:
		lpObj->DelayLevel = 1;
		lpObj->DelayActionTime = 1000;
		break;
	case EFFECT_SEAL_OF_LIFE:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddHPRecoveryRate += lpEffect->m_value[1];
		break;
	case EFFECT_SEAL_OF_MANA:
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMPRecoveryRate += lpEffect->m_value[1];
		break;
	case EFFECT_SCROLL_OF_BATTLE:
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_STRENGTH:
		lpObj->EffectOption.AddExcellentDamageRate += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS1:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense += lpEffect->m_value[1];
		break;
	case EFFECT_CHRISTMAS2:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS3:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS4:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS5:
		lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS6:
		lpObj->EffectOption.AddPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed += lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS7:
		lpObj->EffectOption.AddBPRecovery += lpEffect->m_value[0];
		break;
	case EFFECT_DUEL_ARENA_WATCH:
		break;
	case EFFECT_TALISMAN_OF_GUARDIAN:
		break;
	case EFFECT_TALISMAN_OF_PROTECTION:
		break;
	case EFFECT_MASTER_SEAL_OF_ASCENSION:
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SEAL_OF_WEALTH:
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[1];
		break;
	case EFFECT_GLADIATORS_GLORY:
		lpObj->EffectOption.AddVolcanoExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_PARTY_EXPERIENCE_BONUS:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddPartyBonusExperienceRate += lpEffect->m_value[1];
		break;
	case EFFECT_MAX_AG_BOOST_AURA:
		lpObj->EffectOption.AddMaxBP += (lpEffect->m_value[0] = (lpEffect->m_value[0] * lpObj->Level));
		break;
	case EFFECT_MAX_SD_BOOST_AURA:
		lpObj->EffectOption.AddMaxSD += (lpEffect->m_value[0] = (lpEffect->m_value[0] * lpObj->Level));
		break;
	case EFFECT_SCROLL_OF_HEALING:
		lpObj->EffectOption.AddHPRecoveryRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[1];
		break;
	case EFFECT_HAWK_FIGURINE:
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[0];
		break;
	case EFFECT_GOAT_FIGURINE:
		lpObj->EffectOption.AddExcellentDamageRate += lpEffect->m_value[0];
		break;
	case EFFECT_OAK_CHARM:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_MAPLE_CHARM:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_GOLDEN_OAK_CHARM:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxSD += lpEffect->m_value[1];
		break;
	case EFFECT_GOLDEN_MAPLE_CHARM:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxBP += lpEffect->m_value[1];
		break;
	case EFFECT_WORN_HORSESHOE:
		lpObj->EffectOption.AddItemDropRate += lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_IGNORE_DEFENSE_RATE:
		lpObj->EffectOption.AddIgnoreDefenseRate += lpEffect->m_value[0];
		break;
	case EFFECT_FITNESS:
		lpObj->EffectOption.AddVitality += lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE:
		lpObj->EffectOption.AddDefenseSuccessRate += lpEffect->m_value[0];
		break;
	case EFFECT_IRON_DEFENSE:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense += lpEffect->m_value[1];
		break;
	case EFFECT_GREATER_LIFE_ENHANCED:
		lpObj->EffectOption.MulMaxHP += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		break;
	case EFFECT_GREATER_LIFE_MASTERED:
		lpObj->EffectOption.MulMaxHP += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		lpObj->EffectOption.MulMaxBP += lpEffect->m_value[2];
		break;
	case EFFECT_DEATH_STAB_ENHANCED:
		break;
	case EFFECT_MAGIC_CIRCLE_IMPROVED:
		lpObj->EffectOption.AddMinMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxMagicDamage += lpEffect->m_value[1];
		break;
	case EFFECT_MAGIC_CIRCLE_ENHANCED:
		lpObj->EffectOption.AddMinMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxMagicDamage += lpEffect->m_value[1];
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[2];
		break;
	case EFFECT_MANA_SHIELD_MASTERED:
		lpObj->EffectOption.AddDamageReduction += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		break;
	case EFFECT_FROZEN_STAB_MASTERED:
		lpObj->DelayLevel = 1;
		lpObj->DelayActionTime = 1000;
		break;
	case EFFECT_BLESS:
		lpObj->EffectOption.AddStrength += lpEffect->m_value[0];
		lpObj->EffectOption.AddDexterity += lpEffect->m_value[0];
		lpObj->EffectOption.AddVitality += lpEffect->m_value[0];
		lpObj->EffectOption.AddEnergy += lpEffect->m_value[0];
		break;
	case EFFECT_INFINITY_ARROW_IMPROVED:
		lpObj->EffectOption.MulPhysiDamage += lpEffect->m_value[0];
		break;
	case EFFECT_BLIND_IMPROVED:
		lpObj->EffectOption.DivAttackSuccessRate += lpEffect->m_value[0];
		break;
	case EFFECT_DRAIN_LIFE_ENHANCED:
		break;
	case EFFECT_ICE_STORM_ENHANCED:
		break;
	case EFFECT_EARTH_PRISON:
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE_MASTERED:
		lpObj->EffectOption.AddCriticalDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[1];
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE_EXTENDED:
		lpObj->EffectOption.AddCriticalDamage += lpEffect->m_value[0];
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[1];
		lpObj->EffectOption.AddExcellentDamageRate += lpEffect->m_value[2];
		break;
	case EFFECT_SWORD_POWER_IMPROVED:
		lpObj->EffectOption.AddSwordPowerDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate += lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP += lpEffect->m_value[3];
		break;
	case EFFECT_SWORD_POWER_ENHANCED:
		lpObj->EffectOption.AddSwordPowerDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate += lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP += lpEffect->m_value[3];
		break;
	case EFFECT_SWORD_POWER_MASTERED:
		lpObj->EffectOption.AddSwordPowerDamageRate += lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP += lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate += lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP += lpEffect->m_value[3];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE_IMPROVED:
		lpObj->EffectOption.AddDefenseSuccessRate += lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE_ENHANCED:
		lpObj->EffectOption.AddDefenseSuccessRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense += lpEffect->m_value[1];
		break;
	case EFFECT_FITNESS_IMPROVED:
		lpObj->EffectOption.AddVitality += lpEffect->m_value[0];
		break;
	case EFFECT_DRAGON_ROAR_ENHANCED:
		break;
	case EFFECT_CHAIN_DRIVER_ENHANCED:
		break;
	case EFFECT_POISON_ARROW:
		break;
	case EFFECT_POISON_ARROW_IMPROVED:
		break;
	case EFFECT_BLESS_IMPROVED:
		lpObj->EffectOption.AddStrength += lpEffect->m_value[0];
		lpObj->EffectOption.AddDexterity += lpEffect->m_value[0];
		lpObj->EffectOption.AddVitality += lpEffect->m_value[0];
		lpObj->EffectOption.AddEnergy += lpEffect->m_value[0];
		break;
	case EFFECT_LESSER_DAMAGE_IMPROVED:
		lpObj->EffectOption.DivPhysiDamage += lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicDamage += lpEffect->m_value[0];
		lpObj->EffectOption.DivCurseDamage += lpEffect->m_value[0];
		break;
	case EFFECT_LESSER_DEFENSE_IMPROVED:
		lpObj->EffectOption.DivDefense += lpEffect->m_value[0];
		break;
	case EFFECT_FIRE_SLASH_ENHANCED:
		lpObj->EffectOption.DivDefense += lpEffect->m_value[0];
		break;
	case EFFECT_IRON_DEFENSE_IMPROVED:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense += lpEffect->m_value[1];
		break;
	case EFFECT_BLOOD_HOWLING:
		break;
	case EFFECT_BLOOD_HOWLING_IMPROVED:
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_SD:
		lpObj->EffectOption.DivMaxSD += lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_MP:
		lpObj->EffectOption.DivMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_SPEED:
		lpObj->EffectOption.DivPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicSpeed += lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_HP:
		lpObj->EffectOption.DivMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_STUN:
		break;
	case EFFECT_PENTAGRAM_JEWEL_SLOW:
		break;
	case EFFECT_TALISMAN_OF_ASCENSION1:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_TALISMAN_OF_ASCENSION2:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_TALISMAN_OF_ASCENSION3:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_ASCENSION3:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SEAL_OF_ASCENSION2:
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_BLESSING_OF_LIGHT:
		lpObj->EffectOption.AddExperienceRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_DEFENSE:
		lpObj->EffectOption.AddDefense += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_MAGIC_DAMAGE:
		lpObj->EffectOption.AddMagicDamage += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_LIFE:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_MANA:
		lpObj->EffectOption.AddMaxMP += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_DAMAGE:
		lpObj->EffectOption.AddPhysiDamage += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_HEALING:
		lpObj->EffectOption.AddHPRecoveryRate += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[1];
		break;
	case EFFECT_MASTER_SCROLL_OF_BATTLE:
		lpObj->EffectOption.AddCriticalDamageRate += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_STRENGTH:
		lpObj->EffectOption.AddExcellentDamageRate += lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_QUICK:
		lpObj->EffectOption.AddPhysiSpeed += lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed += lpEffect->m_value[0];
		break;
	case EFFECT_DAME_ITEM_SWICH:
		break;
	case EFFECT_DAME_SET_ITEM_SWICH:
		break;
	case EFFECT_DAME_PENRING_ITEM_SWICH:
		break;
	case EFFECT_DAME_WING_ITEM_SWICH:
		break;
	case EFFECT_DAME_KHIEN_ITEM_SWICH:
		break;
	case EFFECT_GUILD_BUFF:
		lpObj->EffectOption.AddMaxHP += lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxSD += lpEffect->m_value[1];
		lpObj->EffectOption.AddDefense += lpEffect->m_value[2];
		lpObj->EffectOption.AddDamageReduction += lpEffect->m_value[3];
		break;
	}
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
}
void CEffectManager::RemoveEffect(LPOBJ lpObj, CEffect* lpEffect)
{
	switch (lpEffect->m_index)
	{
	case EFFECT_GREATER_DAMAGE:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_DEFENSE:
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_ELF_BUFFER:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[1];
		break;
	case EFFECT_MANA_SHIELD:
		lpObj->EffectOption.AddDamageReduction -= lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE:
		lpObj->EffectOption.AddCriticalDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_INFINITY_ARROW:
		break;
	case EFFECT_BP_RECOVERY:
		break;
	case EFFECT_GREATER_LIFE:
		lpObj->EffectOption.MulMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_MANA:
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_BLESS_POTION:
		break;
	case EFFECT_SOUL_POTION:
		lpObj->EffectOption.AddPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddBPRecovery -= lpEffect->m_value[1];
		break;
	case EFFECT_DISABLE_MAGIC:
		break;
	case EFFECT_CASTLE_GATE_STATE:
		break;
	case EFFECT_GUILD_STATE1:
		break;
	case EFFECT_GUILD_STATE2:
		break;
	case EFFECT_GUILD_STATE3:
		break;
	case EFFECT_GUILD_STATE4:
		break;
	case EFFECT_INVISIBILITY:
		break;
	case EFFECT_GUILD_STATE5:
		break;
	case EFFECT_CASTLE_CROWN_STATE:
		break;
	case EFFECT_CRYWOLF_STATE1:
		break;
	case EFFECT_CRYWOLF_STATE2:
		break;
	case EFFECT_CRYWOLF_STATE3:
		break;
	case EFFECT_CRYWOLF_STATE4:
		break;
	case EFFECT_CRYWOLF_STATE5:
		break;
	case EFFECT_CRYWOLF_STATE6:
		break;
	case EFFECT_CRYWOLF_STATE7:
		break;
	case EFFECT_GAME_MASTER:
		break;
	case EFFECT_SEAL_OF_ASCENSION1:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_WEALTH1:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[1];
		break;
	case EFFECT_SEAL_OF_SUSTENANCE1:
		break;
	case EFFECT_ORDER_OF_SPEED:
		break;
	case EFFECT_ORDER_OF_SUBLIMATION:
		break;
	case EFFECT_ORDER_OF_PROTECTION:
		lpObj->EffectOption.AddDamageReduction -= lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN1:
		lpObj->EffectOption.AddPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed -= lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN2:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN3:
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN4:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_HALLOWEEN5:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_ASCENSION2:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_WEALTH2:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[1];
		break;
	case EFFECT_SEAL_OF_SUSTENANCE2:
		break;
	case EFFECT_SEAL_OF_MOVEMENT:
		break;
	case EFFECT_SCROLL_OF_QUICK:
		lpObj->EffectOption.AddPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed -= lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_DEFENSE:
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_DAMAGE:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_MAGIC_DAMAGE:
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_LIFE:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_MANA:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_STRENGTH:
		lpObj->EffectOption.AddStrength -= lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_DEXTERITY:
		lpObj->EffectOption.AddDexterity -= lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_VITALITY:
		lpObj->EffectOption.AddVitality -= lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_ENERGY:
		lpObj->EffectOption.AddEnergy -= lpEffect->m_value[0];
		break;
	case EFFECT_ELIXIR_OF_LEADERSHIP:
		lpObj->EffectOption.AddLeadership -= lpEffect->m_value[0];
		break;
	case EFFECT_POISON:
		break;
	case EFFECT_ICE:
		lpObj->DelayLevel = 0;
		lpObj->DelayActionTime = 0;
		break;
	case EFFECT_ICE_ARROW:
		break;
	case EFFECT_FIRE_SLASH:
		lpObj->EffectOption.DivDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_PHYSI_DAMAGE_IMMUNITY:
		break;
	case EFFECT_MAGIC_DAMAGE_IMMUNITY:
		break;
	case EFFECT_STERN:
		break;
	case EFFECT_MAGIC_DEFENSE:
		break;
	case EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY:
		break;
	case EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY:
		break;
	case EFFECT_ORDER_OF_RESTRAINT:
		break;
	case EFFECT_CRYWOLF_STATE8:
		break;
	case EFFECT_CRYWOLF_STATE9:
		break;
	case EFFECT_CRYWOLF_STATE10:
		break;
	case EFFECT_CRYWOLF_STATE11:
		break;
	case EFFECT_CRYWOLF_STATE12:
		break;
	case EFFECT_DAMAGE_REFLECT:
		lpObj->EffectOption.AddDamageReflect -= lpEffect->m_value[0];
		break;
	case EFFECT_SLEEP:
		break;
	case EFFECT_BLIND:
		lpObj->EffectOption.DivAttackSuccessRate -= lpEffect->m_value[0];
		break;
	case EFFECT_NEIL:
		break;
	case EFFECT_SAHAMUTT:
		break;
	case EFFECT_LESSER_DAMAGE:
		lpObj->EffectOption.DivPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DivCurseDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_LESSER_DEFENSE:
		lpObj->EffectOption.DivDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_CHERRY_BLOSSOM1:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_CHERRY_BLOSSOM2:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_CHERRY_BLOSSOM3:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_SWORD_POWER:
		lpObj->EffectOption.AddSwordPowerDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate -= lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP -= lpEffect->m_value[3];
		break;
	case EFFECT_MAGIC_CIRCLE:
		lpObj->EffectOption.AddMinMagicDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_SWORD_SLASH:
		break;
	case EFFECT_LIGHTNING_STORM:
		break;
	case EFFECT_RED_STORM:
		break;
	case EFFECT_FROZEN_STAB:
		lpObj->DelayLevel = 0;
		lpObj->DelayActionTime = 0;
		break;
	case EFFECT_SEAL_OF_LIFE:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddHPRecoveryRate -= lpEffect->m_value[1];
		break;
	case EFFECT_SEAL_OF_MANA:
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMPRecoveryRate -= lpEffect->m_value[1];
		break;
	case EFFECT_SCROLL_OF_BATTLE:
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_STRENGTH:
		lpObj->EffectOption.AddExcellentDamageRate -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS1:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[1];
		break;
	case EFFECT_CHRISTMAS2:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS3:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS4:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCurseDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS5:
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS6:
		lpObj->EffectOption.AddPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed -= lpEffect->m_value[0];
		break;
	case EFFECT_CHRISTMAS7:
		lpObj->EffectOption.AddBPRecovery -= lpEffect->m_value[0];
		break;
	case EFFECT_DUEL_ARENA_WATCH:
		break;
	case EFFECT_TALISMAN_OF_GUARDIAN:
		break;
	case EFFECT_TALISMAN_OF_PROTECTION:
		break;
	case EFFECT_MASTER_SEAL_OF_ASCENSION:
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SEAL_OF_WEALTH:
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[1];
		break;
	case EFFECT_GLADIATORS_GLORY:
		lpObj->EffectOption.AddVolcanoExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_PARTY_EXPERIENCE_BONUS:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddPartyBonusExperienceRate -= lpEffect->m_value[1];
		break;
	case EFFECT_MAX_AG_BOOST_AURA:
		lpObj->EffectOption.AddMaxBP -= lpEffect->m_value[0];
		break;
	case EFFECT_MAX_SD_BOOST_AURA:
		lpObj->EffectOption.AddMaxSD -= lpEffect->m_value[0];
		break;
	case EFFECT_SCROLL_OF_HEALING:
		lpObj->EffectOption.AddHPRecoveryRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[1];
		break;
	case EFFECT_HAWK_FIGURINE:
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[0];
		break;
	case EFFECT_GOAT_FIGURINE:
		lpObj->EffectOption.AddExcellentDamageRate -= lpEffect->m_value[0];
		break;
	case EFFECT_OAK_CHARM:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_MAPLE_CHARM:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_GOLDEN_OAK_CHARM:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxSD -= lpEffect->m_value[1];
		break;
	case EFFECT_GOLDEN_MAPLE_CHARM:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxBP -= lpEffect->m_value[1];
		break;
	case EFFECT_WORN_HORSESHOE:
		lpObj->EffectOption.AddItemDropRate -= lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_IGNORE_DEFENSE_RATE:
		lpObj->EffectOption.AddIgnoreDefenseRate -= lpEffect->m_value[0];
		break;
	case EFFECT_FITNESS:
		lpObj->EffectOption.AddVitality -= lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE:
		lpObj->EffectOption.AddDefenseSuccessRate -= lpEffect->m_value[0];
		break;
	case EFFECT_IRON_DEFENSE:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[1];
		break;
	case EFFECT_GREATER_LIFE_ENHANCED:
		lpObj->EffectOption.MulMaxHP -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		break;
	case EFFECT_GREATER_LIFE_MASTERED:
		lpObj->EffectOption.MulMaxHP -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		lpObj->EffectOption.MulMaxBP -= lpEffect->m_value[2];
		break;
	case EFFECT_DEATH_STAB_ENHANCED:
		break;
	case EFFECT_MAGIC_CIRCLE_IMPROVED:
		lpObj->EffectOption.AddMinMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxMagicDamage -= lpEffect->m_value[1];
		break;
	case EFFECT_MAGIC_CIRCLE_ENHANCED:
		lpObj->EffectOption.AddMinMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxMagicDamage -= lpEffect->m_value[1];
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[2];
		break;
	case EFFECT_MANA_SHIELD_MASTERED:
		lpObj->EffectOption.AddDamageReduction -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		break;
	case EFFECT_FROZEN_STAB_MASTERED:
		lpObj->DelayLevel = 0;
		lpObj->DelayActionTime = 0;
		break;
	case EFFECT_BLESS:
		lpObj->EffectOption.AddStrength -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDexterity -= lpEffect->m_value[0];
		lpObj->EffectOption.AddVitality -= lpEffect->m_value[0];
		lpObj->EffectOption.AddEnergy -= lpEffect->m_value[0];
		gFruit.GCFruitResultSend(lpObj, 18, lpEffect->m_value[0], 7);
		break;
	case EFFECT_INFINITY_ARROW_IMPROVED:
		lpObj->EffectOption.MulPhysiDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_BLIND_IMPROVED:
		lpObj->EffectOption.DivAttackSuccessRate -= lpEffect->m_value[0];
		break;
	case EFFECT_DRAIN_LIFE_ENHANCED:
		break;
	case EFFECT_ICE_STORM_ENHANCED:
		lpObj->DelayLevel = 0;
		lpObj->DelayActionTime = 0;
		break;
	case EFFECT_EARTH_PRISON:
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE_MASTERED:
		lpObj->EffectOption.AddCriticalDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[1];
		break;
	case EFFECT_GREATER_CRITICAL_DAMAGE_EXTENDED:
		lpObj->EffectOption.AddCriticalDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[1];
		lpObj->EffectOption.AddExcellentDamageRate -= lpEffect->m_value[2];
		break;
	case EFFECT_SWORD_POWER_IMPROVED:
		lpObj->EffectOption.AddSwordPowerDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate -= lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP -= lpEffect->m_value[3];
		break;
	case EFFECT_SWORD_POWER_ENHANCED:
		lpObj->EffectOption.AddSwordPowerDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate -= lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP -= lpEffect->m_value[3];
		break;
	case EFFECT_SWORD_POWER_MASTERED:
		lpObj->EffectOption.AddSwordPowerDamageRate -= lpEffect->m_value[0];
		lpObj->EffectOption.MulMaxMP -= lpEffect->m_value[1];
		lpObj->EffectOption.AddSwordPowerDefenseRate -= lpEffect->m_value[2];
		lpObj->EffectOption.DivMaxHP -= lpEffect->m_value[3];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE_IMPROVED:
		lpObj->EffectOption.AddDefenseSuccessRate -= lpEffect->m_value[0];
		break;
	case EFFECT_GREATER_DEFENSE_SUCCESS_RATE_ENHANCED:
		lpObj->EffectOption.AddDefenseSuccessRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[1];
		break;
	case EFFECT_FITNESS_IMPROVED:
		lpObj->EffectOption.AddVitality -= lpEffect->m_value[0];
		break;
	case EFFECT_DRAGON_ROAR_ENHANCED:
		break;
	case EFFECT_CHAIN_DRIVER_ENHANCED:
		break;
	case EFFECT_POISON_ARROW:
		break;
	case EFFECT_POISON_ARROW_IMPROVED:
		break;
	case EFFECT_BLESS_IMPROVED:
		lpObj->EffectOption.AddStrength -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDexterity -= lpEffect->m_value[0];
		lpObj->EffectOption.AddVitality -= lpEffect->m_value[0];
		lpObj->EffectOption.AddEnergy -= lpEffect->m_value[0];
		gFruit.GCFruitResultSend(lpObj, 18, lpEffect->m_value[0], 7);
		break;
	case EFFECT_LESSER_DAMAGE_IMPROVED:
		lpObj->EffectOption.DivPhysiDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicDamage -= lpEffect->m_value[0];
		lpObj->EffectOption.DivCurseDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_LESSER_DEFENSE_IMPROVED:
		lpObj->EffectOption.DivDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_FIRE_SLASH_ENHANCED:
		lpObj->EffectOption.DivDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_IRON_DEFENSE_IMPROVED:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[1];
		break;
	case EFFECT_BLOOD_HOWLING:
		break;
	case EFFECT_BLOOD_HOWLING_IMPROVED:
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_SD:
		lpObj->EffectOption.DivMaxSD -= lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_MP:
		lpObj->EffectOption.DivMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_SPEED:
		lpObj->EffectOption.DivPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.DivMagicSpeed -= lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_HALF_HP:
		lpObj->EffectOption.DivMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_PENTAGRAM_JEWEL_STUN:
		break;
	case EFFECT_PENTAGRAM_JEWEL_SLOW:
		break;
	case EFFECT_TALISMAN_OF_ASCENSION1:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_TALISMAN_OF_ASCENSION2:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_TALISMAN_OF_ASCENSION3:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_SEAL_OF_ASCENSION3:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SEAL_OF_ASCENSION2:
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_BLESSING_OF_LIGHT:
		lpObj->EffectOption.AddExperienceRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMasterExperienceRate -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_DEFENSE:
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_MAGIC_DAMAGE:
		lpObj->EffectOption.AddMagicDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_LIFE:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_MANA:
		lpObj->EffectOption.AddMaxMP -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_DAMAGE:
		lpObj->EffectOption.AddPhysiDamage -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_HEALING:
		lpObj->EffectOption.AddHPRecoveryRate -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[1];
		break;
	case EFFECT_MASTER_SCROLL_OF_BATTLE:
		lpObj->EffectOption.AddCriticalDamageRate -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_STRENGTH:
		lpObj->EffectOption.AddExcellentDamageRate -= lpEffect->m_value[0];
		break;
	case EFFECT_MASTER_SCROLL_OF_QUICK:
		lpObj->EffectOption.AddPhysiSpeed -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMagicSpeed -= lpEffect->m_value[0];
		break;
	case EFFECT_DAME_ITEM_SWICH:
		break;
	case EFFECT_DAME_SET_ITEM_SWICH:
		break;
	case EFFECT_DAME_PENRING_ITEM_SWICH:
		break;
	case EFFECT_DAME_WING_ITEM_SWICH:
		break;
	case EFFECT_DAME_KHIEN_ITEM_SWICH:
		break;
	case EFFECT_GUILD_BUFF:
		lpObj->EffectOption.AddMaxHP -= lpEffect->m_value[0];
		lpObj->EffectOption.AddMaxSD -= lpEffect->m_value[1];
		lpObj->EffectOption.AddDefense -= lpEffect->m_value[2];
		lpObj->EffectOption.AddDamageReduction -= lpEffect->m_value[3];
		break;
	}
	if (lpEffect->m_index == EFFECT_ORDER_OF_PROTECTION || lpEffect->m_index == EFFECT_ORDER_OF_RESTRAINT)
	{
		if (IT_MAP_RANGE(lpObj->Map) != 0)
		{
			gIllusionTemple.GCIllusionTempleDelSkillSend(lpObj->Index, lpEffect->m_value[0]);
		}
	}
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
}
bool CEffectManager::ConvertEffectByte(CEffect* lpEffect, BYTE* lpMsg)
{
	if (lpMsg[0] == 0xFF)
	{
		return 0;
	}
	lpEffect->m_index = lpMsg[0];
	lpEffect->m_count = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg[1], lpMsg[2]), MAKE_NUMBERW(lpMsg[3], lpMsg[4]));
	lpEffect->m_value[0] = MAKE_NUMBERW(lpMsg[5], lpMsg[6]);
	lpEffect->m_value[1] = MAKE_NUMBERW(lpMsg[7], lpMsg[8]);
	lpEffect->m_value[2] = MAKE_NUMBERW(lpMsg[9], lpMsg[10]);
	lpEffect->m_value[3] = MAKE_NUMBERW(lpMsg[11], lpMsg[12]);
	return 1;
}
void CEffectManager::EffectByteConvert(BYTE* lpMsg, CEffect* lpEffect)
{
	if (lpEffect->IsEffect() == 0 || lpEffect->m_save == 0)
	{
		memset(lpMsg, 0xFF, 13);
		return;
	}
	lpMsg[0] = lpEffect->m_index;
	if (lpEffect->m_type == 2)
	{
		lpMsg[1] = SET_NUMBERHB(SET_NUMBERHW(lpEffect->m_time));
		lpMsg[2] = SET_NUMBERLB(SET_NUMBERHW(lpEffect->m_time));
		lpMsg[3] = SET_NUMBERHB(SET_NUMBERLW(lpEffect->m_time));
		lpMsg[4] = SET_NUMBERLB(SET_NUMBERLW(lpEffect->m_time));
	}
	else
	{
		lpMsg[1] = SET_NUMBERHB(SET_NUMBERHW(lpEffect->m_count));
		lpMsg[2] = SET_NUMBERLB(SET_NUMBERHW(lpEffect->m_count));
		lpMsg[3] = SET_NUMBERHB(SET_NUMBERLW(lpEffect->m_count));
		lpMsg[4] = SET_NUMBERLB(SET_NUMBERLW(lpEffect->m_count));
	}
	lpMsg[5] = SET_NUMBERHB(lpEffect->m_value[0]);
	lpMsg[6] = SET_NUMBERLB(lpEffect->m_value[0]);
	lpMsg[7] = SET_NUMBERHB(lpEffect->m_value[1]);
	lpMsg[8] = SET_NUMBERLB(lpEffect->m_value[1]);
	lpMsg[9] = SET_NUMBERHB(lpEffect->m_value[2]);
	lpMsg[10] = SET_NUMBERLB(lpEffect->m_value[2]);
	lpMsg[11] = SET_NUMBERHB(lpEffect->m_value[3]);
	lpMsg[12] = SET_NUMBERLB(lpEffect->m_value[3]);
}
int CEffectManager::GenerateEffectList(LPOBJ lpObj, BYTE* lpMsg, int* size)
{
	int count = 0;
	PMSG_VIEWPORT_STATE info;
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		if (lpObj->Effect[n].IsEffect() != 0)
		{
			info.effect = lpObj->Effect[n].m_index;
			memcpy(&lpMsg[(*size)], &info, sizeof(info));
			(*size) += sizeof(info);
			count++;
		}
	}
	return count;
}
int CEffectManager::GeneratePartyEffectList(LPOBJ lpObj, BYTE* lpMsg, int* size)
{
	int count = 0;
	PMSG_PARTY_EFFECT_LIST info;
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		if (lpObj->Effect[n].IsEffect() != 0)
		{
			info.effect = lpObj->Effect[n].m_index;
			info.count = lpObj->Effect[n].m_count;
			memcpy(&lpMsg[(*size)], &info, sizeof(info));
			(*size) += sizeof(info);
			count++;
		}
	}
	return count;
}
bool CEffectManager::CheckStunEffect(LPOBJ lpObj)
{
	if (this->CheckEffect(lpObj, EFFECT_STERN) != 0 || this->CheckEffect(lpObj, EFFECT_SLEEP) != 0 || this->CheckEffect(lpObj, EFFECT_EARTH_PRISON) != 0 || this->CheckEffect(lpObj, EFFECT_PENTAGRAM_JEWEL_STUN) != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool CEffectManager::CheckImmobilizeEffect(LPOBJ lpObj)
{
	if (this->CheckEffect(lpObj, EFFECT_ICE_ARROW) != 0 || this->CheckEffect(lpObj, EFFECT_ORDER_OF_RESTRAINT) != 0 || this->CheckEffect(lpObj, EFFECT_ICE_STORM_ENHANCED) != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
void CEffectManager::ClearAllEffect(LPOBJ lpObj)
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];
		if (lpEffect->IsEffect() == 0 || lpEffect->m_type != 0)
		{
			continue;
		}
		this->RemoveEffect(lpObj, lpEffect);
		this->GCPeriodicEffectSend(lpObj, 0, lpEffect->m_group, 1, 0, lpEffect->m_index);
		lpEffect->Clear();
	}
}
void CEffectManager::ClearDebuffEffect(LPOBJ lpObj, int count)
{
	for (int n = 0; n < MAX_EFFECT_LIST; n++)
	{
		CEffect* lpEffect = &lpObj->Effect[n];
		if (lpEffect->IsEffect() == 0 || lpEffect->m_type != 0 || lpEffect->m_flag == 0)
		{
			continue;
		}
		if ((--count) >= 0)
		{
			this->RemoveEffect(lpObj, lpEffect);
			this->GCPeriodicEffectSend(lpObj, 0, lpEffect->m_group, 1, 0, lpEffect->m_index);
			lpEffect->Clear();
		}
	}
}
void CEffectManager::PeriodicEffect(LPOBJ lpObj, CEffect* lpEffect)
{
	if (lpEffect->m_index != EFFECT_POISON && lpEffect->m_index != EFFECT_NEIL && lpEffect->m_index != EFFECT_SAHAMUTT && lpEffect->m_index != EFFECT_DEATH_STAB_ENHANCED && lpEffect->m_index != EFFECT_DRAIN_LIFE_ENHANCED && lpEffect->m_index != EFFECT_DRAGON_ROAR_ENHANCED && lpEffect->m_index != EFFECT_CHAIN_DRIVER_ENHANCED && lpEffect->m_index != EFFECT_POISON_ARROW && lpEffect->m_index != EFFECT_POISON_ARROW_IMPROVED)
	{
		return;
	}
	if (lpEffect->m_value[1] == 0 || (lpEffect->m_count % lpEffect->m_value[1]) != 0)
	{
		return;
	}
	if (lpObj->Type == OBJECT_NPC || lpObj->Live == 0 || OBJECT_RANGE(lpEffect->m_value[0]) == 0)
	{
		return;
	}
	LPOBJ lpTarget = &gObj[lpEffect->m_value[0]];
	int damage = 0;
	int shield = 0;
	int absorb = 0;
	int effect = 0;
	switch (lpEffect->m_index)
	{
	case EFFECT_POISON:
		damage = (((damage = (int)((lpObj->Life * lpEffect->m_value[2]) / gServerInfo.SkillDocDW)) > lpEffect->m_value[3]) ? ((lpEffect->m_value[3] == 0) ? damage : lpEffect->m_value[3]) : damage);
		absorb = 0;
		effect = 2;
		break;
	case EFFECT_NEIL:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		absorb = 0;
		effect = 3;
		break;
	case EFFECT_SAHAMUTT:
		damage = (((damage = (int)((lpObj->Life * lpEffect->m_value[2]) / gServerInfo.SkillDocDW)) > lpEffect->m_value[3]) ? ((lpEffect->m_value[3] == 0) ? damage : lpEffect->m_value[3]) : damage);
		absorb = 0;
		effect = 3;
		break;
	case EFFECT_DEATH_STAB_ENHANCED:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		absorb = 0;
		effect = 3;
		break;
	case EFFECT_DRAIN_LIFE_ENHANCED:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		absorb = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		effect = 3;
		break;
	case EFFECT_DRAGON_ROAR_ENHANCED:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		absorb = 0;
		effect = 3;
		break;
	case EFFECT_CHAIN_DRIVER_ENHANCED:
		damage = MAKE_NUMBERDW(lpEffect->m_value[2], lpEffect->m_value[3]);
		absorb = 0;
		effect = 3;
		break;
	case EFFECT_POISON_ARROW:
		damage = (((damage = (int)((lpObj->Life * lpEffect->m_value[2]) / gServerInfo.SkillDocDW)) > lpEffect->m_value[3]) ? ((lpEffect->m_value[3] == 0) ? damage : lpEffect->m_value[3]) : damage);
		absorb = 0;
		effect = 2;
		break;
	case EFFECT_POISON_ARROW_IMPROVED:
		damage = (((damage = (int)((lpObj->Life * lpEffect->m_value[2]) / gServerInfo.SkillDocDW)) > lpEffect->m_value[3]) ? ((lpEffect->m_value[3] == 0) ? damage : lpEffect->m_value[3]) : damage);
		absorb = 0;
		effect = 2;
		break;
	}
	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER && effect == 3)
	{
		shield = gAttack.GetShieldDamage(lpTarget, lpObj, damage);
		if (lpObj->Life < (damage - shield))
		{
			lpObj->Life = 0;
		}
		else
		{
			lpObj->Life -= damage - shield;
		}
		if (lpObj->Shield < shield)
		{
			lpObj->Shield = 0;
		}
		else
		{
			lpObj->Shield -= shield;
		}
	}
	else
	{
		if (lpObj->Life < damage)
		{
			lpObj->Life = 0;
		}
		else
		{
			lpObj->Life -= damage;
		}
	}
	if ((lpTarget->Life + absorb) > (lpTarget->MaxLife + lpTarget->AddLife))
	{
		lpTarget->Life = lpTarget->MaxLife + lpTarget->AddLife;
	}
	else
	{
		lpTarget->Life += absorb;
	}
	if (damage > 0)
	{
		gObjectManager.CharacterLifeCheck(lpTarget, lpObj, (damage - shield), effect, 0, 0, 0, shield);
	}
	if (absorb > 0)
	{
		GCLifeSend(lpTarget->Index, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
	}
}
void CEffectManager::GCEffectStateSend(LPOBJ lpObj, BYTE state, BYTE effect)
{
	PMSG_EFFECT_STATE_SEND pMsg;
	pMsg.header.set(0x07, sizeof(pMsg));
	pMsg.state = state;
	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
	pMsg.effect = effect;
	if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
		this->GC_BuffInfo(lpObj->Index, effect);
	}
	if (CC_MAP_RANGE(lpObj->Map) == 0)
	{
		MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
	}
}
void CEffectManager::GCPeriodicEffectSend(LPOBJ lpObj, WORD group, WORD value, BYTE state, DWORD time, BYTE effect)
{
	PMSG_PERIODIC_EFFECT_SEND pMsg;
	pMsg.header.set(0x2D, sizeof(pMsg));
	pMsg.group = group;
	pMsg.value = value;
	pMsg.state = state;
	pMsg.time = time;
	pMsg.effect = effect;
	if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
		this->GC_BuffInfo(lpObj->Index, effect);
	}
	this->GCEffectStateSend(lpObj, ((state == 0) ? 1 : 0), effect);
}
void CEffectManager::GCPartyEffectListSend(LPOBJ lpObj)
{
#if(GAMESERVER_UPDATE>=802 )
	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		return;
	}
	BYTE send[256];
	PMSG_PARTY_EFFECT_LIST_SEND pMsg;
	pMsg.header.set(0x2E, 0);
	int size = sizeof(pMsg);
	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));
	pMsg.count = this->GeneratePartyEffectList(lpObj, send, &size);
	pMsg.header.size = size;
	memcpy(send, &pMsg, sizeof(pMsg));
	for (int n = 0; n < MAX_PARTY_USER; n++)
	{
		if (OBJECT_RANGE(gParty.m_PartyInfo[lpObj->PartyNumber].Index[n]) != 0)
		{
			DataSend(gParty.m_PartyInfo[lpObj->PartyNumber].Index[n], send, size);
		}
	}
#endif
}
void CEffectManager::GC_BuffInfo(int aIndex, int BuffIndex)
{
	if (BuffIndex == 0)
	{
		return;
	}
	EFFECT_INFO* lpBuffData = 0;
	lpBuffData = this->GetInfo(BuffIndex);
	if (lpBuffData == NULL)
	{
		return;
	}
	if (!OBJECT_RANGE(aIndex))
	{
		return;
	}
	LPOBJ lpUser = &gObj[aIndex];
	if (!lpUser)
	{
		return;
	}
	PMSG_SEND_BUFF_ICON_EX pMsg = { 0 };
	pMsg.h.set(0xF3, 0x26, sizeof(pMsg));
	int n = 0;
	for (int i = 0; i < MAX_EFFECT_LIST; i++)
	{
		if (lpUser->Effect[i].m_index == BuffIndex)
		{
			pMsg.btEffectIndex = lpUser->Effect[i].m_index;
			memcpy(pMsg.szBuffName, lpBuffData->Name, sizeof(lpBuffData->Name));
			n = 0;
			pMsg.btEffectType[n] = lpUser->Effect[i].m_index;
			pMsg.iValue[n] = lpUser->Effect[i].m_value[0];
			n++;
			pMsg.btEffectType[n] = lpUser->Effect[i].m_index;
			pMsg.iValue[n] = lpUser->Effect[i].m_value[1];
			n++;
			pMsg.btEffectType[n] = lpUser->Effect[i].m_index;
			pMsg.iValue[n] = lpUser->Effect[i].m_value[2];
			n++;
			pMsg.btEffectType[n] = lpUser->Effect[i].m_index;
			pMsg.iValue[n] = lpUser->Effect[i].m_value[3];
			n++;
		}
	}
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}
