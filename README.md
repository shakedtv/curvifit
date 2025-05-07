# Curve Fitting Tool (C, LabWindows/CVI)

A scientific tool developed as a final project in C, designed to fit experimental measurement data to various models.

## Supported Fitting Models
- Linear
- Polynomial (user-defined degree, up to 10)
- Exponential (base e and 10)
- Logarithmic (any base)
- Gaussian

## Features Overview

- Import data from file or clipboard  
- Display measurement graph with error bars  
- Select and control fitting range within imported data  
- Fit models: linear, polynomial (up to degree 10), exponential, logarithmic (base e and 10), Gaussian  
- Auto-estimation of initial parameters and Levenberg–Marquardt optimization  
- Custom curve fitting algorithm (written from scratch)  
- Calculation of parameter errors and covariance matrix  
- Evaluation of fit quality: Chi-squared, reduced Chi-squared, p-value  
- Graphical output: initial fit, optimized fit, residuals  
- Customize graph/axis titles, toggle graph elements, export plots as images  
- Export fit parameters to a text file  
- Integrated help window with full user instructions

**Graph window features:**

- Manually set axis ranges  
- Add/remove up to 5 labels to graph points  
- Toggle visible curves  
- Export graph as image file

## Screenshots

### Main Interface  
Full control panel for function selection, input data, axis titles, and fitting options.  
![Main Interface](screenshots/main-interface.png)

### Fit Results  
Initial and final parameter values, with uncertainties and covariance matrix shown.  
<img src="screenshots/fit-results.jpg" width="48%">
<img src="screenshots/fit-results2.jpg" width="48%">

### Fit Results – Graph  
Fitted curve (green), initial guess (red), and data points with error bars. Includes interactive point labeling.  
![Fit Results - Graph](screenshots/fit-results-graph.png)

### Residuals Plot  
Residual values plotted against X, highlighting fit quality and outliers.  
![Residuals Plot](screenshots/residualts-plot.png)

### Help Window  
Built-in help window explaining the fitting process, function definitions, and usage steps.  
![Help Window](screenshots/help.png)

## Folder Structure

- `src/`: Source code and UI file  
- `examples/`: Sample input files for different models  
- `screenshots/`: Output images (to be added)

## Usage

To compile and run, use LabWindows/CVI.  
Load `datafit.uir` for the graphical interface. Example input files are provided in `/examples`.

---

**Note:** Project documentation is in Hebrew (see PDF file)
