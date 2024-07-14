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

## Implementing custom seat or interface hardware

The QBox and the Parallel version of the seat hardware are implmented using C++ inheritance from the SeatInterface class.   This is done so that the majority of QuizMachine has no idea what the hardware, interface, or device specific code does.  

So to create a new jump seat hardware device you must create a new class that inherits from the SeatInterface class.   All methods in the SeatInterface class are virtual methods to ensure that the correct method is called even when the object type is not known by the caller. This ensures that we have runtime polymorphism.  Ideally the SeatInterface class should be an abstract class.  However, a SeatInterface class instance is instantiated in QMServer due to the heavy use of QuizMachine classes in QMServer.

### SeatInterface::SeatInterface() - 

Constructor called to initialize the Interface to the hardware.  Often the hardware devices and/or the interface is required to have it's own thread to ensure performance, fairness and to manage the hardware device(s).   All current implementations of this constructor inherit from wxThread to allow this thread to work well with the wxWidgets UI of QuizMachine.  See below for an example boilerplate. 

```C++
QBoxInterface::QBoxInterface() : SeatInterface(), wxThread(wxTHREAD_DETACHED) {
  // All QBox specific Constructor code here.
}
```

### SeatInterface::~SeatInterface() - 

Destructor.   This is called when the interface to the hardware is destroyed when QuizMachine changes to a different type of hardware, interface, or device.

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


