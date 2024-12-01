/**
* SeatInterface.h - Defines the 'Master' seat and button interface controller class.
*/

#ifndef SEATINTERFACE_H
#define SEATINTERFACE_H

const int maxInterfaceSeats	= 16;	// maximum # of seats in all sets
#define MAX_SEATSETS	3			// maximum of of seat sets

/**
* The 'Master' Seat/Button interface class'
*/
class SeatInterface {
public:
	SeatInterface();	// Constructor
	~SeatInterface();	// Destructor

	// Methods
	virtual int quit(int);						// time to exit!
	virtual bool setArmed(bool);						// Set the alarm
	void setTesting(bool);						// Set the testing flag (causes beep on each jump/sit down)
	bool getTesting();							// Is the testing flag set?
	virtual long calculateBogoLoops();			// determine the bogo loops value
	virtual bool getStatus(int,int);			// get the seat status
	virtual long getBogoLoops();				// how many loops / second are there where we test the interface
	virtual void setActive(int,int,bool);				// which seats are we going to activate
	virtual bool getActive(int,int);					// is this seat active?
	virtual int getSeatType();					// Get the seat type we're using
	virtual void setSeatType(int st);			// set the seat type we're using
	virtual int getNumSets();					// how many seat/button sets are there
	virtual int getNumSeatsPerSet();			// how many seats/buttons per set are there
	void updateIgnore();						// update the ignore array
	virtual void optionDialog();				// display the option dialog
	virtual void setOption(wxString,wxString);	// set some interface option
	void setFirstYield(bool);					//set firstYield
	bool requiresConnect();						// is this a seat set that requires manual action to connect
	virtual void startScan();					// start a scan if this is wireless
	virtual std::vector<string> getScanList();		// 
	virtual void onStartup();								// do things that need the model or other things
	virtual bool isRunning();								// is this 
	virtual void setSeatSetDevice(int, wxString);	// set the deviceid we'll try to connect with later
	virtual bool disconnect(int set);						// disconnect this seat set
	virtual bool isconnected(int);							// is this seat set connected
//	virtual bool isthere(int);								// is there a seat set there
	virtual long long int getdeviceid(int);					// get the mac address for use someplace else
	virtual void notify(wxString, std::string);				
	virtual wxString getSeatSetDeviceid(int t);				// get the mac address for this device
	virtual wxString getSeatSeatDeviceStatus(int);			// get the hardware status of this seat set

	// Drop seat info
	void setSeatDropped(int,int,bool);		 // drop this seat
	bool getSeatDropped(int set,int seat);	 // is a particular seat dropped?
 
	//diagnostic information
	virtual bool diagnose();
	int getMajor();
	int getMinor();
	string getVersion();

protected:
	// data
	long oldbits;							// last state of jumps (0-sitting down, 1-jumped)
	int status[16];							// status of each seat
	int shadowStatus[16];					// status (copied) after each loop around the lpt interface
	bool armed;								// Are we armed
	bool working = false;					// the interface code is still doing work
	bool firstYield;						// yield to update the screen
	bool testing;							// Are we testing seats
	long bogoLoops;							// how fast can we check the lpt interface box
	wxString interfaceVersion;				// Version of the box
	int major, minor;						// Major & Minor version #s on the interface
	struct timespec lastInput;				// last time we got an input from the interface
	bool ignore[maxInterfaceSeats];			// which seats are we ignoring?
	bool active[maxInterfaceSeats];			// Is this seat active?
	int seatType;							// what type of seats are we using?
	bool seatDropped[3][5];					// Is this seat dropped?
	int seatDropMap[maxInterfaceSeats];		// Given a physical seat # what is it's seat # after dropping seats.
	bool needsConnect;						// We need to connect the seats manually

private:

};

#endif