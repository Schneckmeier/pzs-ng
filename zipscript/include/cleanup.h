#ifndef CLEANUP_H
#define CLEANUP_H

void scandirectory(char *, int);
char *replace_cookies(char *);
void incomplete_cleanup(char *, int, char *);
void cleanup(char *, char *, int, char *);
int checklink(char *, char *, int);

#endif

