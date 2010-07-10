#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/inotify.h>

#define BUFFERLEN (1024*(sizeof(struct inotify_event)+16))

int
main(int argc, char **argv)
{
  char *filename = "inotify.tracked.conf";
  char buffer[BUFFERLEN];
  int fd, wd, size, i;
  struct inotify_event *event;
  
  fd = inotify_init();
  if(fd < 0)
    return 1;

  wd = inotify_add_watch(fd, filename, IN_MODIFY);
  if(wd < 0)
    return 1;
    
  for(;;){
   i = 0;
   size = read (fd, buffer, BUFFERLEN);
  
    while (i < size) {
      event = (struct inotify_event *)&buffer[i];
      
      if (event->mask & IN_MODIFY) 
        printf("\"%s\" has been modified\n", filename);
      
      i += sizeof(struct inotify_event) + event->len;
    }
  }

  return 0;
}
