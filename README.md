# delayfmu
A native fmus with a parameterized delay
It is automatically built and can be downloaded under actions.

## Delay
The delay is an `int` parameter and has valuereference 0.
The delay is performed in dostep, and the unit of the delay is milliseconds.. And the unit of the delay is milliseconds.

## Outputs
The FMU has a single `real` output with valuereference 1

## Inputs
THe FMU has a single `real` input with valuereference 2

## Dostep
Increments internal counter with 1.
Transfers the input to the output and adds the internal counter. The reason for the addition is simply to see that something is actually happening.
Afterwards, it sleeps for x milliseconds, see Delay.



