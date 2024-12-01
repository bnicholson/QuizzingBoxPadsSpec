/**
* SeatInterface.cpp -> Defines the 'master' class for all seat and button interfaces.
*/

#pragma warning(disable : 4996)
#define _HAS_STD_BYTE 0				// prevents ambiguious issue between windows headers and C++17's inclusion of std::byte

#include <map>
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
using namespace std;
#include <wx/wx.h>
#include <wx/splash.h>
#include <wx/thread.h>
#include <wx/sound.h>
#include <wx/webrequest.h>
#ifdef _WINDOWS
#include <conio.h> 
#endif
#include <time.h>
#include "compat/msvc.h"
#include "config.h"
#include "rptevt.h"
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
#ifdef _WINDOWS
#include <windows.h>
#endif 
#include "SeatInterface.h"
#include "ServerWork.h"
#include "Globals.h"

bool sbInterface_isDone = false;		// is the interface done

/**
* Constructor.
*/
SeatInterface::SeatInterface() {
	// set some variables
	armed=true;
	testing=false;
	needsConnect = false;  // for now most seat or interface types don't require a manual connection

	// figure out the seat type
	string seatTypeStr = configData->getParam("/SeatType");
	seatType = atoi(seatTypeStr.c_str());
	if((seatType != 1)&&(seatType != 2)) {
		seatType = 1;
		rpterr(__FILE__,__LINE__,0,"Illegal seat type %s",seatTypeStr.c_str());
	}
	// Now go load the seat drop map
	for(int i=0;i < 3; i++) {
		for(int j=0;j<5;j++) {
			char param[256];
			sprintf(param,"/DropSeat/%d/%d",i,j);
			string dropped = configData->getParam(param);
			if(strcasecmp(dropped.c_str(),"1")==0)
				setSeatDropped(i,j,true);
			else
				setSeatDropped(i,j,false);
		}
	}
	// Update the ignore list
	updateIgnore();
}

/**
* Destructor.
*/
SeatInterface::~SeatInterface() {
}

/**
* Set the armed flag.  The next jump will cause a 'beep'
*/
bool SeatInterface::setArmed(bool i) {
	armed = i;
	if((armed)&&(oldbits!=0))
		return(false);
	if(armed==true)
		firstYield=true;
	return(i);
}

/**
* Set the testing flag.  All jumps will cause a 'beep'.
*/
void SeatInterface::setTesting(bool i) {
	testing = i;
}

/**
* Get the seat status
*/
bool SeatInterface::getStatus(int set,int seat) {
	return(false);
}

/**
* Get the testing flag.  All jumps will cause a 'wave file to be played and a beep'.
*/
bool SeatInterface::getTesting() {
	return(testing);
}

/**
* How many bogomips are there
*/
long SeatInterface::calculateBogoLoops() {
	return(bogoLoops);
}

/**
* requiresConnect() - is a manual connection to the seat sets or interface box required
*/
bool SeatInterface::requiresConnect() {
	return(needsConnect);
}

/**
* How many bogomips are there
*/
long SeatInterface::getBogoLoops() {
	return(bogoLoops);
}

/**
* getVersion() - what is the version of the interface box
*/
string SeatInterface::getVersion() {
	return(std::string(interfaceVersion.mb_str()));
}

/**
* Set a seat active or inactive.  
*/
void SeatInterface::setActive(int set,int seat,bool val) {
	int setSeat = set*5+seat+1;
	if(setSeat >= maxInterfaceSeats)
		return;
	// now set the active bit
	active[set*5+seat+1] = val;
	// now go update the ignore array
	updateIgnore();
}

/**
* Get the state of the seat.  Is it active or not?
*/
bool SeatInterface::getActive(int set,int seat) {
	return(active[set*5+seat+1]);
}

