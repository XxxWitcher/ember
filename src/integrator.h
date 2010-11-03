#pragma once

#include "mathUtils.h"
#include "sundialsUtils.h"

class ODE
{
public:
    // ODE function defined as ydot = f(t,y)
    virtual void f(const double t, const dvector& y, dvector& ydot) = 0;
};


class LinearODE
{
public:
    LinearODE() { };
    ~LinearODE() { };

    // ODE defined as ydot = f(t,y) = J*y + c
    virtual void get_A(sdBandMatrix& J) = 0;
    virtual void get_C(dvector& y) = 0;
    virtual void resize(int N) {}
    virtual void initialize() {}
};


class Integrator
{
public:
    Integrator();
    ~Integrator() { };

    // Initialization - Each of these must be called before starting integration
    virtual void set_y0(const dvector& y0);
    virtual void initialize(const double t0, const double h);

    // Accessor functions
    double get_h() const;
    double get_t() const;
    virtual const dvector& get_y() const;
    virtual const dvector& get_ydot() = 0;

    // Actually do the integration
    virtual void step() = 0; // take a single step
    virtual void integrateToTime(double tEnd) = 0;

    dvector y; // solution vector
    dvector ydot; // derivative of state vector
    double t; // current time

protected:
    double h; // timestep
    size_t N; // Dimension of y
};

class ExplicitIntegrator : public Integrator
{
    // Integrates an ODE defined as ydot = f(t,y) using Euler's method
public:
    ExplicitIntegrator(ODE& ode);
    ~ExplicitIntegrator();

    void set_y0(const dvector& y0);
    const dvector& get_ydot();

    // Actually do the integration
    void step(); // take a single step
    void integrateToTime(double tEnd);

private:
    ODE& myODE;
    dvector ydot; // time derivative of solution vector
};

class BDFIntegrator : public Integrator
{
public:
    BDFIntegrator(LinearODE& ode);
    ~BDFIntegrator();

    // Setup
    void resize(int N, int upper_bw, int lower_bw);
    void set_y0(const dvector& y0);
    void initialize(const double t0, const double h);

    const dvector& get_ydot();
    const dvector& get_diagonal();

    // Actually do the integration
    void step(); // take a single step
    void integrateToTime(double tEnd);

private:
    LinearODE& myODE;
    sdBandMatrix* A;
    sdBandMatrix* LU;
    vector<int> p; // pivot matrix
    unsigned int stepCount; // the number of steps taken since last initialization
    int N; // The system size
    int upper_bw, lower_bw; // bandwidth of the Jacobian
    dvector yprev; // previous solution
    dvector c;
    dvector Adiag; // diagonal components of A
};
