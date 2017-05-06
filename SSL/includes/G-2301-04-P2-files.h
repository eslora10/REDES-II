#ifndef FILES_H
#define FILES_H

struct fileReceiver_args {
    int sckF;
    char *path;
    long unsigned int length;
};


struct fileSender_args {
    char *data;
    long unsigned int length;
    int sck;
};

void* fileReceiver(void *args);
int fileDialog(char *nick, char *msg);

void* fileSender(void *fs);
#endif /*FILES_H*/
