#include "stdafx.h"
#include <algorithm>
#include <vector>
#include <array>
#include "UIControls.h"
#include "NewUISystem.h"
#include "NewUIMuHelper.h"
#include "CharacterManager.h"
#include "MUHelper/MuHelper.h"
#include "SkillManager.h"
#include "RISE/ZzzToolKit.h"
#include "RISE/WideData.h"
#include "wsclientinline.h"

using namespace MUHelper;
extern ConfigData AIConfig;

constexpr int BITMAP_DISTANCE_BEGIN = BITMAP_INTERFACE_CRYWOLF_BEGIN + 33;
constexpr int MAX_NUMBER_DIGITS = 3;
constexpr int MAX_HUNTING_RANGE = 10;
constexpr int MAX_OBTAINING_RANGE = 8;

BYTE GetY = 25;

SEASON3B::CNewUIMuHelper::CNewUIMuHelper()
{
    m_pNewUIMng = NULL;

    m_Pos.x = 0;
    m_Pos.y = 0;

    m_ButtonList.clear();

    m_iCurrentOpenTab = 0;

    m_iSelectedSkillSlot = 0;
    m_aiSelectedSkills.fill(-1);

    m_iCurrentOpenSubWin = 0;
    m_bSubWinOpen = false;

    pWidthForm = 350.f;
    pHeighForm = 190.f;
}

SEASON3B::CNewUIMuHelper::~CNewUIMuHelper()
{
    Release();
}

bool SEASON3B::CNewUIMuHelper::Create(CNewUIManager* pNewUIMng, int x, int y)
{
    if (NULL == pNewUIMng)
        return false;

    m_pNewUIMng = pNewUIMng;
    m_pNewUIMng->AddUIObj(INTERFACE_MUHELPER, this);

    SetPos(x, y);

    LoadImages();

    InitButtons();

    InitCheckBox();

    InitImage();

    InitText();

    InitTextboxInput();

    Show(false);

    return true;
}

void SEASON3B::CNewUIMuHelper::Release()
{
    UnloadImages();

    if (m_pNewUIMng)
    {
        m_pNewUIMng->RemoveUIObj(this);
        m_pNewUIMng = NULL;
    }
}

void SEASON3B::CNewUIMuHelper::SetPos(int x, int y)
{
    m_Pos.x = (x / 2);
    m_Pos.y = (y / 2) - 40;
}

void SEASON3B::CNewUIMuHelper::InitButtons()
{
    std::list<unicode::t_string> ltext;
    ltext.push_back(GlobalText[3500]);
    ltext.push_back(GlobalText[3501]);
    ltext.push_back(GlobalText[3590]);

    m_TabBtn.CreateRadioGroup(2, IMAGE_WINDOW_TAB_BTN, TRUE);
    m_TabBtn.ChangeRadioText(ltext);
    m_TabBtn.ChangeRadioButtonInfo(true, m_Pos.x + 20, m_Pos.y + 34, 56, 22);
    m_TabBtn.ChangeFrame(m_iCurrentOpenTab);

    InsertButton(IMAGE_IGS_BUTTON, m_Pos.x + 268, m_Pos.y + 190, 52, 26, 1, 0, 1, 1, GlobalText[3503], "", BUTTON_ID_SAVE_CONFIG, 0); // Save Button
    InsertButton(IMAGE_IGS_BUTTON, m_Pos.x + 214, m_Pos.y + 190, 52, 26, 1, 0, 1, 1, GlobalText[3504], "", BUTTON_ID_INIT_CONFIG, 0); // Reset Button
    
    InsertButton(IMAGE_CLEARNESS_BTN, m_Pos.x + 274, m_Pos.y + 68  - GetY,  38, 24, 1, 0, 1, 1, GlobalText[3502], "", BUTTON_ID_POTION_CONFIG, 0);           // Setting All Class Open volume HP
    InsertButton(IMAGE_CLEARNESS_BTN, m_Pos.x + 274, m_Pos.y + 68  - GetY,  38, 24, 1, 0, 1, 1, GlobalText[3502], "", BUTTON_ID_POTION_CONFIG_ELF, 0);       // Setting Elf Class Open volume HP
    InsertButton(IMAGE_CLEARNESS_BTN, m_Pos.x + 274, m_Pos.y + 68  - GetY,  38, 24, 1, 0, 1, 1, GlobalText[3502], "", BUTTON_ID_POTION_CONFIG_SUMMY, 0);     // Setting Sum Class Open volume HP
    InsertButton(IMAGE_IGS_BUTTON,    m_Pos.x + 261, m_Pos.y + 152 - GetY, 52, 26, 1, 0, 1, 1, GlobalText[3554], "", BUTTON_ID_PARTY_CONFIG, 0);            // Setting Ho tro buff nhom Party
    InsertButton(IMAGE_IGS_BUTTON,    m_Pos.x + 261, m_Pos.y + 152 - GetY, 52, 26, 1, 0, 1, 1, GlobalText[3554], "", BUTTON_ID_PARTY_CONFIG_ELF, 0);        // Setting Ho tro buff nhom Party

    InsertButton(IMAGE_IGS_BUTTON, m_Pos.x + 250, m_Pos.y + 155, 52, 26, 1, 0, 1, 1, GlobalText[3505], "", BUTTON_ID_ADD_OTHER_ITEM, 1); //-- Add Item list
    InsertButton(IMAGE_IGS_BUTTON, m_Pos.x + 10, m_Pos.y + 225, 52, 26, 1, 0, 1, 1, GlobalText[3506], "", BUTTON_ID_DELETE_OTHER_ITEM, 1); //-- Del Item list
    //--

    RegisterBtnCharacter(0xFF, BUTTON_ID_ADD_OTHER_ITEM);
    RegisterBtnCharacter(0xFF, BUTTON_ID_DELETE_OTHER_ITEM);
    RegisterBtnCharacter(0xFF, BUTTON_ID_SAVE_CONFIG);
    RegisterBtnCharacter(0xFF, BUTTON_ID_INIT_CONFIG);

    RegisterBtnCharacter(DK, BUTTON_ID_POTION_CONFIG);
    RegisterBtnCharacter(DW, BUTTON_ID_POTION_CONFIG);
    RegisterBtnCharacter(DW, BUTTON_ID_PARTY_CONFIG);
    RegisterBtnCharacter(MG, BUTTON_ID_POTION_CONFIG);
    RegisterBtnCharacter(DL, BUTTON_ID_POTION_CONFIG);
    RegisterBtnCharacter(RF, BUTTON_ID_POTION_CONFIG);
    RegisterBtnCharacter(FE, BUTTON_ID_POTION_CONFIG_ELF);
    RegisterBtnCharacter(FE, BUTTON_ID_PARTY_CONFIG_ELF);
    RegisterBtnCharacter(SM, BUTTON_ID_POTION_CONFIG_SUMMY);
}

