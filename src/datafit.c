//==============================================================================
//
// Title:		datafit
// Purpose:		A short description of the application.
//
// Created by: Shaked Tuval, 2021
// License:    MIT License (see LICENSE file)
//
//==============================================================================

//==============================================================================
// Include files

#include "cvidef.h"
#include <userint.h>

#include "datafit.h"
#include "generalfit.c"
#include "datafitheader.h"

//==============================================================================
// Constants

#define MAXLEN	1000	//	Max no. of points to fit/lines on file.

enum fittype {LIN, EXP, POLY, GAUSS, LOG, LN};

//==============================================================================
// Types

struct pointdata {
	double X[MAXLEN];
	double dX[MAXLEN];
	double Xres[5000];
	double Y[MAXLEN];
	double dY[MAXLEN];
};

//==============================================================================
// Static global variables

static int mainpanel, graphpanel, fitpanel, respanel, axespanel, helppanel, graphmenu,
	   		fittype, N, fitN, na, errplotX[MAXLEN], errplotY[MAXLEN], fitplot, dataplot,
			resplot, initfitplot, rangecheck, dataflag, fitflag, rangeflag, logerror;
static struct pointdata data, fitdata;
static double (*fitfun)();
static struct fitparameters fitpar, initfit;
static char results[2000];


//==============================================================================
// Static functions

static void ChangeDataRange ();
static void GetTitles ();



static void ChangeDataRange () {
	double offset, ymin, ymax, xmin, xmax;
	int ixmin, ixmax, i, j, iymin, iymax;
	
	// Change X range if FITRANGE is selected. X points don't have to be in ascending order.
	if (rangecheck) {
		GetCtrlVal (mainpanel, MAINPANEL_XMIN, &xmin);
		GetCtrlVal (mainpanel, MAINPANEL_XMAX, &xmax);
		
		if (!(xmin < xmax)) {
			MessagePopup ("Value Error", "xmin must be smaller than xmax.\nCheck values and try again.");
			return;
		}
	}
	
	else
		MaxMin1D (data.X, N, &xmax, &ixmax, &xmin, &ixmin);
	
	// Copy points to fit to fitdata arrays.
	for (i = 0, j = 0; i < N; i++) {	
		if (data.X[i] >= xmin && data.X[i] <= xmax) {
			fitdata.X[j] = data.X[i];
			fitdata.dX[j] = data.dX[i];
			fitdata.Y[j] = data.Y[i];
			fitdata.dY[j] = data.dY[i];
			j++;
			if (data.X[i] <= 0)
				logerror = 1;
		}
	}
	
	fitN = j;  // No. of points to fit (points in fitdata).

	// Set graph axes according to min/max values.
	xmin *= (xmin > 0) ? 0.9 : 1.1;
	xmax *= (xmax > 0) ? 1.1 : 0.9;
	MaxMin1D (data.Y, fitN, &ymax, &iymax, &ymin, &iymin);
	ymin *= (ymin > 0) ? 0.9 : 1.1;
	ymax *= (ymax > 0) ? 1.1 : 0.9;
	SetAxisRange (graphpanel, GRAPHPANEL_GRAPH, VAL_MANUAL, xmin, xmax, VAL_MANUAL, ymin, ymax);
	
	// Generate a high resultion X vector for plotting fitted function.
	offset = (xmax - xmin) / 5000;
	for (i = 0; i < 5000; i++)
		fitdata.Xres[i] = xmin + i * offset;
	
	rangeflag = 1;
	
	return;
}
				
