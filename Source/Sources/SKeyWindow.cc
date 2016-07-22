#include "SKeyMessage.h"
#include "SKeyWindow.h"
#include "SKeyView.h"

#include <Window.h>
#include <nustorage/Resources.h>
#include <app/Application.h>

float SKeyWindow::AddMenuBar(BRect frame)
{
	menu_bar = new BMenuBar(frame, "window menu");
	
	// Create Edit menu.
	BMenu *file = new BMenu("File");
	BMenuItem *item = new BMenuItem("About S/Key…",
									 new BMessage(B_ABOUT_REQUESTED));
	item->SetTarget(be_app);
	file->AddItem(item);
	file->AddSeparatorItem();
	item = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q');
	item->SetTarget(this);
	file->AddItem(item);
	BMenu *edit = new BMenu("Edit");
	item = new BMenuItem("Copy S/Key Password",
						  new BMessage(B_COPY), 'C');
	item->SetTarget(this);
	edit->AddItem(item);
	item = new BMenuItem("Paste Seed", new BMessage(B_PASTE), 'V');
	item->SetTarget(this);
	edit->AddItem(item);

	menu_bar->AddItem(file);
	menu_bar->AddItem(edit);
	AddChild(menu_bar);

	Lock();
	float ret_val = menu_bar->Bounds().bottom;
	Unlock();

	return ret_val;
}

SKeyWindow::SKeyWindow(BRect frame, const bool md)
		   : BWindow(frame, "S/Key", B_TITLED_WINDOW,
					 B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	frame.OffsetTo(B_ORIGIN);

	// Add the menu bar.
	BRect view_frame = BRect(0, 0, 0, 0);

	view_frame.top = AddMenuBar(view_frame);
	view_frame.bottom = frame.bottom;
	view_frame.left = 0; view_frame.right = frame.right;

	// Add the view.
	view = new SKeyView(view_frame, md);
	AddChild(view);
	Show();
}

void SKeyWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
	case MD4_MSG:
	case MD5_MSG:
	case B_COPY:
	case B_PASTE:
	case SKEY_COMP_KEY:
		view->MessageReceived(msg);
		break;
	default:
		BWindow::MessageReceived(msg);
	}
}

bool SKeyWindow::QuitRequested()
{
	SavePos();
	be_app->PostMessage(B_QUIT_REQUESTED);
	return TRUE;
}

void SKeyWindow::SavePos()
{
	BMessage msg(SKEY_PREFS_MSG);
	msg.AddRect("position", Frame());
	msg.AddBool("md", view->GetMD());
	be_app->PostMessage(&msg);
}