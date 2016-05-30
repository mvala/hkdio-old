#ifndef __CINT__
#include <TString.h>
#include <AliAnalysisManager.h>
#endif

AliAnalysisTaskSE *AddHkESDFilter() {

  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();

  // create task
  //  gROOT->LoadMacro("HkAliESDFilter.cxx+g");
  AliAnalysisTaskSE *task = new HkAliESDFilter("HkAliESDFilter");
  task->SelectCollisionCandidates(AliVEvent::kMB); // if physics selection
                                                   // performed in UserExec(),
                                                   // this line should be
                                                   // commented
  mgr->AddTask(task);

  TString outputFileName =
      AliAnalysisManager::GetCommonFileName(); // create containers for
                                               // input/output
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  AliAnalysisDataContainer *coutput1 = mgr->CreateContainer(
      "HkAliceESDFilter", TList::Class(), AliAnalysisManager::kOutputContainer,
      outputFileName.Data());

  AliAnalysisDataContainer *cTreeAna =
      mgr->CreateContainer("hkTree", TTree::Class(),
                           AliAnalysisManager::kOutputContainer, "hk.root");

  // connect input/output
  mgr->ConnectInput(task, 0, cinput);
  mgr->ConnectOutput(task, 1, coutput1);
  mgr->ConnectOutput(task, 2, cTreeAna);
  return task;
}
