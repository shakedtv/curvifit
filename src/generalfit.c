//==============================================================================
//
// Title:		generalfit.c
// Purpose:		Main fitting function.
//
// Created by: Shaked Tuval, 2021
// License:    MIT License (see LICENSE file)
//
//==============================================================================

//==============================================================================
// Include files

#include "datafitheader.h"


//==============================================================================
// Constants

#define STEPDOWN	0.1
#define CHICUT		0.00001		// maximum differential allowed between successive chi sqr values 
#define MAXITER		1000000		// Max no. of iterations to minimize chisq.

//==============================================================================
// Types

struct errors {
	double *cov;
	double *err;
};

struct gradstep {
	double *anew;
	double stepsum;
	double *grad;
	int stopflag;
	int iter;
};

//==============================================================================
// Static global variables


//==============================================================================
// Static functions

static void FEvalArray (double (*func)(double, double *, int), double X[], double Y[], int n,
						 double a[], int na);
static struct gradstep GradStep (double (*func)(double, double *, int), double X[], double dX[], double Y[], double dY[],
						int n, double a[], int na, double stepsize[], double stepdown, int iter);
static double CalcChi2 (double (*func)(double, double *, int), double X[], double dX[], double Y[], double dY[],
						int n, double a[], int na);
static struct errors Errors (double (*func)(double, double *, int), double X[], double dX[], double Y[], double dY[], int n,
			   double a[], int na, double stepsize[]);




// Runs func on every item of array inarray and returns array of evaluated values.
static void FEvalArray (double (*func)(double, double *, int), double Xin[], double Yout[], int n,
						 double a[], int na) {
	int i;
	for (i = 0; i < n; i++)
		Yout[i] = func (Xin[i], a, na);
	return;
}

// Calculates chi2 using input coefficients.
// Formula: sum ( ( y - f(x) )^2 / ( dy^2 + ( ( f(x+dx) - f(x-dx) ) / 2 )^2 ) ).
static double CalcChi2 (double (*func)(double, double *, int), double X[], double dX[], double Y[], double dY[],
						int n, double a[], int na) {
	double Xup[n], Xdwn[n], Xeval[n], Xupeval[n], Xdwneval[n], chi2 = 0;
	int i;
	
	Add1D (X, dX, n, Xup);
	Sub1D (X, dX, n, Xdwn);
	
	for (i = 0; i < n; i++) {
		Xeval[i] = func (X[i], a, na);
		Xupeval[i] = func (Xup[i], a, na);
		Xdwneval[i] = func (Xdwn[i], a, na);
	}
	
	for (i = 0; i < n; i++) 
		chi2 += (Y[i] - Xeval[i]) * (Y[i] - Xeval[i]) / ( dY[i] * dY[i] + (Xupeval[i] - Xdwneval[i]) * (Xupeval[i] - Xdwneval[i]) / 4 );
	
	return chi2;
}

// Calculates the gradient at a point in parameter space.
static double *CalcGrad (double (*func)(double, double *, int), double X[], double dX[], double Y[], double dY[],
						 int n, double a[], int na, double stepsize[]) {
	double c[na], chisq1, chisq2, da, t = 0;
	static double grad[11];
	int i;
	
	Copy1D (a, na, grad);
	chisq2 = CalcChi2 (func, X, dX, Y, dY, n, a, na);
	
	for (i = 0; i < na; i++) {
		Copy1D (a, na, c);
		da = 0.01 * stepsize[i];
		c[i] += da;
		chisq1 = CalcChi2 (func, X, dX, Y, dY, n, c, na);
		grad[i] = chisq2 - chisq1;
	}
	
	for (i = 0; i < na; i++)
		t += grad[i] * grad[i];
	
	for (i = 0; i < na; i++)
		grad[i] *= stepsize[i] / sqrt (t);
	
	return grad;
}

