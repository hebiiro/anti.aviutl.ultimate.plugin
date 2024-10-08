CLR_NONE = 0xffffffff

DWMWA_COLOR_DEFAULT = 0xFFFFFFFF
DWMWA_COLOR_NONE = 0xFFFFFFFE

DWMWCP_DEFAULT = 0
DWMWCP_DONOTROUND = 1
DWMWCP_ROUND = 2
DWMWCP_ROUNDSMALL = 3

VSCLASS_MENU = 'MENU'
VSCLASS_SCROLLBAR = 'SCROLLBAR'
VSCLASS_STATIC = 'STATIC'
VSCLASS_BUTTON = 'BUTTON'
VSCLASS_EDIT = 'EDIT'
VSCLASS_LISTBOX = 'LISTBOX'
VSCLASS_COMBOBOX = 'COMBOBOX'
VSCLASS_TOOLTIP = 'TOOLTIP'
VSCLASS_TRACKBAR = 'TRACKBAR'
VSCLASS_SPIN = 'SPIN'
VSCLASS_HEADER = 'HEADER'
VSCLASS_LISTVIEW = 'LISTVIEW'
VSCLASS_TREEVIEW = 'TREEVIEW'
VSCLASS_TOOLBAR = 'TOOLBAR'
VSCLASS_TAB = 'TAB'
VSCLASS_WINDOW = 'WINDOW'

VSCLASS_EXPLORER_MENU = 'Explorer::' + VSCLASS_MENU
VSCLASS_EXPLORER_SCROLLBAR = 'Explorer::' + VSCLASS_SCROLLBAR
VSCLASS_EXPLORER_STATIC = 'Explorer::' + VSCLASS_STATIC
VSCLASS_EXPLORER_BUTTON = 'Explorer::' + VSCLASS_BUTTON
VSCLASS_EXPLORER_EDIT = 'Explorer::' + VSCLASS_EDIT
VSCLASS_EXPLORER_LISTBOX = 'Explorer::' + VSCLASS_LISTBOX
VSCLASS_EXPLORER_COMBOBOX = 'Explorer::' + VSCLASS_COMBOBOX
VSCLASS_EXPLORER_TOOLTIP = 'Explorer::' + VSCLASS_TOOLTIP
VSCLASS_EXPLORER_TRACKBAR = 'Explorer::' + VSCLASS_TRACKBAR
VSCLASS_EXPLORER_SPIN = 'Explorer::' + VSCLASS_SPIN
VSCLASS_EXPLORER_HEADER = 'Explorer::' + VSCLASS_HEADER
VSCLASS_EXPLORER_LISTVIEW = 'Explorer::' + VSCLASS_LISTVIEW
VSCLASS_EXPLORER_TREEVIEW = 'Explorer::' + VSCLASS_TREEVIEW
VSCLASS_EXPLORER_TOOLBAR = 'Explorer::' + VSCLASS_TOOLBAR
VSCLASS_EXPLORER_TAB = 'Explorer::' + VSCLASS_TAB
VSCLASS_EXPLORER_WINDOW = 'Explorer::' + VSCLASS_WINDOW
VSCLASS_EXPLORER_DIALOG = VSCLASS_EXPLORER_WINDOW
VSCLASS_EXPLORER_CTLCOLOR = VSCLASS_EXPLORER_WINDOW
VSCLASS_EXPLORER_EXEDIT = VSCLASS_EXPLORER_WINDOW

VSCLASS_COMBOBOX_LISTBOX = 'Combobox::' + VSCLASS_LISTBOX
VSCLASS_CFD_COMBOBOX = 'CFD::' + VSCLASS_COMBOBOX
VSCLASS_CFD_HEADER = 'CFD::' + VSCLASS_HEADER
VSCLASS_CFD_LISTVIEW = 'CFD::' + VSCLASS_LISTVIEW
VSCLASS_CFD_TREEVIEW = 'CFD::' + VSCLASS_TREEVIEW
VSCLASS_ITEMSVIEW_HEADER = 'ItemsView::' + VSCLASS_HEADER
VSCLASS_ITEMSVIEW_LISTVIEW = 'ItemsView::' + VSCLASS_LISTVIEW
VSCLASS_PLACEBAR_TOOLBAR = 'Placesbar::' + VSCLASS_TOOLBAR
VSCLASS_ADDRESSCOMPOSITED_COMBOBOX = 'AddressComposited::' + VSCLASS_COMBOBOX
VSCLASS_COMMANDMODULE = 'CommandModule'
VSCLASS_PREVIEWPANE = 'PreviewPane'
VSCLASS_NAVBARCOMPOSITED_PREVIEWPANE = 'NavbarComposited::' + 'PreviewPane'
VSCLASS_INACTIVENAVBARCOMPOSITED_PREVIEWPANE = 'InactiveNavbarComposited::' + 'PreviewPane'
VSCLASS_COMMONITEMSDIALOG = 'CommonItemsDialog'
VSCLASS_NAVBARCOMPOSITED_COMMONITEMSDIALOG = 'NavbarComposited::CommonItemsDialog'
VSCLASS_INACTIVENAVBARCOMPOSITED_COMMONITEMSDIALOG = 'InactiveNavbarComposited::CommonItemsDialog'

WP_WINDOW_FACE = 51
WP_WINDOW_EDGE = 52
WP_CLIENT_EDGE = 53
WP_STATIC_EDGE = 54
WP_LIGHT_RAISED_EDGE = 56
WP_LIGHT_SUNKEN_EDGE = 57
WP_RAISED_EDGE = 58
WP_SUNKEN_EDGE = 59
WP_BUMP_EDGE = 60
WP_ETCHED_EDGE = 61
WP_SEPARATOR = 62
WP_EXEDIT = 63

