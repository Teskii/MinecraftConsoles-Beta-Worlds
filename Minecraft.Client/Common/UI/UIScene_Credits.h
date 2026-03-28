#pragma once

#include "UIScene.h"

class UIScene_Credits : public UIScene
{
private:
	enum ECreditIcons
	{
		eCreditIcon_Iggy,
		eCreditIcon_Miles,
		eCreditIcon_Dolby,
	};

	static SCreditTextItemDef gs_aCreditDefs[];
	static int GetBaseCreditTextDefCount();
	
	int		m_iCurrDefIndex;		// Index of last created text def.
	int		m_iNumTextDefs;			// Total number of text defs in the credits.

	bool m_bAddNextLabel;

	IggyName m_funcSetNextLabel, m_funcAddImage;
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_NAME(m_funcSetNextLabel, L"SetNextLabel")
		UI_MAP_NAME(m_funcAddImage, L"AddImage")
	UI_END_MAP_ELEMENTS_AND_NAMES()
public:
	UIScene_Credits(int iPad, void *initData, UILayer *parentLayer);

	virtual EUIScene getSceneType() { return eUIScene_Credits;}
	
	virtual void updateTooltips();
	virtual void updateComponents();

	void handleReload();

	virtual void tick();

protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

	virtual void handleRequestMoreData(F64 startIndex, bool up);

private:
	void setNextLabel(const wstring &label, ECreditTextTypes size);
	void addImage(ECreditIcons icon);
};