// Calculates the (negative) chi^2 gradient at the current point
// in parameter space, and moves in that direction until a minimum is found.
// Returns the new value of the parameters and the total length travelled.
static struct gradstep GradStep (double (*func)(double, double *, int), double X[], double dX[], double Y[],
								 double dY[], int n, double a[], int na, double stepsize[], double stepdown, int iter) {
	double chi1, chi2, chi3, step;
	static double grad[11], anew[11];
	int i;
	struct gradstep gradst;
	gradst.stopflag = 0;
	gradst.stepsum = 0;
	gradst.iter = iter;
	gradst.anew = anew;
	gradst.grad = grad;
	
	chi2 = CalcChi2 (func, X, dX, Y, dY, n, a, na);
	gradst.grad = CalcGrad (func, X, dX, Y, dY, n, a, na, stepsize);
	chi3 = 1.1 * chi2;			
	chi1 = chi3;
	
	// Cut down the step size until a single step yields a decrease in chi^2.
	stepdown *= 2;
	
	for ( ; chi3 > chi2; gradst.iter++) {
		stepdown = stepdown / 2;
		for (i = 0; i < na; i++) 
			gradst.anew[i] = a[i] + stepdown * gradst.grad[i];
		chi3 = CalcChi2 (func, X, dX, Y, dY, n, gradst.anew, na);
		
		if (gradst.iter > MAXITER) {
			gradst.stopflag = 1;
			break;
		}
	}
	
	// Keep going until a minimum is passed.
	for ( ; chi3 < chi2; gradst.iter++) {
		gradst.stepsum = gradst.stepsum + stepdown;
  		chi1 = chi2;
  		chi2 = chi3;
		
		for (i = 0; i < na; i++)
			gradst.anew[i] += gradst.grad[i] * stepdown;	
  		chi3 = CalcChi2 (func, X, dX, Y, dY, n, gradst.anew, na);
		
		if (gradst.iter > MAXITER) {
			gradst.stopflag = 1;
			break;
		}
	}
	
	// Approximate the minimum as a parabola.
	step = stepdown * ((chi3 - chi2) / (chi1 - 2 * chi2 + chi3) + 0.5);
	for (i = 0; i < na; i++)
		gradst.anew[i] -= step * gradst.grad[i];
		
	return gradst;
}

// Calculates the errors on the final fitted parameters by approximating the minimum
// as parabolic in each parameter.
static struct errors Errors (double (*func)(double, double *, int), double X[], double dX[], double Y[],
							 double dY[], int n, double a[], int na, double stepsize[]) {
	double da[na], a1[na], a2[na], a3[na], dChi2da[na][na], err[na], cov[na][na];
	static struct errors errs;
	errs.err = err;
	errs.cov = cov;
	int i, j, k;
	
	for (i = 0; i < na; i++) {
		da[i] = stepsize[i];
		
		Copy1D (a, na, a1);
		a1[i] += da[i];
		
		Copy1D (a, na, a2);
		Copy1D (a1, na, a3);
		
		for (j = 0; j < na; j++) {
			da[j] = stepsize[j];
			a2[j] += da[j];
			a3[j] += da[j];
			dChi2da[i][j] = 0.5 * (CalcChi2 (func, X, dX, Y, dY, n, a, na) - 
								   CalcChi2 (func, X, dX, Y, dY, n, a1, na) -
								   CalcChi2 (func, X, dX, Y, dY, n, a2, na) + 
								   CalcChi2 (func, X, dX, Y, dY, n, a3, na) ) / da[i] / da[j];
		}
	}
	
	InvMatrix (dChi2da, na, errs.cov);
	
	for (k = 0; k < na; k++)
		errs.err[k] = sqrt (fabs (cov[k][k]));
	
	return errs;
}

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/// HIFN  What does your function do?
/// HIPAR x/What inputs does your function expect?
/// HIRET What does your function return?

struct fitparameters GeneralFit (double (*func)(double, double *, int), double X[], double dX[], 
								 double Xres[], double Y[], double dY[], int n, double inita[], int na) {
	double stepsize[na], a[na], stepdown = STEPDOWN, eps, chi1, chi2, cov[na][na], err[na];
	struct gradstep gstep;
	struct errors errors;
	int i;
	gstep.iter = 0;
	eps = pow (2, -52);
	
	for (i = 0; i < na; i++)
		stepsize[i] = fabs (inita[i]) * 0.01 + eps;	
	
	// Initial calculation.
	Copy1D (inita, na, a);							   
	chi2 = CalcChi2 (func, X, dX, Y, dY, n, a, na);						
	chi1 = chi2 + 2 * CHICUT;
	
	// Look for minimal Chisq.
	while (fabs (chi2 - chi1) > CHICUT) {
		gstep = GradStep (func, X, dX, Y, dY, n, a, na, stepsize, stepdown, gstep.iter);
		Copy1D (gstep.anew, na, a);
		stepdown = gstep.stepsum;
		chi1 = chi2;
  		chi2 = CalcChi2 (func, X, dX, Y, dY, n, a, na);
  		
		if (gstep.stopflag == 1) {
			MessagePopup ("Error", "Can't minimize chi^2.\nTry different initial parameters.");
			break;
		}
	}
		
	// Calculate the returned values.
	struct fitparameters fit = {gstep.iter, a, err, cov, CalcChi2 (func, X, dX, Y, dY, n, a, na), n - na}; 
	fit.rchisq = fit.chisq / fit.ndf;
	FEvalArray (func, Xres, fit.fittedY, 5000, a, na);
	XX_Dist (fit.chisq, fit.ndf, &fit.pprob);
	fit.pprob = 1 - fit.pprob;
	errors = Errors (func, X, dX, Y, dY, n, a, na, stepsize);
	fit.aerr = errors.err;
	fit.cov = errors.cov;
	
	return fit;	
}

