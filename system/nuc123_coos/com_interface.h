#ifndef COM_INTERFACE_H
#define COM_INTERFACE_H
#include <stdio.h>
#include <stdint.h>

#define RXBUFSIZE 64

char **strsplit(const char* str, const char* delim, uint8_t* numtokens);
int StringToInt(char* data);
int StringToHex(char* s);
void process_comport_data(uint8_t *comRbuf, uint16_t* RX_Index);


#endif /* COM_INTERFACE_H */