/**
* Update the ignore array.
*/
void SeatInterface::updateIgnore() {
	// It's time to build the ignore list
	for(int i=1;i <= 15;i++) {
		// assume it's not ignored
		ignore[i]=false;
		// if the seat is dropped ignore it
		if(seatDropMap[i] < 0) {
			ignore[i]=true;
		} else {
			int seat = seatDropMap[i];
			if(active[seat]==false)
				ignore[i]=true;
		}
	}
	return;
}

/**
* It's time to exit!
*/
int SeatInterface::quit(int ec) {
	// Spin for a little bit to allow time for the interface code to terminate.
	int count = 30; // thirty seconds
	while ((count-- > 0) && (working == true))
		sleep(1000);
	return(-1);
}

/**
* is this thread running
*/
bool SeatInterface::isRunning() {
	return(working);
}

/**
* What type of seats are we using?
*/
int SeatInterface::getSeatType() {
	return(seatType);
}

/**
* Set the seat type.
*/
void SeatInterface::setSeatType(int st) {
	seatType = st;
	char buffer[32];
	sprintf(buffer,"%d",st);
	configData->setParam("/SeatType",buffer);
	configData->save();
}

/**
* How many sets are there?
*/
int SeatInterface::getNumSets() {
	return(3);
}

/**
* How many seats/buttons per set are there
*/
int SeatInterface::getNumSeatsPerSet() {
	return(5);
}

/**
* Drop a particular seat.
*/
void SeatInterface::setSeatDropped(int set,int seat,bool drop) {
	seatDropped[set][seat] = drop;
	// now rebuild the map
	for(int i=0;i < 3;i++) {
		int start = i*5+1;
		for(int j=0;j < 5;j++) {
			if(seatDropped[i][j]==false)
				seatDropMap[i*5+j+1] = start++;
			else
				seatDropMap[i*5+j+1] = -1;
		}
	}
	// Ok, now we've got the drop map rebuilt
	// It's time to build the ignore list
	updateIgnore();
}

/*
* Get dropped seats.
*/
bool SeatInterface::getSeatDropped(int set,int seat) {
	// ok, see if this set and seat are anywhere?
	if(seatDropMap[set*5+seat+1]<0)
		return(true);
	return(false);
}

/**
* setOption() - Set some option for the interface.
*/
void SeatInterface::setOption(wxString name,wxString val) {
}

/**
* optionDialog() - display the option dialog.
*/
void SeatInterface::optionDialog() {

}

/**
* diagnose() - diagnose this interface.
*/
bool SeatInterface::diagnose() {
	return(false);
}

/**
  setFirstYield() - ensure arm button changes color prior interface thread using up all the CPU
  */
void SeatInterface::setFirstYield(bool nextFirstYield) {
	firstYield = nextFirstYield;
}

/**
* startScan() --> if this was a wireless box this would start a bluetooth or wifi scan
*/
void SeatInterface::startScan() {
}

/**
* get a list of devices if this is a wireless device
*/
std::vector<string> SeatInterface::getScanList() {
	std::vector<string> n;
	return(n);
}

/**
* onStartup()
*/
void SeatInterface::onStartup() {
	return;
}

/**
* Disconnect a set of seats
*/
bool SeatInterface::disconnect(int set) {
	return(true);
}

/**
* is this seat set connected already?
*/
bool SeatInterface::isconnected(int set) {
	return(false);
}

/**
* get the deviceid
*/
long long int SeatInterface::getdeviceid(int set) {
	return(0);
}

/**
* set the seat device id for this seat set
*/
void SeatInterface::setSeatSetDevice(int, wxString) {
	// set the deviceid we'll try to connect with later

}

/**
* A wireless seat set notified us of responses
*/
void SeatInterface::notify(wxString mac, std::string received) {

}

/* 
* get deviceid 
*/
wxString SeatInterface::getSeatSetDeviceid(int t) {
	return("");
}

/*
* what is the status of this device
*/
wxString SeatInterface::getSeatSeatDeviceStatus(int seatset) {
	return("unknown");
}

//* Is the interface completely done
bool sbInterface_isRunning() {
	return(!sbInterface_isDone);
}