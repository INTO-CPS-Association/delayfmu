#ifndef DELAYFMU_FMUCONTAINER_H
#define DELAYFMU_FMUCONTAINER_H

#include "fmi2Functions.h"



using namespace std;

#include <string>
#include <map>
#include <ctime>

const int outputRealInitial = 0.0;
/*
 * These are the scalar variable IDs
 */
const int sleepParameterId = 0;
const int inputRealId = 1;
const int outputRealId = 2;

using namespace std;

enum class FMIState {instantiated, initializing, initialized, error, terminated};

class FmuContainer {
public:
    const fmi2CallbackFunctions *m_functions;
    const string m_name;


    FmuContainer(const fmi2CallbackFunctions *mFunctions,bool loggingOn, const char *mName);

    ~FmuContainer();

    fmi2ComponentEnvironment getComponentEnvironment();

    bool step(fmi2Real currentCommunicationPoint,
              fmi2Real communicationStepSize);

    bool fmi2GetMaxStepsize(fmi2Real *size);

    bool getString(const fmi2ValueReference vr[], size_t nvr, fmi2String value[]);

    bool getReal(const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]);

    bool getBoolean(const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]);

    bool getInteger(const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]);

    bool setString(const fmi2ValueReference vr[], size_t nvr, const fmi2String value[]);

    bool setReal(const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]);

    bool setBoolean(const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]);

    bool setInteger(const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]);

    bool terminate();

    bool setup(fmi2Real startTime);

    bool isLoggingOn();

    /**
     * Starts the core webserver
     * @return
     */
    bool beginInitialize();
    /**
    * Start the core real-time clock.
    * @return
    */
    bool endInitialize();

private:
    int sleep;
    double realOutput = outputRealInitial;
    double realInput = 0.0;

    FMIState state = FMIState::instantiated;

    const bool loggingOn;


    void destroy();

    bool isStateValid(FMIState expected, FMIState actual);
};


#endif //DELAYFMU_FMUCONTAINER_H