void SEASON3B::CNewUIMuHelper::InitCheckBox()
{
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 66  - GetY,  15, 15, 0, GlobalText[3507], CHECKBOX_ID_POTION,        0); // Su dung binh Mau
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 82  - GetY,  15, 15, 0, GlobalText[3516], CHECKBOX_ID_AUTO_HEAL,     0); // Tu dong hoi phuc HP
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 82  - GetY,  15, 15, 0, GlobalText[3517], CHECKBOX_ID_DRAIN_LIFE,    0); // Hut Mau
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 98  - GetY,  15, 15, 0, GlobalText[3508], CHECKBOX_ID_LONG_DISTANCE, 0); // Danh tam xa
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 114 - GetY, 15, 15, 0, GlobalText[3509], CHECKBOX_ID_ORIG_POSITION,  0); // Danh xung quanh nguoi
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 147 - GetY, 15, 15, 0, GlobalText[3513], CHECKBOX_ID_BUFF_DURATION,  0); // Tu buff ban than
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 163 - GetY, 15, 15, 0, GlobalText[3515], CHECKBOX_ID_PARTY,          0); // Ho tro buff party
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 163 - GetY, 15, 15, 0, GlobalText[3512], CHECKBOX_ID_COMBO,          0); // Auto Combo
                                                                                                                                     
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 163 - GetY, 15, 15, 0, GlobalText[3514], CHECKBOX_ID_USE_PET,        0); // Auto Attack Pet
    InsertCheckBox(IMAGE_MACROUI_HELPER_OPTIONBUTTON, m_Pos.x + 153,        m_Pos.y + 189 - GetY, 15, 15, 0, GlobalText[3533], CHECKBOX_ID_DR_ATTACK_CEASE,       0);
    InsertCheckBox(IMAGE_MACROUI_HELPER_OPTIONBUTTON, m_Pos.x + 153 + 55,   m_Pos.y + 189 - GetY, 15, 15, 0, GlobalText[3534], CHECKBOX_ID_DR_ATTACK_AUTO,        0);
    InsertCheckBox(IMAGE_MACROUI_HELPER_OPTIONBUTTON, m_Pos.x + 153 + 110,  m_Pos.y + 189 - GetY, 15, 15, 0, GlobalText[3535], CHECKBOX_ID_DR_ATTACK_TOGETHER,    0);

    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 41,  15, 15, 0, GlobalText[3518], CHECKBOX_ID_REPAIR_ITEM,   1); // Tu dong sua do
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 240, m_Pos.y + 41,  15, 15, 0, GlobalText[3877], CHECKBOX_ID_START_OFFLINE, 1); // Train Offline
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 62,  15, 15, 0, GlobalText[3519], CHECKBOX_ID_PICK_ALL,      1); // Nhat tat ca vat pham
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 83,  15, 15, 0, GlobalText[3520], CHECKBOX_ID_PICK_SELECTED, 1); // Nhat tuy chon
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 163, m_Pos.y + 99,  15, 15, 0, GlobalText[3521], CHECKBOX_ID_PICK_JEWEL,    1); // Nhat ngoc, trang suc
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 260, m_Pos.y + 99,  15, 15, 0, GlobalText[3523], CHECKBOX_ID_PICK_ZEN,      1); // Nhat zen
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 163, m_Pos.y + 114, 15, 15, 0, GlobalText[3522], CHECKBOX_ID_PICK_ANCIENT,  1); // Do Than
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 260, m_Pos.y + 114, 15, 15, 0, GlobalText[3524], CHECKBOX_ID_PICK_EXCELLENT,1); // Nhat do hoan hao
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 138, 15, 15, 0, GlobalText[3525], CHECKBOX_ID_ADD_OTHER_ITEM,1); // Nhat them item khac

    //--
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 192, 15, 15, 0, GlobalText[3591], CHECKBOX_ID_AUTO_ACCEPT_FRIEND, 1);
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 208, 15, 15, 0, GlobalText[3592], CHECKBOX_ID_AUTO_ACCEPT_GUILD, 1);
    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 153, m_Pos.y + 224, 15, 15, 0, GlobalText[3593], CHECKBOX_ID_AUTO_DEFEND, 1);

    InsertCheckBox(IMAGE_CHECKBOX_BTN, m_Pos.x + 18, m_Pos.y + 142, 15, 15, 0, "Auto Reset", CHECKBOX_ID_AUTO_RESET, 2);

    RegisterBoxCharacter(0xFF, CHECKBOX_ID_POTION);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_LONG_DISTANCE);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_ORIG_POSITION);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_BUFF_DURATION);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_REPAIR_ITEM);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_START_OFFLINE);

    RegisterBoxCharacter(0xFF, CHECKBOX_ID_PICK_ALL);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_PICK_SELECTED);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_PICK_JEWEL);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_PICK_ANCIENT);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_PICK_ZEN);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_PICK_EXCELLENT);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_ADD_OTHER_ITEM);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_AUTO_ACCEPT_FRIEND);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_AUTO_DEFEND);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_AUTO_ACCEPT_GUILD);
    RegisterBoxCharacter(0xFF, CHECKBOX_ID_AUTO_RESET);

    RegisterBoxCharacter(DK, CHECKBOX_ID_COMBO);

    RegisterBoxCharacter(DW, CHECKBOX_ID_PARTY);

    RegisterBoxCharacter(DL, CHECKBOX_ID_USE_PET);
    RegisterBoxCharacter(DL, CHECKBOX_ID_DR_ATTACK_CEASE);
    RegisterBoxCharacter(DL, CHECKBOX_ID_DR_ATTACK_AUTO);
    RegisterBoxCharacter(DL, CHECKBOX_ID_DR_ATTACK_TOGETHER);

    RegisterBoxCharacter(FE, CHECKBOX_ID_AUTO_HEAL);
    RegisterBoxCharacter(FE, CHECKBOX_ID_PARTY);

    RegisterBoxCharacter(SM, CHECKBOX_ID_DRAIN_LIFE);
}

void SEASON3B::CNewUIMuHelper::InitImage()
{
    InsertIcon(BITMAP_INTERFACE_NEW_SKILLICON_BEGIN + 4, m_Pos.x + 16,  m_Pos.y + 113, 32, 38, SKILL_SLOT_SKILL1, 0);
    InsertIcon(BITMAP_INTERFACE_NEW_SKILLICON_BEGIN + 4, m_Pos.x + 61,  m_Pos.y + 113, 32, 38, SKILL_SLOT_SKILL2, 0);
    InsertIcon(BITMAP_INTERFACE_NEW_SKILLICON_BEGIN + 4, m_Pos.x + 106, m_Pos.y + 113, 32, 38, SKILL_SLOT_SKILL3, 0);

    InsertIcon(BITMAP_INTERFACE_NEW_SKILLICON_BEGIN + 4, m_Pos.x + 16,  m_Pos.y + 173, 32, 38, SKILL_SLOT_BUFF1, 0);
    InsertIcon(BITMAP_INTERFACE_NEW_SKILLICON_BEGIN + 4, m_Pos.x + 61,  m_Pos.y + 173, 32, 38, SKILL_SLOT_BUFF2, 0);
    InsertIcon(BITMAP_INTERFACE_NEW_SKILLICON_BEGIN + 4, m_Pos.x + 106, m_Pos.y + 173, 32, 38, SKILL_SLOT_BUFF3, 0);

    InsertIcon(IMAGE_MACROUI_HELPER_INPUTNUMBER, m_Pos.x + 154, (m_Pos.y + 131) - GetY, 28, 15, TEXTBOX_IMG_DISTANCE_TIME, 0); // InputBox Quay lai vi tri ban dau
    InsertIcon(IMAGE_MACROUI_HELPER_INPUTSTRING, m_Pos.x + 154, m_Pos.y + 160, 93, 15, TEXTBOX_IMG_ADD_EXTRA_ITEM, 1);

    RegisterIconCharacter(0xFF, SKILL_SLOT_SKILL1);
    RegisterIconCharacter(0xFF, SKILL_SLOT_SKILL2);
    RegisterIconCharacter(0xFF, SKILL_SLOT_SKILL3);

    RegisterIconCharacter(DK, SKILL_SLOT_BUFF1);
    RegisterIconCharacter(DK, SKILL_SLOT_BUFF2);
    RegisterIconCharacter(DK, SKILL_SLOT_BUFF3);

    RegisterIconCharacter(DW, SKILL_SLOT_BUFF1);
    RegisterIconCharacter(DW, SKILL_SLOT_BUFF2);
    RegisterIconCharacter(DW, SKILL_SLOT_BUFF3);

    RegisterIconCharacter(FE, SKILL_SLOT_BUFF1);
    RegisterIconCharacter(FE, SKILL_SLOT_BUFF2);
    RegisterIconCharacter(FE, SKILL_SLOT_BUFF3);

    RegisterIconCharacter(SM, SKILL_SLOT_BUFF1);
    RegisterIconCharacter(SM, SKILL_SLOT_BUFF2);
    RegisterIconCharacter(SM, SKILL_SLOT_BUFF3);

    RegisterIconCharacter(DL, SKILL_SLOT_BUFF1);
    RegisterIconCharacter(DL, SKILL_SLOT_BUFF2);
    RegisterIconCharacter(DL, SKILL_SLOT_BUFF3);

    RegisterIconCharacter(RF, SKILL_SLOT_BUFF1);
    RegisterIconCharacter(RF, SKILL_SLOT_BUFF2);
    RegisterIconCharacter(RF, SKILL_SLOT_BUFF3);

    RegisterIconCharacter(0xFF, TEXTBOX_IMG_DISTANCE_TIME);
    RegisterIconCharacter(0xFF, TEXTBOX_IMG_ADD_EXTRA_ITEM);

}

void SEASON3B::CNewUIMuHelper::InitText()
{
    //Tab 0
    InsertText(m_Pos.x + 19, m_Pos.y + 66,   GlobalText[3526], 1, 0); // Pham vi tan cong
    InsertText(m_Pos.x + 183, (m_Pos.y + 134) - GetY, GlobalText[3527], 3, 0); // Thoi gian quay lai vi tri ban dau

    //Tab 1
    InsertText(m_Pos.x + 19, m_Pos.y + 66, GlobalText[3532], 11, 1); // Pham vi nhat do

    RegisterTextCharacter(0xFF, 1);
    RegisterTextCharacter(0xFF, 2);
    RegisterTextCharacter(0xFF, 3);
    RegisterTextCharacter(0xFF, 4);
    RegisterTextCharacter(0xFF, 5);
    RegisterTextCharacter(0xFF, 7);
    RegisterTextCharacter(0xFF, 8);
    RegisterTextCharacter(0xFF, 11);
    RegisterTextCharacter(0xFF, 12);

    RegisterTextCharacter(DK, 9);
    RegisterTextCharacter(DK, 10);
    RegisterTextCharacter(DW, 9);
    RegisterTextCharacter(DW, 10);
    RegisterTextCharacter(FE, 9);
    RegisterTextCharacter(FE, 10);
    RegisterTextCharacter(MG, 9);
    RegisterTextCharacter(MG, 10);
    RegisterTextCharacter(SM, 9);
    RegisterTextCharacter(SM, 10);
    RegisterTextCharacter(RF, 9);
    RegisterTextCharacter(RF, 10);
}