STAT_TEXT = 1 # vssym32.h : 494
STAT_ETCHED_EDGE = 2

EES_SELECTION_FILL = 1
EES_SELECTION_EDGE = 2
EES_SELECTION_BACKGROUND = 3
EES_SCALE_PRIMARY = 11
EES_SCALE_SECONDARY = 12
EES_SCENE_BUTTON_EDGE = 21
EES_SCENE_BUTTON = 22
EES_EVEN_LAYER_BUTTON_EDGE = 30
EES_ODD_LAYER_BUTTON_EDGE = 31
EES_EVEN_LAYER_BUTTON_ACTIVE = 32
EES_ODD_LAYER_BUTTON_ACTIVE = 33
EES_EVEN_LAYER_BUTTON_INACTIVE = 34
EES_ODD_LAYER_BUTTON_INACTIVE = 35
EES_EVEN_LAYER_BACKGROUND_ACTIVE = 36
EES_ODD_LAYER_BACKGROUND_ACTIVE = 37
EES_EVEN_LAYER_BACKGROUND_INACTIVE = 38
EES_ODD_LAYER_BACKGROUND_INACTIVE = 39
EES_LAYER_LINE_LEFT = 51
EES_LAYER_LINE_TOP = 52
EES_LAYER_LINE_RIGHT = 53
EES_LAYER_LINE_BOTTOM = 54
EES_LAYER_LINE_SEPARATOR = 55
EES_GROUP_BACKGROUND_ACTIVE_1 = 61
EES_GROUP_BACKGROUND_ACTIVE_2 = 62
EES_GROUP_BACKGROUND_ACTIVE_3 = 63
EES_GROUP_BACKGROUND_INACTIVE_1 = 64
EES_GROUP_BACKGROUND_INACTIVE_2 = 65
EES_GROUP_BACKGROUND_INACTIVE_3 = 66

AW_TITLEBAR = 1
AW_HEADERAREA = 2
AW_CONTENTAREA = 3
AW_COMMANDAREA = 4
AW_BUTTON = 5

AW_S_TITLEBAR_ACTIVE = 1
AW_S_TITLEBAR_INACTIVE = 2

AW_S_HEADERAREA_NOMARGIN = 1

AW_S_CONTENTAREA_NOMARGIN = 1

BP_PUSHBUTTON = 1
BP_RADIOBUTTON = 2
BP_CHECKBOX = 3
BP_GROUPBOX = 4
BP_USERBUTTON = 5
BP_COMMANDLINK = 6
BP_COMMANDLINKGLYPH = 7
BP_RADIOBUTTON_HCDISABLED = 8
BP_CHECKBOX_HCDISABLED = 9
BP_GROUPBOX_HCDISABLED = 10
BP_PUSHBUTTONDROPDOWN = 11

PBS_NORMAL = 1
PBS_HOT = 2
PBS_PRESSED = 3
PBS_DISABLED = 4
PBS_DEFAULTED = 5
PBS_DEFAULTED_ANIMATING = 6

RBS_UNCHECKEDNORMAL = 1
RBS_UNCHECKEDHOT = 2
RBS_UNCHECKEDPRESSED = 3
RBS_UNCHECKEDDISABLED = 4
RBS_CHECKEDNORMAL = 5
RBS_CHECKEDHOT = 6
RBS_CHECKEDPRESSED = 7
RBS_CHECKEDDISABLED = 8

CBS_UNCHECKEDNORMAL = 1
CBS_UNCHECKEDHOT = 2
CBS_UNCHECKEDPRESSED = 3
CBS_UNCHECKEDDISABLED = 4
CBS_CHECKEDNORMAL = 5
CBS_CHECKEDHOT = 6
CBS_CHECKEDPRESSED = 7
CBS_CHECKEDDISABLED = 8
CBS_MIXEDNORMAL = 9
CBS_MIXEDHOT = 10
CBS_MIXEDPRESSED = 11
CBS_MIXEDDISABLED = 12
CBS_IMPLICITNORMAL = 13
CBS_IMPLICITHOT = 14
CBS_IMPLICITPRESSED = 15
CBS_IMPLICITDISABLED = 16
CBS_EXCLUDEDNORMAL = 17
CBS_EXCLUDEDHOT = 18
CBS_EXCLUDEDPRESSED = 19
CBS_EXCLUDEDDISABLED = 20

GBS_NORMAL = 1
GBS_DISABLED = 2

CMDLS_NORMAL = 1
CMDLS_HOT = 2
CMDLS_PRESSED = 3
CMDLS_DISABLED = 4
CMDLS_DEFAULTED = 5
CMDLS_DEFAULTED_ANIMATING = 6

CMDLGS_NORMAL = 1
CMDLGS_HOT = 2
CMDLGS_PRESSED = 3
CMDLGS_DISABLED = 4
CMDLGS_DEFAULTED = 5

PBDDS_NORMAL = 1
PBDDS_DISABLED = 2

CP_DROPDOWNBUTTON = 1
CP_BACKGROUND = 2
CP_TRANSPARENTBACKGROUND = 3
CP_BORDER = 4
CP_READONLY = 5
CP_DROPDOWNBUTTONRIGHT = 6
CP_DROPDOWNBUTTONLEFT = 7
CP_CUEBANNER = 8
CP_DROPDOWNITEM = 9

