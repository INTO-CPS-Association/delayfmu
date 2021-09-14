
#include "FmuContainer.h"

#include <utility>
#include <thread>
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


FmuContainer::FmuContainer(const fmi2CallbackFunctions *mFunctions, bool logginOn, const char *mName)
        : m_functions(mFunctions), m_name(mName), loggingOn(logginOn){
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
    if(this->state != FMIState::instantiated){
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "Setup called in the wrong state.",
                         "");
        this->state = FMIState::error;
        return false;
    }
    if(startTime != 0.0) {
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "Expected startTime to be 0.0. Actual start time was: %f",
                         startTime);
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
    if(this->state != FMIState::initialized){
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "Step called in the wrong state.",
                         "");
        this->state = FMIState::error;
        return false;
    }
    return true;
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

    // The only valid boolean is 2
    if(nvr == 1 && vr[0] == outOfSyncId){
        value[0] = false;
        return true;
    }
    else {
        if(nvr != 1){
            FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "getBoolean received invalid arguments. nvr expected: 1, actual: %zu. ",
                                      nvr);
        }
        else {
            if (vr[0] != outOfSyncId){
                FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "getBoolean received invalid arguments. Value references allowed: %i, actual: %i. ",
                                 outOfSyncId, value[0]);
            }
        }
        return false;
    }
}

bool FmuContainer::getInteger(const fmi2ValueReference *vr, size_t nvr, fmi2Integer *value) {
    if (nvr > 2 || nvr == 0)
    {
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "getInteger received invalid arguments. nvr expected: 1 to 2, actual: %zu. ",
                         nvr);
        return false;
    }
    else {
        for(int i = 0; i < nvr; i++){

            if(vr[i] == safeToleranceId)
                value[i] = 1;
            else if (vr[i] == realTimeCheckIntervalID)
                value[i] = 2;
            else {
                FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "setInteger received invalid arguments. Value references allowed: 0 to 1, actual: %i. ",
                                 vr[i]);
                return false;
            }
        }
        return true;
    }
}

bool FmuContainer::getReal(const fmi2ValueReference *vr, size_t nvr, fmi2Real *value) {
    if (nvr > 0){
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "getReal is invalid. There are no reals.","");
        return false;
    }
    else {
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

                    FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "setInteger received invalid arguments. Value references allowed: 0 to 1, actual: %i. ",
                                     1);
                    return false;
}

bool FmuContainer::setReal(const fmi2ValueReference *vr, size_t nvr, const fmi2Real *value) {
    if (nvr > 0){
        FmuContainer_LOG(fmi2Fatal, "logStatusFatal", "setReal is invalid. There are reals to set.","");
        return false;
    }
    else {
        return true;
    }
}

bool FmuContainer::setString(const fmi2ValueReference *vr, size_t nvr, const fmi2String *value) {

                FmuContainer_LOG(fmi2Fatal, "logStatusFatal",
                                 "setString received invalid arguments. Value references allowed: %i, actual: %i. ",
                                 webServerHostnameId, vr[0]);
                return false;

}

bool FmuContainer::beginInitialize() {
return false;
}

bool FmuContainer::endInitialize() {
    return false;
}



