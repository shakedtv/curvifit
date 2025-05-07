#include <ansi_c.h>
//==============================================================================
//
// Title:		fitfunc.c
// Purpose:		A short description of the implementation.
//
// Created on:	16/02/2021 at 17:32:38 by Shaked Tuval.
// Copyright:	Tel Aviv University. All Rights Reserved.
//
//==============================================================================


static double flin (double x, double a[], int na);
static double fexp (double x, double a[], int na);
static double fpoly (double x, double a[], int na);
static double fgauss (double x, double a[], int na);
static double flog (double x, double a[], int na);
static double fln (double x, double a[], int na);


//==============================================================================


static double flin (double x, double a[], int na) {
	
	return a[0] + a[1] * x;
}

static double fexp (double x, double a[], int na) {
	
	return a[0] * exp (a[1] * x);
}

static double fpoly (double x, double a[], int na) {
	double y = 0;
	int i;
	
	for (i = 0; i < na; i++)
		y += a[i] * pow (x, i);
	
	return y;
}

static double fgauss (double x, double a[], int na) {
	
	return a[0] * exp ( - (x - a[1]) * (x - a[1]) / (2 * a[2] * a[2]) );
}

static double flog (double x, double a[], int na) {
	
	return a[0] * log10 (a[1] * x);
}

static double fln (double x, double a[], int na) {
	
	return a[0] * log (a[1] * x);
}