CBXS_NORMAL = 1
CBXS_HOT = 2
CBXS_PRESSED = 3
CBXS_DISABLED = 4

CBXSR_NORMAL = 1
CBXSR_HOT = 2
CBXSR_PRESSED = 3
CBXSR_DISABLED = 4

CBXSL_NORMAL = 1
CBXSL_HOT = 2
CBXSL_PRESSED = 3
CBXSL_DISABLED = 4

CBTBS_NORMAL = 1
CBTBS_HOT = 2
CBTBS_DISABLED = 3
CBTBS_FOCUSED = 4

CBB_NORMAL = 1
CBB_HOT = 2
CBB_FOCUSED = 3
CBB_DISABLED = 4

CBRO_NORMAL = 1
CBRO_HOT = 2
CBRO_PRESSED = 3
CBRO_DISABLED = 4

CBCB_NORMAL = 1
CBCB_HOT = 2
CBCB_PRESSED = 3
CBCB_DISABLED = 4

CBDI_NORMAL = 1
CBDI_HIGHLIGHTED = 2

CSST_TAB = 1

CSTB_NORMAL = 1
CSTB_HOT = 2
CSTB_SELECTED = 3

CPANEL_NAVIGATIONPANE = 1
CPANEL_CONTENTPANE = 2
CPANEL_NAVIGATIONPANELABEL = 3
CPANEL_CONTENTPANELABEL = 4
CPANEL_TITLE = 5
CPANEL_BODYTEXT = 6
CPANEL_HELPLINK = 7
CPANEL_TASKLINK = 8
CPANEL_GROUPTEXT = 9
CPANEL_CONTENTLINK = 10
CPANEL_SECTIONTITLELINK = 11
CPANEL_LARGECOMMANDAREA = 12
CPANEL_SMALLCOMMANDAREA = 13
CPANEL_BUTTON = 14
CPANEL_MESSAGETEXT = 15
CPANEL_NAVIGATIONPANELINE = 16
CPANEL_CONTENTPANELINE = 17
CPANEL_BANNERAREA = 18
CPANEL_BODYTITLE = 19

CPHL_NORMAL = 1
CPHL_HOT = 2
CPHL_PRESSED = 3
CPHL_DISABLED = 4

CPTL_NORMAL = 1
CPTL_HOT = 2
CPTL_PRESSED = 3
CPTL_DISABLED = 4
CPTL_PAGE = 5

CPCL_NORMAL = 1
CPCL_HOT = 2
CPCL_PRESSED = 3
CPCL_DISABLED = 4

CPSTL_NORMAL = 1
CPSTL_HOT = 2

DP_DATETEXT = 1
DP_DATEBORDER = 2
DP_SHOWCALENDARBUTTONRIGHT = 3

DPDT_NORMAL = 1
DPDT_DISABLED = 2
DPDT_SELECTED = 3

DPDB_NORMAL = 1
DPDB_HOT = 2
DPDB_FOCUSED = 3
DPDB_DISABLED = 4

DPSCBR_NORMAL = 1
DPSCBR_HOT = 2
DPSCBR_PRESSED = 3
DPSCBR_DISABLED = 4

DD_COPY = 1
DD_MOVE = 2
DD_UPDATEMETADATA = 3
DD_CREATELINK = 4
DD_WARNING = 5
DD_NONE = 6
DD_IMAGEBG = 7
DD_TEXTBG = 8

DDCOPY_HIGHLIGHT = 1
DDCOPY_NOHIGHLIGHT = 2

DDMOVE_HIGHLIGHT = 1
DDMOVE_NOHIGHLIGHT = 2

DDUPDATEMETADATA_HIGHLIGHT = 1
DDUPDATEMETADATA_NOHIGHLIGHT = 2

DDCREATELINK_HIGHLIGHT = 1
DDCREATELINK_NOHIGHLIGHT = 2

DDWARNING_HIGHLIGHT = 1
DDWARNING_NOHIGHLIGHT = 2

DDNONE_HIGHLIGHT = 1
DDNONE_NOHIGHLIGHT = 2

EP_EDITTEXT = 1
EP_CARET = 2
EP_BACKGROUND = 3
EP_PASSWORD = 4
EP_BACKGROUNDWITHBORDER = 5
EP_EDITBORDER_NOSCROLL = 6
EP_EDITBORDER_HSCROLL = 7
EP_EDITBORDER_VSCROLL = 8
EP_EDITBORDER_HVSCROLL = 9

ETS_NORMAL = 1
ETS_HOT = 2
ETS_SELECTED = 3
ETS_DISABLED = 4
ETS_FOCUSED = 5
ETS_READONLY = 6
ETS_ASSIST = 7
ETS_CUEBANNER = 8

EBS_NORMAL = 1
EBS_HOT = 2
EBS_DISABLED = 3
EBS_FOCUSED = 4
EBS_READONLY = 5
EBS_ASSIST = 6

EBWBS_NORMAL = 1
EBWBS_HOT = 2
EBWBS_DISABLED = 3
EBWBS_FOCUSED = 4

EPSN_NORMAL = 1
EPSN_HOT = 2
EPSN_FOCUSED = 3
EPSN_DISABLED = 4

EPSH_NORMAL = 1
EPSH_HOT = 2
EPSH_FOCUSED = 3
EPSH_DISABLED = 4

EPSV_NORMAL = 1
EPSV_HOT = 2
EPSV_FOCUSED = 3
EPSV_DISABLED = 4

EPSHV_NORMAL = 1
EPSHV_HOT = 2
EPSHV_FOCUSED = 3
EPSHV_DISABLED = 4

