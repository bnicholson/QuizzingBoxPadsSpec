# QuizzingBoxPadsSpec
Specification used to interface Custom Quizzing Boxes or Pads to QuizMachine.

This repository provides examples and the specification for the quizzing pads and interface box used by Quizmachine.

Pads:

Two Types:

Type 1

Type 2

Interface box


C++

SeatInterface::SeatInterface() - Constructor called to initialize the Interface to the hardware.

SeatInterface::~SeatInterface() - Destructor.   This is called when the interface to the hardware is destroyed when QuizMachine changes to a different type of interface

bool SeatInterface::setArmed(bool) - Arm all the seats/pads/boxes needed.  Arming is used to notify the interface and the hardware that a new question is going to be asked and that the next jump will be captured to identify which quizzer jumped first.   A returned value of false indicates that the arming was unsuccessful, perhaps a quizzer is not on their seat.  A return value of true indicates that the arming was successful and that the next jump will trigger a hardware beep if the hardware supports sound.

SeatInterface::calculateBogoLoops() - This number is used to identify a device/interface specific performance number.   For those devices or interfaces that use polling, this is often the number of polls per second.   For those devices that are interrupt driven at the hardware level, this number is often an indicator of how idle the device or interface is.  For those interfaces / devices that are multiple devices this number is somehow aggregate, averaged, or something device or interface specific.   This routine is called to notify the device or interface that a subsequent call one or two seconds later will get the actual performance number.

long SeatInterface::getBogoLoops() - Return the performance number.

std::string SeatInterface::getVersion() - Return a string that identifies the hardware and software version of the device or interface.

void SeatInterface::setActive(int set, int seat, bool isActive) - called by Quizmachine to notify the hardware device / interface to set a particular seat on a particular set to the active state or inactive state.  If isActive is set to false than no jump by a quizzer should be reported to QuizMachine.

bool SeatInterface::getActive(int set, int seat) - Called by QuizMachine when it wants to see if a seat on a particular set is active or inactive.

void SeatInterface::updateIgnore() - Often a device or interface doesn't want to or need to 

int SeatInterface::quit(int ec) -

void SeatInterface::setSeatType() - Is the seats normall open or normally closed.

int SeatInterface::getSeatType() -

int SeatInterface::getNumSets() - Ask the hardware device or interface how many sets of seats it supports. 

int SeatInterface::getNumSeatsPerSet() - As the hardware device or interface how many seats per set there are.

void SeatInterface::setSeatDropped(int set, int seat, bool isDropped) -

bool SeatInterface::getSeatDropped(int set, int seat) -

void SeatInterface::setOption(std::string name, std::string value) -

bool SeatInterface::diagnose() - 









