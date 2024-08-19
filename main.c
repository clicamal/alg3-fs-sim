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
  struct fs_node *parent, *child, *next;
} fs_node;

fs_node *create_node(char *, node_type);
void destroy_node(fs_node **);
bool insert_node(fs_node *, char *, node_type);
bool ma(fs_node *, char *);
bool mp(fs_node *, char *);
bool ls(fs_node *);
fs_node *cd(fs_node *, char *);
bool rm(fs_node *, char *);
void scan(char *);
char *next_token(char *);
cmd parse_cmd(char *);

fs_node *create_node(char *name, node_type type) {
  fs_node *node = (fs_node *) malloc(sizeof(fs_node));

  node->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));

  strcpy(node->name, name);
  node->type = type;
  node->parent = NULL;
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

  fs_node *new_node = create_node(name, type);

  if (new_node == NULL) return false;

  new_node->parent = insert_root;

  if (insert_root->child == NULL) {
    insert_root->child = new_node;
  }

  else {
    if (strcmp(insert_root->child->name, name) > 0) {
      new_node->next = insert_root->child;
      insert_root->child = new_node;
    }

    else {
      fs_node *aux = insert_root->child;

      while (aux->next != NULL && strcmp(aux->next->name, name) < 0)
        aux = aux->next;

      new_node->next = aux->next;
      aux->next = new_node;
    }
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

fs_node *cd(fs_node *cd_root, char *name) {
  if (cd_root->type != DIR || name == NULL) return NULL;

  fs_node *aux = cd_root->child;

  while (aux != NULL) {
    if (aux->type == DIR && strcmp(aux->name, name) == 0) break;
    aux = aux->next;
  }

  return aux;
}

bool rm(fs_node *rm_root, char *name) {
  if (rm_root->type != DIR || name == NULL) return false;

  fs_node *aux = rm_root->child, *prev = NULL;

  while (aux != NULL) {
    if (strcmp(aux->name, name) == 0) break;
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

cmd parse_cmd(char *buff) {
  char *cur_tok = next_token(buff);

  if (cur_tok == NULL) return NOP;

  if (strcmp(cur_tok, "ma") == 0) return MA;
  if (strcmp(cur_tok, "mp") == 0) return MP;
  if (strcmp(cur_tok, "ls") == 0) return LS;
  if (strcmp(cur_tok, "cd") == 0) return CD;
  if (strcmp(cur_tok, "rm") == 0) return RM;
  if (strcmp(cur_tok, "ex") == 0) return EX;

  else return WRNG_CMD;
}

int main(void) {
  fs_node *root = create_node("root", DIR), *cur = root;
  cmd cm;
  char input[LINE_SIZE];
  bool error = false;

  do {
    if (error) {
      printf("opcao invalida\n");
      error = false;
    }

    printf("->");

    scan(input);
    cm = parse_cmd(input);

    switch (cm) {
    case MA:
      error = !ma(cur, next_token(NULL));
      break;
    case MP:
      error = !mp(cur, next_token(NULL));
      break;
    case LS:
      error = !ls(cur);
      break;
    case CD:
      cur = cd(cur, next_token(NULL));
      error = (cur == NULL);
      break;
    case RM:
      error = !rm(cur, next_token(NULL));
      break;
    case EX:
      printf("saindo\n");
      break;
    case NOP:
      continue;
    default:
      error = true;
    }
  } while (cm != EX);

  return 0;
}