EBP_HEADERBACKGROUND = 1
EBP_HEADERCLOSE = 2
EBP_HEADERPIN = 3
EBP_IEBARMENU = 4
EBP_NORMALGROUPBACKGROUND = 5
EBP_NORMALGROUPCOLLAPSE = 6
EBP_NORMALGROUPEXPAND = 7
EBP_NORMALGROUPHEAD = 8
EBP_SPECIALGROUPBACKGROUND = 9
EBP_SPECIALGROUPCOLLAPSE = 10
EBP_SPECIALGROUPEXPAND = 11
EBP_SPECIALGROUPHEAD = 12

EBHC_NORMAL = 1
EBHC_HOT = 2
EBHC_PRESSED = 3

EBHP_NORMAL = 1
EBHP_HOT = 2
EBHP_PRESSED = 3
EBHP_SELECTEDNORMAL = 4
EBHP_SELECTEDHOT = 5
EBHP_SELECTEDPRESSED = 6

EBM_NORMAL = 1
EBM_HOT = 2
EBM_PRESSED = 3

EBNGC_NORMAL = 1
EBNGC_HOT = 2
EBNGC_PRESSED = 3

EBNGE_NORMAL = 1
EBNGE_HOT = 2
EBNGE_PRESSED = 3

EBSGC_NORMAL = 1
EBSGC_HOT = 2
EBSGC_PRESSED = 3

EBSGE_NORMAL = 1
EBSGE_HOT = 2
EBSGE_PRESSED = 3

FLYOUT_HEADER = 1
FLYOUT_BODY = 2
FLYOUT_LABEL = 3
FLYOUT_LINK = 4
FLYOUT_DIVIDER = 5
FLYOUT_WINDOW = 6
FLYOUT_LINKAREA = 7
FLYOUT_LINKHEADER = 8

FLS_NORMAL = 1
FLS_SELECTED = 2
FLS_EMPHASIZED = 3
FLS_DISABLED = 4

FLYOUTLINK_NORMAL = 1
FLYOUTLINK_HOVER = 2

FBS_NORMAL = 1
FBS_EMPHASIZED = 2

FLH_NORMAL = 1
FLH_HOVER = 2

HP_HEADERITEM = 1
HP_HEADERITEMLEFT = 2
HP_HEADERITEMRIGHT = 3
HP_HEADERSORTARROW = 4
HP_HEADERDROPDOWN = 5
HP_HEADERDROPDOWNFILTER = 6
HP_HEADEROVERFLOW = 7

HBG_DETAILS = 1
HBG_ICON = 2

HIS_NORMAL = 1
HIS_HOT = 2
HIS_PRESSED = 3
HIS_SORTEDNORMAL = 4
HIS_SORTEDHOT = 5
HIS_SORTEDPRESSED = 6
HIS_ICONNORMAL = 7
HIS_ICONHOT = 8
HIS_ICONPRESSED = 9
HIS_ICONSORTEDNORMAL = 10
HIS_ICONSORTEDHOT = 11
HIS_ICONSORTEDPRESSED = 12

HILS_NORMAL = 1
HILS_HOT = 2
HILS_PRESSED = 3

HIRS_NORMAL = 1
HIRS_HOT = 2
HIRS_PRESSED = 3

HSAS_SORTEDUP = 1
HSAS_SORTEDDOWN = 2

HDDS_NORMAL = 1
HDDS_SOFTHOT = 2
HDDS_HOT = 3

HDDFS_NORMAL = 1
HDDFS_SOFTHOT = 2
HDDFS_HOT = 3

HOFS_NORMAL = 1
HOFS_HOT = 2

LBCP_BORDER_HSCROLL = 1
LBCP_BORDER_HVSCROLL = 2
LBCP_BORDER_NOSCROLL = 3
LBCP_BORDER_VSCROLL = 4
LBCP_ITEM = 5

LBPSH_NORMAL = 1
LBPSH_FOCUSED = 2
LBPSH_HOT = 3
LBPSH_DISABLED = 4

LBPSHV_NORMAL = 1
LBPSHV_FOCUSED = 2
LBPSHV_HOT = 3
LBPSHV_DISABLED = 4

LBPSN_NORMAL = 1
LBPSN_FOCUSED = 2
LBPSN_HOT = 3
LBPSN_DISABLED = 4

LBPSV_NORMAL = 1
LBPSV_FOCUSED = 2
LBPSV_HOT = 3
LBPSV_DISABLED = 4

LBPSI_HOT = 1
LBPSI_HOTSELECTED = 2
LBPSI_SELECTED = 3
LBPSI_SELECTEDNOTFOCUS = 4

LVP_LISTITEM = 1
LVP_LISTGROUP = 2
LVP_LISTDETAIL = 3
LVP_LISTSORTEDDETAIL = 4
LVP_EMPTYTEXT = 5
LVP_GROUPHEADER = 6
LVP_GROUPHEADERLINE = 7
LVP_EXPANDBUTTON = 8
LVP_COLLAPSEBUTTON = 9
LVP_COLUMNDETAIL = 10

LISS_NORMAL = 1
LISS_HOT = 2
LISS_SELECTED = 3
LISS_DISABLED = 4
LISS_SELECTEDNOTFOCUS = 5
LISS_HOTSELECTED = 6