void SEASON3B::CNewUIMuHelper::InitTextboxInput()
{
    char wsInitText[MAX_NUMBER_DIGITS + 1];
    // Thoi gian quay lai vi tri ban dau [InputBox]
    m_DistanceTimeInput.Init(g_hWnd, 17, 15, MAX_NUMBER_DIGITS, false);
    m_DistanceTimeInput.SetPosition(m_Pos.x + 161, (m_Pos.y + 134) - GetY);
    m_DistanceTimeInput.SetTextColor(255, 0, 0, 0);
    m_DistanceTimeInput.SetBackColor(255, 255, 255, 255);
    m_DistanceTimeInput.SetFont(g_hFont);
    m_DistanceTimeInput.SetState(UISTATE_NORMAL);
    m_DistanceTimeInput.SetOption(UIOPTION_NUMBERONLY);
    snprintf(wsInitText, MAX_NUMBER_DIGITS + 1, "%d", AIConfig.iMaxSecondsAway);
    m_DistanceTimeInput.SetText(wsInitText);

    m_ItemInput.Init(g_hWnd, 88, 15, MAX_ITEM_NAME, false);
    m_ItemInput.SetPosition(m_Pos.x + 157, m_Pos.y + 163);
    m_ItemInput.SetTextColor(255, 0, 0, 0);
    m_ItemInput.SetBackColor(255, 255, 255, 255);
    m_ItemInput.SetFont(g_hFont);
    m_ItemInput.SetState(UISTATE_HIDE);

    m_ItemFilter.SetSize(138, 139);
    m_ItemFilter.SetPosition(m_Pos.x + 14, m_Pos.y + 102 + m_ItemFilter.GetHeight());
}

bool SEASON3B::CNewUIMuHelper::Update()
{
    if (IsVisible())
    {
        int iNumCurOpenTab = m_TabBtn.UpdateMouseEvent();

        if (iNumCurOpenTab == RADIOGROUPEVENT_NONE)
            return true;

        m_iCurrentOpenTab = iNumCurOpenTab;

        if (m_iCurrentOpenTab == 0)
        {
            m_DistanceTimeInput.SetState(UISTATE_NORMAL);
            m_ItemInput.SetState(UISTATE_HIDE);

            m_DistanceTimeInput.GiveFocus();
        }
        else if (m_iCurrentOpenTab == 1)
        {
            m_DistanceTimeInput.SetState(UISTATE_HIDE);
            m_ItemInput.SetState(UISTATE_NORMAL);

            m_ItemInput.GiveFocus();
        }
    }
    return true;
}

bool SEASON3B::CNewUIMuHelper::UpdateMouseEvent()
{   
    int iButtonId = UpdateMouseBtnList();
    if (iButtonId != -1)
    {
        g_ConsoleDebug->Write(MCD_NORMAL, "[MU Helper] Clicked [%d]", iButtonId);        

        if (iButtonId == BUTTON_ID_ADD_OTHER_ITEM)
        {
            SaveExtraItem();
        }
        else if (iButtonId == BUTTON_ID_DELETE_OTHER_ITEM)
        {
            RemoveExtraItem();
        }
        else if (iButtonId == BUTTON_ID_POTION_CONFIG_ELF)
        {
            g_pNewUIMuHelperExt->Toggle(SUB_PAGE_POTION_CONFIG_ELF);
        }
        else if (iButtonId == BUTTON_ID_POTION_CONFIG_SUMMY)
        {
            g_pNewUIMuHelperExt->Toggle(SUB_PAGE_POTION_CONFIG_SUMMY);
        }
        else if (iButtonId == BUTTON_ID_POTION_CONFIG)
        {
            g_pNewUIMuHelperExt->Toggle(SUB_PAGE_POTION_CONFIG);
        }
        else if (iButtonId == BUTTON_ID_PARTY_CONFIG)
        {
            g_pNewUIMuHelperExt->Toggle(SUB_PAGE_PARTY_CONFIG);
        }
        else if (iButtonId == BUTTON_ID_PARTY_CONFIG_ELF)
        {
            g_pNewUIMuHelperExt->Toggle(SUB_PAGE_PARTY_CONFIG_ELF);
        }
        else if (iButtonId == BUTTON_ID_INIT_CONFIG)
        {
            InitConfig();
        }
        else if (iButtonId == BUTTON_ID_SAVE_CONFIG)
        {
            GetAllDataAndSend();

            if (m_DistanceTimeInput.HaveFocus())
                SetFocus(g_hWnd);

            if (m_Skill2DelayInput.HaveFocus())
                SetFocus(g_hWnd);

            if (m_Skill3DelayInput.HaveFocus())
                SetFocus(g_hWnd);

            if (m_ItemInput.HaveFocus())
                SetFocus(g_hWnd);

            g_pNewUISystem->Hide(INTERFACE_MUHELPER);
            SetFocus(g_hWnd);
        }

        return false;
    }

    if (m_iCurrentOpenTab == 0 && CheckMouseIn(m_Pos.x + 15, m_Pos.y + 75, 124, 16))
    {
        if (MouseWheel > 0)
        {
            MouseWheel = 0;
            AIConfig.iHuntingRange++;
            if (AIConfig.iHuntingRange > MAX_HUNTING_RANGE)
                AIConfig.iHuntingRange = MAX_HUNTING_RANGE;
        }
        else if (MouseWheel < 0)
        {
            MouseWheel = 0;
            AIConfig.iHuntingRange--;
            if (AIConfig.iHuntingRange < 0)
                AIConfig.iHuntingRange = 0;
        }

        if (IsRepeat(VK_LBUTTON))
        {
            int x = MouseX - (m_Pos.x + 10);
            if (x < 0) AIConfig.iHuntingRange = 0;
            else if (x > 124) AIConfig.iHuntingRange = MAX_HUNTING_RANGE;
            else
            {
                float fValue = ((float)MAX_HUNTING_RANGE * x) / 124.f;
                AIConfig.iHuntingRange = (int)(fValue + 0.5f);
            }
        }
    }

    if (m_iCurrentOpenTab == 1 && CheckMouseIn(m_Pos.x + 15, m_Pos.y + 75, 124, 16))
    {
        if (MouseWheel > 0)
        {
            MouseWheel = 0;
            AIConfig.iObtainingRange++;
            if (AIConfig.iObtainingRange > MAX_HUNTING_RANGE)
                AIConfig.iObtainingRange = MAX_HUNTING_RANGE;
        }
        else if (MouseWheel < 0)
        {
            MouseWheel = 0;
            AIConfig.iObtainingRange--;
            if (AIConfig.iObtainingRange < 0)
                AIConfig.iObtainingRange = 0;
        }

        if (IsRepeat(VK_LBUTTON))
        {
            int x = MouseX - (m_Pos.x + 10);
            if (x < 0) AIConfig.iObtainingRange = 0;
            else if (x > 124) AIConfig.iObtainingRange = MAX_HUNTING_RANGE;
            else
            {
                float fValue = ((float)MAX_HUNTING_RANGE * x) / 124.f;
                AIConfig.iObtainingRange = (int)(fValue + 0.5f);
            }
        }
    }
    int iCheckboxId = UpdateMouseBoxList();
    if (iCheckboxId != -1) //
    {
        auto element = m_CheckBoxList[iCheckboxId];
        auto state = element.box->GetBoxState();
        g_ConsoleDebug->Write(MCD_NORMAL, "[MU Helper] Clicked checkbox [%d] state[%d]", iCheckboxId, state);

        if (iCheckboxId == CHECKBOX_ID_DR_ATTACK_CEASE)
        {
            m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_CEASE].box->RegisterBoxState(true);
            m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_AUTO].box->RegisterBoxState(false);
            m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_TOGETHER].box->RegisterBoxState(false);

        }
        else if (iCheckboxId == CHECKBOX_ID_DR_ATTACK_AUTO)
        {
            m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_AUTO].box->RegisterBoxState(true);
            m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_CEASE].box->RegisterBoxState(false);
            m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_TOGETHER].box->RegisterBoxState(false);
        }
        else if (iCheckboxId == CHECKBOX_ID_DR_ATTACK_TOGETHER)
        {
            m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_TOGETHER].box->RegisterBoxState(true);
            m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_CEASE].box->RegisterBoxState(false);
            m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_AUTO].box->RegisterBoxState(false);
        }

        ApplyConfigFromCheckbox(iCheckboxId, state);

        return false;
    }

    if (IsRelease(VK_LBUTTON))
    {
        int iPrevIndex = m_iSelectedSkillSlot;
        int iIconIndex = UpdateMouseIconList();

        if (iIconIndex != -1 && iIconIndex < MAX_SKILLS_SLOT)
        {
            g_ConsoleDebug->Write(MCD_NORMAL, "[MU Helper] Clicked skill slot [%d]", iIconIndex);
            m_iSelectedSkillSlot = iIconIndex;

            bool bPrevVisible = g_pNewUISystem->IsVisible(INTERFACE_MUHELPER_SKILL_LIST);

            if (iIconIndex == SKILL_SLOT_SKILL1
                || iIconIndex == SKILL_SLOT_SKILL2
                || iIconIndex == SKILL_SLOT_SKILL3)
            {
                if(g_pNewUIMuHelperSkillList)
                g_pNewUIMuHelperSkillList->FilterByAttackSkills();
            }
            else
            {
                if (g_pNewUIMuHelperSkillList)
                g_pNewUIMuHelperSkillList->FilterByBuffSkills();
            }
            if (g_pNewUIMuHelperSkillList)       // thêm check null
            {
                if (iIconIndex == iPrevIndex && bPrevVisible)
                {
                    g_pNewUISystem->Hide(INTERFACE_MUHELPER_SKILL_LIST);
                }
                else
                {
                    g_pNewUISystem->Show(INTERFACE_MUHELPER_SKILL_LIST);
                }
            }

            return false;
        }
        else if (iIconIndex == TEXTBOX_IMG_DISTANCE_TIME)
        {
            m_DistanceTimeInput.GiveFocus();
        }
        else if (iIconIndex == TEXTBOX_IMG_ADD_EXTRA_ITEM)
        {
            m_ItemInput.GiveFocus();
        }
    }
    if (IsRelease(VK_RBUTTON))
    {
        int iSlotIndex = UpdateMouseIconList();
        if (iSlotIndex != -1)
        {
            g_ConsoleDebug->Write(MCD_NORMAL, "[MU Helper] Clicked slot slot [%d]", iSlotIndex);
            m_aiSelectedSkills[iSlotIndex] = -1;

            auto cboxCombo = m_CheckBoxList[CHECKBOX_ID_COMBO];
            if (cboxCombo.box->GetBoxState() == true)
            {
                cboxCombo.box->RegisterBoxState(false);
                AIConfig.bUseCombo = false;
            }

            return false;
        }
    }

    if (m_iCurrentOpenTab == 1)
    {
        m_ItemFilter.DoAction();
    }

    if (SEASON3B::IsRelease(VK_LBUTTON) && CheckMouseIn((m_Pos.x + pWidthForm) - 40, m_Pos.y + 5, 16, 16))
    {
        if (pKit->CheckPickedItem())
        {
            return false;
        }

        if (m_DistanceTimeInput.HaveFocus())
            SetFocus(g_hWnd);

        if (m_Skill2DelayInput.HaveFocus())
            SetFocus(g_hWnd);

        if (m_Skill3DelayInput.HaveFocus())
            SetFocus(g_hWnd);

        if (m_ItemInput.HaveFocus())
            SetFocus(g_hWnd);

        g_pNewUISystem->Hide(SEASON3B::INTERFACE_MUHELPER);
        PlayBuffer(SOUND_CLICK01);
    }
    return false;
}

