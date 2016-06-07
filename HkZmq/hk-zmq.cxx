#include <getopt.h>
#include <czmq.h>
#include <HkdioConfig.h>
/* The name of this program. */
const char *program_name;

void help(FILE *stream, int exit_code) {

  fprintf(stream, "\n%s %d.%d.%d\n\n", HKDIO_NAME, HKDIO_VERSION_MAJOR,
          HKDIO_VERSION_MINOR, HKDIO_VERSION_PATCH);
  fprintf(stream, "Usage options\n");
  fprintf(stream,
          "  -h --help                   Display help\n"
          "  -v --verbose                Verbose (default off)\n"
          "  -b --base <url>             Base url (default off)\n"
          "  -c --base-connect <url>     Base connect url (default off)\n"
          "  -w --worker <url>           Worker prefix url (default off)\n"
          "  -n --number-of-worker <num> Number of workers (default off)\n"
          "  -m --run-manager            Runs manager (default off)\n"
          "\n");

  exit(exit_code);
}

int main(int argc, char **argv) {

  int next_option;

  /* A string listing valid short options letters. */
  const char *const short_options = "hvb:c:w:n:m";
  /* An array describing valid long options. */
  const struct option long_options[] = {
      {"help", 0, NULL, 'h'},   {"verbose", 0, NULL, 'v'},
      {"base", 1, NULL, 'b'},   {"connect", 1, NULL, 'c'},
      {"worker", 1, NULL, 'w'}, {"number-of-workers", 1, NULL, 'n'},
      {"manager", 0, NULL, 'm'}};

  bool verbose = false;
  char *url_base = 0;
  char *url_base_connect = 0;
  char *url_worker = strdup("wk-");
  bool run_manager = 0;
  int n_workers = 0;
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
      url_base = strdup(optarg);
      break;
    case 'c':
      url_base_connect = strdup(optarg);
      break;
    case 'w':
      free(url_worker);
      url_worker = strdup(optarg);
      break;
    case 'n':
      n_workers = atoi(optarg);
      break;
    case 'm':
      run_manager = true;
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

  zactor_t *base = 0;
  if (url_base) {
    printf("Base is binding to %s ...\n", url_base);
    base = zactor_new(zgossip, (void *)"base");
    assert(base);
    if (verbose)
      zstr_send(base, "VERBOSE");
    zstr_sendx(base, "SET", "server/timeout", "100", NULL);
    zstr_sendx(base, "BIND", url_base, NULL);
    zpoller_add(poller, zactor_sock(base));
  }

  zactor_t *manager = 0;
  if (url_base_connect && run_manager) {
    printf("Manager is connecting to %s ...\n", url_base_connect);
    manager = zactor_new(zgossip, (void *)"manager");
    assert(manager);
    if (verbose)
      zstr_send(manager, "VERBOSE");
    zstr_sendx(manager, "CONNECT", url_base_connect, NULL);
    zpoller_add(poller, zactor_sock(manager));
  }

  zlist_t *list_workers = zlist_new();
  zactor_t *worker = 0;
  for (int i = 0; i < n_workers; i++) {

    if (url_base_connect) {
      printf("Worker %d is connecting to %s ...\n", i, url_base_connect);
      worker = zactor_new(zgossip, (void *)zsys_sprintf("wk-%d", i));
      assert(worker);
      if (verbose)
        zstr_send(worker, "VERBOSE");
      zstr_sendx(worker, "CONNECT", url_base_connect, NULL);
      zstr_sendx(worker, "PUBLISH", zsys_sprintf("%s%d", url_worker, i),
                 zsys_sprintf("val:%d", i), NULL);
      zpoller_add(poller, zactor_sock(worker));
      zlist_append(list_workers, worker);
    }
  }

  zmsg_t *msg;
  while (!zsys_interrupted) {
    zsock_t *which = (zsock_t *)zpoller_wait(poller, -1);
    if (!which)
      break;
    if ((base) && (which == zactor_sock(base))) {
      msg = zmsg_recv(base);
      if (!msg)
        break;
      char *info = zmsg_popstr(msg);
      char *key = zmsg_popstr(msg);
      char *value = zmsg_popstr(msg);
      printf("base: %s %s %s\n", info, key, value);
      free(info);
      free(key);
      free(value);
      zmsg_destroy(&msg);
    } else if ((manager) && (which == zactor_sock(manager))) {
      msg = zmsg_recv(which);
      if (!msg)
        break;
      char *info = zmsg_popstr(msg);
      char *key = zmsg_popstr(msg);
      char *value = zmsg_popstr(msg);
      printf("manager: %s %s %s\n", info, key, value);
      free(info);
      free(key);
      free(value);
      zmsg_destroy(&msg);
    }
  }
  /// we are dropping other traffic

  zactor_t *actor = (zactor_t *)zlist_first(list_workers);
  while (actor) {
    zactor_destroy(&actor);
    actor = (zactor_t *)zlist_next(list_workers);
  }
  zlist_destroy(&list_workers);

  if (manager)
    zactor_destroy(&manager);
  if (base)
    zactor_destroy(&base);

  zpoller_destroy(&poller);

  if (url_base)
    free(url_base);
  if (url_base_connect)
    free(url_base_connect);
  if (url_worker)
    free(url_worker);

  return 0;
}
