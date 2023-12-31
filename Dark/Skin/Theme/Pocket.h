﻿#pragma once

namespace fgo::dark::skin::theme
{
	enum MY_STATICPARTS {
		STAT_ETCHEDEDGE = 2,
	};

	enum MY_EDITTEXTSTATES {
		ETS_STATICTEXT = 9,
	};

	enum THEMES {
		THEME_MENU,
		THEME_SCROLLBAR,
		THEME_STATIC,
		THEME_BUTTON,
		THEME_EDIT,
		THEME_LISTBOX,
		THEME_COMBOBOX,
		THEME_TOOLTIP,
		THEME_TRACKBAR,
		THEME_SPIN,
		THEME_HEADER,
		THEME_LISTVIEW,
		THEME_TREEVIEW,
		THEME_TOOLBAR,
		THEME_TAB,
		THEME_WINDOW,
		THEME_DIALOG,
		THEME_CTLCOLOR,
		THEME_EXEDIT,
		THEME_MAXSIZE,
	};

	enum CUSTOM_PARTS {
		CTLCOLORDLG = 51,
		CTLCOLORMSGBOX,
		CTLCOLORBTN,
		CTLCOLORSTATIC,
		CTLCOLOREDIT,
		CTLCOLORLISTBOX,
		CTLCOLORSCROLLBAR,
		WINDOW_WINDOWFACE,
		WINDOW_DIALOGFACE,
		WINDOW_WINDOWEDGE,
		WINDOW_CLIENTEDGE,
		WINDOW_STATICEDGE,
		WINDOW_BORDER,
		WINDOW_RAISEDOUTEREDGE,
		WINDOW_SUNKENOUTEREDGE,
		WINDOW_RAISEDEDGE,
		WINDOW_SUNKENEDGE,
		WINDOW_BUMPEDGE,
		WINDOW_ETCHEDEDGE,
		EXEDIT_ROOT,
		EXEDIT_LAYER,
		EXEDIT_LAYEREDGE,
		EXEDIT_LONGSCALE,
		EXEDIT_SHORTSCALE,
		EXEDIT_LAYERBACKGROUND,
		EXEDIT_GROUPBACKGROUND,
		EXEDIT_SELECTION,
		EXEDIT_SELECTIONEDGE,
		EXEDIT_SELECTIONBK,
		EXEDIT_LAYERLEFT,
		EXEDIT_LAYERTOP,
		EXEDIT_LAYERRIGHT,
		EXEDIT_LAYERBOTTOM,
		EXEDIT_LAYERSEPARATOR,
	};

	enum CTLCOLOR_STATES {
		CTLCOLOR_NORMAL = 1,
		CTLCOLOR_DISABLED = 2,
		CTLCOLOR_READONLY = 3,
		CTLCOLOR_STATICTEXT = 4,
	};

	enum EXEDIT_LAYER_STATES {
		EXEDIT_LAYER_ACTIVE = 1,
		EXEDIT_LAYER_INACTIVE = 2,
	};

	enum EXEDIT_LAYERBACKGROUND_STATES {
		EXEDIT_LAYERBACKGROUND_ACTIVE = 1,
		EXEDIT_LAYERBACKGROUND_INACTIVE = 2,
	};

	enum EXEDIT_GROUPBACKGROUND_STATES {
		EXEDIT_GROUPBACKGROUND_ACTIVE_1 = 1,
		EXEDIT_GROUPBACKGROUND_ACTIVE_2 = 2,
		EXEDIT_GROUPBACKGROUND_ACTIVE_3 = 3,
		EXEDIT_GROUPBACKGROUND_INACTIVE_1 = 4,
		EXEDIT_GROUPBACKGROUND_INACTIVE_2 = 5,
		EXEDIT_GROUPBACKGROUND_INACTIVE_3 = 6,
	};

