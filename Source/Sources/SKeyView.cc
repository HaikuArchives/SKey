#include "SKeyMessage.h"
#include "SKeyView.h"

#include "skey.h"

#include <interface/Font.h>
#include <interface/View.h>
#include <interface/TextControl.h>
#include <interface/RadioButton.h>
#include <app/Clipboard.h>
#include <app/Looper.h>
#include <app/MessageFilter.h>
#include <kernel/OS.h>

#include <ctype.h>

PWFilter::PWFilter()
		 :BMessageFilter(B_ANY_DELIVERY, B_ANY_SOURCE)
{
}

filter_result PWFilter::Filter(BMessage *msg, BHandler **target)
{
	switch (msg->what) {
	case B_COPY:
	case B_PASTE:
		return B_SKIP_MESSAGE;
	default:
		return msg->WasDropped() ? B_SKIP_MESSAGE : B_DISPATCH_MESSAGE;
	}
}

OTPView::OTPView(BRect frame)
		:BView(frame, NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP,
			   B_WILL_DRAW | B_FRAME_EVENTS)
{
	output_sem = create_sem(1, "output");
	data[0] = '\0';
}

void OTPView::AttachedToWindow()
{
	BView::AttachedToWindow();
	BFont font_info;
	GetFont(&font_info);
	font_info.GetHeight(&font);
	
}

void OTPView::Draw(BRect update_rect)
{
	BRect bounds = Bounds();

	FillRect(update_rect, B_SOLID_LOW);

	// Draw a spiffy shaded rectangle.
	SetHighColor(180, 180, 180);
	StrokeLine(BPoint(0, 0), BPoint(0, bounds.bottom));
	StrokeLine(BPoint(0, 0), BPoint(bounds.right, 0));
	SetHighColor(240, 240, 240);
	StrokeLine(BPoint(bounds.right, 0), BPoint(bounds.right, bounds.bottom));
	StrokeLine(BPoint(0, bounds.bottom), BPoint(bounds.right, bounds.bottom));
	SetHighColor(0, 0, 0);

	acquire_sem(output_sem);
	DrawString(data, BPoint(5, bounds.top+
						    (bounds.bottom+font.ascent)/2));
	release_sem(output_sem);
}

void OTPView::SetText(const char *string)
{
	if (!string || !*string)
		return;

	acquire_sem(output_sem);
	strncpy(data, string, sizeof(data));
	release_sem(output_sem);
	Draw(Bounds());
}

void OTPView::MouseDown(BPoint point)
{
	BMessage msg(B_PASTE);
	const char *data = Text();
	msg.AddData("text/plain", B_MIME_TYPE, (void *)data,
				 strlen(data));
	DragMessage(&msg, BRect(Bounds()));
}

const char *OTPView::Text()
{
	return data;
}

SKeyView::SKeyView(BRect frame, const bool md)
		 :BView(frame, NULL, B_FOLLOW_ALL_SIDES,
				B_WILL_DRAW | B_FRAME_EVENTS)
{
	md4 = md;
}

