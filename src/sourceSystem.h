#pragma once

#include "mathUtils.h"
#include "sundialsUtils.h"
#include "chemistry0d.h"
#include "strainFunction.h"
#include "perfTimer.h"
#include "qssintegrator.h"

#include <boost/shared_ptr.hpp>

class SourceSystem : public sdODE
{
    // This is the system representing the (chemical) source term at a point
public:
    SourceSystem();

    // The ODE function: ydot = f(t,y)
    int f(const realtype t, const sdVector& y, sdVector& ydot);

    // Calculate the Jacobian matrix: J = df/dy
    int denseJacobian(const realtype t, const sdVector& y, const sdVector& ydot, sdMatrix& J);

    // A simpler finite difference based Jacobian
    int fdJacobian(const realtype t, const sdVector& y, const sdVector& ydot, sdMatrix& J);

    void unroll_y(const sdVector& y); // fill in current state variables from sdvector
    void roll_y(sdVector& y) const; // fill in sdvector with current state variables
    void roll_ydot(sdVector& ydot) const; // fill in sdvector with current time derivatives

    // Setup functions
    void resize(size_t nSpec);
    void resetSplitConstants();

    void writeJacobian(sundialsCVODE& solver, ostream& out);
    void writeState(sundialsCVODE& solver, ostream& out, bool init);

    // A class that provides the strain rate and its time derivative
    StrainFunction strainFunction;

    configOptions* options;

    // current state variables
    double U, dUdt; // tangential velocity
    double T, dTdt; // temperature
    dvector Y, dYdt; // species mass fractions

    // Extra constant term introduced by splitting
    dvector splitConst; // constant terms

    // Diagonal Jacobian elements for this this term
    dvector diagonalJac;
    bool updateDiagonalJac;

    // Cantera data
    CanteraGas* gas;
    PerfTimer* reactionRatesTimer;
    PerfTimer* thermoTimer;
    PerfTimer* jacobianTimer;

    // other parameters
    size_t nSpec;
    int j; // grid index for this system
    double x; // grid position for this system
    dvector W; // species molecular weights [kg/kmol]
    double rhou; // density of the unburned mixture

    // Other quantities
    dvector wDot; // species net production rates [kmol/m^3*s]
    double qDot; // heat release rate per unit volume [W/m^3]

private:
    // Physical properties
    double rho; // density [kg/m^3]
    double cp; // specific heat capacity (average) [J/kg*K]
    dvector cpSpec; // species specific heat capacity [J/mol*K]
    double Wmx; // mixture molecular weight [kg/mol]
    dvector hk; // species enthalpies [J/kmol]

};


class SourceSystemQSS : public QSSIntegrator
{
    // This is the system representing the (chemical) source term at a point,
    // Integrated with the QSSIntegrator
public:
    SourceSystemQSS();

    // The ODE function: ydot = f(t,y)
    void odefun(double t, const dvector& y, dvector& q, dvector& d, bool corrector=false);

    void unroll_y(const dvector& y, bool corrector=false); // fill in current state variables from sdvector
    void roll_y(dvector& y) const; // fill in y with current state variables
    void roll_ydot(dvector& q, dvector& d) const; // fill in q and d with current time derivatives

    // Set problem size
    void initialize(size_t nSpec);
    void setOptions(configOptions& options);

    void resetSplitConstants();

    void writeState(ostream& out, bool init);

    // A class that provides the strain rate and its time derivative
    StrainFunction strainFunction;

    configOptions* options;

    // current state variables
    double U, dUdtQ, dUdtD; // tangential velocity
    double T, dTdtQ, dTdtD; // temperature
    dvector Y, dYdtQ, dYdtD; // species mass fractions

    // Cantera data
    CanteraGas* gas;
    PerfTimer* reactionRatesTimer;
    PerfTimer* thermoTimer;

    // other parameters
    size_t nSpec;
    int j; // grid index for this system
    double x; // grid position for this system
    dvector W; // species molecular weights [kg/kmol]
    double rhou; // density of the unburned mixture

    // Constant terms introduced by splitting method
    dvector splitConstY;
    double splitConstT, splitConstU;

    // Other quantities
    dvector wDotQ, wDotD; // species production / destruction rates [kmol/m^3*s]
    double qDot; // heat release rate per unit volume [W/m^3]

private:
    // Physical properties
    double rho; // density [kg/m^3]
    double cp; // specific heat capacity (average) [J/kg*K]
    dvector cpSpec; // species specific heat capacity [J/mol*K]
    double Wmx; // mixture molecular weight [kg/mol]
    dvector hk; // species enthalpies [J/kmol]
};
