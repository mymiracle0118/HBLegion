#define HITRATIOFACTOR		50.0f
#define MAXSKILLPOINTS		700

bool CheckResistingMagicSuccess(short sAttackerH, short sAttackerType, char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio, char cMagicType = 0);
bool checkResistingPoisonSuccess(short sOwnerH, char cOwnerType);
bool checkResistingIceSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio);
void CalculateSSN_SkillIndex(int iClientH, short sSkillIndex, int iValue);
void CalculateSSN_ItemIndex(int iClientH, short sWeaponIndex, int iValue);
int calculateAttackEffect(short sTargetH, char cTargetType, short sAttackerH, char cAttackerType, int tdX, int tdY, int iAttackMode, bool bNearAttack = false, bool bIsDash = false);
void EnduStrippingDamage(short sTargetH, short sAttackerH, char cAttackerType, int item, int higherStripEndu, bool shield = false);