static void GetTitles () {
	char xlabel[100], ylabel[100], resylabel[100], plottitle[100], resplottitle[150];
	
	GetCtrlVal (mainpanel, MAINPANEL_XLABEL, xlabel);
	GetCtrlVal (mainpanel, MAINPANEL_YLABEL, ylabel);
	GetCtrlVal (mainpanel, MAINPANEL_PLOTTITLE, plottitle);
	GetCtrlVal (mainpanel, MAINPANEL_RESYLABEL, resylabel);
	SetCtrlAttribute (graphpanel, GRAPHPANEL_GRAPH, ATTR_LABEL_TEXT, plottitle);
	SetCtrlAttribute (graphpanel, GRAPHPANEL_GRAPH, ATTR_LABEL_JUSTIFY, VAL_CENTER_JUSTIFIED);
	SetCtrlAttribute (graphpanel, GRAPHPANEL_GRAPH, ATTR_XNAME, xlabel);
	SetCtrlAttribute (graphpanel, GRAPHPANEL_GRAPH, ATTR_YNAME, ylabel);
	SetCtrlAttribute (respanel, RESPANEL_GRAPH, ATTR_YNAME, resylabel);
	sprintf (resplottitle, "%s - Residuals Plot", plottitle);
	SetCtrlAttribute (respanel, RESPANEL_GRAPH, ATTR_LABEL_TEXT, resplottitle);
	SetCtrlAttribute (respanel, RESPANEL_GRAPH, ATTR_LABEL_JUSTIFY, VAL_CENTER_JUSTIFIED);
}

//==============================================================================
// Global variables

//==============================================================================
// Global functions
   
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	// out of memory 
	if ((mainpanel = LoadPanel (0, "datafit.uir", MAINPANEL)) < 0)
		return -1;
	if ((graphpanel = LoadPanel (0, "datafit.uir", GRAPHPANEL)) < 0)
		return -1;
	if ((fitpanel = LoadPanel (0, "datafit.uir", FITPANEL)) < 0)
		return -1;
	if ((respanel = LoadPanel (0, "datafit.uir", RESPANEL)) < 0)
		return -1;
	if ((axespanel = LoadPanel (0, "datafit.uir", AXESPANEL)) < 0)
		return -1;
	if ((helppanel = LoadPanel (0, "datafit.uir", HELPPANEL)) < 0)
				return -1;
	DisplayPanel (mainpanel);
	graphmenu = GetPanelMenuBar (graphpanel);
	RunUserInterface ();
	DiscardPanel (mainpanel);
	DiscardPanel (fitpanel);
	DiscardPanel (graphpanel);
	DiscardPanel (respanel);
	DiscardPanel (axespanel);
	DiscardPanel (helppanel);
	return 0;
}		 	   
					
//==============================================================================
// UI callback function prototypes

//	Determine fit type selected by user.
int CVICALLBACK FitTypeSelect (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	int poly = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panel, control, &fittype);
			if (fittype == POLY)
				poly = 1;
			SetCtrlAttribute (mainpanel, MAINPANEL_POLYDEG, ATTR_VISIBLE, poly);

			fitflag = rangeflag = 0;
			break;
	}
	return 0;
}

//	Import data from file when OPENFILE is clicked.
int CVICALLBACK OpenDataFile (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)							
	{
		case EVENT_COMMIT:
			char filepath[500];
			int f, fmtstatus;
	
			if (FileSelectPopupEx ("", "*.*", "*.csv; *.txt", "Select File", VAL_SELECT_BUTTON, 0, 1, filepath) == 1) {			   
				if ((f = OpenFile (filepath, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII)) > 0) {
					for (N = 0; !eof(f) && N < MAXLEN; N++) {
						fmtstatus = ScanFile (f, "%1f%1f%1f%1f", &data.X[N], &data.dX[N], &data.Y[N], &data.dY[N]);
						if (fmtstatus < 4) {
						MessagePopup ("Error", "Input data file is in wrong format.\nMake sure file contains a 4 column table.");
						CloseFile (f);
						return -1;
						}
					}
					CloseFile (f);
					SetCtrlVal (mainpanel, MAINPANEL_DATAINPUT, filepath);
					dataflag = 1;
					fitflag = rangeflag = 0;
				}
			}
			break;
	}
	return 0;
}

