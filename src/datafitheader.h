//==============================================================================
//
// Title:		datafitheader.h
// Purpose:		A short description of the interface.
//
// Created by: Shaked Tuval, 2021
// License:    MIT License (see LICENSE file)
//
//==============================================================================

#ifndef __datafitheader_H__
#define __datafitheader_H__


//==============================================================================
// Include files

#include <ansi_c.h>
#include <analysis.h>
#include <formatio.h>
#include <lowlvlio.h>
#include <cvirte.h>		
#include "toolbox.h"

#include "fitfunc.c"


//==============================================================================
// Constants


		
//==============================================================================
// Types

struct fitparameters {
	int iter;
	double *a;
	double *aerr;
	double *cov;
	double chisq;
	int ndf;
	double rchisq;
	double pprob;
	double fittedY[5000];
};
		

//==============================================================================
// Global functions

static struct fitparameters GeneralFit (double (*func)(double, double *, int), double X[], double dX[], double Xres[], double Y[], double dY[],
								 int n, double inita[], int na);



#endif  /* ndef __datafitheader_H__ */
