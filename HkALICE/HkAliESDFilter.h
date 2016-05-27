/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
/* HkAliESDFilter.h
 *
 * Template task producing a P_t spectrum and pseudorapidity distribution.
 * Includes explanations of physics and primary track selections
 *
 * Based on tutorial example from offline pages
 * Edited by Arvinder Palaha
 */
#ifndef HkAliESDFilter_H
#define HkAliESDFilter_H

#include "AliAnalysisTaskSE.h"

class TH1F;
class TList;
class AliESDtrackCuts;

class HkAliESDFilter : public AliAnalysisTaskSE {
 public:
    HkAliESDFilter();
    HkAliESDFilter(const char *name);
    virtual ~HkAliESDFilter();
    
    virtual void     UserCreateOutputObjects();
    virtual void     UserExec(Option_t *option);
    virtual void     Terminate(Option_t *);
    
 private:
    TList           *fOutput;        // Output list
    AliESDtrackCuts *fTrackCuts;     // Track cuts
    TH1F            *fHistPt;        // Pt spectrum
    TH1F            *fHistCosThetaJ; // Cos(theta) Jackson frame
    TH1F            *fHistCosThetaT; // Cos(theta) Transverse frame
    TH1F            *fHistCosThetaH; // Cos(theta) Helicity frame
    TH1F            *fHistCosThetaA; // Cos(theta) Adair frame
    
    HkAliESDFilter(const HkAliESDFilter&); // not implemented
    HkAliESDFilter& operator=(const HkAliESDFilter&); // not implemented
    
    ClassDef(HkAliESDFilter, 1); // example of analysis
};

#endif