//	Imports data from clipboard when PASTEDATA is clicked.
int CVICALLBACK PasteData (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int stravailable, n = 0, fmtstatus;
			char *pastedstr, *str;
			
			ClipboardGetText (&pastedstr, &stravailable);
			if (!stravailable) {
				MessagePopup ("Error", "No text found on clipboard.\nCopy a table and try again.");
				return -1;
			}
			
			str = pastedstr;					 
			for (N = 0; N < MAXLEN && strlen (str) > 0; N++) {
				fmtstatus = Scan (str, "%1f%1f%1f%1f", &data.X[N], &data.dX[N], &data.Y[N], &data.dY[N]);
				if (fmtstatus < 4) {
					MessagePopup ("Error", "Pasted data is in wrong format or it doesn't contain numbers.\nMake sure to copy a 4 column table.");
					free (pastedstr);
					return -1;
				}
				n = NumFmtdBytes ();
				str += n;
			}
			
			free (pastedstr);
			SetCtrlVal (mainpanel, MAINPANEL_DATAINPUT, "Clipboard");
			dataflag = 1;
			fitflag = rangeflag = 0;
			break;
	}
	return 0;
}

//	Show/hide XMIN and XMAX when FITRANGE is checked/unchecked.
int CVICALLBACK FitRangeSelect (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			rangecheck = !rangecheck;
			SetCtrlAttribute (mainpanel, MAINPANEL_XMIN, ATTR_VISIBLE, rangecheck);
			SetCtrlAttribute (mainpanel, MAINPANEL_XMAX, ATTR_VISIBLE, rangecheck);
			break;
	}
	return 0;
}

