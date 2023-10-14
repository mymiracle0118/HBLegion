// BuildItem.h: interface for the CBuildItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUILDITEM_H__497DD176_DB22_4A58_9572_6E95E3BFFBAA__INCLUDED_)
#define AFX_BUILDITEM_H__497DD176_DB22_4A58_9572_6E95E3BFFBAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class CBuildItem  
{
public:
	CBuildItem();
	virtual ~CBuildItem();

	//char  m_cName[21];
	short m_sItemID;

	int  m_iSkillLimit;

	int  m_iMaterialItemID[6];
	int  m_iMaterialItemCount[6];
	//int  m_iMaterialItemValue[6];	
	BOOL m_bMaterialItemFlag[6];		
	int  m_iIndex[6];

	//int	 m_iMaxValue;
	//int  m_iAverageValue;
	int  m_iMaxSkill;						
	WORD  m_wAttribute;

	BOOL m_bBuildEnabled;

	/*BOOL m_bBuildEnabled;
	char m_cName[21];
	int	 m_iSkillLimit;
	int  m_iMaxSkill;
	int  m_iSprH, m_iSprFrame;	
	char m_cElementName1[21], m_cElementName2[21], m_cElementName3[21]; 
	char m_cElementName4[21], m_cElementName5[21], m_cElementName6[21];*/
};

#endif // !defined(AFX_BUILDITEM_H__497DD176_DB22_4A58_9572_6E95E3BFFBAA__INCLUDED_)