LVGH_OPEN = 1
LVGH_OPENHOT = 2
LVGH_OPENSELECTED = 3
LVGH_OPENSELECTEDHOT = 4
LVGH_OPENSELECTEDNOTFOCUSED = 5
LVGH_OPENSELECTEDNOTFOCUSEDHOT = 6
LVGH_OPENMIXEDSELECTION = 7
LVGH_OPENMIXEDSELECTIONHOT = 8
LVGH_CLOSE = 9
LVGH_CLOSEHOT = 10
LVGH_CLOSESELECTED = 11
LVGH_CLOSESELECTEDHOT = 12
LVGH_CLOSESELECTEDNOTFOCUSED = 13
LVGH_CLOSESELECTEDNOTFOCUSEDHOT = 14
LVGH_CLOSEMIXEDSELECTION = 15
LVGH_CLOSEMIXEDSELECTIONHOT = 16

LVGHL_OPEN = 1
LVGHL_OPENHOT = 2
LVGHL_OPENSELECTED = 3
LVGHL_OPENSELECTEDHOT = 4
LVGHL_OPENSELECTEDNOTFOCUSED = 5
LVGHL_OPENSELECTEDNOTFOCUSEDHOT = 6
LVGHL_OPENMIXEDSELECTION = 7
LVGHL_OPENMIXEDSELECTIONHOT = 8
LVGHL_CLOSE = 9
LVGHL_CLOSEHOT = 10
LVGHL_CLOSESELECTED = 11
LVGHL_CLOSESELECTEDHOT = 12
LVGHL_CLOSESELECTEDNOTFOCUSED = 13
LVGHL_CLOSESELECTEDNOTFOCUSEDHOT = 14
LVGHL_CLOSEMIXEDSELECTION = 15
LVGHL_CLOSEMIXEDSELECTIONHOT = 16

LVEB_NORMAL = 1
LVEB_HOVER = 2
LVEB_PUSHED = 3

LVCB_NORMAL = 1
LVCB_HOVER = 2
LVCB_PUSHED = 3

MENU_MENUITEM_TMSCHEMA = 1
MENU_MENUDROPDOWN_TMSCHEMA = 2
MENU_MENUBARITEM_TMSCHEMA = 3
MENU_MENUBARDROPDOWN_TMSCHEMA = 4
MENU_CHEVRON_TMSCHEMA = 5
MENU_SEPARATOR_TMSCHEMA = 6
MENU_BARBACKGROUND = 7
MENU_BARITEM = 8
MENU_POPUPBACKGROUND = 9
MENU_POPUPBORDERS = 10
MENU_POPUPCHECK = 11
MENU_POPUPCHECKBACKGROUND = 12
MENU_POPUPGUTTER = 13
MENU_POPUPITEM = 14
MENU_POPUPSEPARATOR = 15
MENU_POPUPSUBMENU = 16
MENU_SYSTEMCLOSE = 17
MENU_SYSTEMMAXIMIZE = 18
MENU_SYSTEMMINIMIZE = 19
MENU_SYSTEMRESTORE = 20
MENU_POPUPSUBMENU_HCHOT = 21
MENU_SYSTEMCLOSE_HCHOT = 22
MENU_SYSTEMMAXIMIZE_HCHOT = 23
MENU_SYSTEMMINIMIZE_HCHOT = 24
MENU_SYSTEMRESTORE_HCHOT = 25
MENU_POPUPITEMKBFOCUS = 26
MENU_POPUPITEM_FOCUSABLE = 27

MB_ACTIVE = 1
MB_INACTIVE = 2

MBI_NORMAL = 1
MBI_HOT = 2
MBI_PUSHED = 3
MBI_DISABLED = 4
MBI_DISABLEDHOT = 5
MBI_DISABLEDPUSHED = 6

MC_CHECKMARKNORMAL = 1
MC_CHECKMARKDISABLED = 2
MC_BULLETNORMAL = 3
MC_BULLETDISABLED = 4

MCB_DISABLED = 1
MCB_NORMAL = 2
MCB_BITMAP = 3

MPI_NORMAL = 1
MPI_HOT = 2
MPI_DISABLED = 3
MPI_DISABLEDHOT = 4

MSM_NORMAL = 1
MSM_DISABLED = 2

MSYSC_NORMAL = 1
MSYSC_DISABLED = 2

MSYSMX_NORMAL = 1
MSYSMX_DISABLED = 2

MSYSMN_NORMAL = 1
MSYSMN_DISABLED = 2

MSYSR_NORMAL = 1
MSYSR_DISABLED = 2

MSMHC_HOT = 1

MSYSCHC_HOT = 1

MSYSMXHC_HOT = 1

MSYSMNHC_HOT = 1

MSYSRHC_HOT = 1

MPIKBFOCUS_NORMAL = 1

MPIF_NORMAL = 1
MPIF_HOT = 2
MPIF_DISABLED = 3
MPIF_DISABLEDHOT = 4

NAV_BACKBUTTON = 1
NAV_FORWARDBUTTON = 2
NAV_MENUBUTTON = 3

NAV_BB_NORMAL = 1
NAV_BB_HOT = 2
NAV_BB_PRESSED = 3
NAV_BB_DISABLED = 4

NAV_FB_NORMAL = 1
NAV_FB_HOT = 2
NAV_FB_PRESSED = 3
NAV_FB_DISABLED = 4

NAV_MB_NORMAL = 1
NAV_MB_HOT = 2
NAV_MB_PRESSED = 3
NAV_MB_DISABLED = 4

PP_BAR = 1
PP_BARVERT = 2
PP_CHUNK = 3
PP_CHUNKVERT = 4
PP_FILL = 5
PP_FILLVERT = 6
PP_PULSEOVERLAY = 7
PP_MOVEOVERLAY = 8
PP_PULSEOVERLAYVERT = 9
PP_MOVEOVERLAYVERT = 10
PP_TRANSPARENTBAR = 11
PP_TRANSPARENTBARVERT = 12

