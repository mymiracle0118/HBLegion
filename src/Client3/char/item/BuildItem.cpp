// BuildItem.cpp: implementation of the CBuildItem class.
//
//////////////////////////////////////////////////////////////////////

#include "BuildItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuildItem::CBuildItem()
{
 int i;

	//ZeroMemory(m_cName, sizeof(m_cName));
	m_sItemID = -1;

	m_iSkillLimit = 0;

	for (i = 0; i < 6; i++) {
		m_iMaterialItemID[i] = NULL;
		m_iMaterialItemCount[i] = NULL;
		//m_iMaterialItemValue[i] = NULL;
		m_iIndex[i] = -1;
		m_bMaterialItemFlag[i] = 0;
	}

	//m_iMaxValue     = 0;
	//m_iAverageValue = 0;
	m_iMaxSkill     = 0;
	m_wAttribute    = 0;

	m_bBuildEnabled = FALSE;

	/*ZeroMemory(m_cName, sizeof(m_cName));

	m_bBuildEnabled = FALSE;
	m_iSkillLimit   = 0;
	m_iMaxSkill     = 0;

	m_iSprH     = 0;
	m_iSprFrame = 0;

	ZeroMemory(m_cElementName1, sizeof(m_cElementName1));
	ZeroMemory(m_cElementName2, sizeof(m_cElementName2));
	ZeroMemory(m_cElementName3, sizeof(m_cElementName3));
	ZeroMemory(m_cElementName4, sizeof(m_cElementName4));
	ZeroMemory(m_cElementName5, sizeof(m_cElementName5));
	ZeroMemory(m_cElementName6, sizeof(m_cElementName6));
	
	for (i = 0; i < 7; i++) {
		m_iElementCount[i] = 0;
		m_bElementFlag[i]  = 0;
	}*/
}

CBuildItem::~CBuildItem()
{

}
