#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define MAX_PACKET_SIZE 512
#define STDIN 0
#define MAX_NAME_LEN 16
#define MAX_LOG_ENTRY 256

#define PING_MSG "#zero#"

inline int min(int a, int b) {return (a<b)?a:b;}
inline int max(int a, int b) {return (a>b)?a:b;}
inline int ceildiv(int a, int b) {return (a%b==0)?a/b:a/b+1;}

#endif
