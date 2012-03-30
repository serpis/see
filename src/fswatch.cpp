#include <cassert>
#include <cstring>

// inserts '\0' at last '/', returns the string starting at this index + 1
// result is a split of path into directory and filename
static char *split_path(char *path)
{
    char *p = strrchr(path, '/');
    assert(p);
    *p = '\0';
    return p + 1;
}




#ifdef _OSX

#define MAX_WATCHES 100
static struct watch_t
{
    int wd;
    void *user_data;
    void (*callback)(void *user_data);
    char directory_path[256];
    char *filename;
} watches[MAX_WATCHES];
static int watch_count = 0;

void fswatch_init()
{
}

void fswatch_add_modified_callback(const char *filename, void (*callback)(void *user_data), void *user_data)
{
}

void fswatch_poll()
{
}


#else

#include <sys/inotify.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <cstdio>

static int fd;

#define MAX_WATCHES 100
static struct watch_t
{
    int wd;
    void *user_data;
    void (*callback)(void *user_data);
    char directory_path[256];
    char *filename;
} watches[MAX_WATCHES];
static int watch_count = 0;

void fswatch_init()
{
    fd = inotify_init();
    assert(fd != -1);

    assert(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == 0);
}

void fswatch_add_modified_callback(const char *filename, void (*callback)(void *user_data), void *user_data)
{
    assert(watch_count < MAX_WATCHES);
    int len = strlen(filename);
    assert(len < sizeof(watches[0].directory_path) - 1);

    watch_t &watch = watches[watch_count++];

    strcpy(watch.directory_path, filename);
    watch.filename = split_path(watch.directory_path);

    int wd = inotify_add_watch(fd, watch.directory_path, IN_CLOSE_WRITE | IN_MOVED_TO);
    assert(wd != -1);

    watch.wd = wd;
    watch.callback = callback;
    watch.user_data = user_data;
}

void fswatch_poll()
{
    char buffer[sizeof(inotify_event)*10];
    int read_bytes;
    while ((read_bytes = read(fd, buffer, sizeof(buffer))) != -1)
    {
        char *p = buffer;
        while (p - buffer < read_bytes)
        {
            inotify_event *e = (inotify_event *)p;
            assert(e->len > 0);
            for (int i = 0; i < watch_count; i++)
            {
                watch_t &watch = watches[i];
                if (watch.wd == e->wd && strcmp(watch.filename, e->name) == 0)
                {
                    watch.callback(watch.user_data);
                }
            }

            p += sizeof(inotify_event) + e->len;
        }
    }
    assert(read_bytes == -1 && errno == EAGAIN);
}

#endif

