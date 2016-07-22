#ifndef _SKEY_WINDOW_H
#	define _SKEY_WINDOW_H

#include "SKeyMessage.h"
#include "SKeyView.h"

#include <Window.h>

class SKeyWindow: public BWindow {
private:
	SKeyView *view;
	BMenuBar *menu_bar;
	
	float AddMenuBar(BRect frame);

public:
	SKeyWindow(BRect frame, const bool md);

	void MessageReceived(BMessage *msg);
	bool QuitRequested();

	void SavePos();
};

#endif