//	Fitting of input data to function when FIT is clicked. 
int CVICALLBACK Fit (int panel, int control, int event,
					 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT: {
			
			// Return when there's no input data
			if (!dataflag) {
				MessagePopup ("Error", "No input data found.\nImport data and try again.");
				return -1;
			}
			
			// Return if data was already fitted.
			if (fitflag) {
				DisplayPanel (fitpanel);
				return 0;
			}
			
			int i;
			char fittypestr[100];
			double w[N], err, inita[12];
			initfit.a = inita;
			
			// Change X range if FITRANGE is checked.
			if (!rangeflag)
				ChangeDataRange();
			
			// convert dY to weight for initial fit.
			for (i = 0; i < fitN; i++)
				w[i] = 1 / (fitdata.dY[i] * fitdata.dY[i]);
			
			switch (fittype) {
				case LIN:
					fitfun = flin;
					na = 2;
					LinearFitEx (fitdata.X, fitdata.Y, w, fitN, LEAST_SQUARE, 0, initfit.fittedY, &initfit.a[1], &initfit.a[0], &err);
					sprintf (fittypestr, "Linear fit\ny = a0 + a1 * x");
					break;
			
				case POLY:
					fitfun = fpoly;
					GetCtrlVal (mainpanel, MAINPANEL_POLYDEG, &na);
					if (fitN < na) {
						MessagePopup ("Error", "Number of data points must be greater than\nthe polynomial degree. Try again.");
						return -1;
					}
					na += 1;
					PolyFitWithWeight (fitdata.X, fitdata.Y, w, fitN, na, NULL, NULL, 0, ALGORITHM_POLYFIT_SVD, initfit.fittedY, initfit.a, &err);
					sprintf (fittypestr, "Polynomial fit\ny = a0 + a1 * x + a2 * x^2...");
					break;
					
				case EXP:
					fitfun = fexp;
					na = 2;
					ExpFitEx (fitdata.X, fitdata.Y, w, fitN, LEAST_SQUARE, 0, initfit.fittedY, &initfit.a[0], &initfit.a[1], &err);
					sprintf (fittypestr, "Exponential fit\ny = a0 * exp (a1 * x)");
					break;
					
				case GAUSS:
					fitfun = fgauss;
					na = 3;
					GaussFit (fitdata.X, fitdata.Y, w, fitN, LEAST_SQUARE, 0, NULL, initfit.fittedY, &initfit.a[0], &initfit.a[1], &initfit.a[2], &err);
					sprintf (fittypestr, "Gaussian fit\ny = a0 * exp ( - (x - a1)^2 / (2 * a2^2) )");
					break;
					
				case LOG:
					if (logerror) {
						MessagePopup ("Error", "There are non-positive X values in the input data.\nUse different input data or change data range and try again.");
						return -1;
					}
					fitfun = flog;
					na = 2;
					LogFit (fitdata.X, fitdata.Y, w, fitN, 10, LEAST_SQUARE, 0, initfit.fittedY, &initfit.a[0], &initfit.a[1], &err);
					sprintf (fittypestr, "Base 10 logarithm fit\ny = a0 * log (a1 * x)");
					break;
					
				case LN:
					if (logerror) {
						MessagePopup ("Error", "There are non-positive X values in the input data. Use different input data or delete non-positive X points and try again.");
						return -1;
					}
					fitfun = fln;
					na = 2;
					LogFit (fitdata.X, fitdata.Y, w, fitN, EULER, LEAST_SQUARE, 0, initfit.fittedY, &initfit.a[0], &initfit.a[1], &err);
					sprintf (fittypestr, "Natural logarithm fit\ny = a0 * ln (a1 * x)");
					break;
			}
			
			fitpar = GeneralFit (fitfun, fitdata.X, fitdata.dX, fitdata.Xres, fitdata.Y, fitdata.dY, fitN, initfit.a, na);		
			
			// Calculate goodness of fit for initial fit. 
			initfit.chisq = CalcChi2 (fitfun, fitdata.X, fitdata.dX, fitdata.Y, fitdata.dY, fitN, initfit.a, na);
			XX_Dist (initfit.chisq, fitN - na, &initfit.pprob);
			initfit.pprob = 1 - initfit.pprob;
			initfit.rchisq = initfit.chisq / (fitN - na);
			FEvalArray (fitfun, fitdata.Xres, initfit.fittedY, 5000, initfit.a, na);
			
			// Print fit parameters to FITPANEL.
			char initastr[250] = "", fitparstr[250] = "", covstr[1000] = "", temp[200] = "";
			
			for (i = 0; i < na; i++) {
				sprintf (temp, "a%d = %f ± %f\n", i, initfit.a[i], err);
				strcat (initastr, temp);
			}
			
			for (i = 0; i < na; i++) {
				sprintf (temp, "a%d = %f ± %f\n", i, fitpar.a[i], fitpar.aerr[i]);
				strcat (fitparstr, temp);
			}
			
			if (na > 1) {
				for (i = 0; i < na; i++) {
					for (int j = i + 1; j < na; j++) {
						sprintf (temp, "cov(a%d ,a%d) = %f\n", i, j, *(fitpar.cov + i * na + j));
						strcat (covstr, temp);
					}
				}
			}

			sprintf (results, "%s\n\nIteration no. %d\nInitial parameters' values:\n%schi^2 = %f\nchi^2_red = %f\np_prob = %f\n\nFitted parameters' values:\n%s%schi^2 = %f\nndf = %d\nchi^2_red = %f\np_prob = %f",
					fittypestr, fitpar.iter, initastr, initfit.chisq, initfit.rchisq, initfit.pprob,
					fitparstr, covstr, fitpar.chisq, fitpar.ndf, fitpar.rchisq, fitpar.pprob);
			ResetTextBox (fitpanel, FITPANEL_FITPARAMETERS, results);
			fitflag = 1;
			if (fitplot)
				DeleteGraphPlot (graphpanel, GRAPHPANEL_GRAPH, fitplot, VAL_IMMEDIATE_DRAW);
			if (initfitplot)
				DeleteGraphPlot (graphpanel, GRAPHPANEL_GRAPH, initfitplot, VAL_IMMEDIATE_DRAW);
			if (resplot)
				DeleteGraphPlot (respanel, RESPANEL_GRAPH, resplot, VAL_IMMEDIATE_DRAW);
			fitplot = resplot = initfitplot = 0;
			DisplayPanel (fitpanel);
		}
		break;
	}
	return 0;
}

