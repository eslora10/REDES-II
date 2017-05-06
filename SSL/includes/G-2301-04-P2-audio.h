#ifndef AUDIO_H
#define AUDIO_H

#include "G-2301-04-P2-messages.h"

int audioChat(char *sender, char *msg);

void *audioSend(void *args);

void *audioRecv(void *args);

void *audioSend(void *args);

struct audioArgs{
    char *ip;
    int port;
};



#endif /*AUDIO_H*/
