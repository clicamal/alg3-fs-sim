#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum node_type {
  FL,
  DIR
} node_type;

typedef struct fs_node {
  char *name;
  node_type type;
  struct fs_node *parent, *child, *next;
} fs_node;

fs_node *create_node(char *, node_type);
void destroy_node(fs_node *);
bool insert_node(fs_node *, char *, node_type);
bool ma(fs_node *, char *);
bool mp(fs_node *, char *);
bool ls(fs_node *);

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

void destroy_node(fs_node *node) {
  free(node->name);

  fs_node *aux = node->child;

  while (aux != NULL) {
    fs_node *next = aux->next;

    destroy_node(aux);
    aux = next;
  }

  free(node);
}

bool insert_node(fs_node *insert_root, char *name, node_type type) {
  if (insert_root->type != DIR) return false;

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

      while (aux->next != NULL && strcmp(aux->next->name, name) <= 0)
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

int main(void) {
  fs_node *root = create_node("root", DIR);

  ma(root, "0");
  ma(root, "b");
  mp(root, "A");

  ls(root);

  destroy_node(root);

  return 0;
}
