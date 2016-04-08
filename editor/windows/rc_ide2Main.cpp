/***************************************************************
 * Name:      rc_ide2Main.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2015-08-18
 * Copyright:  ()
 * License:
 **************************************************************/

//#include "wx_pch.h"
#include "rc_ide2Main.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/stdpaths.h>
#include <wx/stc/stc.h>
#include <wx/utils.h>

//(*InternalHeaders(rc_ide2Frame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(rc_ide2Frame)
const long rc_ide2Frame::ID_AUINOTEBOOK1 = wxNewId();
const long rc_ide2Frame::newID = wxNewId();
const long rc_ide2Frame::openID = wxNewId();
const long rc_ide2Frame::saveID = wxNewId();
const long rc_ide2Frame::saveAsID = wxNewId();
const long rc_ide2Frame::idMenuQuit = wxNewId();
const long rc_ide2Frame::undoID = wxNewId();
const long rc_ide2Frame::redoID = wxNewId();
const long rc_ide2Frame::cutID = wxNewId();
const long rc_ide2Frame::copyID = wxNewId();
const long rc_ide2Frame::pasteID = wxNewId();
const long rc_ide2Frame::deleteID = wxNewId();
const long rc_ide2Frame::compileID = wxNewId();
const long rc_ide2Frame::runID = wxNewId();
const long rc_ide2Frame::idMenuAbout = wxNewId();
const long rc_ide2Frame::referenceID = wxNewId();
const long rc_ide2Frame::ID_STATUSBAR1 = wxNewId();
const long rc_ide2Frame::toolNewID = wxNewId();
const long rc_ide2Frame::toolOpenID = wxNewId();
const long rc_ide2Frame::toolSaveID = wxNewId();
const long rc_ide2Frame::toolSaveAsID = wxNewId();
const long rc_ide2Frame::toolRunID = wxNewId();
const long rc_ide2Frame::ID_TOOLBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(rc_ide2Frame,wxFrame)
    //(*EventTable(rc_ide2Frame)
    //*)
END_EVENT_TABLE()

int rc_current_page = 0;
wxString rc_fnames[999];
wxString rc_path = _("");//_("/home/cunningham/Desktop/alt_platforms/rc_ide/rc_ide/bin/Debug/");
wxString rc_keywords = _("");
wxFont rc_font;
int rc_nline = 0;