bool SEASON3B::CNewUIMuHelper::UpdateKeyEvent()
{
    if (IsVisible())
    {
        if (IsPress(VK_ESCAPE) == true)
        {
            if (pKit->CheckPickedItem())
            {
                return false;
            }

            if (m_DistanceTimeInput.HaveFocus())
                SetFocus(g_hWnd);

            if (m_Skill2DelayInput.HaveFocus())
                SetFocus(g_hWnd);

            if (m_Skill3DelayInput.HaveFocus())
                SetFocus(g_hWnd);

            if (m_ItemInput.HaveFocus())
                SetFocus(g_hWnd);

            g_pNewUISystem->Hide(INTERFACE_MUHELPER);
            g_pNewUISystem->Hide(INTERFACE_MUHELPER_SKILL_LIST);
            PlayBuffer(SOUND_CLICK01);
            SetFocus(g_hWnd);

            return false;
        }
    }
    return true;
}

void SEASON3B::CNewUIMuHelper::ApplyConfigFromCheckbox(int iCheckboxId, bool bState)
{
    switch (iCheckboxId) 
    {
    case CHECKBOX_ID_POTION:        AIConfig.bUseHealPotion = bState;        break;

    case CHECKBOX_ID_LONG_DISTANCE:
        AIConfig.bLongRangeCounterAttack = bState;
        break;

    case CHECKBOX_ID_ORIG_POSITION:
        AIConfig.bReturnToOriginalPosition = bState;
        break;

    case CHECKBOX_ID_COMBO:
        auto cboxCombo = m_CheckBoxList[CHECKBOX_ID_COMBO];

        if (bState == true)
        {
            if (m_aiSelectedSkills[0] <= 0 || m_aiSelectedSkills[1] <= 0 || m_aiSelectedSkills[2] <= 0)
            {
                g_pChatListBox->AddText("", GlobalText[3565], SEASON3B::TYPE_ERROR_MESSAGE);
                cboxCombo.box->RegisterBoxState(false);
            }
        }
        
        AIConfig.bUseCombo = cboxCombo.box->GetBoxState();
        break;

    case CHECKBOX_ID_BUFF_DURATION:         AIConfig.bBuffDuration = bState;                 break;

    case CHECKBOX_ID_USE_PET:               AIConfig.bUseDarkRaven = bState;                 break;
    case CHECKBOX_ID_DR_ATTACK_CEASE:       AIConfig.iDarkRavenMode = PET_ATTACK_CEASE;      break;
    case CHECKBOX_ID_DR_ATTACK_AUTO:        AIConfig.iDarkRavenMode = PET_ATTACK_AUTO;       break;
    case CHECKBOX_ID_DR_ATTACK_TOGETHER:    AIConfig.iDarkRavenMode = PET_ATTACK_TOGETHER;   break;

    case CHECKBOX_ID_PARTY:                 AIConfig.bSupportParty = bState;                 break;
    case CHECKBOX_ID_AUTO_HEAL:             AIConfig.bAutoHeal = bState;                     break;
    case CHECKBOX_ID_DRAIN_LIFE:            AIConfig.bUseDrainLife = bState;                 break;
    case CHECKBOX_ID_REPAIR_ITEM:           AIConfig.bRepairItem = bState;                   break;
    case CHECKBOX_ID_START_OFFLINE:         AIConfig.StartOffline = bState;                  break;

    case CHECKBOX_ID_PICK_ALL:
        auto cboxPickSelected = m_CheckBoxList[CHECKBOX_ID_PICK_SELECTED];
        if (cboxPickSelected.box->GetBoxState())
        {
            cboxPickSelected.box->RegisterBoxState(false);
        }
        AIConfig.bPickAllItems = bState;
        break;

    case CHECKBOX_ID_PICK_SELECTED:
        auto cboxPickAll = m_CheckBoxList[CHECKBOX_ID_PICK_ALL];
        if (cboxPickAll.box->GetBoxState())
        {
            cboxPickAll.box->RegisterBoxState(false);
        }
        AIConfig.bPickSelectItems = bState;
        break;

    case CHECKBOX_ID_PICK_JEWEL:        AIConfig.bPickJewel = bState;        break;
    case CHECKBOX_ID_PICK_ANCIENT:      AIConfig.bPickAncient = bState;      break;
    case CHECKBOX_ID_PICK_ZEN:          AIConfig.bPickZen = bState;          break;
    case CHECKBOX_ID_PICK_EXCELLENT:    AIConfig.bPickExcellent = bState;    break;
    case CHECKBOX_ID_ADD_OTHER_ITEM:    AIConfig.bPickExtraItems = bState;   break;
    case CHECKBOX_ID_AUTO_DEFEND:       AIConfig.bUseSelfDefense = bState;   break;
    case CHECKBOX_ID_AUTO_ACCEPT_FRIEND:AIConfig.bAutoAcceptFriend = bState;   break;
    case CHECKBOX_ID_AUTO_ACCEPT_GUILD:AIConfig.bAutoAcceptGuild = bState;   break;

    default:        break;
    }
}

void SEASON3B::CNewUIMuHelper::ApplyConfigFromSkillSlot(int iSlot, int iSkill)
{
    if (iSlot < 3)
    {
        AIConfig.aiSkill[iSlot] = iSkill;
    }
    else
    {
        AIConfig.aiBuff[iSlot - SKILL_SLOT_BUFF1] = iSkill;
    }
}


void SEASON3B::CNewUIMuHelper::SaveExtraItem()
{
    wchar_t wExtraItem[MAX_ITEM_NAME + 1] = { 0 };
    m_ItemInput.GetText(wExtraItem, sizeof(wExtraItem));

    if (wcslen(wExtraItem) == 0)
        return;

    char utf8Item[MAX_ITEM_NAME * 3 + 1] = { 0 };
    int len = WideCharToMultiByte(CP_UTF8, 0, wExtraItem, -1, utf8Item, sizeof(utf8Item), NULL, NULL);

    if (len > 1)
    {
        std::string itemStr(utf8Item);

        m_ItemFilter.AddText(itemStr.c_str());
        m_ItemFilter.Scrolling(-m_ItemFilter.GetBoxSize());

        AIConfig.aExtraItems.insert(itemStr);
    }

    m_ItemInput.SetText("");
}

void SEASON3B::CNewUIMuHelper::RemoveExtraItem()
{
    FILTERLIST_TEXT* pText = m_ItemFilter.GetSelectedText();
    if (pText)
    {
        AIConfig.aExtraItems.erase(std::string(pText->m_szPattern));
        m_ItemFilter.DeleteText(pText->m_szPattern);
    }
}

int SEASON3B::CNewUIMuHelper::GetIntFromTextInput(char* pwsInput)
{
    char* end;

    int value = static_cast<int>(strtol(pwsInput, &end, 10));  // Base 10

    if (*end != L'\0')
    {
        return 0;
    }

    return value;
}

