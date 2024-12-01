/**
* Implements a JumpReport - Who jumped/sat down/release button, when, etc.
*/

#pragma warning(disable : 4996)
#define _HAS_STD_BYTE 0				// prevents ambiguious issue between windows headers and C++17's inclusion of std::byte

#include <map>
#include <string>
#include <vector>
#include <mutex>
using namespace std;
#include "wx/wx.h"
#include <wx/splash.h>
#include <wx/sound.h>
#include <wx/webrequest.h>
#include "http.h"
#include "HardwareReport.h"
#include "JumpReport.h"
#include "ScoreEvent.h"
#include "ScoreTable.h"
#include "HttpAPI.h"
#include "Models.h"
#include "Views.h"
#include "Controllers.h"
#include "QuizMachine.h"

// Declare the new JumpReport Event type
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_JUMPREPORT,7777)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(wxEVT_JUMPREPORT)


/**
* Constructor.
*/
JumpReport::JumpReport(int s,int p,int ns,struct timespec ts,bool d) : wxEvent(QUIZMACHINEFRAME,wxEVT_JUMPREPORT) {
	set = s;
	seat = p;
	simultaneous = ns;
	jumped = false;
	jumptime = ts;
	done = d;
}

/**
* Copy Constructor.
*/
JumpReport::JumpReport(JumpReport *jr) {
	set = jr->getSet();		// which set jumped
	seat = jr->getSeat();	// which seat jumped/sat down?
	jumped = jr->getJumped();	// did they jump or sit down?
	simultaneous = jr->getSimultaneous();	// how many people simultaneously jumped?
	jumptime = jr->getJumpTime();
	done = jr->getDone();
}

/**
* Destructor.
*/
JumpReport::~JumpReport() {
}

/**
* Which set of pads/buttons had the jump in it.
*/
int JumpReport::getSet() {
	return(set);
}

/**
* Set the hardware set.
*/
void JumpReport::setSet(int s) {
	set = s;
}

/** 
* Get the hardware seat # of the person that jumped/sat down/pushed button/released button?
*/
int JumpReport::getSeat() {
	return(seat);
}

/** 
* Set the hardware seat # of the person that jumped/sat down/pushed button/released button?
*/
void JumpReport::setSeat(int s) {
	seat = s;
}

/**
* Did they jump?
*/
bool JumpReport::getJumped() {
	return(jumped);
}

/**
* Set if they jumped?
*/
void JumpReport::setJumped(bool j) {
	jumped = j;
}

/** 
* How many jumped simultaneously?
*/
int JumpReport::getSimultaneous() {
	return(simultaneous);
}

/**
* Set how many jumped simultaneously!
*/
void JumpReport::setSimultaneous(int s) {
	simultaneous = s;
}

/**
* Clone this Jump Report. Used by wxWindows/wxWidgets (the Gui)
*/
wxEvent *JumpReport::Clone(void) const {
	return(new JumpReport(*this)); 
}

/**
* Return the time of the jump.
*/
struct timespec JumpReport::getJumpTime() {
	return(jumptime);
}

/**
* Are we done with a multiple jump?
*/
bool JumpReport::getDone() {
	return(done);
}