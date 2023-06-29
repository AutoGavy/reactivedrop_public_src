#pragma once

#include "vgui_controls/EditablePanel.h"
#include "gameui/swarm/vhybridbutton.h"
#include "rd_hud_glow_helper.h"

class CRD_VGUI_Main_Menu_Top_Bar : public vgui::EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE( CRD_VGUI_Main_Menu_Top_Bar, vgui::EditablePanel );

	CRD_VGUI_Main_Menu_Top_Bar( vgui::Panel *parent, const char *panelName );
	~CRD_VGUI_Main_Menu_Top_Bar();

	void ApplySchemeSettings( vgui::IScheme *pScheme ) override;
	void OnCommand( const char *command ) override;
	void PaintBackground() override;
	void NavigateTo() override;

	void DismissMainMenuScreens();

	vgui::DHANDLE<BaseModUI::BaseModHybridButton> m_hActiveButton;

	BaseModUI::BaseModHybridButton *m_pBtnSettings;
	BaseModUI::BaseModHybridButton *m_pBtnLogo;
	BaseModUI::BaseModHybridButton *m_pTopButton[6];
	BaseModUI::BaseModHybridButton *m_pBtnQuit;

	HUDGlowHelper_t m_GlowSettings;
	HUDGlowHelper_t m_GlowLogo;
	HUDGlowHelper_t m_GlowTopButton[6];
	HUDGlowHelper_t m_GlowQuit;

	uint8_t m_iLeftGlow;
	uint8_t m_iRightGlow;
};