void rc_initKeywords()
{
    rc_keywords = wxT("mod and or xor shl shr not true false text_input text_output text_append text_input_plus text_output_plus text_append_plus ");
    rc_keywords += wxT("binary_input binary_output binary_append binary_input_plus binary_output_plus binary_append_plus ");
    rc_keywords += wxT("if then else elseif end select case default do loop until while wend for to step next exit function return sub byref print dim ");
    rc_keywords += wxT("fprint input input$ arraydim arraysize delete asc chr chr$ insert insert$ instr lcase lcase$ left left$ length ltrim ltrim$ mid mid$ ");
    rc_keywords += wxT("replace replace$ replacesubstr replacesubstr$ reverse reverse$ right right$ rtrim rtrim$ fillstring fillstring$ str str$ tally trim trim$ ucase ucase$ val ");
    rc_keywords += wxT("abs acos andbit asin atan bin bin$ cos exp frac hex hex$ int log max min orbit randomize rand round sign sin sqrt tan xorbit ");
    rc_keywords += wxT("fileopen fileclose readbyte writebyte readline readline$ writeline copyfile removefile fileexists movefile renamefile filelength tell seek eof freefile ");
    rc_keywords += wxT("changedir direxists dirfirst dirfirst$ dir dir$ dirnext dirnext$ makedir removedir ");
    rc_keywords += wxT("date date$ ticks time time$ timer wait ");
    rc_keywords += wxT("windowopen windowclose raisewindow window update showwindow hidewindow setwindowtitle windowtitle windowtitle$ setwindowposition getwindowposition setwindowsize getwindowsize ");
    rc_keywords += wxT("setwindowminsize getwindowminsize setwindowmaxsize getwindowmaxsize windowisfullscreen windowisvisible windowisbordered windowisresizable windowisminimized ");
    rc_keywords += wxT("windowismaximized setwindowfullscreen maximizewindow minimizewindow setwindowborder ");
    rc_keywords += wxT("canvasopen canvasclose setcanvasviewport getcanvasviewport canvas setcanvasoffset getcanvasoffset getcanvassize clearcanvas setcanvasalpha copycanvas clonecanvas ");
    rc_keywords += wxT("setcanvasz box boxfill circle circlefill ellipse ellipsefill floodfill pixel setcolor line rgb rgba pset loadimage imageexists colorkey copyimage deleteimage setimagealpha ");
    rc_keywords += wxT("getimagealpha getimagesize flipimage drawimage drawimage_blit drawimage_blit_ex drawimage_rotate drawimage_rotate_ex drawimage_zoom drawimage_zoom_ex drawimage_rotozoom drawimage_rotozoom_ex ");
    rc_keywords += wxT("getcursor prints inputs inputs$ locate fontisloaded font loadfont deletefont setfontstyle drawtext drawtext_shaded drawtext_blended inkey key waitkey hidemouse showmouse mouseisvisible getmouse ");
    rc_keywords += wxT("getmousewheel numjoysticks joyaxis joybutton joyname joyname$ loadsound loadmusic playsound playsoundtimed playmusic pausesound resumesound pausemusic resumemusic ");
    rc_keywords += wxT("deletesound fademusicin fademusicout musicexists setmusicvolume musicvolume setmusicposition musicposition rewindmusic setsoundchannels numsoundchannels soundisenabled soundexists ");
    rc_keywords += wxT("setchannelvolume channelvolume setsoundvolume soundvolume stopmusic stopsound ");
    rc_keywords += wxT("tcp_socketopen tcp_socketclose tcp_remotehost tcp_remoteport tcp_getdata tcp_senddata tcp_acceptsocket ");
    rc_keywords += wxT("udp_socketopen udp_socketclose udp_getdata udp_length udp_maxlength udp_remotehost udp_remotehost$ udp_remoteport udp_senddata ");
    rc_keywords += wxT("zoneinput zoneinput$ touchpressure gettouch getmultitouch readinput_start readinput_stop readinput_text readinput_text$ canvasclip windowclip ");
    rc_keywords += wxT("setcanvasvisible canvasisvisible rendertext gettextsize createimage createimage_ex localip localip$ ");
    rc_keywords += wxT("readinput_settext gettouchfinger numfingers loadimage_ex rect rectfill os os$ numjoybuttons numjoyaxes ");
    rc_keywords += wxT("loadvideo playvideo pausevideo stopvideo setvideoposition resumevideo videoposition deletevideo deletemusic ");
    rc_keywords += wxT("system videoisplaying roundrect roundrectfill videoend getvideostats setvideodrawrect getvideodrawrect ");
    rc_keywords += wxT("getvideosize videoexists setvideoalpha createsound command command$ numcommands str_f str_f$ str_s str_s$ cls env env$ setenv prefpath prefpath$ ");
}