//	Plot input data to graph, including error bars on both axes, when PLOTDATA button is clicked.
int CVICALLBACK PlotData (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	int i;
	
	switch (event)
	{
		case EVENT_COMMIT:
			if (!dataflag) {
				MessagePopup ("Error", "No input data found.\nImport data and try again.");
				return -1;
			}
			
			if (!rangeflag)
				ChangeDataRange();
			
			GetTitles();
			DisplayPanel (graphpanel);
			
			if (!dataplot) {
				dataplot = PlotXY (graphpanel, GRAPHPANEL_GRAPH, data.X, data.Y, N, VAL_DOUBLE,
								   VAL_DOUBLE, VAL_SCATTER, VAL_SMALL_SOLID_SQUARE, VAL_SOLID, 1, VAL_BLUE);
			
				for (i = 0; i < N; i++) {
					errplotY[i] = PlotLine (graphpanel, GRAPHPANEL_GRAPH, data.X[i], data.Y[i] - data.dY[i],
								data.X[i], data.Y[i] + data.dY[i], VAL_BLUE);
					errplotX[i] = PlotLine (graphpanel, GRAPHPANEL_GRAPH, data.X[i] - data.dX[i],
								data.Y[i], data.X[i] + data.dX[i], data.Y[i], VAL_BLUE);
				}
				eventData1 = 1;
				break;
			}
			
			
			GetPlotAttribute (graphpanel, GRAPHPANEL_GRAPH, dataplot, ATTR_TRACE_VISIBLE, &eventData1);
			if (eventData1)
				break;
			eventData1 = !eventData1;
				
		case 1000:
			for (i = 0; i < N; i++) 
				SetPlotAttribute (graphpanel, GRAPHPANEL_GRAPH, errplotX[i], ATTR_TRACE_VISIBLE, eventData1);
			for (i = 0; i < N; i++) 
				SetPlotAttribute (graphpanel, GRAPHPANEL_GRAPH, errplotY[i], ATTR_TRACE_VISIBLE, eventData1);	
			break;
	}
	
	SetMenuBarAttribute (graphmenu, GRAPHMENU_PLOTS_DATAPLOT, ATTR_CHECKED, eventData1);
	
	return 0;
}

//	Plot fitted function or inital fit to graph when PLOTFIT/PLOTINITFIT is clicked.
int CVICALLBACK PlotFit (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if (!fitflag) {
				MessagePopup ("Error", "Data was not fitted.\nFit data and try again.");
				return -1;
			}
	
			switch (control) {
				case MAINPANEL_PLOTFIT:
					if (!fitplot) {
						fitplot = PlotXY (graphpanel, GRAPHPANEL_GRAPH, fitdata.Xres, fitpar.fittedY, 5000,
								VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_RED);
					}
					
					SetMenuBarAttribute (graphmenu, GRAPHMENU_PLOTS_FITPLOT, ATTR_CHECKED, 1);
					SetPlotAttribute (graphpanel, GRAPHPANEL_GRAPH, fitplot, ATTR_TRACE_VISIBLE, 1);
					break;
					
				case MAINPANEL_PLOTINITFIT:
					if (!initfitplot) {
						initfitplot = PlotXY (graphpanel, GRAPHPANEL_GRAPH, fitdata.Xres, initfit.fittedY, 5000,
								VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_GREEN);
					}
					
					SetMenuBarAttribute (graphmenu, GRAPHMENU_PLOTS_INITFITPLOT, ATTR_CHECKED, 1);
					SetPlotAttribute (graphpanel, GRAPHPANEL_GRAPH, initfitplot, ATTR_TRACE_VISIBLE, 1);
					break;
			}
			
			GetTitles();
			DisplayPanel (graphpanel);
			break;
	}
	return 0;
}