	inline struct Pocket
	{
#define IF_RETURN(name) if (::lstrcmpiW(id, L#name) == 0) return name
		inline static int getPartId(LPCWSTR id)
		{
			// Window

			IF_RETURN(WP_CAPTION);
			IF_RETURN(WP_SMALLCAPTION);
			IF_RETURN(WP_MINCAPTION);
			IF_RETURN(WP_SMALLMINCAPTION);
			IF_RETURN(WP_MAXCAPTION);
			IF_RETURN(WP_SMALLMAXCAPTION);
			IF_RETURN(WP_FRAMELEFT);
			IF_RETURN(WP_FRAMERIGHT);
			IF_RETURN(WP_FRAMEBOTTOM);
			IF_RETURN(WP_SMALLFRAMELEFT);
			IF_RETURN(WP_SMALLFRAMERIGHT);
			IF_RETURN(WP_SMALLFRAMEBOTTOM);
			IF_RETURN(WP_SYSBUTTON);
			IF_RETURN(WP_MDISYSBUTTON);
			IF_RETURN(WP_MINBUTTON);
			IF_RETURN(WP_MDIMINBUTTON);
			IF_RETURN(WP_MAXBUTTON);
			IF_RETURN(WP_CLOSEBUTTON);
			IF_RETURN(WP_SMALLCLOSEBUTTON);
			IF_RETURN(WP_MDICLOSEBUTTON);
			IF_RETURN(WP_RESTOREBUTTON);
			IF_RETURN(WP_MDIRESTOREBUTTON);
			IF_RETURN(WP_HELPBUTTON);
			IF_RETURN(WP_MDIHELPBUTTON);
			IF_RETURN(WP_HORZSCROLL);
			IF_RETURN(WP_HORZTHUMB);
			IF_RETURN(WP_VERTSCROLL);
			IF_RETURN(WP_VERTTHUMB);
			IF_RETURN(WP_DIALOG);
			IF_RETURN(WP_CAPTIONSIZINGTEMPLATE);
			IF_RETURN(WP_SMALLCAPTIONSIZINGTEMPLATE);
			IF_RETURN(WP_FRAMELEFTSIZINGTEMPLATE);
			IF_RETURN(WP_SMALLFRAMELEFTSIZINGTEMPLATE);
			IF_RETURN(WP_FRAMERIGHTSIZINGTEMPLATE);
			IF_RETURN(WP_SMALLFRAMERIGHTSIZINGTEMPLATE);
			IF_RETURN(WP_FRAMEBOTTOMSIZINGTEMPLATE);
			IF_RETURN(WP_SMALLFRAMEBOTTOMSIZINGTEMPLATE);
			IF_RETURN(WP_FRAME);
			IF_RETURN(WP_BORDER);

			IF_RETURN(WINDOW_WINDOWFACE);
			IF_RETURN(WINDOW_DIALOGFACE);
			IF_RETURN(WINDOW_WINDOWEDGE);
			IF_RETURN(WINDOW_CLIENTEDGE);
			IF_RETURN(WINDOW_STATICEDGE);
			IF_RETURN(WINDOW_BORDER);
			IF_RETURN(WINDOW_RAISEDOUTEREDGE);
			IF_RETURN(WINDOW_SUNKENOUTEREDGE);
			IF_RETURN(WINDOW_RAISEDEDGE);
			IF_RETURN(WINDOW_SUNKENEDGE);
			IF_RETURN(WINDOW_BUMPEDGE);
			IF_RETURN(WINDOW_ETCHEDEDGE);

			// CtlColor

			IF_RETURN(CTLCOLORDLG);
			IF_RETURN(CTLCOLORMSGBOX);
			IF_RETURN(CTLCOLORBTN);
			IF_RETURN(CTLCOLORSTATIC);
			IF_RETURN(CTLCOLOREDIT);
			IF_RETURN(CTLCOLORLISTBOX);
			IF_RETURN(CTLCOLORSCROLLBAR);

			// ExEdit

			IF_RETURN(EXEDIT_ROOT);
			IF_RETURN(EXEDIT_LAYER);
			IF_RETURN(EXEDIT_LAYEREDGE);
			IF_RETURN(EXEDIT_LONGSCALE);
			IF_RETURN(EXEDIT_SHORTSCALE);
			IF_RETURN(EXEDIT_LAYERBACKGROUND);
			IF_RETURN(EXEDIT_GROUPBACKGROUND);
			IF_RETURN(EXEDIT_SELECTION);
			IF_RETURN(EXEDIT_SELECTIONEDGE);
			IF_RETURN(EXEDIT_SELECTIONBK);
			IF_RETURN(EXEDIT_LAYERLEFT);
			IF_RETURN(EXEDIT_LAYERTOP);
			IF_RETURN(EXEDIT_LAYERRIGHT);
			IF_RETURN(EXEDIT_LAYERBOTTOM);
			IF_RETURN(EXEDIT_LAYERSEPARATOR);

			// Menu

			IF_RETURN(MENU_MENUITEM_TMSCHEMA);
			IF_RETURN(MENU_MENUDROPDOWN_TMSCHEMA);
			IF_RETURN(MENU_MENUBARITEM_TMSCHEMA);
			IF_RETURN(MENU_MENUBARDROPDOWN_TMSCHEMA);
			IF_RETURN(MENU_CHEVRON_TMSCHEMA);
			IF_RETURN(MENU_SEPARATOR_TMSCHEMA);
			IF_RETURN(MENU_BARBACKGROUND);
			IF_RETURN(MENU_BARITEM);
			IF_RETURN(MENU_POPUPBACKGROUND);
			IF_RETURN(MENU_POPUPBORDERS);
			IF_RETURN(MENU_POPUPCHECK);
			IF_RETURN(MENU_POPUPCHECKBACKGROUND);
			IF_RETURN(MENU_POPUPGUTTER);
			IF_RETURN(MENU_POPUPITEM);
			IF_RETURN(MENU_POPUPSEPARATOR);
			IF_RETURN(MENU_POPUPSUBMENU);
			IF_RETURN(MENU_SYSTEMCLOSE);
			IF_RETURN(MENU_SYSTEMMAXIMIZE);
			IF_RETURN(MENU_SYSTEMMINIMIZE);
			IF_RETURN(MENU_SYSTEMRESTORE);

			// ScrollBar

			IF_RETURN(SBP_ARROWBTN);
			IF_RETURN(SBP_THUMBBTNHORZ);
			IF_RETURN(SBP_THUMBBTNVERT);
			IF_RETURN(SBP_LOWERTRACKHORZ);
			IF_RETURN(SBP_UPPERTRACKHORZ);
			IF_RETURN(SBP_LOWERTRACKVERT);
			IF_RETURN(SBP_UPPERTRACKVERT);
			IF_RETURN(SBP_GRIPPERHORZ);
			IF_RETURN(SBP_GRIPPERVERT);
			IF_RETURN(SBP_SIZEBOX);
			IF_RETURN(SBP_SIZEBOXBKGND);

			// Static

			IF_RETURN(STAT_TEXT);
			IF_RETURN(STAT_ETCHEDEDGE);

			// Button

			IF_RETURN(BP_PUSHBUTTON);
			IF_RETURN(BP_RADIOBUTTON);
			IF_RETURN(BP_CHECKBOX);
			IF_RETURN(BP_GROUPBOX);
			IF_RETURN(BP_USERBUTTON);
			IF_RETURN(BP_COMMANDLINK);
			IF_RETURN(BP_COMMANDLINKGLYPH);
			IF_RETURN(BP_RADIOBUTTON_HCDISABLED);
			IF_RETURN(BP_CHECKBOX_HCDISABLED);
			IF_RETURN(BP_GROUPBOX_HCDISABLED);
			IF_RETURN(BP_PUSHBUTTONDROPDOWN);

			// Edit

			IF_RETURN(EP_EDITTEXT);
			IF_RETURN(EP_CARET);
			IF_RETURN(EP_BACKGROUND);
			IF_RETURN(EP_PASSWORD);
			IF_RETURN(EP_BACKGROUNDWITHBORDER);
			IF_RETURN(EP_EDITBORDER_NOSCROLL);
			IF_RETURN(EP_EDITBORDER_HSCROLL);
			IF_RETURN(EP_EDITBORDER_VSCROLL);
			IF_RETURN(EP_EDITBORDER_HVSCROLL);

			// ListBox

			IF_RETURN(LBCP_BORDER_HSCROLL);
			IF_RETURN(LBCP_BORDER_HVSCROLL);
			IF_RETURN(LBCP_BORDER_NOSCROLL);
			IF_RETURN(LBCP_BORDER_VSCROLL);
			IF_RETURN(LBCP_ITEM);

			// ComboBox

			IF_RETURN(CP_DROPDOWNBUTTON);
			IF_RETURN(CP_BACKGROUND);
			IF_RETURN(CP_TRANSPARENTBACKGROUND);
			IF_RETURN(CP_BORDER);
			IF_RETURN(CP_READONLY);
			IF_RETURN(CP_DROPDOWNBUTTONRIGHT);
			IF_RETURN(CP_DROPDOWNBUTTONLEFT);
			IF_RETURN(CP_CUEBANNER);
			IF_RETURN(CP_DROPDOWNITEM);

			// ToolTip

			IF_RETURN(TTP_STANDARD);
			IF_RETURN(TTP_STANDARDTITLE);
			IF_RETURN(TTP_BALLOON);
			IF_RETURN(TTP_BALLOONTITLE);
			IF_RETURN(TTP_CLOSE);
			IF_RETURN(TTP_BALLOONSTEM);
			IF_RETURN(TTP_WRENCH);

			// TrackBar

			IF_RETURN(TKP_TRACK);
			IF_RETURN(TKP_TRACKVERT);
			IF_RETURN(TKP_THUMB);
			IF_RETURN(TKP_THUMBBOTTOM);
			IF_RETURN(TKP_THUMBTOP);
			IF_RETURN(TKP_THUMBVERT);
			IF_RETURN(TKP_THUMBLEFT);
			IF_RETURN(TKP_THUMBRIGHT);
			IF_RETURN(TKP_TICS);
			IF_RETURN(TKP_TICSVERT);

			// Spin

			IF_RETURN(SPNP_UP);
			IF_RETURN(SPNP_DOWN);
			IF_RETURN(SPNP_UPHORZ);
			IF_RETURN(SPNP_DOWNHORZ);

			// Header

			IF_RETURN(HP_HEADERITEM);
			IF_RETURN(HP_HEADERITEMLEFT);
			IF_RETURN(HP_HEADERITEMRIGHT);
			IF_RETURN(HP_HEADERSORTARROW);
			IF_RETURN(HP_HEADERDROPDOWN);
			IF_RETURN(HP_HEADERDROPDOWNFILTER);
			IF_RETURN(HP_HEADEROVERFLOW);

			// ListView

			IF_RETURN(LVP_LISTITEM);
			IF_RETURN(LVP_LISTGROUP);
			IF_RETURN(LVP_LISTDETAIL);
			IF_RETURN(LVP_LISTSORTEDDETAIL);
			IF_RETURN(LVP_EMPTYTEXT);
			IF_RETURN(LVP_GROUPHEADER);
			IF_RETURN(LVP_GROUPHEADERLINE);
			IF_RETURN(LVP_EXPANDBUTTON);
			IF_RETURN(LVP_COLLAPSEBUTTON);
			IF_RETURN(LVP_COLUMNDETAIL);

			// TreeView

			IF_RETURN(TVP_TREEITEM);
			IF_RETURN(TVP_GLYPH);
			IF_RETURN(TVP_BRANCH);
			IF_RETURN(TVP_HOTGLYPH);

			// ToolBar

			IF_RETURN(TP_BUTTON);
			IF_RETURN(TP_DROPDOWNBUTTON);
			IF_RETURN(TP_SPLITBUTTON);
			IF_RETURN(TP_SPLITBUTTONDROPDOWN);
			IF_RETURN(TP_SEPARATOR);
			IF_RETURN(TP_SEPARATORVERT);
			IF_RETURN(TP_DROPDOWNBUTTONGLYPH);

			// Tab

			IF_RETURN(TABP_TABITEM);
			IF_RETURN(TABP_TABITEMLEFTEDGE);
			IF_RETURN(TABP_TABITEMRIGHTEDGE);
			IF_RETURN(TABP_TABITEMBOTHEDGE);
			IF_RETURN(TABP_TOPTABITEM);
			IF_RETURN(TABP_TOPTABITEMLEFTEDGE);
			IF_RETURN(TABP_TOPTABITEMRIGHTEDGE);
			IF_RETURN(TABP_TOPTABITEMBOTHEDGE);
			IF_RETURN(TABP_PANE);
			IF_RETURN(TABP_BODY);
			IF_RETURN(TABP_AEROWIZARDBODY);

			return wcstol(id, 0, 0);
		}

		inline static int getStateId(LPCWSTR id)
		{
			// Window

			IF_RETURN(FS_ACTIVE);
			IF_RETURN(FS_INACTIVE);

			IF_RETURN(CS_ACTIVE);
			IF_RETURN(CS_INACTIVE);
			IF_RETURN(CS_DISABLED);

			IF_RETURN(MXCS_ACTIVE);
			IF_RETURN(MXCS_INACTIVE);
			IF_RETURN(MXCS_DISABLED);

			IF_RETURN(MNCS_ACTIVE);
			IF_RETURN(MNCS_INACTIVE);
			IF_RETURN(MNCS_DISABLED);

			IF_RETURN(HSS_NORMAL);
			IF_RETURN(HSS_HOT);
			IF_RETURN(HSS_PUSHED);
			IF_RETURN(HSS_DISABLED);

			IF_RETURN(HTS_NORMAL);
			IF_RETURN(HTS_HOT);
			IF_RETURN(HTS_PUSHED);
			IF_RETURN(HTS_DISABLED);

			IF_RETURN(VSS_NORMAL);
			IF_RETURN(VSS_HOT);
			IF_RETURN(VSS_PUSHED);
			IF_RETURN(VSS_DISABLED);

			IF_RETURN(VTS_NORMAL);
			IF_RETURN(VTS_HOT);
			IF_RETURN(VTS_PUSHED);
			IF_RETURN(VTS_DISABLED);

			IF_RETURN(SBS_NORMAL);
			IF_RETURN(SBS_HOT);
			IF_RETURN(SBS_PUSHED);
			IF_RETURN(SBS_DISABLED);

			IF_RETURN(MINBS_NORMAL);
			IF_RETURN(MINBS_HOT);
			IF_RETURN(MINBS_PUSHED);
			IF_RETURN(MINBS_DISABLED);

			IF_RETURN(MAXBS_NORMAL);
			IF_RETURN(MAXBS_HOT);
			IF_RETURN(MAXBS_PUSHED);
			IF_RETURN(MAXBS_DISABLED);

			IF_RETURN(RBS_NORMAL);
			IF_RETURN(RBS_HOT);
			IF_RETURN(RBS_PUSHED);
			IF_RETURN(RBS_DISABLED);

			IF_RETURN(HBS_NORMAL);
			IF_RETURN(HBS_HOT);
			IF_RETURN(HBS_PUSHED);
			IF_RETURN(HBS_DISABLED);

			IF_RETURN(CBS_NORMAL);
			IF_RETURN(CBS_HOT);
			IF_RETURN(CBS_PUSHED);
			IF_RETURN(CBS_DISABLED);

			IF_RETURN(SCBS_NORMAL);
			IF_RETURN(SCBS_HOT);
			IF_RETURN(SCBS_PUSHED);
			IF_RETURN(SCBS_DISABLED);

			IF_RETURN(FRB_ACTIVE);
			IF_RETURN(FRB_INACTIVE);

			IF_RETURN(FRL_ACTIVE);
			IF_RETURN(FRL_INACTIVE);

			IF_RETURN(FRR_ACTIVE);
			IF_RETURN(FRR_INACTIVE);

			IF_RETURN(SCS_ACTIVE);
			IF_RETURN(SCS_INACTIVE);
			IF_RETURN(SCS_DISABLED);

			IF_RETURN(SFRB_ACTIVE);
			IF_RETURN(SFRB_INACTIVE);

			IF_RETURN(SFRL_ACTIVE);
			IF_RETURN(SFRL_INACTIVE);

			IF_RETURN(SFRR_ACTIVE);
			IF_RETURN(SFRR_INACTIVE);

			IF_RETURN(MDCL_NORMAL);
			IF_RETURN(MDCL_HOT);
			IF_RETURN(MDCL_PUSHED);
			IF_RETURN(MDCL_DISABLED);

			IF_RETURN(MDMI_NORMAL);
			IF_RETURN(MDMI_HOT);
			IF_RETURN(MDMI_PUSHED);
			IF_RETURN(MDMI_DISABLED);

			IF_RETURN(MDRE_NORMAL);
			IF_RETURN(MDRE_HOT);
			IF_RETURN(MDRE_PUSHED);
			IF_RETURN(MDRE_DISABLED);

			// CtlColor

			IF_RETURN(CTLCOLOR_NORMAL);
			IF_RETURN(CTLCOLOR_DISABLED);
			IF_RETURN(CTLCOLOR_READONLY);
			IF_RETURN(CTLCOLOR_STATICTEXT);

			// ExEdit

			IF_RETURN(EXEDIT_LAYER_ACTIVE);
			IF_RETURN(EXEDIT_LAYER_INACTIVE);
			IF_RETURN(EXEDIT_LAYERBACKGROUND_ACTIVE);
			IF_RETURN(EXEDIT_LAYERBACKGROUND_INACTIVE);
			IF_RETURN(EXEDIT_GROUPBACKGROUND_ACTIVE_1);
			IF_RETURN(EXEDIT_GROUPBACKGROUND_ACTIVE_2);
			IF_RETURN(EXEDIT_GROUPBACKGROUND_ACTIVE_3);
			IF_RETURN(EXEDIT_GROUPBACKGROUND_INACTIVE_1);
			IF_RETURN(EXEDIT_GROUPBACKGROUND_INACTIVE_2);
			IF_RETURN(EXEDIT_GROUPBACKGROUND_INACTIVE_3);

			// Menu

			IF_RETURN(MB_ACTIVE);
			IF_RETURN(MB_INACTIVE);

			IF_RETURN(MBI_NORMAL);
			IF_RETURN(MBI_HOT);
			IF_RETURN(MBI_PUSHED);
			IF_RETURN(MBI_DISABLED);
			IF_RETURN(MBI_DISABLEDHOT);
			IF_RETURN(MBI_DISABLEDPUSHED);

			IF_RETURN(MC_CHECKMARKNORMAL);
			IF_RETURN(MC_CHECKMARKDISABLED);
			IF_RETURN(MC_BULLETNORMAL);
			IF_RETURN(MC_BULLETDISABLED);

			IF_RETURN(MCB_DISABLED);
			IF_RETURN(MCB_NORMAL);
			IF_RETURN(MCB_BITMAP);

			IF_RETURN(MPI_NORMAL);
			IF_RETURN(MPI_HOT);
			IF_RETURN(MPI_DISABLED);
			IF_RETURN(MPI_DISABLEDHOT);

			IF_RETURN(MSM_NORMAL);
			IF_RETURN(MSM_DISABLED);

			IF_RETURN(MSYSC_NORMAL);
			IF_RETURN(MSYSC_DISABLED);

			IF_RETURN(MSYSMX_NORMAL);
			IF_RETURN(MSYSMX_DISABLED);

			IF_RETURN(MSYSMN_NORMAL);
			IF_RETURN(MSYSMN_DISABLED);

			IF_RETURN(MSYSR_NORMAL);
			IF_RETURN(MSYSR_DISABLED);

			// ScrollBar

			IF_RETURN(ABS_UPNORMAL);
			IF_RETURN(ABS_UPHOT);
			IF_RETURN(ABS_UPPRESSED);
			IF_RETURN(ABS_UPDISABLED);
			IF_RETURN(ABS_DOWNNORMAL);
			IF_RETURN(ABS_DOWNHOT);
			IF_RETURN(ABS_DOWNPRESSED);
			IF_RETURN(ABS_DOWNDISABLED);
			IF_RETURN(ABS_LEFTNORMAL);
			IF_RETURN(ABS_LEFTHOT);
			IF_RETURN(ABS_LEFTPRESSED);
			IF_RETURN(ABS_LEFTDISABLED);
			IF_RETURN(ABS_RIGHTNORMAL);
			IF_RETURN(ABS_RIGHTHOT);
			IF_RETURN(ABS_RIGHTPRESSED);
			IF_RETURN(ABS_RIGHTDISABLED);
			IF_RETURN(ABS_UPHOVER);
			IF_RETURN(ABS_DOWNHOVER);
			IF_RETURN(ABS_LEFTHOVER);
			IF_RETURN(ABS_RIGHTHOVER);

			IF_RETURN(SCRBS_NORMAL);
			IF_RETURN(SCRBS_HOT);
			IF_RETURN(SCRBS_PRESSED);
			IF_RETURN(SCRBS_DISABLED);
			IF_RETURN(SCRBS_HOVER);

			IF_RETURN(SZB_RIGHTALIGN);
			IF_RETURN(SZB_LEFTALIGN);
			IF_RETURN(SZB_TOPRIGHTALIGN);
			IF_RETURN(SZB_TOPLEFTALIGN);
			IF_RETURN(SZB_HALFBOTTOMRIGHTALIGN);
			IF_RETURN(SZB_HALFBOTTOMLEFTALIGN);
			IF_RETURN(SZB_HALFTOPRIGHTALIGN);
			IF_RETURN(SZB_HALFTOPLEFTALIGN);

			// Static

			// Button

			IF_RETURN(PBS_NORMAL);
			IF_RETURN(PBS_HOT);
			IF_RETURN(PBS_PRESSED);
			IF_RETURN(PBS_DISABLED);
			IF_RETURN(PBS_DEFAULTED);
			IF_RETURN(PBS_DEFAULTED_ANIMATING);

			IF_RETURN(RBS_UNCHECKEDNORMAL);
			IF_RETURN(RBS_UNCHECKEDHOT);
			IF_RETURN(RBS_UNCHECKEDPRESSED);
			IF_RETURN(RBS_UNCHECKEDDISABLED);
			IF_RETURN(RBS_CHECKEDNORMAL);
			IF_RETURN(RBS_CHECKEDHOT);
			IF_RETURN(RBS_CHECKEDPRESSED);
			IF_RETURN(RBS_CHECKEDDISABLED);

			IF_RETURN(CBS_UNCHECKEDNORMAL);
			IF_RETURN(CBS_UNCHECKEDHOT);
			IF_RETURN(CBS_UNCHECKEDPRESSED);
			IF_RETURN(CBS_UNCHECKEDDISABLED);
			IF_RETURN(CBS_CHECKEDNORMAL);
			IF_RETURN(CBS_CHECKEDHOT);
			IF_RETURN(CBS_CHECKEDPRESSED);
			IF_RETURN(CBS_CHECKEDDISABLED);
			IF_RETURN(CBS_MIXEDNORMAL);
			IF_RETURN(CBS_MIXEDHOT);
			IF_RETURN(CBS_MIXEDPRESSED);
			IF_RETURN(CBS_MIXEDDISABLED);
			IF_RETURN(CBS_IMPLICITNORMAL);
			IF_RETURN(CBS_IMPLICITHOT);
			IF_RETURN(CBS_IMPLICITPRESSED);
			IF_RETURN(CBS_IMPLICITDISABLED);
			IF_RETURN(CBS_EXCLUDEDNORMAL);
			IF_RETURN(CBS_EXCLUDEDHOT);
			IF_RETURN(CBS_EXCLUDEDPRESSED);
			IF_RETURN(CBS_EXCLUDEDDISABLED);

			IF_RETURN(GBS_NORMAL);
			IF_RETURN(GBS_DISABLED);

			IF_RETURN(CMDLS_NORMAL);
			IF_RETURN(CMDLS_HOT);
			IF_RETURN(CMDLS_PRESSED);
			IF_RETURN(CMDLS_DISABLED);
			IF_RETURN(CMDLS_DEFAULTED);
			IF_RETURN(CMDLS_DEFAULTED_ANIMATING);

			IF_RETURN(CMDLGS_NORMAL);
			IF_RETURN(CMDLGS_HOT);
			IF_RETURN(CMDLGS_PRESSED);
			IF_RETURN(CMDLGS_DISABLED);
			IF_RETURN(CMDLGS_DEFAULTED);

			IF_RETURN(PBDDS_NORMAL);
			IF_RETURN(PBDDS_DISABLED);

			// Edit

			IF_RETURN(ETS_NORMAL);
			IF_RETURN(ETS_HOT);
			IF_RETURN(ETS_SELECTED);
			IF_RETURN(ETS_DISABLED);
			IF_RETURN(ETS_FOCUSED);
			IF_RETURN(ETS_READONLY);
			IF_RETURN(ETS_ASSIST);
			IF_RETURN(ETS_CUEBANNER);
			IF_RETURN(ETS_STATICTEXT);

			IF_RETURN(EBS_NORMAL);
			IF_RETURN(EBS_HOT);
			IF_RETURN(EBS_DISABLED);
			IF_RETURN(EBS_FOCUSED);
			IF_RETURN(EBS_READONLY);
			IF_RETURN(EBS_ASSIST);

			IF_RETURN(EBWBS_NORMAL);
			IF_RETURN(EBWBS_HOT);
			IF_RETURN(EBWBS_DISABLED);
			IF_RETURN(EBWBS_FOCUSED);

			IF_RETURN(EPSN_NORMAL);
			IF_RETURN(EPSN_HOT);
			IF_RETURN(EPSN_FOCUSED);
			IF_RETURN(EPSN_DISABLED);

			IF_RETURN(EPSH_NORMAL);
			IF_RETURN(EPSH_HOT);
			IF_RETURN(EPSH_FOCUSED);
			IF_RETURN(EPSH_DISABLED);

			IF_RETURN(EPSV_NORMAL);
			IF_RETURN(EPSV_HOT);
			IF_RETURN(EPSV_FOCUSED);
			IF_RETURN(EPSV_DISABLED);

			IF_RETURN(EPSHV_NORMAL);
			IF_RETURN(EPSHV_HOT);
			IF_RETURN(EPSHV_FOCUSED);
			IF_RETURN(EPSHV_DISABLED);

			// ListBox

			IF_RETURN(LBPSH_NORMAL);
			IF_RETURN(LBPSH_FOCUSED);
			IF_RETURN(LBPSH_HOT);
			IF_RETURN(LBPSH_DISABLED);

			IF_RETURN(LBPSHV_NORMAL);
			IF_RETURN(LBPSHV_FOCUSED);
			IF_RETURN(LBPSHV_HOT);
			IF_RETURN(LBPSHV_DISABLED);

			IF_RETURN(LBPSN_NORMAL);
			IF_RETURN(LBPSN_FOCUSED);
			IF_RETURN(LBPSN_HOT);
			IF_RETURN(LBPSN_DISABLED);

			IF_RETURN(LBPSV_NORMAL);
			IF_RETURN(LBPSV_FOCUSED);
			IF_RETURN(LBPSV_HOT);
			IF_RETURN(LBPSV_DISABLED);

			IF_RETURN(LBPSI_HOT);
			IF_RETURN(LBPSI_HOTSELECTED);
			IF_RETURN(LBPSI_SELECTED);
			IF_RETURN(LBPSI_SELECTEDNOTFOCUS);

			// ComboBox

			IF_RETURN(CBXS_NORMAL);
			IF_RETURN(CBXS_HOT);
			IF_RETURN(CBXS_PRESSED);
			IF_RETURN(CBXS_DISABLED);

			IF_RETURN(CBXSR_NORMAL);
			IF_RETURN(CBXSR_HOT);
			IF_RETURN(CBXSR_PRESSED);
			IF_RETURN(CBXSR_DISABLED);

			IF_RETURN(CBXSL_NORMAL);
			IF_RETURN(CBXSL_HOT);
			IF_RETURN(CBXSL_PRESSED);
			IF_RETURN(CBXSL_DISABLED);

			IF_RETURN(CBTBS_NORMAL);
			IF_RETURN(CBTBS_HOT);
			IF_RETURN(CBTBS_DISABLED);
			IF_RETURN(CBTBS_FOCUSED);

			IF_RETURN(CBB_NORMAL);
			IF_RETURN(CBB_HOT);
			IF_RETURN(CBB_FOCUSED);
			IF_RETURN(CBB_DISABLED);

			IF_RETURN(CBRO_NORMAL);
			IF_RETURN(CBRO_HOT);
			IF_RETURN(CBRO_PRESSED);
			IF_RETURN(CBRO_DISABLED);

			IF_RETURN(CBCB_NORMAL);
			IF_RETURN(CBCB_HOT);
			IF_RETURN(CBCB_PRESSED);
			IF_RETURN(CBCB_DISABLED);

			IF_RETURN(CBDI_NORMAL);
			IF_RETURN(CBDI_HIGHLIGHTED);

			// ToolTip

			IF_RETURN(TTCS_NORMAL);
			IF_RETURN(TTCS_HOT);
			IF_RETURN(TTCS_PRESSED);

			IF_RETURN(TTSS_NORMAL);
			IF_RETURN(TTSS_LINK);

			IF_RETURN(TTBS_NORMAL);
			IF_RETURN(TTBS_LINK);

			IF_RETURN(TTBSS_POINTINGUPLEFTWALL);
			IF_RETURN(TTBSS_POINTINGUPCENTERED);
			IF_RETURN(TTBSS_POINTINGUPRIGHTWALL);
			IF_RETURN(TTBSS_POINTINGDOWNRIGHTWALL);
			IF_RETURN(TTBSS_POINTINGDOWNCENTERED);
			IF_RETURN(TTBSS_POINTINGDOWNLEFTWALL);

			IF_RETURN(TTWS_NORMAL);
			IF_RETURN(TTWS_HOT);
			IF_RETURN(TTWS_PRESSED);

			// TrackBar

			IF_RETURN(TKS_NORMAL);

			IF_RETURN(TRS_NORMAL);

			IF_RETURN(TRVS_NORMAL);

			IF_RETURN(TUS_NORMAL);
			IF_RETURN(TUS_HOT);
			IF_RETURN(TUS_PRESSED);
			IF_RETURN(TUS_FOCUSED);
			IF_RETURN(TUS_DISABLED);

			IF_RETURN(TUBS_NORMAL);
			IF_RETURN(TUBS_HOT);
			IF_RETURN(TUBS_PRESSED);
			IF_RETURN(TUBS_FOCUSED);
			IF_RETURN(TUBS_DISABLED);

			IF_RETURN(TUTS_NORMAL);
			IF_RETURN(TUTS_HOT);
			IF_RETURN(TUTS_PRESSED);
			IF_RETURN(TUTS_FOCUSED);
			IF_RETURN(TUTS_DISABLED);

			IF_RETURN(TUVS_NORMAL);
			IF_RETURN(TUVS_HOT);
			IF_RETURN(TUVS_PRESSED);
			IF_RETURN(TUVS_FOCUSED);
			IF_RETURN(TUVS_DISABLED);

			IF_RETURN(TUVLS_NORMAL);
			IF_RETURN(TUVLS_HOT);
			IF_RETURN(TUVLS_PRESSED);
			IF_RETURN(TUVLS_FOCUSED);
			IF_RETURN(TUVLS_DISABLED);

			IF_RETURN(TUVRS_NORMAL);
			IF_RETURN(TUVRS_HOT);
			IF_RETURN(TUVRS_PRESSED);
			IF_RETURN(TUVRS_FOCUSED);
			IF_RETURN(TUVRS_DISABLED);

			IF_RETURN(TSS_NORMAL);

			IF_RETURN(TSVS_NORMAL);

			// Spin

			IF_RETURN(UPS_NORMAL);
			IF_RETURN(UPS_HOT);
			IF_RETURN(UPS_PRESSED);
			IF_RETURN(UPS_DISABLED);

			IF_RETURN(DNS_NORMAL);
			IF_RETURN(DNS_HOT);
			IF_RETURN(DNS_PRESSED);
			IF_RETURN(DNS_DISABLED);

			IF_RETURN(UPHZS_NORMAL);
			IF_RETURN(UPHZS_HOT);
			IF_RETURN(UPHZS_PRESSED);
			IF_RETURN(UPHZS_DISABLED);

			IF_RETURN(DNHZS_NORMAL);
			IF_RETURN(DNHZS_HOT);
			IF_RETURN(DNHZS_PRESSED);
			IF_RETURN(DNHZS_DISABLED);

			// Header

			IF_RETURN(HBG_DETAILS);
			IF_RETURN(HBG_ICON);

			IF_RETURN(HIS_NORMAL);
			IF_RETURN(HIS_HOT);
			IF_RETURN(HIS_PRESSED);
			IF_RETURN(HIS_SORTEDNORMAL);
			IF_RETURN(HIS_SORTEDHOT);
			IF_RETURN(HIS_SORTEDPRESSED);
			IF_RETURN(HIS_ICONNORMAL);
			IF_RETURN(HIS_ICONHOT);
			IF_RETURN(HIS_ICONPRESSED);
			IF_RETURN(HIS_ICONSORTEDNORMAL);
			IF_RETURN(HIS_ICONSORTEDHOT);
			IF_RETURN(HIS_ICONSORTEDPRESSED);

			IF_RETURN(HILS_NORMAL);
			IF_RETURN(HILS_HOT);
			IF_RETURN(HILS_PRESSED);

			IF_RETURN(HIRS_NORMAL);
			IF_RETURN(HIRS_HOT);
			IF_RETURN(HIRS_PRESSED);

			IF_RETURN(HSAS_SORTEDUP);
			IF_RETURN(HSAS_SORTEDDOWN);

			IF_RETURN(HDDS_NORMAL);
			IF_RETURN(HDDS_SOFTHOT);
			IF_RETURN(HDDS_HOT);

			IF_RETURN(HDDFS_NORMAL);
			IF_RETURN(HDDFS_SOFTHOT);
			IF_RETURN(HDDFS_HOT);

			IF_RETURN(HOFS_NORMAL);
			IF_RETURN(HOFS_HOT);

			// ListView

			IF_RETURN(LISS_NORMAL);
			IF_RETURN(LISS_HOT);
			IF_RETURN(LISS_SELECTED);
			IF_RETURN(LISS_DISABLED);
			IF_RETURN(LISS_SELECTEDNOTFOCUS);
			IF_RETURN(LISS_HOTSELECTED);

			IF_RETURN(LVGH_OPEN);
			IF_RETURN(LVGH_OPENHOT);
			IF_RETURN(LVGH_OPENSELECTED);
			IF_RETURN(LVGH_OPENSELECTEDHOT);
			IF_RETURN(LVGH_OPENSELECTEDNOTFOCUSED);
			IF_RETURN(LVGH_OPENSELECTEDNOTFOCUSEDHOT);
			IF_RETURN(LVGH_OPENMIXEDSELECTION);
			IF_RETURN(LVGH_OPENMIXEDSELECTIONHOT);
			IF_RETURN(LVGH_CLOSE);
			IF_RETURN(LVGH_CLOSEHOT);
			IF_RETURN(LVGH_CLOSESELECTED);
			IF_RETURN(LVGH_CLOSESELECTEDHOT);
			IF_RETURN(LVGH_CLOSESELECTEDNOTFOCUSED);
			IF_RETURN(LVGH_CLOSESELECTEDNOTFOCUSEDHOT);
			IF_RETURN(LVGH_CLOSEMIXEDSELECTION);
			IF_RETURN(LVGH_CLOSEMIXEDSELECTIONHOT);

			IF_RETURN(LVGHL_OPEN);
			IF_RETURN(LVGHL_OPENHOT);
			IF_RETURN(LVGHL_OPENSELECTED);
			IF_RETURN(LVGHL_OPENSELECTEDHOT);
			IF_RETURN(LVGHL_OPENSELECTEDNOTFOCUSED);
			IF_RETURN(LVGHL_OPENSELECTEDNOTFOCUSEDHOT);
			IF_RETURN(LVGHL_OPENMIXEDSELECTION);
			IF_RETURN(LVGHL_OPENMIXEDSELECTIONHOT);
			IF_RETURN(LVGHL_CLOSE);
			IF_RETURN(LVGHL_CLOSEHOT);
			IF_RETURN(LVGHL_CLOSESELECTED);
			IF_RETURN(LVGHL_CLOSESELECTEDHOT);
			IF_RETURN(LVGHL_CLOSESELECTEDNOTFOCUSED);
			IF_RETURN(LVGHL_CLOSESELECTEDNOTFOCUSEDHOT);
			IF_RETURN(LVGHL_CLOSEMIXEDSELECTION);
			IF_RETURN(LVGHL_CLOSEMIXEDSELECTIONHOT);

			IF_RETURN(LVEB_NORMAL);
			IF_RETURN(LVEB_HOVER);
			IF_RETURN(LVEB_PUSHED);

			IF_RETURN(LVCB_NORMAL);
			IF_RETURN(LVCB_HOVER);
			IF_RETURN(LVCB_PUSHED);

			// TreeView

			IF_RETURN(TREIS_NORMAL);
			IF_RETURN(TREIS_HOT);
			IF_RETURN(TREIS_SELECTED);
			IF_RETURN(TREIS_DISABLED);
			IF_RETURN(TREIS_SELECTEDNOTFOCUS);
			IF_RETURN(TREIS_HOTSELECTED);

			IF_RETURN(GLPS_CLOSED);
			IF_RETURN(GLPS_OPENED);

			IF_RETURN(HGLPS_CLOSED);
			IF_RETURN(HGLPS_OPENED);

			// ToolBar

			IF_RETURN(TS_NORMAL);
			IF_RETURN(TS_HOT);
			IF_RETURN(TS_PRESSED);
			IF_RETURN(TS_DISABLED);
			IF_RETURN(TS_CHECKED);
			IF_RETURN(TS_HOTCHECKED);
			IF_RETURN(TS_NEARHOT);
			IF_RETURN(TS_OTHERSIDEHOT);

			// Tab

			IF_RETURN(TIS_NORMAL);
			IF_RETURN(TIS_HOT);
			IF_RETURN(TIS_SELECTED);
			IF_RETURN(TIS_DISABLED);
			IF_RETURN(TIS_FOCUSED);

			IF_RETURN(TILES_NORMAL);
			IF_RETURN(TILES_HOT);
			IF_RETURN(TILES_SELECTED);
			IF_RETURN(TILES_DISABLED);
			IF_RETURN(TILES_FOCUSED);

			IF_RETURN(TIRES_NORMAL);
			IF_RETURN(TIRES_HOT);
			IF_RETURN(TIRES_SELECTED);
			IF_RETURN(TIRES_DISABLED);
			IF_RETURN(TIRES_FOCUSED);

			IF_RETURN(TIBES_NORMAL);
			IF_RETURN(TIBES_HOT);
			IF_RETURN(TIBES_SELECTED);
			IF_RETURN(TIBES_DISABLED);
			IF_RETURN(TIBES_FOCUSED);

			IF_RETURN(TTIS_NORMAL);
			IF_RETURN(TTIS_HOT);
			IF_RETURN(TTIS_SELECTED);
			IF_RETURN(TTIS_DISABLED);
			IF_RETURN(TTIS_FOCUSED);

			IF_RETURN(TTILES_NORMAL);
			IF_RETURN(TTILES_HOT);
			IF_RETURN(TTILES_SELECTED);
			IF_RETURN(TTILES_DISABLED);
			IF_RETURN(TTILES_FOCUSED);

			IF_RETURN(TTIRES_NORMAL);
			IF_RETURN(TTIRES_HOT);
			IF_RETURN(TTIRES_SELECTED);
			IF_RETURN(TTIRES_DISABLED);
			IF_RETURN(TTIRES_FOCUSED);

			IF_RETURN(TTIBES_NORMAL);
			IF_RETURN(TTIBES_HOT);
			IF_RETURN(TTIBES_SELECTED);
			IF_RETURN(TTIBES_DISABLED);
			IF_RETURN(TTIBES_FOCUSED);

			return wcstol(id, 0, 0);
		}
#undef IF_RETURN
	} pocket;
}
