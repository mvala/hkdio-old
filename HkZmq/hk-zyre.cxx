#include "HkdioConfig.h"

#include <getopt.h>
#include <zyre.h>
#include <THnSparse.h>
#include <TBufferFile.h>

void help(FILE *stream, int exit_code) {

  fprintf(stream, "\n%s %d.%d.%d\n\n", HKDIO_NAME, HKDIO_VERSION_MAJOR,
          HKDIO_VERSION_MINOR, HKDIO_VERSION_PATCH);
  fprintf(stream, "Usage options\n");
  fprintf(stream, "  -h --help                   Display help\n"
                  "  -v --verbose                Verbose (default off)\n"
                  "  -b --bin                    Is bin (default off)\n"
                  "  -f --fill                   Is fill (default off)\n"
                  "  -n --name <name>            Name (default 'node')\n"
                  "\n");

  exit(exit_code);
}

void fill_actor(zsock_t *pipe, void *args) {
  //  Do some initialization
  char *name = (char *)args;
  zyre_t *node = zyre_new(name);
  assert(node);
  zyre_set_header(node, "X-ZHIST-FILL", name);
  if (zyre_start(node)) {
    zsys_error("Problem starting node '%s' !!!", zyre_name(node));
    return;
  };
  char *hostname = zsys_hostname();

  //  Set up poller
  zpoller_t *poller = zpoller_new(pipe, zyre_socket(node), NULL);
  assert(poller);

  Int_t bins[2] = {10, 20};
  Double_t min[2] = {0., -5.};
  Double_t max[2] = {10., 5.};
  THnSparse *hs = new THnSparseD("hs", "hs", 2, bins, min, max);
  THnSparseI *hsId = new THnSparseI("hs", "hs", 2, bins, min, max);
  Double_t fill_val[2];
  zsock_signal(pipe, 0);

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
        if (zyre_event_header(event, "X-ZHIST-BIN")) {
          zmsg_t *msg_hist = zmsg_new();
          // https://root.cern.ch/root/roottalk/roottalk11/att-1290/server.cpp
          // https://root.cern.ch/root/roottalk/roottalk11/att-1290/client.cpp
          TBufferFile bf(TBuffer::kWrite);
          bf.Reset();
          if (bf.WriteObjectAny(hs, hs->Class()) != 1) {
            zsys_error("failed to serialize root object!");
            return;
          }
          zmsg_addmem(msg_hist, bf.Buffer(), bf.Length());
          zsys_info("Sending %lld bytes", bf.Length());
          zyre_whisper(node, zyre_event_peer_uuid(event), &msg_hist);

          // One bin enters
          fill_val[0] = 3;
          fill_val[1] = 2;
          int my_id = 2;
          Int_t iBin = hsId->Fill(fill_val);
          hsId->SetBinContent(iBin, my_id);
          zsys_info("Bin %d", iBin);
        }
      } else if (streq(zyre_event_type(event), "EXIT")) {
        // One bin exits
      }

      zsys_info("%s[%s]: %s %s %s", zyre_name(node), hostname,
                zyre_event_type(event), zyre_event_peer_uuid(event),
                zyre_event_peer_name(event));
      zyre_event_destroy(&event);
    }
  }
  zyre_stop(node);

  delete hs;

  zyre_destroy(&node);
  zstr_free(&hostname);
}

void bin_actor(zsock_t *pipe, void *args) {
  //  Do some initialization
  char *name = (char *)args;
  zyre_t *node = zyre_new(name);
  assert(node);
  zyre_set_header(node, "X-ZHIST-BIN", name);
  if (zyre_start(node)) {
    zsys_error("Problem starting node '%s' !!!", zyre_name(node));
    return;
  };
  char *hostname = zsys_hostname();

  //  Set up poller
  zpoller_t *poller = zpoller_new(pipe, zyre_socket(node), NULL);
  assert(poller);

  zsock_signal(pipe, 0);

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

      zmsg_t *msg;

      zyre_event_t *event = zyre_event_new(node);
      if (!event)
        break;
      msg = zyre_event_msg(event);
      if (msg) {
        zframe_t *buf = zmsg_pop(msg);
        if (buf) {
          zsys_info("Recieved %lld bytes", zframe_size(buf));

          TBufferFile buf_file(TBuffer::kRead, zframe_size(buf),
                               zframe_data(buf), false);
          THnSparse *hsOut =
              (THnSparse *)(buf_file.ReadObjectAny(THnSparse::Class()));
          if (!hsOut)
            zsys_error("no hsOut!!!");
          hsOut->Print("all");
          delete hsOut;
        }
      }

      zsys_info("%s[%s]: %s %s %s", zyre_name(node), hostname,
                zyre_event_type(event), zyre_event_peer_uuid(event),
                zyre_event_peer_name(event));
      zyre_event_destroy(&event);
    }
  }
  zyre_stop(node);
  zyre_destroy(&node);
  zstr_free(&hostname);
}

