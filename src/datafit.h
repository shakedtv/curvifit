/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  AXESPANEL                        1       /* callback function: PanelCB */
#define  AXESPANEL_XMIN                   2       /* control type: numeric, callback function: (none) */
#define  AXESPANEL_XMAX                   3       /* control type: numeric, callback function: (none) */
#define  AXESPANEL_YMIN                   4       /* control type: numeric, callback function: (none) */
#define  AXESPANEL_YMAX                   5       /* control type: numeric, callback function: (none) */
#define  AXESPANEL_OKBUTTON               6       /* control type: command, callback function: ChangeAxes */

#define  FITPANEL                         2       /* callback function: PanelCB */
#define  FITPANEL_FITPARAMETERS           2       /* control type: textBox, callback function: (none) */
#define  FITPANEL_SAVEFITPAR              3       /* control type: command, callback function: SaveFitParameters */

#define  GRAPHPANEL                       3       /* callback function: PanelCB */
#define  GRAPHPANEL_GRAPH                 2       /* control type: graph, callback function: GraphClick */

#define  HELPPANEL                        4       /* callback function: PanelCB */
#define  HELPPANEL_PICTURE                2       /* control type: picture, callback function: (none) */

#define  MAINPANEL                        5       /* callback function: PanelCB */
#define  MAINPANEL_FITTYPE                2       /* control type: ring, callback function: FitTypeSelect */
#define  MAINPANEL_PLOTFIT                3       /* control type: command, callback function: PlotFit */
#define  MAINPANEL_PLOTRES                4       /* control type: command, callback function: PlotResiduals */
#define  MAINPANEL_PLOTINITFIT            5       /* control type: command, callback function: PlotFit */
#define  MAINPANEL_FIT                    6       /* control type: command, callback function: Fit */
#define  MAINPANEL_PLOTDATA               7       /* control type: command, callback function: PlotData */
#define  MAINPANEL_OPENFILE               8       /* control type: command, callback function: OpenDataFile */
#define  MAINPANEL_PASTEDATA              9       /* control type: command, callback function: PasteData */
#define  MAINPANEL_POLYDEG                10      /* control type: numeric, callback function: FitTypeSelect */
#define  MAINPANEL_DECORATION             11      /* control type: deco, callback function: (none) */
#define  MAINPANEL_DATAINPUT              12      /* control type: string, callback function: (none) */
#define  MAINPANEL_RESYLABEL              13      /* control type: string, callback function: (none) */
#define  MAINPANEL_YLABEL                 14      /* control type: string, callback function: (none) */
#define  MAINPANEL_XLABEL                 15      /* control type: string, callback function: (none) */
#define  MAINPANEL_PLOTTITLE              16      /* control type: string, callback function: (none) */
#define  MAINPANEL_FITPARAMETERS          17      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_DATASELECTION          18      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_FITRANGE               19      /* control type: radioButton, callback function: FitRangeSelect */
#define  MAINPANEL_XMAX                   20      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_XMIN                   21      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_HELP                   22      /* control type: command, callback function: OpenHelp */
#define  MAINPANEL_PLOTTITLES             23      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_DECORATION_3           24      /* control type: deco, callback function: (none) */
#define  MAINPANEL_PLOTOPTIONS            25      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_TEXTMSG_4              26      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_DECORATION_2           27      /* control type: deco, callback function: (none) */
#define  MAINPANEL_DECORATION_4           28      /* control type: deco, callback function: (none) */

#define  RESPANEL                         6       /* callback function: PanelCB */
#define  RESPANEL_GRAPH                   2       /* control type: graph, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  GRAPHMENU                        1
#define  GRAPHMENU_FILE                   2
#define  GRAPHMENU_FILE_ITEM3             3       /* callback function: SaveGraph */
#define  GRAPHMENU_GRAPH                  4
#define  GRAPHMENU_GRAPH_Change           5       /* callback function: ChangeAxesClick */
#define  GRAPHMENU_GRAPH_SNAPTOPOINT      6       /* callback function: SnaptoPoint */
#define  GRAPHMENU_GRAPH_CLEARANNOT       7       /* callback function: ClearAnnotations */
#define  GRAPHMENU_PLOTS                  8
#define  GRAPHMENU_PLOTS_DATAPLOT         9       /* callback function: PlotMenuClick */
#define  GRAPHMENU_PLOTS_INITFITPLOT      10      /* callback function: PlotMenuClick */
#define  GRAPHMENU_PLOTS_FITPLOT          11      /* callback function: PlotMenuClick */

#define  RESMENUBAR                       2
#define  RESMENUBAR_FILE                  2
#define  RESMENUBAR_FILE_SAVE             3       /* callback function: SaveGraph */


     /* Callback Prototypes: */

int  CVICALLBACK ChangeAxes(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK ChangeAxesClick(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK ClearAnnotations(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK Fit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FitRangeSelect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FitTypeSelect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK GraphClick(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenDataFile(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PasteData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PlotData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PlotFit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK PlotMenuClick(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK PlotResiduals(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SaveFitParameters(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK SaveGraph(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SnaptoPoint(int menubar, int menuItem, void *callbackData, int panel);


#ifdef __cplusplus
    }
#endif
