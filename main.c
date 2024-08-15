#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum fs_node_type_e {
  ROOT,
  DIR,
  FL,
} fs_node_type_e;

typedef struct fs_node_s {
  char *name;
  fs_node_type_e type;
  struct fs_node_s *prev, *next;
} fs_node_s;

fs_node_s *create_node(char *name, fs_node_type_e type) {
  fs_node_s *node = (fs_node_s *) malloc(sizeof(fs_node_s));

  node->name = (char *) malloc(sizeof(char) * strlen(name));

  strcpy(node->name, name);
  node->type = type;
  node->prev = NULL;
  node->next = NULL;

  return node;
}

bool streq(char *a, char *b) {
  return strncmp(a, b, 3) == 0;
}

int main(void) {
  fs_node_s *root = create_node("root", ROOT), *cur_node = root;
  char line_buff[255];
  bool run = true;

  while (run) {
    char *cmd;
    size_t line_size = sizeof(line_buff);

    if (cur_node->type == ROOT) printf("-> ");

    fgets(line_buff, line_size, stdin);

    line_buff[line_size - 1] = 0;

    cmd = strtok(line_buff, " ");

    if (streq(cmd, "ex\n")) run = false;
    else printf("opcao invalida\n");
  }

  return 0;
}