void SEASON3B::CNewUIMuHelper::Reset()
{
    AIConfig.iHuntingRange = MAX_HUNTING_RANGE / 2;

    AIConfig.iMaxSecondsAway = 20;
    AIConfig.bLongRangeCounterAttack = false;
    AIConfig.bReturnToOriginalPosition = false;

    AIConfig.aiSkill.fill(0);
    AIConfig.bUseCombo           = false;

    AIConfig.aiBuff.fill(0);

    AIConfig.bBuffDuration       = false;
    AIConfig.bBuffDurationParty  = false;
    AIConfig.iBuffCastInterval   = 0;

    AIConfig.bAutoHeal           = false;
    AIConfig.iHealThreshold      = 0;
    AIConfig.bUseDrainLife       = false;
    AIConfig.bUseHealPotion      = false;
    AIConfig.iPotionThreshold    = 0;
    AIConfig.bSupportParty       = false;
    AIConfig.bAutoHealParty      = false;
    AIConfig.iHealPartyThreshold = 0;
    AIConfig.iDrainThreshold     = 0;

    AIConfig.bUseDarkRaven       = false;
    AIConfig.iDarkRavenMode      = PET_ATTACK_CEASE;
    AIConfig.bRepairItem         = false;

    AIConfig.iObtainingRange     = MAX_HUNTING_RANGE / 2;
    AIConfig.bPickAllItems       = false;
    AIConfig.bPickSelectItems    = false;
    AIConfig.bPickZen            = false;
    AIConfig.bPickJewel          = false;
    AIConfig.bPickExcellent      = false;
    AIConfig.bPickAncient        = false;
    AIConfig.bPickExtraItems     = false;
    AIConfig.StartOffline        = false;
    AIConfig.bAutoAcceptFriend   = false;
    AIConfig.bAutoAcceptGuild    = false;
    AIConfig.bUseSelfDefense     = false;
    AIConfig.aExtraItems.clear();

    ApplyConfig();
}

void SEASON3B::CNewUIMuHelper::InitAllConfigData(const ConfigData& config)
{
    AIConfig = config;
    ApplyConfig();
}

void SEASON3B::CNewUIMuHelper::ApplyConfig()
{
    g_MuHelper.Load(AIConfig);

    m_aiSelectedSkills[0] = AIConfig.aiSkill[0] ? AIConfig.aiSkill[0] : -1;
    m_aiSelectedSkills[1] = AIConfig.aiSkill[1] ? AIConfig.aiSkill[1] : -1;
    m_aiSelectedSkills[2] = AIConfig.aiSkill[2] ? AIConfig.aiSkill[2] : -1;
    m_aiSelectedSkills[3] = AIConfig.aiBuff[0] ? AIConfig.aiBuff[0] : -1;
    m_aiSelectedSkills[4] = AIConfig.aiBuff[1] ? AIConfig.aiBuff[1] : -1;
    m_aiSelectedSkills[5] = AIConfig.aiBuff[2] ? AIConfig.aiBuff[2] : -1;

    m_CheckBoxList[CHECKBOX_ID_POTION].box->RegisterBoxState(AIConfig.bUseHealPotion);
    m_CheckBoxList[CHECKBOX_ID_AUTO_HEAL].box->RegisterBoxState(AIConfig.bAutoHeal);
    m_CheckBoxList[CHECKBOX_ID_DRAIN_LIFE].box->RegisterBoxState(AIConfig.bUseDrainLife);
    m_CheckBoxList[CHECKBOX_ID_LONG_DISTANCE].box->RegisterBoxState(AIConfig.bLongRangeCounterAttack);
    m_CheckBoxList[CHECKBOX_ID_ORIG_POSITION].box->RegisterBoxState(AIConfig.bReturnToOriginalPosition);
    m_CheckBoxList[CHECKBOX_ID_COMBO].box->RegisterBoxState(AIConfig.bUseCombo);

    char wsTempNum[MAX_NUMBER_DIGITS + 1];
    memset(wsTempNum, 0, sizeof(wsTempNum));
    snprintf(wsTempNum, MAX_NUMBER_DIGITS + 1, "%d", AIConfig.iMaxSecondsAway);
    m_DistanceTimeInput.SetText(wsTempNum);

    m_CheckBoxList[CHECKBOX_ID_BUFF_DURATION].box->RegisterBoxState(AIConfig.bBuffDuration);
    m_CheckBoxList[CHECKBOX_ID_PARTY].box->RegisterBoxState(AIConfig.bSupportParty);

    m_CheckBoxList[CHECKBOX_ID_USE_PET].box->RegisterBoxState(AIConfig.bUseDarkRaven);
    m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_CEASE].box->RegisterBoxState(AIConfig.iDarkRavenMode == PET_ATTACK_CEASE);
    m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_AUTO].box->RegisterBoxState(AIConfig.iDarkRavenMode == PET_ATTACK_AUTO);
    m_CheckBoxList[CHECKBOX_ID_DR_ATTACK_TOGETHER].box->RegisterBoxState(AIConfig.iDarkRavenMode == PET_ATTACK_TOGETHER);

    m_CheckBoxList[CHECKBOX_ID_REPAIR_ITEM].box->RegisterBoxState(AIConfig.bRepairItem);
    m_CheckBoxList[CHECKBOX_ID_START_OFFLINE].box->RegisterBoxState(AIConfig.StartOffline);
    m_CheckBoxList[CHECKBOX_ID_PICK_ALL].box->RegisterBoxState(AIConfig.bPickAllItems);
    m_CheckBoxList[CHECKBOX_ID_PICK_SELECTED].box->RegisterBoxState(AIConfig.bPickSelectItems);
    m_CheckBoxList[CHECKBOX_ID_PICK_JEWEL].box->RegisterBoxState(AIConfig.bPickJewel);
    m_CheckBoxList[CHECKBOX_ID_PICK_ZEN].box->RegisterBoxState(AIConfig.bPickZen);
    m_CheckBoxList[CHECKBOX_ID_PICK_EXCELLENT].box->RegisterBoxState(AIConfig.bPickExcellent);
    m_CheckBoxList[CHECKBOX_ID_PICK_ANCIENT].box->RegisterBoxState(AIConfig.bPickAncient);
    m_CheckBoxList[CHECKBOX_ID_ADD_OTHER_ITEM].box->RegisterBoxState(AIConfig.bPickExtraItems);

    m_CheckBoxList[CHECKBOX_ID_AUTO_ACCEPT_FRIEND].box->RegisterBoxState(AIConfig.bAutoAcceptFriend);
    m_CheckBoxList[CHECKBOX_ID_AUTO_ACCEPT_GUILD].box->RegisterBoxState(AIConfig.bAutoAcceptGuild);
    m_CheckBoxList[CHECKBOX_ID_AUTO_DEFEND].box->RegisterBoxState(AIConfig.bUseSelfDefense);

    m_ItemFilter.Clear();
    for (const auto& item : AIConfig.aExtraItems)
    {
        m_ItemFilter.AddText(item.c_str());
    }
}

void SEASON3B::CNewUIMuHelper::InitConfig()
{
    Reset();

    g_pNewUIMuHelperExt->InitConfig();
}

void SEASON3B::CNewUIMuHelper::SaveConfig()
{
    char wsNumberInput[MAX_NUMBER_DIGITS + 1]{};

    m_DistanceTimeInput.GetText(wsNumberInput, sizeof(wsNumberInput));
    AIConfig.iMaxSecondsAway = GetIntFromTextInput(wsNumberInput);

    AIConfig.aiSkill[0] = m_aiSelectedSkills[0] > 0 ? m_aiSelectedSkills[0] : 0;
    AIConfig.aiSkill[1] = m_aiSelectedSkills[1] > 0 ? m_aiSelectedSkills[1] : 0;
    AIConfig.aiSkill[2] = m_aiSelectedSkills[2] > 0 ? m_aiSelectedSkills[2] : 0;
    AIConfig.aiBuff[0] = m_aiSelectedSkills[3] > 0 ? m_aiSelectedSkills[3] : 0;
    AIConfig.aiBuff[1] = m_aiSelectedSkills[4] > 0 ? m_aiSelectedSkills[4] : 0;
    AIConfig.aiBuff[2] = m_aiSelectedSkills[5] > 0 ? m_aiSelectedSkills[5] : 0;

    this->AI.iDarkRavenMode = AIConfig.iDarkRavenMode;
}

float SEASON3B::CNewUIMuHelper::GetLayerDepth()
{
    return 3.4;
}

float SEASON3B::CNewUIMuHelper::GetKeyEventOrder()
{
    return 3.4;
}

void SEASON3B::CNewUIMuHelper::Show(bool bShow)
{
    CNewUIObj::Show(bShow);

    if (bShow == false)
    {
        if (g_pNewUIMuHelperExt)
            g_pNewUIMuHelperExt->Show(false);

        if (g_pNewUIMuHelperSkillList)
            g_pNewUIMuHelperSkillList->Show(false);
    }

    SetFocus(g_hWnd);
}

