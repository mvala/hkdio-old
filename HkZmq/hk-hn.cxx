#include <HkdioConfig.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <HkHn.h>
#include <THnSparse.h>

void help(FILE *stream, int exit_code) {

  fprintf(stream, "\n%s %d.%d.%d\n\n", HKDIO_NAME, HKDIO_VERSION_MAJOR,
          HKDIO_VERSION_MINOR, HKDIO_VERSION_PATCH);
  fprintf(stream, "Usage options\n");
  fprintf(stream, "  -h --help                   Display help\n"
                  "  -v --verbose                Verbose (default off)\n"
                  "  -g --gossip <url>           Gossip url (e.g. "
                  "\"inproc://gossip-hub\") (default off)\n"
                  "\n");

  exit(exit_code);
}

int main(int argc, char **argv) {

  int next_option;

  /* A string listing valid short options letters. */
  const char *const short_options = "hvg:";
  /* An array describing valid long options. */
  const struct option long_options[] = {{"help", 0, NULL, 'h'},
                                        {"verbose", 0, NULL, 'v'},
                                        {"gossip", 1, NULL, 'g'}};

  bool verbose = false;
  char *gossip_url = 0;

  do {
    next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option) {
    case 'h':
      help(stdout, 0);
      break;
    case 'v':
      verbose = true;
      break;
    case 'g':
      free(gossip_url);
      gossip_url = strdup(optarg);
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

  //  Create two nodes
  zyre_t *node_gossip = 0;
  if (gossip_url) {
    node_gossip = zyre_new("gossip");
    assert(node_gossip);
    zyre_set_header(node_gossip, "X-GOSSIP", "Test");
    if (verbose)
      zyre_set_verbose(node_gossip);

    //  Set inproc endpoint for this node
    zyre_set_endpoint(node_gossip, "inproc://zyre-node1");
    zyre_gossip_bind(node_gossip, gossip_url);
    zyre_start(node_gossip);
  }
  HkHn *h = new HkHn();

  const Int_t nDim = 2;
  Int_t bins[nDim] = {10, 20};
  Double_t min[nDim] = {0., -5.};
  Double_t max[nDim] = {10., 5.};
  THnSparse *hs = new THnSparseD("hs", "hs", nDim, bins, min, max);
  h->SetHistogram(hs);
  if (node_gossip && gossip_url)
    h->SetDiscovery(gossip_url);

  h->SetFiller(0, "F");
  // h->SetFiller(1, "F");
  // h->SetFiller(2, "F");

  for (int i = 0; i < 5; i++) {
    char bin_name[100];
    snprintf(bin_name, sizeof(bin_name), "B%d", i);
    h->SetBin(i, bin_name);
  }

  h->Start();

  h->Print(verbose);
  Double_t fill_val[nDim];
  for (Int_t i = 0; i < 10; ++i) {
    fill_val[0] = i;
    fill_val[1] = 5 - i;
    Int_t iBin = h->Fill(hs->GetNdimensions(), fill_val);
    // Int_t iBin = h->FillFast(fill_val);
    printf("Filled bin : %d\n", iBin);
  }

  hs = h->GetHistogram();
  if (hs) {
    hs->Print();
  }

  h->Stop();
  delete h;

  if (node_gossip) {
    zyre_stop(node_gossip);

    zyre_destroy(&node_gossip);
  }
  free(gossip_url);
  return 0;
}
