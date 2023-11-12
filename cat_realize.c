#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct config {
  int b;
  int e;
  int E;
  int n;
  int s;
  int t;
  int T;
  int v[8];
};

void flags_parser(struct config *conf, int argc, char **argv);

int print_file(char *name, struct config conf);

void print_symb(int c, int *prev, struct config conf, int *index,
                bool *eline_printed, bool *empty_line);

int main(int argc, char **argv) {
  struct config conf = {0};
  flags_parser(&conf, argc, argv);

  for (int i = optind; i < argc; i++) {
    if (strcmp(argv[i], "--") == 0) continue;
    print_file(argv[i], conf);
  }

  return 0;
}

void flags_parser(struct config *conf, int argc, char **argv) {
  int res;
  int idx = 0;

  char *short_options = "beEnstTv";
  struct option long_options[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                  {"number", no_argument, NULL, 'n'},
                                  {"squeeze-blank", no_argument, NULL, 's'},
                                  {"show-ends", no_argument, NULL, 'E'},
                                  {"show-tabs", no_argument, NULL, 'T'},
                                  {"show-nonprinting", no_argument, NULL, 'v'}};

  while ((res = getopt_long(argc, argv, short_options, long_options, &idx)) !=
         -1) {
    switch (res) {
      case 'b':
        conf->b = 1;
        break;
      case 'e':
        conf->e = 1;
        conf->v[0] = 1;
        conf->v[7] = 1;
        break;
      case 'E':
        conf->E = 1;
        break;
      case 'n':
        conf->n = 1;
        break;
      case 's':
        conf->s = 1;
        break;
      case 't':
        conf->t = 1;
        conf->v[1] = 1;
        conf->v[7] = 1;
        break;
      case 'T':
        conf->T = 1;
        break;
      case 'v':
        conf->v[7] = 1;
        break;
      case '?':
        break;
      default:
        break;
    }
  }
}

int print_file(char *name, struct config conf) {
  int err_code = 0;
  FILE *f;

  if (strcmp("-", name) == 0)
    f = stdin;
  else
    f = fopen(name, "rt");

  if (f != NULL) {
    int index = 0;
    bool eline_printed = false;
    bool empty_line = false;
    int c = fgetc(f), prev = '\n';
    while (c != EOF) {
      print_symb(c, &prev, conf, &index, &eline_printed, &empty_line);
      c = fgetc(f);
    }
    if (f != stdin) fclose(f);
  } else {
    err_code = 1;
  }
  return err_code;
}

void print_symb(int c, int *prev, struct config conf, int *index,
                bool *eline_printed, bool *empty_line) {
  if (!(conf.s && *prev == '\n' && c == '\n' && *eline_printed)) {
    if (*prev == '\n' && c == '\n') {
      *eline_printed = true;
    } else {
      *eline_printed = false;
    }

    if (conf.s && c == '\n') {
      if (!*empty_line) {
        putchar(c);
        *empty_line = true;
      }
    } else {
      *empty_line = false;

      if (conf.b && *prev == '\n' && *eline_printed == false) {
        *index += 1;
        printf("%6d\t", *index);
      }
      if (conf.e && c == '\n') {
        printf("$");
      }
      if (conf.E && c == '\n') {
        printf("$");
      }
      if (conf.n && *prev == '\n') {
        *index += 1;
        printf("%6d\t", *index);
      }
      if (conf.t && c == '\t') {
        printf("^");
        c = c + 64;
      }

      if (conf.T && c == '\t') {
        printf("^");
        c = '\t' + 64;
      }
      if (conf.v[7] && c >= 0 && c <= 31 && c != '\n' && c != '\t') {
        printf("^");
        c = c + 64;
      }
      putchar(c);

      if (c == '\n' && !conf.s) {
        *prev = '\n';
      }
    }
  }
  *prev = c;
}
