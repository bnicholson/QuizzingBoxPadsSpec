# QuizzingBoxPadsSpec
Specification used to interface custom Quizzing Boxes or Pads to QuizMachine.

This repository provides examples and the specification for the quizzing pads and interface box used by Quizmachine.

The standard setup for quizzing consists of one to three sets of seats that connect to a QBox. 

Pads:

Two Types:

#### Type 1 Seats

Type 1 Seats are seats that are normally open electrically when no quizzer is setting on the seats.  When a quizzer sets on the seats the seats become closed electrically.

Type 2

Type 2 Seats are seats that are normally closed electrically when no quizzer is setting on the seats.  When a quizzer sets on the seats the seats become open electrically.

Interface box

## How is QuizMachine Implemented

QuizMachine as of version 6 is implemented in C++ (Std v20) using SQLite 3.40 as the underlying database, and wxWidgets3.2.4 for the UI and various other functions.  

I've included the actual code to the SeatInterface class and to the JumpReport class.   Note:  It will not compile because it's dependent upon classes not included in this specificiation but you should be able to see a few of the tasks needed internally by reviewing the code.  configData for instance is an instance of a class that reads from the QuizMachine.ini file.   

Notice the oldbits variable.  That variable contains a bit for each seat starting with bit 0 (low bit) being the status of the first seat (Quzzer #1) on the red set.   When the interface box reports a changed status for all the seats (it always does that) that status is compared with oldbits to decide what JumpReports should be sent to the UI GUI.  One JumpReport is sent for each seat that changed.   Note: The original LPTInterface had a condition where it was possible for multiple seats to be reported simultaneously.  Now with interrupt driven devices that never has happened.   

We spent months with a three sets of seats connected to a capturing sixteen channel oscilloscope looking for the differences between jumps.  The minimum time between jumps seen during that testing was three microseconds.  However that was very rare.  Commonly jumps could be seen with a difference in the ten microsecond range. 

## Current Implementations

### LPT - Line Printer interface.  

This interface is a crude interface and was the original interface.   It reads from 0x378 directly.   This interface typically doesn't work anymore because few modern computers have a LPT port.

### USB Serial Interface.  

This interface simply searches all the USB ports and actual COM ports for a responding device.  A tiny set of commands and data is available to communicate back and forth between the USB/COM ports and the interface itself.

### Bluetooth Interface.   

Quizmachine searches all BLE devices in the neighborhood for those with a specific name.   Once that specific name is found Quizmachine connects to them (if configured by the quizmaster).   A service is then read to receive data and another service is written to write data.   The data is nearly identical to the the USB serial interface.   Some commands and data responses were enhanced to handle the timing issues associated with the inherent unreliabilities of Bluetooth BLE.

## Implementing custom seat or interface hardware

The Bluetooth, QBox and the Parallel versions of the seat hardware are implemented using C++ inheritance from the SeatInterface class.   This is done so that the majority of QuizMachine has no idea what the hardware, interface, or device specific code does.  

So to create a new jump seat hardware device you must create a new class that inherits from the SeatInterface class.   All methods in the SeatInterface class and derived classes must be virtual methods to ensure that the correct method is called even when the object type is not known by the caller. This ensures that we have runtime polymorphism.  Ideally the SeatInterface class should be an abstract class.  However, a SeatInterface class instance is instantiated in QMServer due to the heavy use of QuizMachine classes in QMServer.

### SeatInterface::SeatInterface() - 

Constructor called to initialize the Interface to the hardware.  Often the hardware devices and/or the interface is required to have it's own thread to ensure performance, fairness and to manage the hardware device(s).   All current implementations of this constructor inherit from wxThread to allow this thread to work well with the wxWidgets UI of QuizMachine.  See below for an example boilerplate.  Note: In the future this will change.   This class will inherit from std::thread rather than from wxThread.   These threads should never interface with the UI except through JumpReport and event queuing later.  

```C++
QBoxInterface::QBoxInterface() : SeatInterface(), wxThread(wxTHREAD_DETACHED) {
  // All QBox specific Constructor code here.
}
```

### SeatInterface::~SeatInterface() - 

Destructor.   This is called when the interface to the hardware is destroyed when QuizMachine changes to a different type of hardware, interface, or device.

```c++
QBoxInterface::~QBoxInterface() {
}
```

### bool SeatInterface::setArmed(bool) - 

Arm all the seats/pads/boxes needed.  Arming is used to notify the interface and the hardware that a new question is going to be asked and that the next jump will be captured to identify which quizzer jumped first.   A returned value of false indicates that the arming was unsuccessful, perhaps a quizzer is not on their seat.  A return value of true indicates that the arming was successful and that the next jump will trigger a hardware beep if the hardware supports sound.

### SeatInterface::calculateBogoLoops() - 

This number is used to identify a device/interface specific performance number.   For those devices or interfaces that use polling, this is often the number of polls per second.   For those devices that are interrupt driven at the hardware level, this number is often an indicator of how idle the device or interface is.  For those interfaces / devices that are multiple devices this number is somehow aggregate, averaged, or something device or interface specific.   This routine is called to notify the device or interface that a subsequent call one or two seconds later will get the actual performance number.

### long SeatInterface::getBogoLoops() - 

Return the performance number.

### std::string SeatInterface::getVersion() - 

Return a string that identifies the hardware and software version of the device or interface.  Today this really isn't used but might be in the futre to allow version specific results.

### void SeatInterface::setActive(int set, int seat, bool isActive) - 

called by Quizmachine to notify the hardware device / interface to set a particular seat on a particular set to the active state or inactive state.  If isActive is set to false than no jump by a quizzer should be reported to QuizMachine.

### bool SeatInterface::getActive(int set, int seat) - 

Called by QuizMachine when it wants to see if a seat on a particular set is active or inactive.

### void SeatInterface::updateIgnore() - 

Often a device or interface doesn't want to or need to report certain seats.  

### int SeatInterface::quit(int ec) -

This method is called right before QuizMachine exits or if the QuizMaster decides to change the hardware, device or interface used to manage the seats.  This routine should quickly do any work required to shut down the hardware, release any software resources such as memory, and then exit the thread.   Note:  This must happen very fast, ideally within thirty seconds.

### void SeatInterface::setSeatType() - 

Is the seats normally open or normally closed.

### int SeatInterface::getSeatType() -

### int SeatInterface::getNumSets() - 

Ask the hardware device or interface how many sets of seats it supports. 

### int SeatInterface::getNumSeatsPerSet() - 

As the hardware device or interface how many seats per set there are.

### void SeatInterface::setSeatDropped(int set, int seat, bool isDropped) -

### bool SeatInterface::getSeatDropped(int set, int seat) -

### void SeatInterface::setOption(std::string name, std::string value) -

### bool SeatInterface::diagnose() - 

This routine allows the Quizmaster to test the hardware, interface, seats, or device.   Currently this is really unused, but is often used to test various parameters and routines during the development of new versions of the interface box or seats.  

Any class derived from SeatInterface must report a jump to the rest of the QuizMachine UI via an event.   See below for for an example of that code. 

```C++
    // First create the event to report a jump or a person sitting down
	JumpReport jr(set,seat,1,now);	// fix a tie jump issue
	jr.setJumped(true);   // false would indicate that the person sat down
	// now send the event to the GUI/UX
	wxPostEvent(frame,jr);
```

#### Commands and Responses

The protocol between the interfaces and QuizMachine follows a specific format.

c n=d

c is a command, sometimes commonly called a verb.  

n is a noun and is often optional.  Only the Mennonite interface uses this to my knowledge.

d is data.   Different commands have different data.

Generally the desire is to have the data be small to facilite speed and simplicity of processing.

#### Verify Version Example:

Command: 

v

This is a command sent to the interface box asking for information about the interface.

Response:

v=QBOX EEECDQR 5.0.1.3 Dec 1 2024 16:59:37 

	QBOX -> Identifies the type of interface.

	EEECDQR -> Identifies a unique interface id.  This is generally used for tracking of 	interfaces in the manufacturing process or identifying specific interface boxes if multiple interface boxes are being used together.

	5.0.1.3 -> Identifies the version of the firmware on the interface box.

	Dec  1 2024 16:59:37 -> Identifies the build date of the firmware

	Generally less useful information should be later in the response so that processing can be minimized.

### Output Example:

Command:

s

Ask the interface box to send us the state of each seat.  

Response:

s=7fe0

	7fe0 -> This interface box is reporting up to fifteen seats status.  Some of which may not be even implemented.   Seat one is always the low bit.   Seat fifteen would be the high bit.  The sign bit is reserved for future use.

	This response is sent to the laptop any time a seat changes state.  From down to jump or from jumped to down.   Note:  See the Arm command below for an exception to this rule.

### Arm Example:

Command:

a=7fe0

	Any bit set to a 1 notifies the interface to ignore this seat. The interface will not arm if
	a seat not ignored is up.

Response:

a=0

	This response says that the arm failed.

a=1 

	This response says that the arm was successful.   Note:  The interface will send an o response any time a seat changes state except when that seat has been marked ignore by an arm command.   The interface becomes unarmed the moment a nonignored seat changes state.

### Dearm example:

Command:

d

	Disable the arm.

Response:

	Commonly the interface response with the same response as a version/verify.

### Echo example

Command:

	e=Nazarene Quizzing is fun.

Response:

	e=Nazarene Quizzing is fun.

### Other examples:

There are other commands, but most of them are interface specific.

## Reliability 

Not only does QuizMachine track the electrical signals on the USB and the keep alives on the bluetooth but it also keeps track of when the last valid response from an interface box was.  If QuizMachine does not receive a response within thirteen seconds then it assumes the interface box is no longer functioning and marks the box as having disconnected or failed.   

# Final Command/Response Notes:

This command/response structure originates from several Bell Laboratories telecommunications devices implemented as early as the 1970s and is still used in modern Bell Laboratories designed devices having been built as late as the early 2020s.  I stole it.  It's been proven to be simple to use, easy to develop on CPU and memory restricted devices and reliable.  I personally have used this structure on cameras, motor control devices, robots, sensors, monitors, and various other control devices.  There are various other versions of this protocol running around.  Simple is best. 