//	Plot the residuals plot when PLOTRES is clicked.
int CVICALLBACK PlotResiduals (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if (!fitflag) {
				MessagePopup ("Error", "Data was not fitted.\nFit data and try again.");
				return -1;
			}
	
			int i, resplotX[N], resplotY[N], zero;
			double resY[N];
			
			if (!resplot) {
				GetTitles();
				
				// Calculate residuals and save in resY; Plot error bars and save in resplotX and resplotY.
				for (i = 0; i < N; i++) {
					resY[i] = data.Y[i] - fitfun (data.X[i], fitpar.a, na);
				
					resplotY[i] = PlotLine (respanel, RESPANEL_GRAPH, data.X[i], resY[i] - data.dY[i],
								data.X[i], resY[i] + data.dY[i], VAL_BLUE);
					resplotX[i] = PlotLine (respanel, RESPANEL_GRAPH, data.X[i] - data.dX[i],
								resY[i], data.X[i] + data.dX[i], resY[i], VAL_BLUE);
				}
				
				// zero - baseline plot; resplot - data points.
				zero = PlotLine (respanel, RESPANEL_GRAPH, fitdata.Xres[0], 0, fitdata.Xres[4999], 0,
								 VAL_RED);
				resplot = PlotXY (respanel, RESPANEL_GRAPH, data.X, resY, N, VAL_DOUBLE, VAL_DOUBLE,
								  VAL_SCATTER, VAL_SMALL_SOLID_SQUARE, VAL_SOLID, 1, VAL_BLUE);
			}
			
			DisplayPanel (respanel);
			break;
	}
	return 0;
}

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK PanelCB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			switch (panel) {
				case 1:
					QuitUserInterface (0);
					break;
					
				default:
					HidePanel(panel);
					break;
			}
			break;
	}
	
	return 0;
}

//	Show help panel when HELP is clicked.
int CVICALLBACK OpenHelp (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel (helppanel);
			break;
	}
	return 0;
}

//	Export GRAPH to image file.
void CVICALLBACK SaveGraph (int menuBar, int menuItem, void *callbackData,
							int panel)
{
	int control;
	char filepath[500];
	
	control = GetActiveCtrl (panel);
	if (FileSelectPopupEx ("", "*.*", "Bitmap (*.bmp); PNG (*.png); JPEG (*.jpg);", "Save Graph", VAL_SAVE_BUTTON, 0, 0, filepath) > 0)
		SaveCtrlDisplayToFile (panel, control, 1, -1, -1, filepath);
}

//	Open dialog for changing GRAPH axes range.
void CVICALLBACK ChangeAxesClick (int menuBar, int menuItem, void *callbackData,
								  int panel)
{ 
	int xscaling, yscaling;
	double xmin, xmax, ymin, ymax;
	
	GetAxisRange (graphpanel, GRAPHPANEL_GRAPH, &xscaling, &xmin, &xmax, &yscaling, &ymin, &ymax);
	SetCtrlVal (axespanel, AXESPANEL_XMIN, xmin);
	SetCtrlVal (axespanel, AXESPANEL_XMAX, xmax);	
	SetCtrlVal (axespanel, AXESPANEL_YMIN, ymin);	
	SetCtrlVal (axespanel, AXESPANEL_YMAX, ymax);
	InstallPopup (axespanel);
}

//	Change GRAPH axes range dialog callback.
int CVICALLBACK ChangeAxes (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			double xmin, xmax, ymin, ymax;
			GetCtrlVal (axespanel, AXESPANEL_XMIN, &xmin);
			GetCtrlVal (axespanel, AXESPANEL_XMAX, &xmax);	
			GetCtrlVal (axespanel, AXESPANEL_YMIN, &ymin);	
			GetCtrlVal (axespanel, AXESPANEL_YMAX, &ymax);
			SetAxisRange (graphpanel, GRAPHPANEL_GRAPH, VAL_MANUAL, xmin, xmax, VAL_MANUAL, ymin, ymax);
			RemovePopup (0);
			break;
	}
	return 0;
}

