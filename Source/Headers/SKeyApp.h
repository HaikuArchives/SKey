#ifndef _SKEY_APP_H
#	define _SKEY_APP_H

#include "SKeyMessage.h"
#include "SKeyWindow.h"

#include <Application.h>
#include <nustorage/Resources.h>

class SKeyApp: public BApplication {
public:
	SKeyApp();
	
	void AboutRequested();
	void MessageReceived(BMessage *msg);
	bool QuitRequested();
	void ReadyToRun();

	void WritePrefs(BRect frame, bool md);
};

#endif