PBBS_NORMAL = 1
PBBS_PARTIAL = 2

PBBVS_NORMAL = 1
PBBVS_PARTIAL = 2

PBFS_NORMAL = 1
PBFS_ERROR = 2
PBFS_PAUSED = 3
PBFS_PARTIAL = 4

PBFVS_NORMAL = 1
PBFVS_ERROR = 2
PBFVS_PAUSED = 3
PBFVS_PARTIAL = 4

RP_GRIPPER = 1
RP_GRIPPERVERT = 2
RP_BAND = 3
RP_CHEVRON = 4
RP_CHEVRONVERT = 5
RP_BACKGROUND = 6
RP_SPLITTER = 7
RP_SPLITTERVERT = 8

CHEVS_NORMAL = 1
CHEVS_HOT = 2
CHEVS_PRESSED = 3

CHEVSV_NORMAL = 1
CHEVSV_HOT = 2
CHEVSV_PRESSED = 3

SPLITS_NORMAL = 1
SPLITS_HOT = 2
SPLITS_PRESSED = 3

SPLITSV_NORMAL = 1
SPLITSV_HOT = 2
SPLITSV_PRESSED = 3

SBP_ARROWBTN = 1
SBP_THUMBBTNHORZ = 2
SBP_THUMBBTNVERT = 3
SBP_LOWERTRACKHORZ = 4
SBP_UPPERTRACKHORZ = 5
SBP_LOWERTRACKVERT = 6
SBP_UPPERTRACKVERT = 7
SBP_GRIPPERHORZ = 8
SBP_GRIPPERVERT = 9
SBP_SIZEBOX = 10
SBP_SIZEBOXBKGND = 11

ABS_UPNORMAL = 1
ABS_UPHOT = 2
ABS_UPPRESSED = 3
ABS_UPDISABLED = 4
ABS_DOWNNORMAL = 5
ABS_DOWNHOT = 6
ABS_DOWNPRESSED = 7
ABS_DOWNDISABLED = 8
ABS_LEFTNORMAL = 9
ABS_LEFTHOT = 10
ABS_LEFTPRESSED = 11
ABS_LEFTDISABLED = 12
ABS_RIGHTNORMAL = 13
ABS_RIGHTHOT = 14
ABS_RIGHTPRESSED = 15
ABS_RIGHTDISABLED = 16
ABS_UPHOVER = 17
ABS_DOWNHOVER = 18
ABS_LEFTHOVER = 19
ABS_RIGHTHOVER = 20

SCRBS_NORMAL = 1
SCRBS_HOT = 2
SCRBS_PRESSED = 3
SCRBS_DISABLED = 4
SCRBS_HOVER = 5

SZB_RIGHTALIGN = 1
SZB_LEFTALIGN = 2
SZB_TOPRIGHTALIGN = 3
SZB_TOPLEFTALIGN = 4
SZB_HALFBOTTOMRIGHTALIGN = 5
SZB_HALFBOTTOMLEFTALIGN = 6
SZB_HALFTOPRIGHTALIGN = 7
SZB_HALFTOPLEFTALIGN = 8

SPNP_UP = 1
SPNP_DOWN = 2
SPNP_UPHORZ = 3
SPNP_DOWNHORZ = 4

UPS_NORMAL = 1
UPS_HOT = 2
UPS_PRESSED = 3
UPS_DISABLED = 4

DNS_NORMAL = 1
DNS_HOT = 2
DNS_PRESSED = 3
DNS_DISABLED = 4

UPHZS_NORMAL = 1
UPHZS_HOT = 2
UPHZS_PRESSED = 3
UPHZS_DISABLED = 4

DNHZS_NORMAL = 1
DNHZS_HOT = 2
DNHZS_PRESSED = 3
DNHZS_DISABLED = 4

SP_PANE = 1
SP_GRIPPERPANE = 2
SP_GRIPPER = 3


TABP_TABITEM = 1
TABP_TABITEMLEFTEDGE = 2
TABP_TABITEMRIGHTEDGE = 3
TABP_TABITEMBOTHEDGE = 4
TABP_TOPTABITEM = 5
TABP_TOPTABITEMLEFTEDGE = 6
TABP_TOPTABITEMRIGHTEDGE = 7
TABP_TOPTABITEMBOTHEDGE = 8
TABP_PANE = 9
TABP_BODY = 10
TABP_AEROWIZARDBODY = 11

TIS_NORMAL = 1
TIS_HOT = 2
TIS_SELECTED = 3
TIS_DISABLED = 4
TIS_FOCUSED = 5

TILES_NORMAL = 1
TILES_HOT = 2
TILES_SELECTED = 3
TILES_DISABLED = 4
TILES_FOCUSED = 5

TIRES_NORMAL = 1
TIRES_HOT = 2
TIRES_SELECTED = 3
TIRES_DISABLED = 4
TIRES_FOCUSED = 5

TIBES_NORMAL = 1
TIBES_HOT = 2
TIBES_SELECTED = 3
TIBES_DISABLED = 4
TIBES_FOCUSED = 5

TTIS_NORMAL = 1
TTIS_HOT = 2
TTIS_SELECTED = 3
TTIS_DISABLED = 4
TTIS_FOCUSED = 5

TTILES_NORMAL = 1
TTILES_HOT = 2
TTILES_SELECTED = 3
TTILES_DISABLED = 4
TTILES_FOCUSED = 5

