#include "SKeyMessage.h"
#include "SKeyApp.h"
#include "SKeyWindow.h"

#include <Application.h>
#include <nustorage/Resources.h>

#define ABOUT_MESS "S/Key One Time Password Generator\n\
Copyright" B_UTF8_COPYRIGHT " 1997 Brian Cully <shmit@kublai.com>\n\n\
This program comes with ABSOLUTELY NO WARRANTY. This is free software, \
you are welcome to redistribute it and modify it under the terms of the \
conditions of the GNU General Public License.\n\n\
The RSA Data Security, Inc. MD4 Message-Digest Algorithm and \
RSA Data Security, Inc. MD5 Message-Digest Algorithm are \
Copyright " B_UTF8_COPYRIGHT " 1990-1992 RSA Data Security."

SKeyApp::SKeyApp()
	    : BApplication("application/x-kublai-skey")
{
}

void SKeyApp::AboutRequested()
{
	BAlert *about_box = new BAlert("", ABOUT_MESS, "OK", NULL, NULL, B_WIDTH_AS_USUAL,
								   B_INFO_ALERT);

	if (about_box)
		about_box->Go();
}

void SKeyApp::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
	case SKEY_PREFS_MSG:
		BRect pos;
		bool md;

		// TODO: Make sure this data exists.
		msg->FindRect("position", &pos);
		msg->FindBool("md", &md);
		WritePrefs(pos, md);
		break;
	default:
		BApplication::MessageReceived(msg);
	}
}

bool SKeyApp::QuitRequested()
{
	return TRUE;
}

void SKeyApp::ReadyToRun()
{
	app_info info; 
	BRect win_pos = BRect(100, 100, 500, 210);
	bool md = TRUE;

	if (GetAppInfo(&info) != B_OK) {
		BAlert *oops = new BAlert("", "Can't find myself!", "Argh", NULL,
								   NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT);
					
		oops->Go();
		return;
	}
	BFile *app_file = new BFile;
    if (app_file->SetTo(&info.ref, O_RDWR) != B_OK) {
		BAlert *oops = new BAlert("", "Can't open myself!", "Argh", NULL,
								   NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT);
					
		oops->Go();
		return;
	}
	BResources *app_res = new BResources;
    app_res->SetTo(app_file);

	if (app_res->HasResource(B_RECT_TYPE, SKEY_WINPOS_RES))
		// TODO: sanity check the results of the read.
		app_res->ReadResource(B_RECT_TYPE, SKEY_WINPOS_RES, &win_pos,
							   0, sizeof(win_pos));
	else
		app_res->AddResource(B_RECT_TYPE, SKEY_WINPOS_RES,
							  &win_pos, sizeof(win_pos));

	if (app_res->HasResource(B_BOOL_TYPE, SKEY_MD_RES)) {
		app_res->ReadResource(B_BOOL_TYPE, SKEY_MD_RES, &md, 0, sizeof(md));
	} else
		app_res->AddResource(B_BOOL_TYPE, SKEY_MD_RES, &md, sizeof(md));

	delete app_file; delete app_res;
	SKeyWindow *win = new SKeyWindow(win_pos, md);
}

void SKeyApp::WritePrefs(BRect frame, bool md)
{
	app_info info; 

	if (GetAppInfo(&info) != B_OK)
		return;

	BFile *app_file = new BFile;
	if (app_file->SetTo(&info.ref, O_RDWR) != B_OK) {
		delete app_file;
		return;
	}
	BResources *app_res = new BResources;
	if (app_res->SetTo(app_file) != B_OK) {
		delete app_file; delete app_res;
		return;
	}
	app_res->WriteResource(B_RECT_TYPE, SKEY_WINPOS_RES, &frame, 0,
							sizeof(frame));
	app_res->WriteResource(B_BOOL_TYPE, SKEY_MD_RES, &md, 0,
							sizeof(md));
	delete app_file;
	delete app_res;
}