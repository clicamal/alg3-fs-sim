#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LINE_SIZE 255

typedef enum node_type { FL, DIR } node_type;
typedef enum cmd { MA, MP, LS, CD, RM, EX, NOP, WRNG_CMD } cmd;

typedef struct fs_node {
  char *name;
  node_type type;
  struct fs_node *child, *next;
} fs_node;

typedef struct fs_node_stack_el {
  fs_node *node;
  struct fs_node_stack_el *prev, *next;
} fs_node_stack_el;

typedef struct fs_node_stack {
  fs_node_stack_el *base, *top;
} fs_node_stack;

fs_node *create_node(char *, node_type);
void destroy_node(fs_node **);
bool insert_node(fs_node *, char *, node_type);
bool ma(fs_node *, char *);
bool mp(fs_node *, char *);
bool ls(fs_node *);
fs_node *cd(fs_node_stack *, fs_node *, fs_node *, char *);
bool rm(fs_node *, char *);
void scan(char *);
char *next_token(char *);
bool streq(char *, char *);
cmd parse_cmd(char *);
fs_node_stack_el *create_fs_node_stack_el(fs_node *);
void destroy_fs_node_stack(fs_node_stack *);
fs_node *fs_node_stack_push(fs_node_stack *, fs_node *);
fs_node *fs_node_stack_pop(fs_node_stack *);

fs_node *create_node(char *name, node_type type) {
  fs_node *node = (fs_node *) malloc(sizeof(fs_node));

  node->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));

  strcpy(node->name, name);
  node->type = type;
  node->child = NULL;
  node->next = NULL;

  return node;
}

void destroy_node(fs_node **node) {
  free((*node)->name);

  fs_node *aux = (*node)->child;

  while (aux != NULL) {
    fs_node *next = aux->next;
    destroy_node(&aux);
    aux = next;
  }

  free(*node);
  *node = NULL;
}

bool insert_node(fs_node *insert_root, char *name, node_type type) {
  if (insert_root->type != DIR || name == NULL) return false;

  fs_node *aux = insert_root->child, *prev = NULL;

  while (aux != NULL) {
    if (streq(aux->name, name)) return false;
    if (strcmp(aux->name, name) > 0) break;

    prev = aux;
    aux = aux->next;
  }

  fs_node *new_node = create_node(name, type);

  if (new_node == NULL) return false;

  if (prev == NULL) {
    new_node->next = insert_root->child;
    insert_root->child = new_node;
  } else {
    new_node->next = prev->next;
    prev->next = new_node;
  }

  return true;
}

bool ma(fs_node *insert_root, char *name) {
  return insert_node(insert_root, name, FL);
}

bool mp(fs_node *insert_root, char *name) {
  return insert_node(insert_root, name, DIR);
}

bool ls(fs_node *ls_root) {
  if (ls_root->type != DIR) return false;

  fs_node *aux = ls_root->child;

  while (aux != NULL) {
    printf("%s%s", aux->name, aux->type == DIR ? "-\n" : "\n");
    aux = aux->next;
  }

  return true;
}

fs_node *cd(fs_node_stack *fs_node_s, fs_node *root_dir, fs_node *cd_root, char *name) {
  if (cd_root->type != DIR || name == NULL) return NULL;

  if (streq(name, "..")) {
    if (fs_node_s->top && fs_node_s->top->next == NULL) {
      fs_node_stack_pop(fs_node_s);

      return root_dir;
    }

    return fs_node_stack_pop(fs_node_s);
  }

  fs_node *aux = cd_root->child;

  while (aux != NULL) {
    if (aux->type == DIR && streq(aux->name, name)) break;
    aux = aux->next;
  }

  return fs_node_stack_push(fs_node_s, aux);
}

bool rm(fs_node *rm_root, char *name) {
  if (rm_root->type != DIR || name == NULL) return false;

  fs_node *aux = rm_root->child, *prev = NULL;

  while (aux != NULL) {
    if (streq(aux->name, name)) break;
    prev = aux;
    aux = aux->next;
  }

  if (aux == NULL) return false;

  if (prev == NULL) rm_root->child = aux->next;
  else prev->next = aux->next;

  destroy_node(&aux);

  return true;
}

