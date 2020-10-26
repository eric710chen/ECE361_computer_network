#ifndef SESSION_H
#define SESSION_H

#include "message.h"

#define MAX_SESSION 128
#define MAX_USER_SESSION 128

struct user;

struct session {
  size_t sid; // index in session array
  char session_id[MAX_SESSION_ID];
  size_t user_num; // current user number of session
  struct user *users[MAX_USER_SESSION];
  struct user *creator;
};

extern fd_set server_fds;
extern struct session *sessions[MAX_SESSION];

int new_session(const char* session_id, struct user* creator);
int session_is_full(struct session* s);
struct session* find_session(const char* session_id);
// Boardcast to all users in session
int session_send(struct session* s, const char* source, const char* msg);
int session_remove_user(struct session* s, struct user* user);
int session_add_user(struct session* s, struct user* user);
int session_destory(struct session* s);
// return the formatted session info
int get_session_info(struct session* s, char* dest);
int get_all_session_info(char* dest);
#endif
