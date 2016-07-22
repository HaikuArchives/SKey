#ifndef _SKEY_VIEW_H
#	define _SKEY_VIEW_H

#include "SKeyMessage.h"

#include <interface/Font.h>
#include <interface/View.h>
#include <app/MessageFilter.h>
#include <kernel/OS.h>

#define SEED_PROMPT		"Enter Seed:"
#define KEY_PROMPT		"Enter Secret Key:"
#define PW_MESS			"S/Key Password:"

#define BUTTON_LABEL	"Compute Password"
#define BUTTON_WIDTH 	125
#define OTP_BOX_WIDTH	250
#define OTP_BOX_HEIGHT	40

class PWFilter: public BMessageFilter {
public:
	PWFilter();

	virtual filter_result Filter(BMessage *msg, BHandler **target);
};

class OTPView: public BView {
private:
	char data[256];
	sem_id output_sem;
	font_height font;

public:
	OTPView(BRect frame);

	void AttachedToWindow();
	void Draw(BRect update_rect);
	void MouseDown(BPoint point);
	void SetText(const char *string);
	const char *Text();	
};

class SKeyView: public BView {
private:
	BTextControl *SeedView;
	OTPView *KeyView;
	BTextControl *PWView;
	
	bool md4;

public:
	SKeyView(BRect frame, const bool md);
	
	void AttachedToWindow();
	void MessageReceived(BMessage *msg);

	bool GetMD();
};

// TODO: Move me somewhere more appropriate.
char *ComputeKey(const char *seed, const char *key, bool md4);

#endif