TTIRES_NORMAL = 1
TTIRES_HOT = 2
TTIRES_SELECTED = 3
TTIRES_DISABLED = 4
TTIRES_FOCUSED = 5

TTIBES_NORMAL = 1
TTIBES_HOT = 2
TTIBES_SELECTED = 3
TTIBES_DISABLED = 4
TTIBES_FOCUSED = 5

TDLG_PRIMARYPANEL = 1
TDLG_MAININSTRUCTIONPANE = 2
TDLG_MAINICON = 3
TDLG_CONTENTPANE = 4
TDLG_CONTENTICON = 5
TDLG_EXPANDEDCONTENT = 6
TDLG_COMMANDLINKPANE = 7
TDLG_SECONDARYPANEL = 8
TDLG_CONTROLPANE = 9
TDLG_BUTTONSECTION = 10
TDLG_BUTTONWRAPPER = 11
TDLG_EXPANDOTEXT = 12
TDLG_EXPANDOBUTTON = 13
TDLG_VERIFICATIONTEXT = 14
TDLG_FOOTNOTEPANE = 15
TDLG_FOOTNOTEAREA = 16
TDLG_FOOTNOTESEPARATOR = 17
TDLG_EXPANDEDFOOTERAREA = 18
TDLG_PROGRESSBAR = 19
TDLG_IMAGEALIGNMENT = 20
TDLG_RADIOBUTTONPANE = 21

TDLGCPS_STANDALONE = 1

TDLGEBS_NORMAL = 1
TDLGEBS_HOVER = 2
TDLGEBS_PRESSED = 3
TDLGEBS_EXPANDEDNORMAL = 4
TDLGEBS_EXPANDEDHOVER = 5
TDLGEBS_EXPANDEDPRESSED = 6
TDLGEBS_NORMALDISABLED = 7
TDLGEBS_EXPANDEDDISABLED = 8

TEXT_MAININSTRUCTION = 1
TEXT_INSTRUCTION = 2
TEXT_BODYTITLE = 3
TEXT_BODYTEXT = 4
TEXT_SECONDARYTEXT = 5
TEXT_HYPERLINKTEXT = 6
TEXT_EXPANDED = 7
TEXT_LABEL = 8
TEXT_CONTROLLABEL = 9

TS_HYPERLINK_NORMAL = 1
TS_HYPERLINK_HOT = 2
TS_HYPERLINK_PRESSED = 3
TS_HYPERLINK_DISABLED = 4

TS_CONTROLLABEL_NORMAL = 1
TS_CONTROLLABEL_DISABLED = 2

TP_BUTTON = 1
TP_DROPDOWNBUTTON = 2
TP_SPLITBUTTON = 3
TP_SPLITBUTTONDROPDOWN = 4
TP_SEPARATOR = 5
TP_SEPARATORVERT = 6
TP_DROPDOWNBUTTONGLYPH = 7

TS_NORMAL = 1
TS_HOT = 2
TS_PRESSED = 3
TS_DISABLED = 4
TS_CHECKED = 5
TS_HOTCHECKED = 6
TS_NEARHOT = 7
TS_OTHERSIDEHOT = 8

TTP_STANDARD = 1
TTP_STANDARDTITLE = 2
TTP_BALLOON = 3
TTP_BALLOONTITLE = 4
TTP_CLOSE = 5
TTP_BALLOONSTEM = 6
TTP_WRENCH = 7

TTCS_NORMAL = 1
TTCS_HOT = 2
TTCS_PRESSED = 3

TTSS_NORMAL = 1
TTSS_LINK = 2

TTBS_NORMAL = 1
TTBS_LINK = 2

TTBSS_POINTINGUPLEFTWALL = 1
TTBSS_POINTINGUPCENTERED = 2
TTBSS_POINTINGUPRIGHTWALL = 3
TTBSS_POINTINGDOWNRIGHTWALL = 4
TTBSS_POINTINGDOWNCENTERED = 5
TTBSS_POINTINGDOWNLEFTWALL = 6

TTWS_NORMAL = 1
TTWS_HOT = 2
TTWS_PRESSED = 3

TKP_TRACK = 1
TKP_TRACKVERT = 2
TKP_THUMB = 3
TKP_THUMBBOTTOM = 4
TKP_THUMBTOP = 5
TKP_THUMBVERT = 6
TKP_THUMBLEFT = 7
TKP_THUMBRIGHT = 8
TKP_TICS = 9
TKP_TICSVERT = 10

TKS_NORMAL = 1

TRS_BACKGROUND = 0
TRS_NORMAL = 1
TRS_DISABLED = 2
TRS_SELECTED = 3

TRVS_NORMAL = 1

TUS_NORMAL = 1
TUS_HOT = 2
TUS_PRESSED = 3
TUS_FOCUSED = 4
TUS_DISABLED = 5

TUBS_NORMAL = 1
TUBS_HOT = 2
TUBS_PRESSED = 3
TUBS_FOCUSED = 4
TUBS_DISABLED = 5

TUTS_NORMAL = 1
TUTS_HOT = 2
TUTS_PRESSED = 3
TUTS_FOCUSED = 4
TUTS_DISABLED = 5

TUVS_NORMAL = 1
TUVS_HOT = 2
TUVS_PRESSED = 3
TUVS_FOCUSED = 4
TUVS_DISABLED = 5

TUVLS_NORMAL = 1
TUVLS_HOT = 2
TUVLS_PRESSED = 3
TUVLS_FOCUSED = 4
TUVLS_DISABLED = 5

TUVRS_NORMAL = 1
TUVRS_HOT = 2
TUVRS_PRESSED = 3
TUVRS_FOCUSED = 4
TUVRS_DISABLED = 5

