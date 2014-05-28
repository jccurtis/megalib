/*
 * MCalibrationFit.cxx
 *
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


// Include the header:
#include "MCalibrationFit.h"

// Standard libs:

// ROOT libs:
#include "TMath.h"
#include "TH1.h"
#include "TF1.h"

// MEGAlib libs:
#include "MStreams.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(MCalibrationFit)
#endif


////////////////////////////////////////////////////////////////////////////////


//! Default constructor
MCalibrationFit::MCalibrationFit() : m_Fit(0), m_IsFitUpToDate(false), m_BackgroundModel(0), m_EnergyLossModel(0)
{
}


////////////////////////////////////////////////////////////////////////////////


//! Copy constructor
MCalibrationFit::MCalibrationFit(const MCalibrationFit& Fit)
{
  // We don't care about double initilization, so:
  m_Fit = 0;
  *this = Fit;
}


////////////////////////////////////////////////////////////////////////////////


//! Default destructor
MCalibrationFit::~MCalibrationFit()
{
  delete m_Fit;
}


////////////////////////////////////////////////////////////////////////////////


//! The assignment operator
MCalibrationFit& MCalibrationFit::operator= (const MCalibrationFit& CalibrationFit)
{
  delete m_Fit;
  if (CalibrationFit.m_Fit != 0) {
    m_Fit = new TF1(*CalibrationFit.m_Fit);
    for (int i = 0; i < CalibrationFit.m_Fit->GetNpar(); ++i) {
      m_Fit->SetParameter(i, CalibrationFit.m_Fit->GetParameter(i));
    }
  } else {
    m_Fit = 0;
  }
    
  m_IsFitUpToDate = CalibrationFit.m_IsFitUpToDate;
  m_BackgroundModel = CalibrationFit.m_BackgroundModel;
  m_EnergyLossModel = CalibrationFit.m_EnergyLossModel;

  return *this;
}


////////////////////////////////////////////////////////////////////////////////


//! Clone this fit - the returned element must be deleted!
MCalibrationFit* MCalibrationFit::Clone() const
{
  return new MCalibrationFit(*this);
}


////////////////////////////////////////////////////////////////////////////////


//! Set the background model
//! If it doesn't exist don't use any
void MCalibrationFit::SetBackgroundModel(unsigned int BackgroundModel)
{
  if (BackgroundModel == c_BackgroundModelNone ||
      BackgroundModel == c_BackgroundModelFlat ||
      BackgroundModel == c_BackgroundModelLinear) {
    m_BackgroundModel = BackgroundModel;  
  } else {
    merr<<"Unknown background model ID: "<<BackgroundModel<<". Using no background model"<<endl; 
    m_BackgroundModel = c_BackgroundModelNone;
  }
}


////////////////////////////////////////////////////////////////////////////////


//! Set the energy loss model
//! If it doesn't exist don't use any
void MCalibrationFit::SetEnergyLossModel(unsigned int EnergyLossModel)
{
  if (EnergyLossModel == c_EnergyLossModelNone ||
      EnergyLossModel == c_EnergyLossModelGaussianConvolvedDeltaFunction) {
    m_EnergyLossModel = EnergyLossModel;  
  } else {
    merr<<"Unknown energy loss model ID: "<<EnergyLossModel<<". Using no energy loss model"<<endl; 
    m_EnergyLossModel = c_EnergyLossModelNone;
  }
}


////////////////////////////////////////////////////////////////////////////////


//! Return the number of fit parameters in the background fit
int MCalibrationFit::GetBackgroundFitParameters()
{
  if (m_BackgroundModel == c_BackgroundModelNone) {
    return 0;
  } else if (m_BackgroundModel == c_BackgroundModelFlat) {
    return 1;
  } else if (m_BackgroundModel == c_BackgroundModelLinear) {
    return 2;
  }
  
  merr<<"Unknown background model ID: "<<m_BackgroundModel<<". Using no background model"<<endl; 
  return 0;
}


////////////////////////////////////////////////////////////////////////////////


//! Return the number of fit parameters in the energy loss fit
int MCalibrationFit::GetEnergyLossFitParameters()
{
  if (m_EnergyLossModel == c_EnergyLossModelNone) {
    return 0;
  } else if (m_EnergyLossModel == c_EnergyLossModelGaussianConvolvedDeltaFunction) {
    return 1;
  }
  
  merr<<"Unknown energy loss model ID: "<<m_EnergyLossModel<<". Using no energy loss model"<<endl; 
  return 0;
}


////////////////////////////////////////////////////////////////////////////////


//! The function for fitting the background
double MCalibrationFit::BackgroundFit(double* X, double* P)
{
  if (m_BackgroundModel == c_BackgroundModelNone) {
    return 0;
  } else if (m_BackgroundModel == c_BackgroundModelFlat) {
    return P[0];
  } else if (m_BackgroundModel == c_BackgroundModelLinear) {
    return P[0] + P[1]*X[0];
  }
  
  merr<<"Unknown background model ID: "<<m_BackgroundModel<<". Using no background model"<<endl; 
  return 0;
}


////////////////////////////////////////////////////////////////////////////////


//! The function for fitting the energy loss
double MCalibrationFit::EnergyLossFit(double* X, double* P)
{
  int BPM = GetBackgroundFitParameters();
  
  if (m_EnergyLossModel == c_EnergyLossModelNone) {
    return 0;
  } else if (m_EnergyLossModel == c_EnergyLossModelGaussianConvolvedDeltaFunction) {
    double Return = 0.5*(P[0+BPM] + P[1+BPM]);
    if (P[3+BPM] != 0) {
      Return += 0.5*(P[1+BPM] - P[0+BPM])*TMath::Erf((X[0]-P[2+BPM])/sqrt(2.0)/P[3+BPM]);
    }
    return Return;
  }
  
  merr<<"Unknown energy loss model ID: "<<m_EnergyLossModel<<". Using no energy loss model"<<endl; 
  return 0;
}


////////////////////////////////////////////////////////////////////////////////


//! Set all fit parameters
void MCalibrationFit::SetFitParameters(TH1D& Hist, double Min, double Max)
{
  if (m_Fit == 0) return;
  
  if (m_BackgroundModel == c_BackgroundModelNone) {
  } else if (m_BackgroundModel == c_BackgroundModelFlat) {
    m_Fit->SetParName(0, "Offset flat background");
    m_Fit->SetParameter(0, 0);
    m_Fit->SetParLimits(0, 0, 2*Hist.GetMaximum());
  } else if (m_BackgroundModel == c_BackgroundModelLinear) {
    m_Fit->SetParName(0, "Offset linear background");
    m_Fit->SetParameter(0, 0);
    m_Fit->SetParName(1, "Gradient linear background");
    m_Fit->SetParameter(1, 0);
  }
  
  int BPM = GetBackgroundFitParameters();
  
  if (m_EnergyLossModel == c_EnergyLossModelNone) {
  } else if (m_EnergyLossModel == c_EnergyLossModelGaussianConvolvedDeltaFunction) {
    m_Fit->SetParName(0+BPM, "Left offset energy loss");
    m_Fit->SetParameter(0+BPM, 0.1*Hist.GetMaximum());
    m_Fit->SetParLimits(0+BPM, 0, 2*Hist.GetMaximum());
    m_Fit->SetParName(1+BPM, "Right offset energy loss");
    m_Fit->SetParameter(1+BPM, 0.1*Hist.GetMaximum());
    m_Fit->SetParLimits(1+BPM, 0, 2*Hist.GetMaximum());
    m_Fit->SetParName(2+BPM, "Mean energy loss");
    m_Fit->SetParameter(2+BPM, 0.5*(Min+Max));
    m_Fit->SetParLimits(2+BPM, Min, Max);
    m_Fit->SetParName(3+BPM, "Sigma energy loss");
    m_Fit->SetParameter(3+BPM, 5*Hist.GetBinWidth(1));
    m_Fit->SetParLimits(3+BPM, Hist.GetBinWidth(1), Max-Min);
  }  
}


////////////////////////////////////////////////////////////////////////////////


//! Mimic ROOT Draw functionality
void MCalibrationFit::Draw(MString Options)
{
  if (m_Fit != 0 && m_IsFitUpToDate == true) {
    m_Fit->DrawCopy(Options);
  } else {
    merr<<"Fit cannot be drawn since it is not up to date"<<endl; 
  }
}


// MCalibrationFit.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
