#include "HkHn.h"
#include <THnSparse.h>
#include <iostream>

string HkHn::fDiscoveryUrl;

HkHn::HkHn() : fFillIter(0), fBinIter(0), fHist(0), fListOfActors(0) {
  ///
  /// Default constructor
  ///

  fFillIter = fFillers.begin();
  fBinIter = fBins.begin();
}

HkHn::~HkHn() {
  ///
  /// Destructor
  ///
}

bool HkHn::Start() {

  if (!fListOfActors)
    fListOfActors = zlist_new();

  for (map<int, string>::iterator ii = fFillers.begin(); ii != fFillers.end();
       ++ii) {
    cout << (*ii).first << ": " << (*ii).second << endl;
    zactor_t *actor =
        zactor_new(HkHn::StartFiller, (void *)(*ii).second.data());
    assert(actor);
    char *name = zstr_recv(actor);
    (*ii).second = name;
    zstr_free(&name);
    zlist_append(fListOfActors, actor);
  }

  for (map<int, string>::iterator ii = fBins.begin(); ii != fBins.end(); ++ii) {
    cout << (*ii).first << ": " << (*ii).second << endl;
    zactor_t *actor = zactor_new(HkHn::StartBin, (void *)(*ii).second.data());
    assert(actor);
    char *name = zstr_recv(actor);
    (*ii).second = name;
    zstr_free(&name);
    zlist_append(fListOfActors, actor);
  }

  return true;
}

bool HkHn::Stop() {

  zclock_sleep(1000);

  zactor_t *actor = (zactor_t *)zlist_first(fListOfActors);
  while (actor) {
    zactor_destroy(&actor);
    actor = (zactor_t *)zlist_next(fListOfActors);
  }
  zlist_destroy(&fListOfActors);
  fListOfActors = 0;

  return true;
}

void HkHn::StartFiller(zsock_t *pipe, void *args) {

  //  Do some initialization
  char *name = (char *)args;

  zsock_signal(pipe, 0);
  zyre_t *node = zyre_new(name);
  assert(node);
  zyre_set_header(node, "X-ZHIST-FILL", name);

  if (!HkHn::fDiscoveryUrl.empty()) {
    zyre_set_endpoint(node, "inproc://%s", name);
    zyre_gossip_connect(node, "inproc://gossip-hub");
  }
  if (zyre_start(node)) {
    zsys_error("Problem starting node '%s' !!!", zyre_name(node));
    return;
  };

  char *hostname = zsys_hostname();

  //  Set up poller
  zpoller_t *poller = zpoller_new(pipe, zyre_socket(node), NULL);
  assert(poller);
  zstr_send(pipe, zyre_uuid(node));

  bool terminated = false;
  while (!terminated) {
    zsock_t *which = (zsock_t *)zpoller_wait(poller, -1);
    if (!which)
      break;
    if (which == pipe) {
      zmsg_t *msg = zmsg_recv(pipe);
      if (!msg)
        break; //  Interrupted
      char *command = zmsg_popstr(msg);
      //  All actors must handle $TERM in this way
      if (streq(command, "$TERM"))
        terminated = true;
      else {
        puts("E: invalid message to actor");
        assert(false);
      }
      free(command);
      zmsg_destroy(&msg);
    } else if (which == zyre_socket(node)) {

      zyre_event_t *event = zyre_event_new(node);
      if (!event)
        break;

      if (streq(zyre_event_type(event), "ENTER")) {

      } else if (streq(zyre_event_type(event), "EXIT")) {
        // One bin exits
      } else {

        zsys_info("%s[%s]: %s %s %s", zyre_name(node), hostname,
                  zyre_event_type(event), zyre_event_peer_uuid(event),
                  zyre_event_peer_name(event));
      }
      zyre_event_destroy(&event);
    }
  }
  zyre_stop(node);

  // delete hs;

  zyre_destroy(&node);
  zstr_free(&hostname);
}
void HkHn::StartBin(zsock_t *pipe, void *args) {
  //  Do some initialization
  char *name = (char *)args;

  zsock_signal(pipe, 0);

  zyre_t *node = zyre_new(name);
  assert(node);
  zyre_set_header(node, "X-ZHIST-BIN", name);
  if (!HkHn::fDiscoveryUrl.empty()) {
    zyre_set_endpoint(node, "inproc://%s", name);
    zyre_gossip_connect(node, "inproc://gossip-hub");
  }
  if (zyre_start(node)) {
    zsys_error("Problem starting node '%s' !!!", zyre_name(node));
    return;
  };
  char *hostname = zsys_hostname();

  //  Set up poller
  zpoller_t *poller = zpoller_new(pipe, zyre_socket(node), NULL);
  assert(poller);

  zstr_send(pipe, zyre_uuid(node));

  bool terminated = false;
  while (!terminated) {
    zsock_t *which = (zsock_t *)zpoller_wait(poller, -1);
    if (!which)
      break;
    if (which == pipe) {
      zmsg_t *msg = zmsg_recv(pipe);
      if (!msg)
        break; //  Interrupted
      char *command = zmsg_popstr(msg);
      //  All actors must handle $TERM in this way
      if (streq(command, "$TERM"))
        terminated = true;
      else {
        puts("E: invalid message to actor");
        assert(false);
      }
      free(command);
      zmsg_destroy(&msg);
    } else if (which == zyre_socket(node)) {
      zyre_event_t *event = zyre_event_new(node);
      if (!event)
        break;
      if (streq(zyre_event_type(event), "ENTER")) {

      } else if (streq(zyre_event_type(event), "EXIT")) {
        // One bin exits
      } else {

        zsys_info("%s[%s]: %s %s %s", zyre_name(node), hostname,
                  zyre_event_type(event), zyre_event_peer_uuid(event),
                  zyre_event_peer_name(event));
      }
    }
  }
  zyre_stop(node);
  zyre_destroy(&node);
  zstr_free(&hostname);
}

void HkHn::Print(bool verbose) {
  zsys_info("Number of fillers: %d", fFillers.size());
  if (verbose) {
    for (map<int, string>::iterator ii = fFillers.begin(); ii != fFillers.end();
         ++ii) {
      cout << (*ii).first << ": " << (*ii).second << endl;
    }
  }
  zsys_info("Number of bins: %d", fBins.size());
  if (verbose) {
    for (map<int, string>::iterator ii = fBins.begin(); ii != fBins.end();
         ++ii) {
      cout << (*ii).first << ": " << (*ii).second << endl;
    }
  }
}

Int_t HkHn::Fill(Int_t size, Double_t *val, Double_t w) {

  if (!fHist) {
    printf("fHist is null!!!\n");
    return -100;
  }

  if (fHist->GetNdimensions() != size) {
    printf("size and hist dimensions are not equal\n");
    return -101;
  }

  return FillFast(val, w);
}

Int_t HkHn::FillFast(Double_t *val, Double_t w) {

  if (fFillIter == fFillers.end())
    fFillIter = fFillers.begin();

  // Using round robin fillers
  printf("Using %s as filler %d ...\n", fFillIter->second.data(),
         fFillIter->first);

  Int_t iBin = fHist->Fill(val, w);
  if (iBin > 0) {
    // TODO find bin worker
    zsys_info("Sending it to bin %d and worker with name=%s", iBin,
              fBins[iBin].data());
              // zyre_whispers()
  }

  fFillIter++;

  return iBin;
}