void scan(char *input_buff) {
  fgets(input_buff, LINE_SIZE, stdin);

  input_buff[(int8_t) (strchr(input_buff, '\n') - input_buff)] = '\0';
}

char *next_token(char *buff) { return strtok(buff, " "); }

bool streq(char *str1, char *str2) { return strcmp(str1, str2) == 0; }

cmd parse_cmd(char *buff) {
  char *cur_tok = next_token(buff);

  if (cur_tok == NULL) return NOP;

  if (streq(cur_tok, "ma")) return MA;
  if (streq(cur_tok, "mp")) return MP;
  if (streq(cur_tok, "ls")) return LS;
  if (streq(cur_tok, "cd")) return CD;
  if (streq(cur_tok, "rm")) return RM;
  if (streq(cur_tok, "ex")) return EX;

  else return WRNG_CMD;
}

fs_node_stack *create_fs_node_stack(void) {
  fs_node_stack *fs_node_s = (fs_node_stack *) malloc(sizeof(fs_node_stack));

  fs_node_s->top = NULL;
  fs_node_s->base = NULL;

  return fs_node_s;
}

fs_node_stack_el *create_fs_node_stack_el(fs_node *node) {
  fs_node_stack_el *el = (fs_node_stack_el *) malloc(sizeof(fs_node_stack_el));

  if (el == NULL) return NULL;

  el->node = node;
  el->prev = NULL;
  el->next = NULL;

  return el;
}

void destroy_fs_node_stack(fs_node_stack *fs_node_s) {
  fs_node_stack_el *aux = fs_node_s->top;

  while (aux != NULL) {
    fs_node_stack_el *next = aux->next;
    free(aux);
    aux = next;
  }
}

fs_node *fs_node_stack_push(fs_node_stack *fs_node_s, fs_node *node) {
  if (fs_node_s == NULL || node == NULL) return NULL;

  fs_node_stack_el *new_el = create_fs_node_stack_el(node);

  if (fs_node_s->top == NULL) {
    fs_node_s->top = fs_node_s->base = new_el;
  }

  else {
    new_el->next = fs_node_s->top;
    fs_node_s->top->prev = new_el;
    fs_node_s->top = new_el;
  }

  return node;
}

fs_node *fs_node_stack_pop(fs_node_stack *fs_node_s) {
  if (fs_node_s == NULL) return NULL;
  if (fs_node_s->top == NULL) return NULL;


  fs_node_stack_el *aux = fs_node_s->top;

  fs_node_s->top = aux->next;

  free(aux);

  if (fs_node_s->top != NULL) {
    fs_node_s->top->prev = NULL;
  }

  else {
    fs_node_s->base = NULL;
    return NULL;
  }

  return fs_node_s->top->node;
}

int main(void) {
  fs_node *root = create_node("root", DIR), *cur = root;
  fs_node_stack *fs_node_s = create_fs_node_stack();
  cmd cm;
  char input[LINE_SIZE];
  bool error = false;

  do {
    if (error) {
      printf("opcao invalida\n");
      error = false;
    }

    fs_node_stack_el *aux = fs_node_s->base;

    printf("-");

    while (aux != NULL) {
      printf("%s-", aux->node->name);
      aux = aux->prev;
    }

    printf(">");

    scan(input);
    cm = parse_cmd(input);

    switch (cm) {
      case MA: {
        error = !ma(cur, next_token(NULL));
        break;
      }

      case MP: {
        error = !mp(cur, next_token(NULL));
        break;
      }

      case LS: {
        error = !ls(cur);
        break;
      }

      case CD: {
        fs_node *aux = cd(fs_node_s, root, cur, next_token(NULL));

        error = (aux == NULL);

        if (!error) cur = aux;

        break;
      }

      case RM: {
        error = !rm(cur, next_token(NULL));
        break;
      }

      case EX: {
        printf("sistema encerrado\n");
        break;
      }

      case NOP: {
        continue;
      }

      default: {
        error = true;
      }
    }
  } while (cm != EX);

  destroy_node(&root);
  destroy_fs_node_stack(fs_node_s);

  return 0;
}
