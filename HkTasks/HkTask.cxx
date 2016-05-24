#include <TDirectory.h>
#include <TROOT.h>
#include <Riostream.h>
#include "HkTask.h"

/// \cond CLASSIMP
ClassImp(HkTask);
/// \endcond

HkTask::HkTask() : TTask("task", "Task"), fParent(0), fOutput(0) {}

HkTask::HkTask(const char *name, const char *title)
    : TTask(name, title), fParent(0), fOutput(0) {

  ///
  /// Default constructor
  ///
  fOutput = new TList();
  fOutput->SetName("out");
  fOutput->SetOwner();
}

HkTask::~HkTask() {
  ///
  /// Destructor
  ///
}

void HkTask::Add(TTask *task) {
  if (!task)
    return;

  TTask::Add(task);

  if (task->InheritsFrom(HkTask::Class())) {
    ((HkTask *)task)->SetParent(this);
  }
}

void HkTask::ExecuteTask(Option_t *option) {

  ///
  /// Execute current task
  ///

  if (fgBeginTask) {
    Error("ExecuteTask", "Cannot execute task:%s, already running task: %s",
          GetName(), fgBeginTask->GetName());
    return;
  }
  if (!IsActive())
    return;

  fOption = option;
  fgBeginTask = this;
  fgBreakPoint = 0;

  if (fBreakin)
    return;
  if (gDebug > 1) {
    TROOT::IndentLevel();
    std::cout << "Execute task:" << GetName() << " : " << GetTitle()
              << std::endl;
    TROOT::IncreaseDirLevel();
  }

  Exec(option);

  fHasExecuted = kTRUE;
  ExecuteTasks(option);
  ExecPost(option);

  if (gDebug > 1)
    TROOT::DecreaseDirLevel();
  if (fBreakout)
    return;

  if (!fgBreakPoint) {
    fgBeginTask->CleanTasks();
    fgBeginTask = 0;
  }
}

void HkTask::ExecuteTasks(Option_t *option) {
  ///
  /// Execute all the subtasks of a task.
  ///
  TIter next(fTasks);
  HkTask *task;
  while ((task = (HkTask *)next())) {
    if (fgBreakPoint)
      return;
    if (!task->IsActive())
      continue;
    if (task->fHasExecuted) {
      task->ExecuteTasks(option);
      continue;
    }
    if (task->fBreakin == 1) {
      printf("Break at entry of task: %s\n", task->GetName());
      fgBreakPoint = this;
      task->fBreakin++;
      return;
    }

    if (gDebug > 1) {
      TROOT::IndentLevel();
      std::cout << "Execute task:" << task->GetName() << " : "
                << task->GetTitle() << std::endl;
      TROOT::IncreaseDirLevel();
    }
    task->Exec(option);
    task->fHasExecuted = kTRUE;
    task->ExecuteTasks(option);
    task->ExecPost(option);
    if (gDebug > 1)
      TROOT::DecreaseDirLevel();
    if (task->fBreakout == 1) {
      printf("Break at exit of task: %s\n", task->GetName());
      fgBreakPoint = this;
      task->fBreakout++;
      return;
    }
  }
}

void HkTask::Init(Option_t *option) {
  TIter next(fTasks);
  HkTask *t;
  while ((t = (HkTask *)next())) {
    t->Init(option);
  }
}

void HkTask::Exec(Option_t * /*option*/) {}

void HkTask::ExecPost(Option_t * /*option*/) {}

void HkTask::Browse(TBrowser *b) {
  fTasks->Browse(b);
  if (fOutput)
    fOutput->Browse(b);
}

void HkTask::Export(TDirectory *parent) {

  if (!parent)
    return;
  TDirectory *out = parent->mkdir(GetName(), GetTitle());
  if (!out)
    return;

  TIter next(fTasks);
  HkTask *t;
  while ((t = (HkTask *)next())) {
    t->Export(out);
  }
  out->cd();
  if (fOutput)
    fOutput->Write();

  parent->cd();
}