int main(int argc, char **argv) {

  int next_option;

  /* A string listing valid short options letters. */
  const char *const short_options = "hvbfn:";
  /* An array describing valid long options. */
  const struct option long_options[] = {{"help", 0, NULL, 'h'},
                                        {"verbose", 0, NULL, 'v'},
                                        {"bin", 1, NULL, 'b'},
                                        {"fill", 1, NULL, 'f'},
                                        {"name", 1, NULL, 'n'}};

  // char *hostname = zsys_hostname();
  bool verbose = false;
  char *name = strdup("node");
  bool is_bin = false;
  bool is_fill = false;

  do {
    next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option) {
    case 'h':
      help(stdout, 0);
      break;
    case 'v':
      verbose = true;
      break;
    case 'b':
      is_bin = true;
      break;
    case 'f':
      is_fill = true;
      break;
    case 'n':
      free(name);
      name = strdup(optarg);
      break;
    case '?':
      /* The user specified an invalid option. */
      /* Print usage information to standard error, and exit with exit
       code one (indicating abnormal termination). */
      help(stderr, 1);
      break;
    case -1:
      break;
    /* Done with options.
     */
    default:
      /* Something else: unexpected.*/
      abort();
    }
  } while (next_option != -1);

  /// MAIN
  zactor_t *actor;
  if (is_fill) {
    actor = zactor_new(fill_actor, name);
    assert(actor);
  }
  if (is_bin) {
    actor = zactor_new(bin_actor, name);
    assert(actor);
  }

  if (verbose)
    zstr_send(actor, "VERBOSE");

  //  Set up poller
  zpoller_t *poller = zpoller_new(NULL);
  assert(poller);
  while (!zsys_interrupted) {
    zsock_t *which = (zsock_t *)zpoller_wait(poller, -1);
    if (!which)
      break;
  }

  zactor_destroy(&actor);

  // zyre_t *node = zyre_new(name);
  // assert(node);
  // if (is_bin)
  //   zyre_set_header(node, "X-ZHIST-BIN", "XXX");
  // if (is_fill)
  //   zyre_set_header(node, "X-ZHIST-FILL", "XXX");
  // if (verbose)
  //   zyre_set_verbose(node);
  // zpoller_add(poller, zyre_socket(node));

  // if (zyre_start(node)) {
  //   zsys_error("Problem starting node '%s' !!!", zyre_name(node));
  //   return 1;
  // };

  // zmsg_t *msg;
  // while (!zsys_interrupted) {
  //   zsock_t *which = (zsock_t *)zpoller_wait(poller, -1);
  //   if (!which)
  //     break;
  //   if ((node) && (which == zyre_socket(node))) {
  //     zyre_event_t *event = zyre_event_new(node);
  //     if (!event)
  //       break;
  //     const char *type = zyre_event_type(event);
  //     const char *peer_uuid = zyre_event_peer_uuid(event);
  //     const char *peer_name = zyre_event_peer_name(event);
  //     msg = zyre_event_msg(event);
  //     char *msg_str = 0;
  //     if (msg) {
  //       msg_str = zmsg_popstr(msg);
  //       zframe_t *buf = zmsg_pop(msg);
  //       if (buf) {
  //         zsys_info("Recieved %lld bytes", zframe_size(buf));

  //         TBufferFile buf_file(TBuffer::kRead, zframe_size(buf),
  //                              zframe_data(buf), false);
  //         THnSparse *hsOut =
  //             (THnSparse *)(buf_file.ReadObjectAny(THnSparse::Class()));
  //         if (!hsOut)
  //           zsys_error("no hsOut!!!");
  //         hsOut->Print("all");
  //         delete hsOut;
  //       }
  //     }
  //     zsys_info("%s[%s]: %s %s %s %s", zyre_name(node), hostname, type,
  //               peer_uuid, peer_name, msg_str);

  //     if (streq(zyre_event_type(event), "ENTER")) {
  //       if (zyre_event_header(event, "X-ZHIST-BIN")) {
  //         zsys_info("%s: %s is X-ZHIST-BIN (%s)", zyre_name(node), peer_uuid,
  //                   zyre_event_header(event, "X-ZHIST-BIN"));
  //       }
  //       if (zyre_event_header(event, "X-ZHIST-FILL")) {
  //         zsys_info("%s: %s is X-ZHIST-FILL (%s)", zyre_name(node),
  //         peer_uuid,
  //                   zyre_event_header(event, "X-ZHIST-FILL"));
  //       }
  //       if (zyre_event_header(event, "X-ZHIST-BIN")) {
  //         zmsg_t *msg_hist = zmsg_new();

  //         Int_t bins[2] = {10, 20};
  //         Double_t min[2] = {0., -5.};
  //         Double_t max[2] = {10., 5.};
  //         THnSparse *hs = new THnSparseD("hs", "hs", 2, bins, min, max);
  //         Double_t val[2];
  //         Int_t n = 10;
  //         for (Int_t i = 0; i < n; i++) {
  //           val[0] = i;
  //           val[1] = i - 5;
  //           hs->Fill(val);
  //         }

  //         // source server and client :
  //         //
  //         https://root.cern.ch/root/roottalk/roottalk11/att-1290/server.cpp
  //         //
  //         https://root.cern.ch/root/roottalk/roottalk11/att-1290/client.cpp
  //         TBufferFile bf(TBuffer::kWrite);
  //         bf.Reset();
  //         if (bf.WriteObjectAny(hs, hs->Class()) != 1) {
  //           zsys_error("failed to serialize root object!");
  //           return 1;
  //         }

  //         zmsg_addstr(msg_hist, "zhist");
  //         zmsg_addmem(msg_hist, bf.Buffer(), bf.Length());
  //         zsys_info("Sending %lld bytes", bf.Length());
  //         zyre_whisper(node, peer_uuid, &msg_hist);
  //       } else {
  //         zyre_whispers(node, peer_uuid, "Testing from %s ",
  //         zyre_name(node));
  //       }
  //     }

  //     if (msg_str)
  //       free(msg_str);

  //     zyre_event_destroy(&event);
  //   }
  // }

  // zyre_stop(node);
  // zyre_destroy(&node);
  zpoller_destroy(&poller);
  // zstr_free(&hostname);

  return 0;
}
