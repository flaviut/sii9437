#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "com_interface.h"
#include "sii_platform_api.h"
#include <UART.h>
#include <sii_sys_log_api.h>

extern volatile uint8_t comRbuf[RXBUFSIZE];
extern volatile uint16_t RX_Index;

extern void* the_pdev;

char **strsplit(const char* str, const char* delim, uint8_t* numtokens)
{
    // copy the original string so that we don't overwrite parts of it
    // (don't do this if you don't need to keep the old line,
    // as this is less efficient)

    // these three variables are part of a very common idiom to
    // implement a dynamically-growing array
	uint8_t tokens_alloc = 15;
	uint8_t tokens_used = 0;
   // char **tokens = calloc(tokens_alloc, sizeof(char*));
    char **tokens = (char **)kzalloc(tokens_alloc*sizeof(char*), GFP_KERNEL);

    char *token, *strtok_ctx;
    for (token = strtok_r((char *)str, delim, &strtok_ctx);
            token != NULL;
            token = strtok_r(NULL, delim, &strtok_ctx)) {
        // check if we need to allocate more space for tokens
        if (tokens_used == tokens_alloc) {
            tokens_alloc *= 2;
            //tokens = realloc(tokens, tokens_alloc * sizeof(char*));
            tokens = (char **)kzalloc(tokens_alloc*sizeof(char*), GFP_KERNEL);
        }
        tokens[tokens_used++] = token;
    }
#if 0
    // cleanup
    if (tokens_used == 0) {
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
#endif
    *numtokens = tokens_used;
    //free(s);

    return tokens;
}

int StringToInt(char* data)
{
	int i = 0, num = 0;

	while(data[i] != '\0')
	{
		num = num * 10 + ( data[i] - '0' );
		i++;
	}
	return num;
}

int StringToHex(char* s)
{
	 unsigned int result = 0;
	 int c ;
	 if ('0' == *s && 'x' == *(s+1))
	 {
		 s+=2;
		 while (*s)
		 {
			 result = result << 4;
			 if (c=(*s-'0'),(c>=0 && c <=9)) result|=c;
			 else if (c=(*s-'A'),(c>=0 && c <=5)) result|=(c+10);
			 else if (c=(*s-'a'),(c>=0 && c <=5)) result|=(c+10);
			 else break;
			 ++s;
		 }
	 }
	 return result;
}
void process_comport_data(uint8_t *comRbuf, uint16_t* RX_Index)
{
	char *sent = (char *)kzalloc(*RX_Index, GFP_KERNEL);
	uint8_t commandSize = 0;
	int enable = -1, status = 0;
	char **splitData;

	sent = (char *) comRbuf;

	if(strcmp(sent, "NUCAPI") == 0) {
		return;
	}

	splitData = strsplit((char *)sent," ", &commandSize);

	if(strcmp(splitData[commandSize - 1],"enable_comm_mode") == 0)	{
		SII_LOG1A("", NULL, (" Received command: enable_comm_mode\n"));
		enable =  StringToHex(splitData[2]);
		if (enable == 0) {
			SII_LOG1A("", NULL, (" Disable Comm Mode\n"));
			/* Call API */ /* TODO*/
		} else {
			SII_LOG1A("", NULL, (" Enable Comm Mode\n"));
			/* Call API */ /* TODO*/
		}
	} else {
		SII_LOG1A("", NULL, (" Received Invalid Command\n"));
		return;
	}
	if(!status) {
		SII_LOG1A("", NULL, (" Command Execution Unsuccessfull \n"));
		return;
	}
}
