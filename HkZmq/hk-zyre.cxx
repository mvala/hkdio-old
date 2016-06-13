#include <getopt.h>
#include <zyre.h>
#include <HkdioConfig.h>

void help(FILE *stream, int exit_code) {

  fprintf(stream, "\n%s %d.%d.%d\n\n", HKDIO_NAME, HKDIO_VERSION_MAJOR,
          HKDIO_VERSION_MINOR, HKDIO_VERSION_PATCH);
  fprintf(stream, "Usage options\n");
  fprintf(stream, "  -h --help                   Display help\n"
                  "  -v --verbose                Verbose (default off)\n"
                  "\n");

  exit(exit_code);
}

int main(int argc, char **argv) {

  int next_option;

  /* A string listing valid short options letters. */
  const char *const short_options = "hvn:";
  /* An array describing valid long options. */
  const struct option long_options[] = {{"help", 0, NULL, 'h'},
                                        {"verbose", 0, NULL, 'v'},
                                        {"name", 1, NULL, 'n'}};

  char *hostname = zsys_hostname();
  bool verbose = false;
  char *name = strdup("node");
  do {
    next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option) {
    case 'h':
      help(stdout, 0);
      break;
    case 'v':
      verbose = true;
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
  //  Set up poller
  zpoller_t *poller = zpoller_new(NULL);
  assert(poller);

  zyre_t *node = zyre_new(name);
  assert(node);
  zyre_set_header(node, "X-HELLO", "World");
  if (verbose)
    zyre_set_verbose(node);
  zpoller_add(poller, zyre_socket(node));

  if (zyre_start(node)) {
    zsys_error("Problem starting node '%s' !!!", zyre_name(node));
    return 1;
  };

  zmsg_t *msg;
  while (!zsys_interrupted) {
    zsock_t *which = (zsock_t *)zpoller_wait(poller, -1);
    if (!which)
      break;
    if ((node) && (which == zyre_socket(node))) {
      zyre_event_t *event = zyre_event_new(node);
      if (!event)
        break;
      const char *type = zyre_event_type(event);
      const char *peer_uuid = zyre_event_peer_uuid(event);

      const char *peer_name = zyre_event_peer_name(event);
      msg = zyre_event_msg(event);
      char *msg_str = 0;
      if (msg)
        msg_str = zmsg_popstr(msg);

      printf("%s[%s]: %s %s %s %s\n", zyre_name(node), hostname, type,
             peer_uuid, peer_name, msg_str);
      
      if (streq(zyre_event_type(event), "ENTER")) {
        zyre_whispers(node, peer_uuid, "Testing from %s ", zyre_name(node));
      }
      
      if (msg_str)
        free(msg_str);
      
      zyre_event_destroy(&event);
    }
  }

  zyre_stop(node);

  zyre_destroy(&node);

  zpoller_destroy(&poller);

  zstr_free(&hostname);

  return 0;
}