void SKeyView::AttachedToWindow()
{
	BRect bounds = Bounds();

	SetViewColor(235, 235, 235);

	// Create all the sub-views here.
	SeedView = new BTextControl(BRect(5, 3, bounds.right-5, 20), NULL,
								SEED_PROMPT, NULL, new BMessage(SKEY_COMP_KEY));
	AddChild(SeedView);
	SeedView->SetDivider(SeedView->StringWidth(SEED_PROMPT)+10);

	PWView = new BTextControl(BRect(5, 25, bounds.right-5, 42), NULL, KEY_PROMPT, NULL,
						   new BMessage(SKEY_COMP_KEY));
	// Ugly hack to not echo password below.
	PWView->TextView()->SetFontAndColor(new BFont);
	PWView->TextView()->AddFilter(new PWFilter);
	PWView->TextView()->MakeSelectable(FALSE);
	AddChild(PWView);

	PWView->SetDivider(SeedView->StringWidth(KEY_PROMPT)+10);

	BRadioButton *radio_button = new BRadioButton(BRect(bounds.right-BUTTON_WIDTH-5, 50,
										  				  bounds.right-85, 65), NULL,
										  		   "MD4", new BMessage(MD4_MSG));
	AddChild(radio_button);
	if (md4)
		radio_button->SetValue(B_CONTROL_ON);

	BRect frame = radio_button->Frame();
	frame.left = frame.right + 5; frame.right = bounds.right-5;
	radio_button = new BRadioButton(frame, NULL, "MD5", new BMessage(MD5_MSG));
	AddChild(radio_button);
	if (!md4)
		radio_button->SetValue(B_CONTROL_ON);

	// Put a box around the password view.
	BBox *pw_box = new BBox(BRect(5, 50, 5+OTP_BOX_WIDTH, 50+OTP_BOX_HEIGHT));
	pw_box->SetLabel("S/Key Password");
	pw_box->SetViewColor(235, 235, 235);
	pw_box->SetLowColor(235, 235, 235);
	pw_box->SetFont(be_plain_font);
	AddChild(pw_box);

	// Create the view for the One Time Password.
	BFont font;
	font_height font_height;
	pw_box->GetFont(&font);
	font.GetHeight(&font_height);
	KeyView = new OTPView(BRect(5, font_height.ascent+font_height.descent,
								 OTP_BOX_WIDTH-5, OTP_BOX_HEIGHT-5));
	pw_box->AddChild(KeyView);

	// Now add the button.
	BButton *button = new BButton(BRect(bounds.right-BUTTON_WIDTH-5, 68,
								  bounds.right-5, 0), NULL, BUTTON_LABEL,
								  new BMessage(SKEY_COMP_KEY));
	AddChild(button);

	SeedView->MakeFocus();
}

void SKeyView::MessageReceived(BMessage *msg)
{
	switch (msg->what) {
	case MD4_MSG:
		md4 = true;
		break;
	case MD5_MSG:
		md4 = false;
		break;
	case B_COPY: {
		const char *data = KeyView->Text();
		if (!data || !*data)
			return;
		be_clipboard->Lock();
		be_clipboard->Clear();
		be_clipboard->Data()->AddData("text/plain", B_MIME_TYPE, (void *)data,
									   strlen(data));
		be_clipboard->Commit();
		be_clipboard->Unlock();
		SeedView->MakeFocus();
	}; break;
	case B_PASTE: {
		const char *data;
		ssize_t length;
		be_clipboard->Lock();
		be_clipboard->Data()->FindData("text/plain", B_MIME_TYPE, 0, &data, &length);
		be_clipboard->Unlock();
		if (!data || !*data)
			return;
		SeedView->SetText(data);
	}; break;
	case SKEY_COMP_KEY: {
		const char *seed = SeedView->Text();
		const char *key = PWView->Text();
		char *otp = ComputeKey(seed, key, md4);
		KeyView->SetText(otp);
	}; break;
	default:
		BView::MessageReceived(msg);
	}
}

bool SKeyView::GetMD()
{
	return md4;
}

char *ComputeKey(const char *seed, const char *key, bool md4)
{
	if (!seed)
		return NULL;

	char *bogon, number[10], crunched[128];
	static char otp8[64];

	// Ignore everything before the first digit.
	for (; *seed && !isdigit(*seed); seed++);

	if (!*seed)
		return NULL;

	// Set the seed index to the first non-digit after that.
	long i;
	for (i=0; i < sizeof(number) && *seed && isdigit(*seed); seed++) {
		number[i] = *seed; i++;
	}
	number[i] = '\0';

	if (!*seed)
		return NULL;

	int iter = strtol(number, &bogon, NULL);

	// Skip the white space.
	for (; seed && *seed && isspace(*seed); seed++);

	if (md4) {
		md4_keycrunch(crunched, (char *)seed, (char *)key);
		for (; iter > 0; iter--)
			md4_f(crunched);
	} else {
		md5_keycrunch(crunched, (char *)seed, (char *)key);
		for (; iter > 0; iter--)
			md5_f(crunched);
	}

	// If you really like hex output, uncomment the line below.
	// return put8(otp8, crunched);
	return btoe(otp8, crunched);
}
