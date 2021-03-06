#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include "stringhandle.h"
#define wlen 45

typedef struct Nodes {
  char word[wlen];
  int times;
  struct Appear *head;
  struct Nodes *next;
} node;

typedef struct Appear {
  int line;
  struct Appear *next;
} appear;

node *ahead = NULL;

typedef struct Cap {
  char word[wlen];
  int line;
  struct Cap *next;
} cap;

cap *def_head = NULL;
cap *undef_head = NULL;

void insert(char *word, int line, cap **head) {
  cap *new = (cap *)malloc(sizeof(cap));
  strcpy(new->word, word);
  new->line = line;
  new->next = *head;
  *head = new;
}

int search(char *word, cap *head) {
  cap *cur = head;
  if (head == NULL) return 0;
  while(strcmp(cur->word, word) != 0) {
    if (cur->next == NULL) return 0;
    else cur = cur->next;
  }
  return 1;
}

int linesearch(int line, appear *head) {
  appear *cur = head;
  if (head == NULL) return 0;
  while (cur->line != line) {
    if (cur->next == NULL) return 0;
    else cur = cur->next;
  }
  return 1;
}

void insertline(int line, appear **head) {
  appear *new = (appear *)malloc(sizeof(appear));
  new->line = line;
  if (*head == NULL || (*head)->line > line) {
    new->next = *head;
    *head = new;
  } else {
    appear *cur = *head;
    while(cur->next != NULL && cur->next->line < line) {
      cur = cur->next;
    }
    new->next = cur->next;
    cur->next = new;
    }
}

node *wordsearch(char *word, node *head) {
  node *cur = head;
  if (head == NULL) return NULL;
  while (strcmp(cur->word, word) != 0) {
    if (cur->next == NULL) return NULL;
    else cur = cur->next;
  }
  return cur;
}

void insertword(char *word, int line, node **head) {
  node *result = wordsearch(word, *head);
  if(result != NULL) {
    result->times ++;
    if (linesearch(line, result->head) == 0) {
      insertline(line, &result->head);
    }
  }else {
    node *new = (node *)malloc(sizeof(node));
    strcpy(new->word, word);
    new->times = 1;
    new->head = NULL;
    insertline(line, &new->head);
    if (*head == NULL || strcmp((*head)->word, word) > 0) {
      new->next = *head;
      *head = new;
    } else {
      node *cur = *head;
      while(cur->next != NULL && strcmp(cur->next->word, word) < 0) {
	cur = cur->next;
      }
      new->next = cur->next;
      cur->next = new;
      }
  }
}

int main() {
  char **wordlist;
  FILE *sfptr;

  // Capitalized words list
  if((sfptr = fopen("stopw.txt", "r")) == NULL) {
      return 0;
  }
  int line_count = 0;
  char c;
  while((c = fgetc(sfptr)) != EOF) {
    if (c == '\n')
      line_count++;
  }

  wordlist = (char **)malloc(line_count * sizeof(char*));
  int i;
  rewind(sfptr);
  for (i = 0; i < line_count; i++) {
    wordlist[i] = (char *)malloc(wlen * sizeof(char));
    fscanf(sfptr, "%s", wordlist[i]);
  }

  fclose(sfptr);
  // Read text
  FILE *fptr;
  if ((fptr = fopen("vanban.txt", "r")) == NULL) {
    return 0;
  }

  int beginofsen = 1;
  int dot = 1;
  char word[wlen];
  int pos = 0;
  int line = 1;
  int len;
  int capword = 0;
  while ((c = fgetc(fptr)) != EOF) {
    if (c == '\n' || c == ' ' || c == '\r') {
      word[pos] = '\0';

      if (strlen(word) != 0) {
	if (word[pos-1] == ',') dot = 1;
	else dot = 0;

	if (isupper(word[0])) capword = 1;
	else capword = 0;

	len = standardword(word, strlen(word));

	if (valid(word, strlen(word)) && !avalable(word, wordlist, line_count) && capword == 0) {
	  insertword(word, line, &ahead);
	} else if (beginofsen == 0 && capword == 1) {
	  insert(word, line, &def_head);
	} else if (beginofsen = 1 && capword == 1) {
	  insert(word, line, &undef_head);
	}
      }
      if (c == '\n') line++;
      beginofsen = dot;
      pos = 0;
      word[pos] = '\0';
    } else {
      word[pos] = c;
      pos++;
    }
  }

  cap *temp = undef_head;
  while(temp != NULL) {
    if(search(temp->word, def_head) == 0) {
      insertword(temp->word, temp->line, &ahead);
    }
    temp = temp->next;
  }

  node *cur;
  appear *acur;
  cur = ahead;
  while(cur != NULL) {
    printf("%s %d,", cur->word, cur->times);
    acur = cur->head;
    while(acur != NULL) {
      if (acur->next != NULL) printf("%d,", acur->line);
      else printf("%d\n", acur->line);
      acur = acur->next;
    }
    cur = cur->next;
  }

  fclose(fptr);

  return 0;
}
		     