//	Show/hide plots by user from GRAPHMENU. 	
void CVICALLBACK PlotMenuClick (int menuBar, int menuItem, void *callbackData,
						   int panel)
{
	int x, plot;

	GetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, &x);
	
	switch (menuItem) {
		case GRAPHMENU_PLOTS_INITFITPLOT:
			if (!initfitplot) 
				PlotFit (MAINPANEL, MAINPANEL_PLOTINITFIT, EVENT_COMMIT, 0, 0, 0);
			plot = initfitplot;
			break;
			
		case GRAPHMENU_PLOTS_FITPLOT:
			if (!fitplot)
				PlotFit (MAINPANEL, MAINPANEL_PLOTFIT, EVENT_COMMIT, 0, 0, 0);
			plot = fitplot;
			break;
		
		case GRAPHMENU_PLOTS_DATAPLOT:
			if (!dataplot) 
				PlotData (0, 0, EVENT_COMMIT, 0, 0, 0);
			else
				PlotData (0, 0, 1000, 0, !x, 0);
			plot = dataplot;
			break;
			
	}
	SetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, !x);
	SetPlotAttribute (graphpanel, GRAPHPANEL_GRAPH, plot, ATTR_TRACE_VISIBLE, !x);
}

//	Show/hide annotations when GRAPH is clicked.
int CVICALLBACK GraphClick (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	double x, y;
	static int annotation = 1;
	char annotationtxt[100];
	
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			if (GetGraphCoordsFromPoint (panel, control, MakePoint (eventData2, eventData1), &x, &y) != 1)
				return -1;
			SetGraphCursor (panel ,control, 1, x, y);
			GetGraphCursor (panel ,control, 1, &x, &y);
			sprintf (annotationtxt, "X: %f\nY: %f", x, y);
			
			SetAnnotationAttribute (panel, control, annotation, ATTR_ANNOTATION_XVALUE, x);
			SetAnnotationAttribute (panel, control, annotation, ATTR_ANNOTATION_YVALUE, y);
			SetAnnotationAttribute (panel, control, annotation, ATTR_ANNOTATION_CAPTION, annotationtxt);
			SetAnnotationAttribute (panel, control, annotation, ATTR_ANNOTATION_VISIBLE, 1);
			SetAnnotationAttribute (panel, control, annotation, ATTR_ANNOTATION_CAPTION_BGCOLOR, VAL_OFFWHITE);
		
			if (annotation < 5)
				annotation++;
			else
				annotation = 1;  
			break;
			
		case EVENT_RIGHT_CLICK:
			if (annotation == 1)
				annotation = 5;
			else
				annotation--;	 
			SetAnnotationAttribute (panel, control, annotation, ATTR_ANNOTATION_VISIBLE, 0);
			
			break;  
		
	}
	return 0;
}

//	Clear all annotations from GRAPH when user clicks on menu item CLEARANNOT.
void CVICALLBACK ClearAnnotations (int menuBar, int menuItem, void *callbackData,
								   int panel)
{
	int i;
	for (i = 1; i < 6; i++)
		SetAnnotationAttribute (graphpanel, GRAPHPANEL_GRAPH, i, ATTR_ANNOTATION_VISIBLE, 0);
}

// Export the fit parameters to a text file when user clicks on SAVEFITPAR.
int CVICALLBACK SaveFitParameters (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			char filepath[500];
			int f;
				 
			if (FileSelectPopupEx ("", "*.txt", "", "Save Fit Parameters", VAL_SAVE_BUTTON, 0, 0, filepath) > 0) {
				if ((f = OpenFile (filepath, VAL_WRITE_ONLY, VAL_TRUNCATE, VAL_ASCII)) > 0) {
					WriteFile (f, results, strlen (results));
					CloseFile (f);
				}
			}

			break;
	}
	return 0;
}

// Determines if annotations snap to point or not when user clicks on menu item SNAPTOPOINT.
void CVICALLBACK SnaptoPoint (int menuBar, int menuItem, void *callbackData,
							  int panel)
{
	int x;
	GetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, &x);
	SetCursorAttribute (graphpanel, GRAPHPANEL_GRAPH, 1, ATTR_CURSOR_MODE, !x);
	SetMenuBarAttribute (menuBar, menuItem, ATTR_CHECKED, !x);	
}