bool SEASON3B::CNewUIMuHelper::Render()
{
    EnableAlphaTest();
    glColor4f(1.f, 1.f, 1.f, 1.f);

    DWORD TextColor = g_pRenderText->GetTextColor();
    g_pRenderText->SetFont(g_hFont);
    g_pRenderText->SetTextColor(HEX_COLOR_WHITE);
    g_pRenderText->SetBgColor(0);

    g_pRenderText->SetBgColor(60, 0, 0, 0);
    g_pRenderText->SetTextColor(255, 255, 0, 205);
    unicode::t_char strClassName[128];
    unicode::_sprintf(strClassName, "%s [%s]", GlobalText[3536], Hero->ID);

    g_pUIForm->Form(m_Pos.x, m_Pos.y, pWidthForm, pHeighForm, INTERFACE_MUHELPER, strClassName);

    m_TabBtn.Render();

    if (m_iCurrentOpenTab == 1)
    {
        g_pUIForm->RenderBack(m_Pos.x + 10, m_Pos.y + 60, 136, 40);
        g_pUIForm->RenderBack(m_Pos.x + 10, m_Pos.y + 99, 136, 128);
        g_pUIForm->RenderBack(m_Pos.x + 144, m_Pos.y + 35, 180, 215);

        g_pUIForm->RenderVolumeBar(m_Pos.x + 15, m_Pos.y + 75, 124, 16, AIConfig.iObtainingRange, MAX_HUNTING_RANGE);
        
        RenderNumber(m_Pos.x + 130.8f, m_Pos.y + 65.5f, AIConfig.iObtainingRange, 0.88f);

        RenderImage(CNewUIOptionWindow::IMAGE_OPTION_LINE, m_Pos.x + 148.f, m_Pos.y + 57.f, 154.f, 2.f);
        RenderImage(CNewUIOptionWindow::IMAGE_OPTION_LINE, m_Pos.x + 302.f, m_Pos.y + 57.f, 16.f, 2.f);

        RenderImage(CNewUIOptionWindow::IMAGE_OPTION_LINE, m_Pos.x + 148.f, m_Pos.y + 79.f, 154.f, 2.f);
        RenderImage(CNewUIOptionWindow::IMAGE_OPTION_LINE, m_Pos.x + 302.f, m_Pos.y + 79.f, 16.f, 2.f);

        RenderImage(CNewUIOptionWindow::IMAGE_OPTION_LINE, m_Pos.x + 148.f, m_Pos.y + 131.f, 154.f, 2.f);
        RenderImage(CNewUIOptionWindow::IMAGE_OPTION_LINE, m_Pos.x + 302.f, m_Pos.y + 131.f, 16.f, 2.f);

        RenderImage(CNewUIOptionWindow::IMAGE_OPTION_LINE, m_Pos.x + 148.f, m_Pos.y + 185.f, 154.f, 2.f);
        RenderImage(CNewUIOptionWindow::IMAGE_OPTION_LINE, m_Pos.x + 302.f, m_Pos.y + 185.f, 16.f, 2.f);

        m_ItemFilter.Render();
    }
    else if (m_iCurrentOpenTab == 2)
    {
        g_pUIForm->RenderBack(m_Pos.x + 12, m_Pos.y + 73, 165, 50);
        g_pUIForm->RenderBack(m_Pos.x + 12, m_Pos.y + 120, 165, 222);
    }
    else
    {
        g_pUIForm->RenderBack(m_Pos.x + 10, m_Pos.y + 60, 136, 40);
        g_pUIForm->RenderBack(m_Pos.x + 10, m_Pos.y + 99, 136, 61.5);
        g_pUIForm->RenderBack(m_Pos.x + 10, m_Pos.y + 159, 136, 61.5);
        g_pUIForm->RenderBack(m_Pos.x + 144, m_Pos.y + 35, 180, 155);

        g_pUIForm->RenderVolumeBar(m_Pos.x + 15, m_Pos.y + 75, 124, 16, AIConfig.iHuntingRange, MAX_HUNTING_RANGE);

        RenderNumber(m_Pos.x + 130.8f, m_Pos.y + 65.5f, AIConfig.iHuntingRange, 0.88f);

        if(CheckMouseIn(m_Pos.x + 183, (m_Pos.y + 132) - GetY, 100, 13))
        {
            RenderTipText(m_Pos.x + 110, m_Pos.y + 225, GlobalText[3806]);
        }

        g_pRenderText->SetBgColor(60, 0, 0, 255);
        g_pRenderText->SetTextColor(255, 255, 255, 255);
        g_pRenderText->RenderText(m_Pos.x + 14, m_Pos.y + 101, GlobalText[3875], 126, 0, RT3_SORT_CENTER);

        int Class = gCharacterManager.GetBaseClass(Hero->Class);
        if (Class != CLASS_MAGIC)
        {
            g_pRenderText->RenderText(m_Pos.x + 14, m_Pos.y + 161, GlobalText[3876], 126, 0, RT3_SORT_CENTER);
        }
        else
        {
            g_pRenderText->SetBgColor(60, 0, 0, 0);
            g_pRenderText->RenderText(m_Pos.x + 14, m_Pos.y + 181, "No buff list", 126, 0, RT3_SORT_CENTER);
        }

        RenderImage(CNewUIOptionWindow::IMAGE_OPTION_LINE, m_Pos.x + 148.f, m_Pos.y + 158.f, 154.f, 2.f);
        RenderImage(CNewUIOptionWindow::IMAGE_OPTION_LINE, m_Pos.x + 302.f, m_Pos.y + 158.f, 16.f, 2.f);
    }

    RenderBoxList();
    RenderIconList();
    RenderTextList();
    RenderBtnList();

    if (m_iCurrentOpenTab == 0)
    {
        m_DistanceTimeInput.Render();
    }
    else if (m_iCurrentOpenTab == 1)
    {
        m_ItemInput.Render();
    }

    DisableAlphaBlend();

    return true;
}

void SEASON3B::CNewUIMuHelper::LoadImages()
{
    LoadBitmapFile("Interface\\MacroUI\\MacroUI_RangeMinus.tga", IMAGE_MACROUI_HELPER_RAGEMINUS, GL_LINEAR, GL_CLAMP, 1, 0);
    LoadBitmapFile("Interface\\MacroUI\\MacroUI_OptionButton.tga", IMAGE_MACROUI_HELPER_OPTIONBUTTON, GL_LINEAR, GL_CLAMP, 1, 0);
    LoadBitmapFile("Interface\\MacroUI\\MacroUI_InputNumber.tga", IMAGE_MACROUI_HELPER_INPUTNUMBER, GL_LINEAR, GL_CLAMP, 1, 0);
    LoadBitmapFile("Interface\\MacroUI\\MacroUI_InputString.tga", IMAGE_MACROUI_HELPER_INPUTSTRING, GL_LINEAR, GL_CLAMP, 1, 0);
    //--
    LoadBitmapFile("Interface\\InGameShop\\Ingame_Bt03.tga", IMAGE_IGS_BUTTON, GL_LINEAR, GL_CLAMP, 1, 0);
}

void SEASON3B::CNewUIMuHelper::UnloadImages()
{
    DeleteBitmap(IMAGE_MACROUI_HELPER_RAGEMINUS);
    DeleteBitmap(IMAGE_MACROUI_HELPER_OPTIONBUTTON);
    DeleteBitmap(IMAGE_MACROUI_HELPER_INPUTNUMBER);
    DeleteBitmap(IMAGE_MACROUI_HELPER_INPUTSTRING);
    //--
    DeleteBitmap(IMAGE_IGS_BUTTON);
}

void SEASON3B::CNewUIMuHelper::RegisterButton(int Identifier, CButtonTap button)
{
    m_ButtonList.insert(std::pair<int, CButtonTap>(Identifier, button));
}

void SEASON3B::CNewUIMuHelper::RegisterBtnCharacter(BYTE class_character, int Identifier)
{
    auto li = m_ButtonList.find(Identifier);

    if (li != m_ButtonList.end())
    {
        CButtonTap* cBTN = &li->second;
        if (class_character >= 0 && class_character < MAX_CLASS)
        {
            cBTN->class_character[class_character] = TRUE;
        }
        else
        {
            memset(cBTN->class_character, 1, sizeof(cBTN->class_character));
        }
    }
}

void SEASON3B::CNewUIMuHelper::InsertButton(int imgindex, int x, int y, int sx, int sy, bool overflg, bool isimgwidth, bool bClickEffect, bool MoveTxt, std::string btname, std::string tooltiptext, int Identifier, int iNumTab)
{
    CButtonTap cBTN;
    auto* button = new CNewUIButton();

    button->ChangeButtonImgState(1, imgindex, overflg, isimgwidth, bClickEffect);
    button->ChangeButtonInfo(x, y, sx, sy);

    button->ChangeText(btname);
    button->ChangeToolTipText(tooltiptext, TRUE);

    if (MoveTxt)
    {
        button->MoveTextPos(0, -1);
    }

    cBTN.btn = button;
    cBTN.iNumTab = iNumTab;
    memset(cBTN.class_character, 0, sizeof(cBTN.class_character));

    RegisterButton(Identifier, cBTN);
}

