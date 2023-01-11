#include "cbase.h"
#include "vdemos.h"
#include "nb_header_footer.h"
#include "vfooterpanel.h"
#include "vgenericpanellist.h"
#include "rd_demo_utils.h"
#include "rd_missions_shared.h"
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui_controls/ImagePanel.h"
#include "filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace BaseModUI;

class DemoInfoPanel final : public vgui::EditablePanel, public IGenericPanelListItem
{
	DECLARE_CLASS_SIMPLE( DemoInfoPanel, vgui::EditablePanel );
public:
	DemoInfoPanel( vgui::Panel *parent, const char *panelName ) :
		BaseClass( parent, panelName )
	{
		SetProportional( true );

		m_szFileName[0] = '\0';
		m_bWatchable = false;
		m_bCurrentlySelected = false;

		m_LblName = new vgui::Label( this, "LblName", "" );
		m_LblName->SetMouseInputEnabled( false );
		m_LblFileSize = new vgui::Label( this, "LblFileSize", "" );
		m_LblFileSize->SetMouseInputEnabled( false );
		m_LblError = new vgui::Label( this, "LblError", "" );
		m_LblError->SetMouseInputEnabled( false );
		m_LblDuration = new vgui::Label( this, "LblDuration", "" );
		m_LblDuration->SetMouseInputEnabled( false );
		m_LblRecordedBy = new vgui::Label( this, "LblRecordedBy", "" );
		m_LblRecordedBy->SetMouseInputEnabled( false );
		m_LblMissionName = new vgui::Label( this, "LblMissionName", "" );
		m_LblMissionName->SetMouseInputEnabled( false );
		m_ImgMissionIcon = new vgui::ImagePanel( this, "ImgMissionIcon" );
		m_ImgMissionIcon->SetMouseInputEnabled( false );
	}

	bool IsLabel() override { return false; }

	void ApplySchemeSettings( vgui::IScheme *pScheme ) override
	{
		BaseClass::ApplySchemeSettings( pScheme );

		static KeyValues *s_pPreloadedDemoInfoItemLayout = NULL;

#ifdef _DEBUG
		if ( s_pPreloadedDemoInfoItemLayout )
		{
			s_pPreloadedDemoInfoItemLayout->deleteThis();
			s_pPreloadedDemoInfoItemLayout = NULL;
		}
#endif

		if ( !s_pPreloadedDemoInfoItemLayout )
		{
			const char *pszResource = "Resource/UI/BaseModUI/DemoListItem.res";
			s_pPreloadedDemoInfoItemLayout = new KeyValues( pszResource );
			s_pPreloadedDemoInfoItemLayout->LoadFromFile( g_pFullFileSystem, pszResource );
		}

		LoadControlSettings( "", NULL, s_pPreloadedDemoInfoItemLayout );
	}

	void OnMousePressed( vgui::MouseCode code ) override
	{
		if ( MOUSE_LEFT == code )
		{
			GenericPanelList *pGenericList = dynamic_cast< GenericPanelList * >( GetParent()->GetParent() );

			unsigned short nindex;
			if ( pGenericList && pGenericList->GetPanelItemIndex( this, nindex ) )
			{
				pGenericList->SelectPanelItem( nindex, GenericPanelList::SD_DOWN );
			}
		}

		BaseClass::OnMousePressed( code );
	}

	void OnMessage( const KeyValues *params, vgui::VPANEL ifromPanel ) override
	{
		BaseClass::OnMessage( params, ifromPanel );

		if ( !V_strcmp( params->GetName(), "PanelSelected" ) )
		{
			m_bCurrentlySelected = true;
		}
		else if ( !V_strcmp( params->GetName(), "PanelUnSelected" ) )
		{
			m_bCurrentlySelected = false;
		}
	}

	void Paint() override
	{
		BaseClass::Paint();

		// Draw the graded outline for the selected item only
		if ( m_bCurrentlySelected )
		{
			int nPanelWide, nPanelTall;
			GetSize( nPanelWide, nPanelTall );

			vgui::surface()->DrawSetColor( Color( 169, 213, 255, 128 ) );
			// Top lines
			vgui::surface()->DrawFilledRectFade( 0, 0, 0.5f * nPanelWide, 2, 0, 255, true );
			vgui::surface()->DrawFilledRectFade( 0.5f * nPanelWide, 0, nPanelWide, 2, 255, 0, true );

			// Bottom lines
			vgui::surface()->DrawFilledRectFade( 0, nPanelTall - 2, 0.5f * nPanelWide, nPanelTall, 0, 255, true );
			vgui::surface()->DrawFilledRectFade( 0.5f * nPanelWide, nPanelTall - 2, nPanelWide, nPanelTall, 255, 0, true );

			// Text Blotch
			int nTextWide, nTextTall, nNameX, nNameY, nNameWide, nNameTall;
			wchar_t wszName[MAX_PATH];

			m_LblName->GetPos( nNameX, nNameY );
			m_LblName->GetSize( nNameWide, nNameTall );
			m_LblName->GetText( wszName, sizeof( wszName ) );
			vgui::surface()->GetTextSize( m_LblName->GetFont(), wszName, nTextWide, nTextTall );
			int nBlotchWide = nNameX + nTextWide + vgui::scheme()->GetProportionalScaledValueEx( GetScheme(), 75 );

			vgui::surface()->DrawFilledRectFade( 0, 2, 0.50f * nBlotchWide, nPanelTall - 2, 0, 50, true );
			vgui::surface()->DrawFilledRectFade( 0.50f * nBlotchWide, 2, nBlotchWide, nPanelTall - 2, 50, 0, true );
		}
	}

