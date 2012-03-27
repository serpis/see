#ifndef _FSWATCH_HPP
#define _FSWATCH_HPP

void fswatch_init();
void fswatch_add_modified_callback(const char *filename, void (*callback)(void *user_data), void *user_data);
void fswatch_poll();

#endif // _FSWATCH_HPP

