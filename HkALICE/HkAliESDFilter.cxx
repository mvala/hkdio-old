/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

/* HkAliESDFilter.cxx
 *
 * Template task producing a P_t spectrum and pseudorapidity distribution.
 * Includes explanations of physics and primary track selections
 *
 * Instructions for adding histograms can be found below, starting with NEW
 * HISTO
 *
 * Based on tutorial example from offline pages
 * Edited by Arvinder Palaha
 */
#include "HkAliESDFilter.h"

#include "Riostream.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TList.h"

#include "AliAnalysisTaskSE.h"
#include "AliAnalysisManager.h"
#include "AliStack.h"
#include "AliESDtrackCuts.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliAODEvent.h"
#include "AliMCEvent.h"

ClassImp(HkAliESDFilter)

    //________________________________________________________________________
    HkAliESDFilter::HkAliESDFilter() // All data members should be initialised
                                     // here
    : AliAnalysisTaskSE(),
      fOutput(0),
      fTrackCuts(
          0) // The last in the above list should not have a comma after it
{
  // Dummy constructor ALWAYS needed for I/O.
}

//________________________________________________________________________
HkAliESDFilter::HkAliESDFilter(
    const char *name) // All data members should be initialised here
    : AliAnalysisTaskSE(name),
      fOutput(0),
      fTrackCuts(
          0) // The last in the above list should not have a comma after it
{
  // Constructor
  // Define input and output slots here (never in the dummy constructor)
  // Input slot #0 works with a TChain - it is connected to the default input
  // container
  // Output slot #1 writes into a TH1 container
  DefineOutput(1, TList::Class()); // for output list
}

//________________________________________________________________________
HkAliESDFilter::~HkAliESDFilter() {
  // Destructor. Clean-up the output list, but not the histograms that are put
  // inside
  // (the list is owner and will clean-up these histograms). Protect in PROOF
  // case.
  if (fOutput && !AliAnalysisManager::GetAnalysisManager()->IsProofMode()) {
    delete fOutput;
  }
  delete fTrackCuts;
}

//________________________________________________________________________
void HkAliESDFilter::UserCreateOutputObjects() {
  // Create histograms
  // Called once (on the worker node)

  fOutput = new TList();
  fOutput->SetOwner(); // IMPORTANT!

  fTrackCuts = AliESDtrackCuts::GetStandardITSTPCTrackCuts2010(kTRUE);
  // === Primary Track Selection ===
  //
  // The definition of a primary track is taken from the ALICE Twiki
  // page
  // https://twiki.cern.ch/twiki/bin/view/ALICE/SelectionOfPrimaryTracksForPpDataAnalysis
  // using the following parameters for a standard dN/dPt analysis:
  //  track quality cuts:
  //          esdTrackCuts->SetMinNClustersTPC(70);
  //          esdTrackCuts->SetMaxChi2PerClusterTPC(4);
  //          esdTrackCuts->SetAcceptKinkDaughters(kFALSE);
  //          esdTrackCuts->SetRequireTPCRefit(kTRUE);
  //          esdTrackCuts->SetRequireITSRefit(kTRUE);
  //          esdTrackCuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,
  //                                                                                     AliESDtrackCuts::kAny);
  //  dca:
  //          if(selPrimaries) {
  //                  // 7*(0.0026+0.0050/pt^1.01)
  //                  esdTrackCuts->SetMaxDCAToVertexXYPtDep("0.0182+0.0350/pt^1.01");
  //          }
  //          esdTrackCuts->SetMaxDCAToVertexZ(2);
  //          esdTrackCuts->SetDCAToVertex2D(kFALSE);
  //          esdTrackCuts->SetRequireSigmaToVertex(kFALSE);
  //
  // The Primary Track Selection is implemented here by creating an
  // AliESDtrackCuts object, with kTRUE argument to choose primary tracks.
  //
  // By default, it is set to the above conditions which are suitable for
  // a standard inclusive dN/dPt analysis. For others, such as identified
  // dN/dPt or strangeness as well as others, follow the above link for
  // the specific changes to include in the selection.

  // To change cuts after selecting some default set, one can use
  // esdtrackcuts->SetMinNClustersTPC(70) for example

  // NEW HISTO added to fOutput here
  PostData(1, fOutput); // Post data for ALL output slots >0 here, to get at
                        // least an empty histogram
}

//________________________________________________________________________
void HkAliESDFilter::UserExec(Option_t *) {
  // Main loop
  // Called for each event

  // Create pointer to reconstructed event
  AliVEvent *event = InputEvent();
  if (!event) {
    Printf("ERROR: Could not retrieve event");
    return;
  }

  AliESDEvent *esd = dynamic_cast<AliESDEvent *>(event);
  if (esd) {

    printf("REC=%d", esd->GetNumberOfTracks());
    AliMCEvent *mcEvent = MCEvent();
    if (mcEvent) {
      Printf(" MC=%d", mcEvent->GetNumberOfTracks());

      // Track loop for reconstructed event
      Int_t ntracks = mcEvent->GetNumberOfTracks();
      for (Int_t i = 0; i < ntracks; i++) {
        AliMCParticle *esdMcTrack = (AliMCParticle *)mcEvent->GetTrack(
            i); // pointer to reconstructed to track
        if (!esdMcTrack) {
          AliError(Form("ERROR: Could not retrieve esdtrack %d", i));
          continue;
        }
      }
    } else {
      printf("\n");
    }
  }
  // NEW HISTO should be filled before this point, as PostData puts the
  // information for this iteration of the UserExec in the container
  PostData(1, fOutput);
}

//________________________________________________________________________
void HkAliESDFilter::Terminate(Option_t *) {
  // Draw result to screen, or perform fitting, normalizations
  // Called once at the end of the query
  fOutput = dynamic_cast<TList *>(GetOutputData(1));
  if (!fOutput) {
    Printf("ERROR: could not retrieve TList fOutput");
    return;
  }
}