	void PerformLayout() override
	{
		BaseClass::PerformLayout();

		int w, t;
		m_LblMissionName->GetContentSize( w, t );
		int x, y;
		m_LblMissionName->GetPos( x, y );
		y += m_LblMissionName->GetTall() - t;
		m_LblMissionName->SetPos( x, y );
		m_LblMissionName->SetTall( t );
	}

	void SetDemoInfo( const RD_Demo_Info_t &info )
	{
		V_strncpy( m_szFileName, info.szFileName, sizeof( m_szFileName ) );

		wchar_t buf[255];

		V_UTF8ToUnicode( info.szFileName, buf, sizeof( buf ) );
		m_LblName->SetText( buf );

		wchar_t wszSize[64];
		V_snwprintf( wszSize, NELEMS( wszSize ), L"%.1f", info.nFileSize / 1024.0f / 1024.0f );
		g_pVGuiLocalize->ConstructString( buf, sizeof( buf ),
			g_pVGuiLocalize->Find( "#rd_demo_size" ), 1, wszSize );
		m_LblFileSize->SetText( buf );

		if ( info.wszCantWatchReason[0] != L'\0' )
		{
			m_bWatchable = false;
			m_LblError->SetText( info.wszCantWatchReason );

			m_LblName->SetAlpha( 128 );
			m_LblFileSize->SetAlpha( 128 );
		}
		else
		{
			m_bWatchable = true;
			V_snwprintf( buf, NELEMS( buf ), L"%d:%06.3f", Floor2Int( info.Header.playback_time / 60 ), fmodf( info.Header.playback_time, 60 ) );
			m_LblDuration->SetText( buf );
			V_UTF8ToUnicode( info.Header.clientname, buf, sizeof( buf ) );
			m_LblRecordedBy->SetText( buf );

			m_LblName->SetAlpha( 255 );
			m_LblFileSize->SetAlpha( 255 );
		}

		m_LblMissionName->SetText( info.pMission ? STRING( info.pMission->MissionTitle ) : info.Header.mapname );
		m_ImgMissionIcon->SetImage( info.pMission ? STRING( info.pMission->Image ) : "swarm/missionpics/unknownmissionpic" );
	}

	char m_szFileName[MAX_PATH];
	bool m_bWatchable;
	bool m_bCurrentlySelected;

	vgui::Label *m_LblName;
	vgui::Label *m_LblFileSize;
	vgui::Label *m_LblError;
	vgui::Label *m_LblDuration;
	vgui::Label *m_LblRecordedBy;
	vgui::Label *m_LblMissionName;
	vgui::ImagePanel *m_ImgMissionIcon;
};

Demos::Demos( vgui::Panel *parent, const char *panelName ) :
	BaseClass( parent, panelName, false, true )
{
	GameUI().PreventEngineHideGameUI();

	SetDeleteSelfOnClose( true );
	SetProportional( true );

	m_pHeaderFooter = new CNB_Header_Footer( this, "HeaderFooter" );
	m_pHeaderFooter->SetTitle( "" );
	m_pHeaderFooter->SetHeaderEnabled( false );
	m_pHeaderFooter->SetGradientBarEnabled( true );
	m_pHeaderFooter->SetGradientBarPos( 75, 350 );

	m_LblNoRecordings = new vgui::Label( this, "LblNoRecordings", "#rd_demo_list_empty" );

	m_GplRecordingList = new GenericPanelList( this, "GplRecordingList", GenericPanelList::ISM_PERITEM );
	m_GplRecordingList->SetScrollBarVisible( true );

	SetLowerGarnishEnabled( true );
	m_ActiveControl = m_GplRecordingList;

	LoadControlSettings( "Resource/UI/BaseModUI/demos.res" );
}

Demos::~Demos()
{
	GameUI().AllowEngineHideGameUI();
}

void Demos::Activate()
{
	BaseClass::Activate();

	MakeReadyForUse();

	CUtlVector<RD_Demo_Info_t> DemoList;
	g_RD_Auto_Record_System.ReadDemoList( DemoList );

	m_GplRecordingList->RemoveAllPanelItems();
	FOR_EACH_VEC( DemoList, i )
	{
		m_GplRecordingList->AddPanelItem<DemoInfoPanel>( "DemoListItem" )->SetDemoInfo( DemoList[i] );
	}

	if ( DemoList.Count() > 0 )
	{
		m_GplRecordingList->NavigateTo();
		m_LblNoRecordings->SetVisible( false );
	}
	else
	{
		m_LblNoRecordings->SetVisible( true );
	}

	m_GplRecordingList->InvalidateLayout();
}

void Demos::OnCommand( const char *command )
{
	if ( FStrEq( command, "Back" ) )
	{
		// Act as though 360 back button was pressed
		OnKeyCodePressed( KEY_XBUTTON_B );
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

void Demos::OnMessage( const KeyValues *params, vgui::VPANEL ifromPanel )
{
	BaseClass::OnMessage( params, ifromPanel );

	if ( !V_strcmp( params->GetName(), "OnItemSelected" ) )
	{
		int index = const_cast< KeyValues * >( params )->GetInt( "index" );

		// TODO
	}
}

void Demos::OnThink()
{
	BaseClass::OnThink();
}

void Demos::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	SetupAsDialogStyle();
}

void Demos::PerformLayout()
{
	BaseClass::PerformLayout();

	SetBounds( 0, 0, ScreenWidth(), ScreenHeight() );
}

CON_COMMAND_F( rd_auto_record_ui, "Displays demo list.", FCVAR_NOT_CONNECTED )
{
	CBaseModPanel::GetSingleton().OpenWindow( WT_DEMOS, CBaseModPanel::GetSingleton().GetWindow( WT_MAINMENU ) );
}