TSS_NORMAL = 1

TSVS_NORMAL = 1

TVP_TREEITEM = 1
TVP_GLYPH = 2
TVP_BRANCH = 3
TVP_HOTGLYPH = 4

TREIS_NORMAL = 1
TREIS_HOT = 2
TREIS_SELECTED = 3
TREIS_DISABLED = 4
TREIS_SELECTEDNOTFOCUS = 5
TREIS_HOTSELECTED = 6

GLPS_CLOSED = 1
GLPS_OPENED = 2

HGLPS_CLOSED = 1
HGLPS_OPENED = 2

UTP_STROKEBACKGROUND = 1
UTP_HOVERBACKGROUND = 2

UTS_NORMAL = 1
UTS_HOT = 2
UTS_PRESSED = 3

TSGP_GRIPPER = 1

TSGS_NORMAL = 1
TSGS_CENTERED = 2

WP_CAPTION = 1
WP_SMALLCAPTION = 2
WP_MINCAPTION = 3
WP_SMALLMINCAPTION = 4
WP_MAXCAPTION = 5
WP_SMALLMAXCAPTION = 6
WP_FRAMELEFT = 7
WP_FRAMERIGHT = 8
WP_FRAMEBOTTOM = 9
WP_SMALLFRAMELEFT = 10
WP_SMALLFRAMERIGHT = 11
WP_SMALLFRAMEBOTTOM = 12
WP_SYSBUTTON = 13
WP_MDISYSBUTTON = 14
WP_MINBUTTON = 15
WP_MDIMINBUTTON = 16
WP_MAXBUTTON = 17
WP_CLOSEBUTTON = 18
WP_SMALLCLOSEBUTTON = 19
WP_MDICLOSEBUTTON = 20
WP_RESTOREBUTTON = 21
WP_MDIRESTOREBUTTON = 22
WP_HELPBUTTON = 23
WP_MDIHELPBUTTON = 24
WP_HORZSCROLL = 25
WP_HORZTHUMB = 26
WP_VERTSCROLL = 27
WP_VERTTHUMB = 28
WP_DIALOG = 29
WP_CAPTIONSIZINGTEMPLATE = 30
WP_SMALLCAPTIONSIZINGTEMPLATE = 31
WP_FRAMELEFTSIZINGTEMPLATE = 32
WP_SMALLFRAMELEFTSIZINGTEMPLATE = 33
WP_FRAMERIGHTSIZINGTEMPLATE = 34
WP_SMALLFRAMERIGHTSIZINGTEMPLATE = 35
WP_FRAMEBOTTOMSIZINGTEMPLATE = 36
WP_SMALLFRAMEBOTTOMSIZINGTEMPLATE = 37
WP_FRAME = 38
WP_BORDER = 39

FS_ACTIVE = 1
FS_INACTIVE = 2

CS_ACTIVE = 1
CS_INACTIVE = 2
CS_DISABLED = 3

MXCS_ACTIVE = 1
MXCS_INACTIVE = 2
MXCS_DISABLED = 3

MNCS_ACTIVE = 1
MNCS_INACTIVE = 2
MNCS_DISABLED = 3

HSS_NORMAL = 1
HSS_HOT = 2
HSS_PUSHED = 3
HSS_DISABLED = 4

HTS_NORMAL = 1
HTS_HOT = 2
HTS_PUSHED = 3
HTS_DISABLED = 4

VSS_NORMAL = 1
VSS_HOT = 2
VSS_PUSHED = 3
VSS_DISABLED = 4

VTS_NORMAL = 1
VTS_HOT = 2
VTS_PUSHED = 3
VTS_DISABLED = 4

SBS_NORMAL = 1
SBS_HOT = 2
SBS_PUSHED = 3
SBS_DISABLED = 4

MINBS_NORMAL = 1
MINBS_HOT = 2
MINBS_PUSHED = 3
MINBS_DISABLED = 4

MAXBS_NORMAL = 1
MAXBS_HOT = 2
MAXBS_PUSHED = 3
MAXBS_DISABLED = 4

RBS_NORMAL = 1
RBS_HOT = 2
RBS_PUSHED = 3
RBS_DISABLED = 4

HBS_NORMAL = 1
HBS_HOT = 2
HBS_PUSHED = 3
HBS_DISABLED = 4

CBS_NORMAL = 1
CBS_HOT = 2
CBS_PUSHED = 3
CBS_DISABLED = 4

SCBS_NORMAL = 1
SCBS_HOT = 2
SCBS_PUSHED = 3
SCBS_DISABLED = 4

FRB_ACTIVE = 1
FRB_INACTIVE = 2

FRL_ACTIVE = 1
FRL_INACTIVE = 2

FRR_ACTIVE = 1
FRR_INACTIVE = 2

SCS_ACTIVE = 1
SCS_INACTIVE = 2
SCS_DISABLED = 3

SFRB_ACTIVE = 1
SFRB_INACTIVE = 2

SFRL_ACTIVE = 1
SFRL_INACTIVE = 2

SFRR_ACTIVE = 1
SFRR_INACTIVE = 2

MDCL_NORMAL = 1
MDCL_HOT = 2
MDCL_PUSHED = 3
MDCL_DISABLED = 4

MDMI_NORMAL = 1
MDMI_HOT = 2
MDMI_PUSHED = 3
MDMI_DISABLED = 4

MDRE_NORMAL = 1
MDRE_HOT = 2
MDRE_PUSHED = 3
MDRE_DISABLED = 4