rc_ide2Frame::rc_ide2Frame(wxWindow* parent,wxWindowID id)
{
    rc_initKeywords();
    wxFont rc_font(12,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
    rc_path =  wxStandardPaths::Get().GetExecutablePath();
    rc_path = rc_path.substr(0, rc_path.find_last_of(_("\\"))) +_("\\");
    //(*Initialize(rc_ide2Frame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;

    Create(parent, id, _("RC Basic"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetClientSize(wxSize(640,480));
    SetMinSize(wxSize(640,480));
    SetExtraStyle( GetExtraStyle() | wxFRAME_EX_METAL );
    {
    	wxIcon FrameIcon;
    	FrameIcon.CopyFromBitmap(wxBitmap(wxImage(_T("C:\\Users\\Recademics\\Desktop\\rc_basic_win\\ide\\rc_ide2\\wxsmith\\rcbasic.ico"))));
    	SetIcon(FrameIcon);
    }
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    AuiNotebook1 = new wxAuiNotebook(this, ID_AUINOTEBOOK1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE);
    BoxSizer1->Add(AuiNotebook1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
    SetSizer(BoxSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem6 = new wxMenuItem(Menu1, newID, _("New"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem6);
    MenuItem5 = new wxMenuItem(Menu1, openID, _("Open"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem5);
    MenuItem4 = new wxMenuItem(Menu1, saveID, _("Save"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    MenuItem3 = new wxMenuItem(Menu1, saveAsID, _("Save As"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem7 = new wxMenuItem(Menu3, undoID, _("Undo"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem7);
    MenuItem8 = new wxMenuItem(Menu3, redoID, _("Redo"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem8);
    Menu3->AppendSeparator();
    MenuItem9 = new wxMenuItem(Menu3, cutID, _("Cut"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem9);
    MenuItem10 = new wxMenuItem(Menu3, copyID, _("Copy"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem10);
    MenuItem11 = new wxMenuItem(Menu3, pasteID, _("Paste"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem11);
    MenuItem12 = new wxMenuItem(Menu3, deleteID, _("Delete"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem12);
    MenuBar1->Append(Menu3, _("Edit"));
    Menu4 = new wxMenu();
    MenuItem13 = new wxMenuItem(Menu4, compileID, _("Compile"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem13);
    MenuItem14 = new wxMenuItem(Menu4, runID, _("Run"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem14);
    MenuBar1->Append(Menu4, _("Build"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuItem15 = new wxMenuItem(Menu2, referenceID, _("Reference"), wxEmptyString, wxITEM_NORMAL);
    Menu2->Append(MenuItem15);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER, _T("ID_TOOLBAR1"));
    ToolBarItem1 = ToolBar1->AddTool(toolNewID, _("New File"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_NEW")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, _("New File"));
    ToolBarItem2 = ToolBar1->AddTool(toolOpenID, _("Open File"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, _("Open File"));
    ToolBarItem3 = ToolBar1->AddTool(toolSaveID, _("Save File"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_SAVE")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    ToolBarItem4 = ToolBar1->AddTool(toolSaveAsID, _("Save File As"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_SAVE_AS")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, _("Save File As"));
    ToolBar1->AddSeparator();
    ToolBarItem5 = ToolBar1->AddTool(toolRunID, _("Run Program"), wxBitmap(wxImage(_T("C:\\Users\\Recademics\\Desktop\\rc_basic_win\\ide\\rc_ide2\\wxsmith\\player_play.png"))), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, _("Run Program"));
    ToolBar1->Realize();
    SetToolBar(ToolBar1);
    SetSizer(BoxSizer1);
    Layout();
    Center();

    Connect(newID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnNewPage);
    Connect(openID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnPageOpen);
    Connect(saveID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::onSavePage);
    Connect(saveAsID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnSavePageAs);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnQuit);
    Connect(undoID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnUndo);
    Connect(redoID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnRedo);
    Connect(cutID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnCut);
    Connect(copyID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnCopy);
    Connect(pasteID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnPaste);
    Connect(deleteID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnDelete);
    Connect(compileID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnCompile);
    Connect(runID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnRun);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnAbout);
    Connect(referenceID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&rc_ide2Frame::OnReference);
    Connect(toolNewID,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&rc_ide2Frame::OnNewPage);
    Connect(toolOpenID,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&rc_ide2Frame::OnPageOpen);
    Connect(toolSaveID,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&rc_ide2Frame::onSavePage);
    Connect(toolSaveAsID,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&rc_ide2Frame::OnSavePageAs);
    Connect(toolRunID,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&rc_ide2Frame::OnRun);
    //*)
}

rc_ide2Frame::~rc_ide2Frame()
{
    //(*Destroy(rc_ide2Frame)
    //*)
}

void rc_ide2Frame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void rc_ide2Frame::OnAbout(wxCommandEvent& event)
{
    wxString msg = _("RC BASIC v2.0\nCopyright (C) 2015-2016 Rodney Cunningham\n\nFor latest release, updates, and info go to http://www.rcbasic.com");
    wxMessageBox(msg);
}

void rc_ide2Frame::OnNewPage(wxCommandEvent& event)
{
    wxString txtID_str = _("ID_RC_TXT") + wxString::Format(wxT("%i"),rc_current_page);
    rc_current_page++;
    //wxTextCtrl * rc_txtCtrl = new wxTextCtrl(AuiNotebook1, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_DONTWRAP, wxDefaultValidator, txtID_str);
    wxStyledTextCtrl * rc_txtCtrl = new wxStyledTextCtrl(AuiNotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, txtID_str);
    rc_txtCtrl->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    rc_txtCtrl->SetMarginWidth(0, 40);
    rc_txtCtrl->SetUndoCollection(true);
    rc_txtCtrl->EmptyUndoBuffer();
    rc_txtCtrl->SetLexer(wxSTC_LEX_POWERBASIC);
    rc_txtCtrl->StyleSetForeground(wxSTC_B_KEYWORD, wxColour(0, 0, 200));
    rc_txtCtrl->StyleSetForeground(wxSTC_B_NUMBER, wxColour(200, 40, 40));
    rc_txtCtrl->StyleSetForeground(wxSTC_B_STRING, wxColour(0, 140, 0));
    rc_txtCtrl->StyleSetForeground(wxSTC_B_COMMENT, wxColour(128,128,128));
    rc_txtCtrl->SetKeyWords(0, rc_keywords);
    wxFont rc_font(12,wxFONTFAMILY_TELETYPE,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
    rc_txtCtrl->StyleSetFont(wxSTC_STYLE_DEFAULT, rc_font);
    rc_txtCtrl->SetTabWidth(4);
    rc_txtCtrl->Connect(wxEVT_STC_CHARADDED, wxStyledTextEventHandler(rc_ide2Frame::OnDocumentKey), NULL, this);
    AuiNotebook1->AddPage(rc_txtCtrl, _("Untitled"));
    rc_fnames[AuiNotebook1->GetPageIndex(rc_txtCtrl)] = _("");
}

void rc_ide2Frame::OnPageOpen(wxCommandEvent& event)
{
    wxFileDialog * FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_OPEN, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    FileDialog1->ShowModal();
    wxString rc_filename = FileDialog1->GetDirectory() + _("\\") + FileDialog1->GetFilename();
    //wxMessageBox(rc_filename);
    if(FileDialog1->GetReturnCode() == wxID_CANCEL)
    {
        FileDialog1->Destroy();
        return;
    }

    FileDialog1->Destroy();


    if(rc_filename.compare(_("\\"))!=0)
    {
        wxFileInputStream input(rc_filename);

        if(input.IsOk())
        {
            wxString txtID_str = _("ID_RC_TXT") + wxString::Format(wxT("%i"),rc_current_page);
            rc_current_page++;
            wxStyledTextCtrl * rc_txtCtrl = new wxStyledTextCtrl(AuiNotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, txtID_str);

            wxTextInputStream text(input);

            long i = 0;
            rc_txtCtrl->GotoPos(0);
            rc_txtCtrl->GotoLine(i);
            //wxString line=text.ReadLine();

            while(input.IsOk() && !input.Eof() )
            {
                //rc_txtCtrl->GotoPos(0);
                //rc_txtCtrl->GotoLine(i);
                wxString line=text.ReadLine();
                if(input.Eof())
                    break;
                if(i > 0)
                    rc_txtCtrl->AddText( _("\r\n"));
                rc_txtCtrl->GotoPos(0);
                rc_txtCtrl->GotoLine(i);
                int spos = line.find_last_not_of("\r")+1;
                line = line.substr(0, line.find_last_not_of("\r")+1);
                rc_txtCtrl->AddText(line);
                i++;
            }

            rc_txtCtrl->SetMarginType(0, wxSTC_MARGIN_NUMBER);
            rc_txtCtrl->SetMarginWidth(0, 40);
            rc_txtCtrl->SetUndoCollection(true);
            rc_txtCtrl->EmptyUndoBuffer();
            rc_txtCtrl->SetLexer(wxSTC_LEX_POWERBASIC);
            rc_txtCtrl->StyleSetForeground(wxSTC_B_KEYWORD, wxColour(0, 0, 200));
            rc_txtCtrl->StyleSetForeground(wxSTC_B_NUMBER, wxColour(200, 40, 40));
            rc_txtCtrl->StyleSetForeground(wxSTC_B_STRING, wxColour(0, 140, 0));
            rc_txtCtrl->StyleSetForeground(wxSTC_B_COMMENT, wxColour(128,128,128));
            rc_txtCtrl->SetKeyWords(0, rc_keywords);
            wxFont rc_font(12,wxFONTFAMILY_TELETYPE,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
            rc_txtCtrl->StyleSetFont(wxSTC_STYLE_DEFAULT, rc_font);
            rc_txtCtrl->SetTabWidth(4);
            rc_txtCtrl->Connect(wxEVT_STC_CHARADDED, wxStyledTextEventHandler(rc_ide2Frame::OnDocumentKey), NULL, this);
            AuiNotebook1->AddPage(rc_txtCtrl, FileDialog1->GetFilename());
            rc_fnames[AuiNotebook1->GetPageIndex(rc_txtCtrl)] = rc_filename;
        }
    }
}

void rc_ide2Frame::onSavePage(wxCommandEvent& event)
{
    if(AuiNotebook1->GetSelection() < 0)
    {
        wxMessageBox(_("Nothing to Save"));
        return;
    }
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    //wxMessageBox(t->GetLabelText(),_("test"));


    if(rc_fnames[AuiNotebook1->GetPageIndex(t)].compare(_(""))!=0)
    {
        wxFileOutputStream output(rc_fnames[AuiNotebook1->GetPageIndex(t)]);

        if(output.IsOk())
        {
            wxTextOutputStream text(output);
            long i = 0;
            while(i < t->GetLineCount())
            {
                wxString line = t->GetLine(i);
                line = line.substr(0, line.find_first_of("\r")) + _("\n");
                text.WriteString(line);
                i++;
            }
            output.Close();
        }
    }
    else
    {
        wxFileDialog * FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
        FileDialog1->ShowModal();
        wxString rc_filename = FileDialog1->GetDirectory() + _("\\") + FileDialog1->GetFilename();
        //wxMessageBox(rc_filename);

        if(FileDialog1->GetReturnCode() == wxID_CANCEL)
        {
            FileDialog1->Destroy();
            return;
        }

        FileDialog1->Destroy();

        if(rc_filename.compare(_("\\"))!=0)
        {
            wxFileOutputStream output(rc_filename);

            if(!output.IsOk())
            {
                output.Close();
                return;
            }

            wxTextOutputStream text(output);
            long i = 0;
            while(i < t->GetLineCount())
            {
                wxString line = t->GetLine(i);
                line = line.substr(0, line.find_first_of("\r")) + _("\n");
                text.WriteString(line);
                i++;
            }
            output.Close();
            rc_fnames[AuiNotebook1->GetPageIndex(t)] = rc_filename;
            AuiNotebook1->SetPageText(AuiNotebook1->GetPageIndex(t), FileDialog1->GetFilename());
        }

    }
}

void rc_ide2Frame::OnSavePageAs(wxCommandEvent& event)
{
    if(AuiNotebook1->GetSelection() < 0)
    {
        wxMessageBox(_("Nothing to Save"));
        return;
    }
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    //wxMessageBox(t->GetLabelText(),_("test"));

    wxFileDialog * FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr,wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    FileDialog1->ShowModal();
    wxString rc_filename = FileDialog1->GetDirectory() + _("\\") + FileDialog1->GetFilename();
    //wxMessageBox(rc_filename);

    if(FileDialog1->GetReturnCode() == wxID_CANCEL)
    {
        FileDialog1->Destroy();
        return;
    }

    FileDialog1->Destroy();

    if(rc_filename.compare(_("\\"))!=0)
    {
        wxFileOutputStream output(rc_filename);

        if(!output.IsOk())
        {
            output.Close();
            return;
        }

        wxTextOutputStream text(output);
        long i = 0;
        while(i < t->GetLineCount())
        {
            wxString line = t->GetLine(i);
            line = line.substr(0, line.find_first_of("\r")) + _("\n");
            text.WriteString(line);
            i++;
        }
        output.Close();
        rc_fnames[AuiNotebook1->GetPageIndex(t)] = rc_filename;
        AuiNotebook1->SetPageText(AuiNotebook1->GetPageIndex(t), FileDialog1->GetFilename());
    }
}

void rc_ide2Frame::OnUndo(wxCommandEvent& event)
{
    if(AuiNotebook1->GetSelection() < 0)
    {
        return;
    }
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    t->Undo();
}

void rc_ide2Frame::OnRedo(wxCommandEvent& event)
{
    if(AuiNotebook1->GetSelection() < 0)
    {
        return;
    }
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    t->Redo();
}

void rc_ide2Frame::OnCut(wxCommandEvent& event)
{
    if(AuiNotebook1->GetSelection() < 0)
    {
        return;
    }
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    t->Cut();
}

void rc_ide2Frame::OnCopy(wxCommandEvent& event)
{
    if(AuiNotebook1->GetSelection() < 0)
    {
        return;
    }
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    t->Copy();
}

void rc_ide2Frame::OnPaste(wxCommandEvent& event)
{
    if(AuiNotebook1->GetSelection() < 0)
    {
        return;
    }
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    t->Paste();
}

void rc_ide2Frame::OnDelete(wxCommandEvent& event)
{
    if(AuiNotebook1->GetSelection() < 0)
    {
        return;
    }
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    t->Clear();
}

void rc_ide2Frame::OnCompile(wxCommandEvent& event)
{
    //wxMessageBox(wxString::Format(wxT("%i"),AuiNotebook1->GetSelection()),_("WX AUI"));
    if(AuiNotebook1->GetSelection() < 0)
    {
        wxMessageBox(_("Nothing to Compile"));
        return;
    }
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    if(t == NULL)
        return;
    if(rc_fnames[AuiNotebook1->GetSelection()].compare(_(""))==0)
    {
        wxMessageDialog * MessageDialog1 = new wxMessageDialog(this, _("--You must save the file first."), wxEmptyString , wxOK, wxDefaultPosition);
        wxFileDialog * FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
        if(MessageDialog1->ShowModal() == wxID_OK)
        {
            //wxTextCtrl * t = (wxTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
            wxFileDialog * FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
            FileDialog1->ShowModal();
            wxString rc_filename = FileDialog1->GetDirectory() + _("\\") + FileDialog1->GetFilename();
            //wxMessageBox(rc_filename);
            FileDialog1->Destroy();

            if(rc_filename.compare(_("\\"))!=0)
            {
                wxFileOutputStream output(rc_filename);

                if(!output.IsOk())
                {
                    output.Close();
                    return;
                }

                wxTextOutputStream text(output);
                long i = 0;
                while(i < t->GetLineCount())
                {
                    wxString line = t->GetLine(i);
                    line = line.substr(0, line.find_first_of("\r")) + _("\n");
                    text.WriteString(line);
                    i++;
                }
                output.Close();
                rc_fnames[AuiNotebook1->GetPageIndex(t)] = rc_filename;
                AuiNotebook1->SetPageText(AuiNotebook1->GetPageIndex(t), FileDialog1->GetFilename());
            }
        }
        MessageDialog1->Destroy();
        FileDialog1->Destroy();
    }
    else
    {
        //wxMessageBox(_("File to Run = ") + rc_fnames[AuiNotebook1->GetSelection()]);
        wxFileOutputStream output(rc_fnames[AuiNotebook1->GetPageIndex(t)]);

        if(!output.IsOk())
        {
            output.Close();
            return;
        }

        wxTextOutputStream text(output);
        long i = 0;
        while(i < t->GetLineCount())
        {
            wxString line = t->GetLine(i);
            line = line.substr(0, line.find_first_of("\r")) + _("\n");
            text.WriteString(line);
            i++;
        }
        output.Close();
    }

    wxString fs = rc_path + _("rbc.exe ") + _("\"") + rc_fnames[AuiNotebook1->GetPageIndex(t)] + _("\"");

    //wxMessageBox(fs);
    //wxMessageBox(rc_path + _("rb_rt.exe"));

    int rt = -1;
    //wxString cv_rt;
    rt = wxExecute(fs, wxEXEC_SYNC);
}

void rc_ide2Frame::OnRun(wxCommandEvent& event)
{
    //wxMessageBox(wxString::Format(wxT("%i"),AuiNotebook1->GetSelection()),_("WX AUI"));
    if(AuiNotebook1->GetSelection() < 0)
    {
        wxMessageBox(_("Nothing to Run"));
        return;
    }
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    if(t == NULL)
        return;
    if(rc_fnames[AuiNotebook1->GetSelection()].compare(_(""))==0)
    {
        wxMessageDialog * MessageDialog1 = new wxMessageDialog(this, _("--You must save the file first."), wxEmptyString , wxOK, wxDefaultPosition);
        wxFileDialog * FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
        if(MessageDialog1->ShowModal() == wxID_OK)
        {
            //wxTextCtrl * t = (wxTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
            wxFileDialog * FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
            FileDialog1->ShowModal();
            wxString rc_filename = FileDialog1->GetDirectory() + _("\\") + FileDialog1->GetFilename();
            //wxMessageBox(rc_filename);
            FileDialog1->Destroy();

            if(rc_filename.compare(_("\\"))!=0)
            {
                wxFileOutputStream output(rc_filename);

                if(!output.IsOk())
                {
                    output.Close();
                    return;
                }

                wxTextOutputStream text(output);
                long i = 0;
                while(i < t->GetLineCount())
                {
                    wxString line = t->GetLine(i);
                    line = line.substr(0, line.find_first_of("\r")) + _("\n");
                    text.WriteString(line);
                    i++;
                }
                output.Close();
                rc_fnames[AuiNotebook1->GetPageIndex(t)] = rc_filename;
                AuiNotebook1->SetPageText(AuiNotebook1->GetPageIndex(t), FileDialog1->GetFilename());
            }
        }
        MessageDialog1->Destroy();
        FileDialog1->Destroy();
    }
    else
    {
        //wxMessageBox(_("File to Run = ") + rc_fnames[AuiNotebook1->GetSelection()]);
        wxFileOutputStream output(rc_fnames[AuiNotebook1->GetPageIndex(t)]);

        if(!output.IsOk())
        {
            output.Close();
            return;
        }

        wxTextOutputStream text(output);
        long i = 0;
        while(i < t->GetLineCount())
        {
            wxString line = t->GetLine(i);
            line = line.substr(0, line.find_first_of("\r")) + _("\n");
            text.WriteString(line);
            i++;
        }
        output.Close();
    }

    wxString fs = rc_path + _("rbc.exe ") + _("\"") + rc_fnames[AuiNotebook1->GetPageIndex(t)] + _("\"");

    //wxMessageBox(fs);
    //wxMessageBox(rc_path + _("rb_rt.exe"));

    int rt = -1;
    //wxString cv_rt;
    rt = wxExecute(fs, wxEXEC_SYNC);
    if(rt != 0)
        return;
    //wxMessageBox(wxString::Format(wxT("%i"),rt));
    //cv_rt << rt;
    //wxMessageBox(_("Return: ") + cv_rt);
    wxString fc = rc_fnames[AuiNotebook1->GetPageIndex(t)];
    fc = fc.substr(0, fc.find_last_of(_("."))) + _(".cbc");
    wxString fcc = fc;
    fc = rc_path + _("rb_rt.exe");// + fc;
    //wxMessageBox(fs,_("FS"));
    //wxMessageBox(fc,_("FC"));
    //wxMessageBox(rc_fnames[AuiNotebook1->GetPageIndex(t)].substr(0, rc_fnames[AuiNotebook1->GetPageIndex(t)].find_last_of(_("/"))));
    //wxShell(fc);
    //wxExecute(_("/home/cunningham/Desktop/test_runtime/bin/Debug/test_runtime"), wxEXEC_NOHIDE);
    wxString rc_cwd = rc_fnames[AuiNotebook1->GetPageIndex(t)].substr(0, rc_fnames[AuiNotebook1->GetPageIndex(t)].find_last_of(_("/")));
    //wxSetWorkingDirectory(_("/home/cunningham/Desktop/test_runtime/bin/Debug/"));
    //wxSetWorkingDirectory(rc_cwd);
    //wxMessageBox(rc_cwd,_("Directory"));

//    wxFileOutputStream rc_dtr(rc_path + _("rc_dtr"));
//    wxTextOutputStream text(rc_dtr);
//    text.WriteString(fcc);
//    rc_dtr.Close();

    fs = rc_path + _("rb_rt.exe ") + _("\"") + rc_fnames[AuiNotebook1->GetPageIndex(t)].substr(0,rc_fnames[AuiNotebook1->GetPageIndex(t)].find_first_of(".")) + _(".cbc\"");
    //wxMessageBox(rc_fnames[AuiNotebook1->GetPageIndex(t)].substr(0, rc_fnames[AuiNotebook1->GetPageIndex(t)].find_last_of("\\")), _("runtime"));
    wxSetWorkingDirectory(rc_fnames[AuiNotebook1->GetPageIndex(t)].substr(0, rc_fnames[AuiNotebook1->GetPageIndex(t)].find_last_of("\\")));
    //wxSetWorkingDirectory(rc_path);
    wxExecute(fs, wxEXEC_SYNC);
    //wxShell(rc_path + _("rb_rt ") + fc);
}

void rc_ide2Frame::OnReference(wxCommandEvent& event)
{
    //wxMessageBox(_("Refer to the documentation in rc_basic\\docs"));
    wxLaunchDefaultBrowser(rc_path+_("rc_basic_doc\\rc_basic_man.html"));
}

void rc_ide2Frame::OnDocumentKey(wxStyledTextEvent& event)
{
    wxStyledTextCtrl * t = (wxStyledTextCtrl*)AuiNotebook1->GetPage(AuiNotebook1->GetSelection());
    char chr = event.GetKey();
    int currentLine = t->GetCurrentLine();
    int indent = 0;

    if(chr == '\n')
    {
        //wxMessageBox(_("Current Line = ") + wxString::Format("%i",currentLine));
        if(currentLine > 0)
        {
            indent = t->GetLineIndentation(currentLine-1);
        }
        if(indent == 0)
            return;
        //wxMessageBox(_("Position from line = ") + wxString::Format("%i",t->PositionFromLine(currentLine)));
        t->SetLineIndentation(currentLine, indent);
        t->GotoPos(t->GetLineIndentPosition(currentLine));
    }

}