void SEASON3B::CNewUIMuHelper::RenderBtnList()
{
    auto li = m_ButtonList.begin();

    for (; li != m_ButtonList.end(); li++)
    {
        CButtonTap* cBTN = &li->second;

        if ((cBTN->class_character[gCharacterManager.GetBaseClass(Hero->Class)]) && (cBTN->iNumTab == m_iCurrentOpenTab || cBTN->iNumTab == -1))
        {
            cBTN->btn->Render();
        }
    }
}

int SEASON3B::CNewUIMuHelper::UpdateMouseBtnList()
{
    auto li = m_ButtonList.begin();

    for (; li != m_ButtonList.end(); li++)
    {
        CButtonTap* cBTN = &li->second;

        if ((cBTN->class_character[gCharacterManager.GetBaseClass(Hero->Class)]) && (cBTN->iNumTab == m_iCurrentOpenTab || cBTN->iNumTab == -1))
        {
            if (cBTN->btn->UpdateMouseEvent())
            {
                return li->first;
            }
        }
    }
    return -1;
}

void SEASON3B::CNewUIMuHelper::RegisterBoxCharacter(BYTE class_character, int Identifier)
{
    auto li = m_CheckBoxList.find(Identifier);

    if (li != m_CheckBoxList.end())
    {
        CheckBoxTap* cBOX = &li->second;

        if (class_character >= 0 && class_character < MAX_CLASS)
        {
            cBOX->class_character[class_character] = TRUE;
        }
        else
        {
            memset(cBOX->class_character, 1, sizeof(cBOX->class_character));
        }
    }
}

void SEASON3B::CNewUIMuHelper::RegisterCheckBox(int Identifier, CheckBoxTap button)
{
    m_CheckBoxList.insert(std::pair<int, CheckBoxTap>(Identifier, button));
}

void SEASON3B::CNewUIMuHelper::InsertCheckBox(int imgindex, int x, int y, int sx, int sy, bool overflg, std::string btname, int Identifier, int iNumTab)
{
    CheckBoxTap cBOX;

    auto* cbox = new CNewUICheckBox;

    cbox->CheckBoxImgState(imgindex);
    cbox->CheckBoxInfo(x, y, sx, sy);

    cbox->ChangeText(btname);
    cbox->RegisterBoxState(overflg);

    cBOX.box = cbox;
    cBOX.iNumTab = iNumTab;
    memset(cBOX.class_character, 0, sizeof(cBOX.class_character));

    RegisterCheckBox(Identifier, cBOX);
}

void SEASON3B::CNewUIMuHelper::RenderBoxList()
{
    auto li = m_CheckBoxList.begin();

    for (; li != m_CheckBoxList.end(); li++)
    {
        CheckBoxTap* cBOX = &li->second;

        if ((cBOX->class_character[gCharacterManager.GetBaseClass(Hero->Class)]) && (cBOX->iNumTab == m_iCurrentOpenTab || cBOX->iNumTab == -1))
        {
            cBOX->box->Render();
        }
    }
}

int SEASON3B::CNewUIMuHelper::UpdateMouseBoxList()
{
    auto li = m_CheckBoxList.begin();

    for (; li != m_CheckBoxList.end(); li++)
    {
        CheckBoxTap* cBOX = &li->second;

        if ((cBOX->class_character[gCharacterManager.GetBaseClass(Hero->Class)]) && (cBOX->iNumTab == m_iCurrentOpenTab || cBOX->iNumTab == -1))
        {
            if (cBOX->box->UpdateMouseEvent())
            {
                return li->first;
            }
        }
    }
    return -1;
}

void SEASON3B::CNewUIMuHelper::RenderIconList()
{
    auto li = m_IconList.begin();

    for (; li != m_IconList.end(); li++)
    {
        cTexture* cImage = &li->second;

        if ((cImage->class_character[gCharacterManager.GetBaseClass(Hero->Class)]) && (cImage->iNumTab == m_iCurrentOpenTab || cImage->iNumTab == -1))
        {
            RenderImage(cImage->s_ImgIndex, cImage->m_Pos.x, cImage->m_Pos.y, cImage->m_Size.x, cImage->m_Size.y);

            if (li->first < MAX_SKILLS_SLOT)
            {
                if (m_aiSelectedSkills[li->first] >= 0 && m_aiSelectedSkills[li->first] < MAX_SKILL_TYPES)
                {
                    g_pSkillList->RenderSkillIcon(m_aiSelectedSkills[li->first], cImage->m_Pos.x + 6, cImage->m_Pos.y + 6, 20, 28, 1);
                }
            }
        }
    }
}

int SEASON3B::CNewUIMuHelper::UpdateMouseIconList()
{
    auto li = m_IconList.begin();

    for (; li != m_IconList.end(); li++)
    {
        cTexture* cImage = &li->second;

        if ((cImage->class_character[gCharacterManager.GetBaseClass(Hero->Class)]) && (cImage->iNumTab == m_iCurrentOpenTab || cImage->iNumTab == -1))
        {
            if (CheckMouseIn(cImage->m_Pos.x, cImage->m_Pos.y, cImage->m_Size.x, cImage->m_Size.y))
            {
                return li->first;
            }
        }
    }

    return -1;
}

void SEASON3B::CNewUIMuHelper::RegisterIconCharacter(BYTE class_character, int Identifier)
{
    auto li = m_IconList.find(Identifier);

    if (li != m_IconList.end())
    {
        cTexture* cImage = &li->second;

        if (class_character >= 0 && class_character < MAX_CLASS)
        {
            cImage->class_character[class_character] = TRUE;
        }
        else
        {
            memset(cImage->class_character, 1, sizeof(cImage->class_character));
        }
    }
}

void SEASON3B::CNewUIMuHelper::RegisterIcon(int Identifier, cTexture button)
{
    m_IconList.insert(std::pair<int, cTexture>(Identifier, button));
}

void SEASON3B::CNewUIMuHelper::InsertIcon(int imgindex, int x, int y, int sx, int sy, int Identifier, int iNumTab)
{
    cTexture cImage;

    cImage.s_ImgIndex = imgindex;
    cImage.m_Pos.x = x;
    cImage.m_Pos.y = y;
    cImage.m_Size.x = sx;
    cImage.m_Size.y = sy;
    cImage.iNumTab = iNumTab;

    memset(cImage.class_character, 0, sizeof(cImage.class_character));

    RegisterIcon(Identifier, cImage);
}

void SEASON3B::CNewUIMuHelper::RenderTextList()
{
    auto li = m_TextNameList.begin();

    for (; li != m_TextNameList.end(); li++)
    {
        cTextName* cImage = &li->second;

        if ((cImage->class_character[gCharacterManager.GetBaseClass(Hero->Class)]) && (cImage->iNumTab == m_iCurrentOpenTab || cImage->iNumTab == -1))
        {
            g_pRenderText->RenderText(cImage->m_Pos.x, cImage->m_Pos.y, cImage->m_Name.c_str());
        }
    }
}

void SEASON3B::CNewUIMuHelper::RegisterTextCharacter(BYTE class_character, int Identifier)
{
    auto li = m_TextNameList.find(Identifier);

    if (li != m_TextNameList.end())
    {
        cTextName* cImage = &li->second;

        if (class_character >= 0 && class_character < MAX_CLASS)
        {
            cImage->class_character[class_character] = TRUE;
        }
        else
        {
            memset(cImage->class_character, 1, sizeof(cImage->class_character));
        }
    }
}

void SEASON3B::CNewUIMuHelper::RegisterText(int Identifier, cTextName button)
{
    m_TextNameList.insert(std::pair<int, cTextName>(Identifier, button));
}

void SEASON3B::CNewUIMuHelper::InsertText(int x, int y, std::string Name, int Identifier, int iNumTab)
{
    cTextName cText;

    cText.m_Pos.x = x;
    cText.m_Pos.y = y;
    cText.m_Name = Name;
    cText.iNumTab = iNumTab;

    memset(cText.class_character, 0, sizeof(cText.class_character));
    RegisterText(Identifier, cText);
}

void SEASON3B::CNewUIMuHelper::AssignSkill(int iSkill)
{
    if (m_iSelectedSkillSlot != -1 && m_iSelectedSkillSlot < MAX_SKILLS_SLOT)
    {
        if (!IsSkillAssigned(iSkill))
        {
            m_aiSelectedSkills[m_iSelectedSkillSlot] = iSkill;
            ApplyConfigFromSkillSlot(m_iSelectedSkillSlot, iSkill);

            g_ConsoleDebug->Write(MCD_NORMAL, "[MU Helper] Assign m_aiSelectedSkills[%d] = %d", m_iSelectedSkillSlot, iSkill);
        }
        else
        {
            int iPrevIndex = GetSkillIndex(iSkill);
            m_aiSelectedSkills[iPrevIndex] = -1;
            m_aiSelectedSkills[m_iSelectedSkillSlot] = iSkill;

            auto cboxCombo = m_CheckBoxList[CHECKBOX_ID_COMBO];
            if (cboxCombo.box->GetBoxState() == true)
            {
                cboxCombo.box->RegisterBoxState(false);
                AIConfig.bUseCombo = false;
            }
        }
    }
}

