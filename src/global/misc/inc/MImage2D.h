/*
 * MImage2D.h
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MImage2D__
#define __MImage2D__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TCanvas.h>
#include <TH2.h>
#include <TSystem.h>
#include <TStyle.h>

// MEGAlib libs:
#include "MGlobal.h"
#include "MImage.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MImage2D : public MImage
{
  // Public Interface:
 public:
  //! Default constructor
  MImage2D();
  //! Standard constructor
  MImage2D(MString Title, double* IA, 
           MString xTitle, double xMin, double xMax, int xNBins, 
           MString yTitle, double yMin, double yMax, int yNBins, 
           int Spectrum = c_Incendescent, int DrawOption = c_COLCONT4Z);
  //! Standard destructor
  virtual ~MImage2D();

  //! Clone this image
  virtual MImage* Clone();

  //! Set the image array and redisplay it
	virtual void SetImageArray(double* Array);

  //! Set the y-Axis attributes
	virtual void SetYAxis(MString yTitle, double yMin, double yMax, int yNBins);

  //! Display the histogram in the given canvas
	virtual void Display(TCanvas* Canvas = 0);

  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  //! Title of the y axis
	MString m_yTitle;
  //! Minimum y value
	double m_yMin;
  //! Maximum y value
	double m_yMax;
  //! Number of y bins
	int m_yNBins;


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(MImage2D, 0)        // basic 2D image class
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
