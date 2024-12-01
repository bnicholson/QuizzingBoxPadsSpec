/**
* Defines the JumpReport class.
*/

#ifndef JUMPREPORT_H
#define JUMPREPORT_H

#define EVT_JUMPREPORT(id,fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
		wxEVT_JUMPREPORT,id,-1, \
		(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
		(wxObject *)NULL \
		),

extern const wxEventType wxEVT_JUMPREPORT;

class JumpReport : public wxEvent {
public:
	JumpReport(int,int,int,struct timespec ts,bool done=false);		// Constructor
	JumpReport(JumpReport *);	// Copy constructor
	~JumpReport();				// destructor

	// Methods
	wxEvent *Clone(void) const ;	// Clone this Report
	int getSet();					// which set jumped.
	void setSet(int);				// set the jump set
	void setSeat(int);				// which seat/button jumped
	int getSeat();					// get which seat/button jumped
	void setJumped(bool);			// set the jumped value
	bool getJumped();				// did the person jump or sat down?
	void setSimultaneous(int);		// Set the # of simultaneous jumps
	int getSimultaneous();			// Get the # of simultaneous jumps
	struct timespec getJumpTime();	// Get the time we jumped
	bool getDone();					// are we done?  (used for multiple jumps)

protected:
private:
	int set;					// which set of pads/buttons jumped
	int seat;					// which seat jumped.
	int simultaneous;			// how many simultaneous jumps were there
	bool jumped;				// true if this is a 'jump' false otherwise (if they sat down
	struct timespec jumptime;	// when they jumped
	bool done;					// are we done?
};

#endif