
#include "FmuContainer.h"

#include <utility>
#include <thread>
#include <chrono>
/* LOGGING EXAMPLE
 *
 * FmuContainer_LOG(fmi2OK, "logAll",
 *      "Initialization completed with: Hostname='%s', Port='%d', Username='%s', routingkey='%s', starttimestamp='%s', communication timeout %d s",
 *      hostname.c_str(), port, username.c_str(), routingKey.c_str(), startTimeStamp.str().c_str(),
 *      this->communicationTimeout);
 */
#define FmuContainer_LOG(status, category, message, args...)       \
  if (m_functions != NULL) {                                             \
    if (m_functions->logger != NULL) {                                   \
      m_functions->logger(m_functions->componentEnvironment, m_name.c_str(), status, \
                        category, message, args);                      \
    }                                                                  \
  } else {                                                             \
    fprintf(stderr, "Name '%s', Category: '%s'\n", m_name.c_str(), category);    \
    fprintf(stderr, message, args);                                    \
    fprintf(stderr, "\n");                                             \
  }

std::string printFMIState(FMIState state){
    switch (state) {
        case FMIState::initializing:
            return "initializing";
        case FMIState::instantiated:
            return "instantiated";
        case FMIState::initialized:
            return "initialized";
        case FMIState::error:
            return "error";
        case FMIState::terminated:
            return "terminated";

    }
}

bool FmuContainer::isStateValid(FMIState expected, FMIState actual){
    if(expected != actual ) {
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal",
                         "FMU is in the wrong state. Expected state: %s - Actual state: %s",
                         printFMIState(expected).c_str(), printFMIState(actual).c_str());
        this->state = FMIState::error;
        return false;
    }
    else {return true;}
}



FmuContainer::FmuContainer(const fmi2CallbackFunctions *mFunctions, bool logginOn, const char *mName)
        : m_functions(mFunctions), m_name(mName), loggingOn(logginOn){
    this->state = FMIState::instantiated;
}

FmuContainer::~FmuContainer() {
    this->terminate();

}

bool FmuContainer::isLoggingOn() {
    return this->loggingOn;
}

/*####################################################
 *  Other
 ###################################################*/

bool FmuContainer::setup(fmi2Real startTime) {
    if(!isStateValid(FMIState::instantiated, this->state))
    {
        return false;
    }

    if(startTime != 0.0) {
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "Expected startTime to be 0.0. Actual start time was: %f",
                         startTime);
        this->state = FMIState::error;
        return false;
    }
    else {
        return true;
    }
}

bool FmuContainer::terminate() {
    this->state = FMIState::terminated;
    return true;
}


fmi2ComponentEnvironment FmuContainer::getComponentEnvironment() { return (fmi2ComponentEnvironment) this; }

bool FmuContainer::step(fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize) {
    if(!isStateValid(FMIState::initialized, this->state))
    {
        return false;
    }
    else {
        this->realOutput = this->realInput;


        std::this_thread::sleep_for(std::chrono::milliseconds (this->sleep));
        return true;
    }
}

/*####################################################
 *  Custom
 ###################################################*/

bool FmuContainer::fmi2GetMaxStepsize(fmi2Real *size) {

    return false;
}

/*####################################################
 *  GET
 ###################################################*/

bool FmuContainer::getBoolean(const fmi2ValueReference *vr, size_t nvr, fmi2Boolean *value) {

    if (nvr > 0){
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "getBoolean is invalid. There are no booleans.","");
        return false;
    }
    else {
        return true;
    }
}

bool FmuContainer::getInteger(const fmi2ValueReference *vr, size_t nvr, fmi2Integer *value) {
    if (nvr > 0){
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "getInteger is invalid. There are no integers.","");
        return false;
    }
    else {
        return true;
    }
}

bool FmuContainer::getReal(const fmi2ValueReference *vr, size_t nvr, fmi2Real *value) {

    if (nvr != 1)
    {
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "getInteger received invalid arguments. nvr expected: 1 , actual: %zu. ",
                         nvr);
        return false;
    }
    else {
        for(int i = 0; i < 1; i++){
            if(vr[i] == outputRealId)
                value[i] = this->realOutput;
            else {
                FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "setInteger received invalid arguments. Value references allowed: 1, actual: %i. ",
                                 vr[i]);
                return false;
            }
        }
        return true;
    }
}

bool FmuContainer::getString(const fmi2ValueReference *vr, size_t nvr, fmi2String *value) {
    if (nvr > 0){
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "getString is invalid. There are no strings.","");
        return false;
    }
    else {
        return true;
    }
}


/*####################################################
 *  SET
 ###################################################*/

bool FmuContainer::setBoolean(const fmi2ValueReference *vr, size_t nvr, const fmi2Boolean *value) {
    if (nvr > 0){
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "setBoolean is invalid. There are booleans to set.","");
        return false;
    }
    else {
        return true;
    }
}

bool FmuContainer::setInteger(const fmi2ValueReference *vr, size_t nvr, const fmi2Integer *value) {
    size_t expected_nvr = 1;
    if(!isStateValid(FMIState::instantiated, this->state))
    {
        return false;
    }
    if (nvr != expected_nvr)
    {
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "setInteger received invalid arguments. nvr expected: %zu , actual: %zu. ",
                         expected_nvr, nvr);
        return false;
    }
    else {
        for(int i = 0; i < expected_nvr; i++){
            if(vr[i] == sleepParameterId)
                this->sleep = value[i];
            else {
                FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "setInteger received invalid arguments. Value references allowed: %i, actual: %i. ",
                                 sleepParameterId, vr[i]);
                return false;
            }
        }
        return true;
    }
}

bool FmuContainer::setReal(const fmi2ValueReference *vr, size_t nvr, const fmi2Real *value) {
    size_t expected_nvr = 1;
    if(!isStateValid(FMIState::initialized, this->state)) {
        return false;
    }
    if (nvr != expected_nvr)
    {
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "setReal received invalid arguments. nvr expected: %zu , actual: %zu. ",
                         expected_nvr, nvr);
        return false;
    }
    else {
        for(int i = 0; i < expected_nvr; i++){

            if(vr[i] == inputRealId)
                this->realInput = value[i];
            else {
                FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "setReal received invalid arguments. Value references allowed: %i, actual: %i. ",
                                 inputRealId, vr[i]);
                return false;
            }
        }
        return true;
    }
}

bool FmuContainer::setString(const fmi2ValueReference *vr, size_t nvr, const fmi2String *value) {
    if (nvr > 0){
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "setString is invalid. There are strings to set.","");
        return false;
    }
    else {
        return true;
    }

}

bool FmuContainer::beginInitialize() {
    if(!isStateValid(FMIState::initializing, this->state))
    {
        return false;
    }
     else {
        this->state = FMIState::initializing;
        return true;
    }
}

bool FmuContainer::endInitialize() {
    if(!isStateValid(FMIState::initializing, this->state))
    {
        return false;
    }
    else{
        this->state = FMIState::initialized;
        return true;
    }
}