bool SEASON3B::CNewUIMuHelper::IsSkillAssigned(int iSkill)
{
    return std::find(m_aiSelectedSkills.begin(), m_aiSelectedSkills.end(), iSkill) != m_aiSelectedSkills.end();
}

int SEASON3B::CNewUIMuHelper::GetSkillIndex(int iSkill)
{
    auto it = std::find(m_aiSelectedSkills.begin(), m_aiSelectedSkills.end(), iSkill);

    if (it != m_aiSelectedSkills.end()) {
        return std::distance(m_aiSelectedSkills.begin(), it);
    }

    return -1;
}

void SEASON3B::CNewUIMuHelper::AutoReset()
{
    SystemDataSendTwo(0xFD, 0x32);
}

void SEASON3B::CNewUIMuHelper::LoadConfig(BYTE* Data)
{
    ::memset(&this->AI, 0, sizeof(AI));

    const bool empty = Data == NULL ||
        std::all_of(Data, Data + sizeof(MuHelperDataPack), [](BYTE b) { return b == 0; }) ||
        std::all_of(Data, Data + sizeof(MuHelperDataPack), [](BYTE b) { return b == 0xFF; });
    if (empty)
    {
        this->AI.Range[0] = MAX_HUNTING_RANGE / 2;
        this->AI.Range[1] = MAX_HUNTING_RANGE / 2;
        this->AI.iMaxSecondsAway = 20;
    }
    else
    {
        MuHelperDataPack* AI = (MuHelperDataPack*)Data;
        this->AI.Range[0]                   = AI->Range & 0x0F;
        this->AI.Range[1]                   = (AI->Range >> 4) & 0x0F;
        this->AI.iMaxSecondsAway            = AI->iMaxSecondsAway;
        this->AI.aiSkill[0]                 = AI->aiSkill[0];
        this->AI.aiSkill[1]                 = AI->aiSkill[1];
        this->AI.aiSkill[2]                 = AI->aiSkill[2];

        this->AI.aiBuff[0]                  = AI->aiBuff[0];
        this->AI.aiBuff[1]                  = AI->aiBuff[1];
        this->AI.aiBuff[2]                  = AI->aiBuff[2];

        this->AI.iPotionThreshold           = 10 * (AI->PercentFlag & 0x0F);
        this->AI.iHealThreshold             = 10 * ((AI->PercentFlag >> 4) & 0x0F);
        this->AI.iDrainThreshold            = 10 * ((AI->PercentFlag >> 8) & 0x0F);
        this->AI.iHealPartyThreshold        = 10 * ((AI->PercentFlag >> 12) & 0x0F);

        this->AI.bPickJewel                 = (AI->PickFlag >> 3) & 1;
        this->AI.bPickExcellent             = (AI->PickFlag >> 4) & 1;
        this->AI.bPickAncient               = (AI->PickFlag >> 5) & 1;
        this->AI.bPickZen                   = (AI->PickFlag >> 6) & 1;
        this->AI.bPickExtraItems            = (AI->PickFlag >> 7) & 1;

        this->AI.bUseHealPotion             = (AI->OptionFlag & 1);
        this->AI.bAutoHeal                  = (AI->OptionFlag >> 1) & 1;
        this->AI.bUseDrainLife              = (AI->OptionFlag >> 2) & 1;
        this->AI.bLongRangeCounterAttack    = (AI->OptionFlag >> 3) & 1;
        this->AI.bReturnToOriginalPosition  = (AI->OptionFlag >> 4) & 1;
        this->AI.bUseCombo                  = (AI->OptionFlag >> 5) & 1;
        this->AI.bSupportParty              = (AI->OptionFlag >> 6) & 1;
        this->AI.bAutoHealParty             = (AI->OptionFlag >> 7) & 1;
        this->AI.bBuffDurationParty         = (AI->OptionFlag >> 8) & 1;
        this->AI.bUseDarkRaven              = (AI->OptionFlag >> 9) & 1;
        this->AI.bBuffDuration              = (AI->OptionFlag >> 10) & 1;

        this->AI.bRepairItem                = (AI->OptionFlag >> 21) & 1;
        this->AI.bPickAllItems              = (AI->OptionFlag >> 22) & 1;
        this->AI.bPickSelectItems           = (AI->OptionFlag >> 23) & 1;
        this->AI.iDarkRavenMode             = (AI->OptionFlag >> 25) & 3;
        this->AI.bAutoAcceptFriend          = (AI->OptionFlag >> 26) & 1;
        this->AI.bAutoAcceptGuild           = (AI->OptionFlag >> 27) & 1;
        this->AI.bUseSelfDefense            = (AI->OptionFlag >> 30) & 1;

        this->AI.iBuffCastInterval          = AI->iBuffCastInterval;

        memset(this->AI.aExtraItems, 0, sizeof(this->AI.aExtraItems));
        int CountItemList = 0;
        for (int n = 0; n < 12; n++)
        {
            memcpy(this->AI.aExtraItems[n], (AI->aExtraItems[n]), sizeof(this->AI.aExtraItems[n]));
            if (strlen(this->AI.aExtraItems[n]) > 1) CountItemList++;
        }

    }
}

MuHelperDataPack ConvertToHelperInfo(const MUHelper::ConfigData& AI)
{
    MuHelperDataPack pAI{};
    pAI.PickFlag = 0;

    pAI.Range                = (AI.iHuntingRange & 0x0F) | ((AI.iObtainingRange & 0x0F) << 4);

    pAI.iMaxSecondsAway      = static_cast<WORD>(AI.iMaxSecondsAway);
    pAI.aiSkill[0]           = static_cast<WORD>(AI.aiSkill[0]);
    pAI.aiSkill[1]           = static_cast<WORD>(AI.aiSkill[1]);
    pAI.aiSkill[2]           = static_cast<WORD>(AI.aiSkill[2]);

    pAI.aiBuff[0]            = static_cast<WORD>(AI.aiBuff[0]);
    pAI.aiBuff[1]            = static_cast<WORD>(AI.aiBuff[1]);
    pAI.aiBuff[2]            = static_cast<WORD>(AI.aiBuff[2]);
    pAI.iBuffCastInterval    = static_cast<WORD>(AI.iBuffCastInterval);

    pAI.PercentFlag          = (AI.iPotionThreshold / 10)
                            | ((AI.iHealThreshold / 10) << 4)
                            | ((AI.iDrainThreshold / 10) << 8)
                            | ((AI.iHealPartyThreshold / 10) << 12);
    pAI.PickFlag            |= (AI.bPickJewel & 1) << 3;
    pAI.PickFlag            |= (AI.bPickExcellent & 1) << 4;
    pAI.PickFlag            |= (AI.bPickAncient & 1) << 5;
    pAI.PickFlag            |= (AI.bPickZen & 1) << 6;
    pAI.PickFlag            |= (AI.bPickExtraItems & 1) << 7;
    pAI.OptionFlag           = 0;
    pAI.OptionFlag          |= AI.bUseHealPotion & 1;
    pAI.OptionFlag          |= (AI.bAutoHeal & 1) << 1;
    pAI.OptionFlag          |= (AI.bUseDrainLife & 1) << 2;
    pAI.OptionFlag          |= (AI.bLongRangeCounterAttack & 1) << 3;
    pAI.OptionFlag          |= (AI.bReturnToOriginalPosition & 1) << 4;
    pAI.OptionFlag          |= (AI.bUseCombo & 1) << 5;
    pAI.OptionFlag          |= (AI.bSupportParty & 1) << 6;
    pAI.OptionFlag          |= (AI.bAutoHealParty & 1) << 7;
    pAI.OptionFlag          |= (AI.bBuffDurationParty & 1) << 8;
    pAI.OptionFlag          |= (AI.bUseDarkRaven & 1) << 9;
    pAI.OptionFlag          |= (AI.bBuffDuration & 1) << 10;
                            
    pAI.OptionFlag          |= (AI.bRepairItem & 1) << 21;
    pAI.OptionFlag          |= (AI.bPickAllItems & 1) << 22;
    pAI.OptionFlag          |= (AI.bPickSelectItems & 1) << 23;
    pAI.OptionFlag          |= (AI.iDarkRavenMode & 3) << 25;
    pAI.OptionFlag          |= (AI.bAutoAcceptFriend & 1) << 26;
    pAI.OptionFlag          |= (AI.bAutoAcceptGuild & 1) << 27;
    pAI.OptionFlag          |= (AI.bUseSelfDefense & 1) << 30;

    memset(pAI.aExtraItems, 0, sizeof(pAI.aExtraItems));

    int idx = 0;
    for (const auto& s : AI.aExtraItems)
    {
        if (idx >= 12) break;
        strncpy(pAI.aExtraItems[idx], s.c_str(), 15);
        pAI.aExtraItems[idx][15] = '\0';
        ++idx;
    }
    return pAI;
}

void SEASON3B::CNewUIMuHelper::GetAllDataAndSend()
{
    this->SaveConfig();
    this->ApplyConfig();

    MuHelperDataPack AIPack = ConvertToHelperInfo(AIConfig);

    CGSaveDataMuHelper((BYTE*)&AIPack);
}
