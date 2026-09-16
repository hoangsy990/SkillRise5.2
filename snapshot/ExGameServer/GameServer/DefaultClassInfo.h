#pragma once
#define MAX_CLASS 8

#define CLASS_DW 0
#define CLASS_DK 1
#define CLASS_FE 2
#define CLASS_MG 3
#define CLASS_DL 4
#define CLASS_SU 5
#define CLASS_RF 6
#define CLASS_GL 7 // S21 CalcCharacter.lua base class; no create/login activation here

#define DB_CLASS_DW		0
#define DB_CLASS_SM		1
#define DB_CLASS_GM		2
#define DB_CLASS_DK		16
#define DB_CLASS_BK		17
#define DB_CLASS_BM		18
#define DB_CLASS_FE		32
#define DB_CLASS_ME		33
#define DB_CLASS_HE		34
#define DB_CLASS_MG		48
#define DB_CLASS_DM		50
#define DB_CLASS_DL		64
#define DB_CLASS_LE		66
#define DB_CLASS_SU		80
#define DB_CLASS_BS		81
#define DB_CLASS_DS		82
#define DB_CLASS_RF		96
#define DB_CLASS_FM		98

struct DEFAULT_CLASS_INFO
{
	int Class;
	int Strength;
	int Dexterity;
	int Vitality;
	int Energy;
	int Leadership;
	float MaxLife;
	float MaxMana;
	float LevelLife;
	float LevelMana;
	float VitalityToLife;
	float EnergyToMana;
};
class CDefaultClassInfo
{
public:
	CDefaultClassInfo();
	void Init();
	void Load(char* path);
	void SetInfo(DEFAULT_CLASS_INFO info);
	int GetCharacterDefaultStat(int Class,int stat);
	DEFAULT_CLASS_INFO m_DefaultClassInfo[MAX_CLASS];
};
extern CDefaultClassInfo gDefaultClassInfo;
