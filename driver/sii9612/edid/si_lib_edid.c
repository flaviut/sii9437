/******************************************************************************
 *
 * 2017 (c) Lattice Semiconductor Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it only under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation, and not any later version.
 * 
 * This program is distributed "AS IS" WITHOUT ANY WARRANTY of any kind,
 * whether express or implied, including without limitation the implied warranty
 * of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE or NON-INFRINGEMENT.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * You may contact Lattice Semiconductor Corporation at
 * 111 SW 5th Ave, Suite 700, Portland, OR 97204.
 *
 *****************************************************************************/
/**
 * @file si_lib_edid.c
 *
 * @brief edid library.
 *
 *****************************************************************************/
 #define SII_DEBUG	3

/***** #include statements ***************************************************/
#include "sii_datatypes.h"
#include "sii_system_api.h"
#include "si_lib_edid_api.h"
#include "sii_sys_log_api.h"

/***** Register Module name **************************************************/
SII_MODULE_NAME_SET(LibEDID);

/******************************* Locals Functions ****************************/
#define EDD_TAG(a) ((a >> 5) & 0x7)
#define EDD_LENGTH(a) (a & 0x1F)

/******************************* local edid data *****************************/
#if 0
uint8_t gEdidData[SII_EDID_LENGTH] = {
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4D, 0xD9, 0x02, 0x2C, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x15, 0x01, 0x03, 0x80, 0x59, 0x32, 0x78, 0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27,
	0x12, 0x48, 0x4C, 0x21, 0x08, 0x00, 0x81, 0x80, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0x76, 0xF2, 0x31, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20,
	0x6E, 0x28, 0x55, 0x00, 0x76, 0xF2, 0x31, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x53,
	0x4F, 0x4E, 0x59, 0x20, 0x54, 0x56, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x30, 0x3E, 0x0E, 0x46, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x09,
	0x02, 0x03, 0x3B, 0xF0, 0x53, 0x1F, 0x10, 0x14, 0x05, 0x13, 0x04, 0x20, 0x22, 0x3C, 0x3E, 0x12,
	0x16, 0x03, 0x07, 0x11, 0x15, 0x02, 0x06, 0x01, 0x26, 0x09, 0x07, 0x07, 0x15, 0x07, 0x50, 0x83,
	0x01, 0x00, 0x00, 0x74, 0x03, 0x0C, 0x00, 0x10, 0x00, 0xB8, 0x2D, 0x2F, 0xD0, 0x0A, 0x01, 0x40,
	0x00, 0x7F, 0x20, 0x30, 0x70, 0x80, 0x90, 0x76, 0xE2, 0x00, 0xFB, 0x02, 0x3A, 0x80, 0xD0, 0x72,
	0x38, 0x2D, 0x40, 0x10, 0x2C, 0x45, 0x80, 0x76, 0xF2, 0x31, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x00,
	0xBC, 0x52, 0xD0, 0x1E, 0x20, 0xB8, 0x28, 0x55, 0x40, 0x76, 0xF2, 0x31, 0x00, 0x00, 0x1E, 0x01,
	0x1D, 0x80, 0xD0, 0x72, 0x1C, 0x16, 0x20, 0x10, 0x2C, 0x25, 0x80, 0x76, 0xF2, 0x31, 0x00, 0x00,
	0x9E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65
};
#else
uint8_t gEdidData[SII_EDID_LENGTH] = {
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x32, 0x63, 0x38, 0x94, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x1B, 0x01, 0x03, 0x80, 0x59, 0x32, 0x78, 0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27,
	0x12, 0x48, 0x4C, 0x21, 0x08, 0x00, 0x81, 0x80, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0x76, 0xF2, 0x31, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20,
	0x6E, 0x28, 0x55, 0x00, 0x76, 0xF2, 0x31, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x53,
	0x4F, 0x4E, 0x59, 0x20, 0x54, 0x56, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x30, 0x3E, 0x0E, 0x46, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xF6,
	0x02, 0x03, 0x47, 0xF3, 0x53, 0x1F, 0x10, 0x14, 0x05, 0x13, 0x04, 0x20, 0x22, 0x3C, 0x3E, 0x12,
	0x16, 0x03, 0x07, 0x11, 0x15, 0x02, 0x06, 0x01, 0x35, 0x09, 0x7F, 0x07, 0x0F, 0x7F, 0x07, 0x17,
	0x07, 0x50, 0x3F, 0x06, 0xC0, 0x57, 0x06, 0x00, 0x5F, 0x7E, 0x01, 0x67, 0x5E, 0x00, 0x83, 0x4F,
	0x00, 0x00, 0x74, 0x03, 0x0C, 0x00, 0x10, 0x00, 0x00, 0x2D, 0x2F, 0xD0, 0x0A, 0x01, 0x40, 0x00,
	0x7F, 0x20, 0x30, 0x70, 0x80, 0x90, 0x76, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20, 0x58,
	0x2C, 0x25, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52
};
#endif


/******************************* local variables  *****************************/
uint8_t edid[SII_EDID_AUD_SIZE];

/******************************* debug Functions *****************************/
static void printEdidData(void)
{
	uint16_t i;

	/*printf("\n\n  ");
	for (i = 0; i < 16; i++)
		printf(" %02X", i);*/
	for (i = 0; i < SII_EDID_LENGTH; i+=8) {
		SII_LOG1A("", NULL,
			("%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X\n",
					gEdidData[i], gEdidData[i+1],
					gEdidData[i+2], gEdidData[i+3],
					gEdidData[i+4], gEdidData[i+5],
					gEdidData[i+6], gEdidData[i+7]));

	}
}
/*
static void edidTagPrint(uint8_t *pEdid)
{
	uint8_t length = EDD_LENGTH(pEdid[0]);
	uint8_t i;
	for (i = 0; i < length+1; i++)
		printf("%02X.", pEdid[i]);
	printf("\n");
}
*/
/******************************* local Functions *****************************/
uint8_t *sEdidTagParser(uint8_t *pEdid, SII_EDID_TAG tagId,
		SII_EDID_EX_TAG extendedTag)
{
	uint16_t length = SII_EDID_BLOCK_SIZE;
	uint16_t tagLength;
	uint16_t cnt = 0;
	uint8_t tagCode;

	if (pEdid[126]) { /* check if a second edid block */
		length += 3;
		tagLength = 0;
		length += tagLength + 1;
		while (cnt < SII_EDID_BLOCK_SIZE) {
			/* Get the tag and length */
			tagCode = EDD_TAG(pEdid[length]);
			tagLength = EDD_LENGTH(pEdid[length]);
			if (tagCode == tagId){
				if(tagCode != EDB_USE_EXTENDED_TAG)
					return &pEdid[length];
				/*Return only when extendedTag is matched else
				 * continue to next extended block*/
				if(extendedTag == pEdid[length+1]){
					return &pEdid[length];
				}
			}
			length += tagLength + 1;
			cnt++;
		}
	}
	return NULL;
}

uint8_t sEdidAudioDataGet(uint8_t *pEdid, SiiEdidAudioData_t *pAudEdid,
		uint8_t index)
{
	uint8_t *pIndex;
	uint8_t length = 0;

	pIndex = sEdidTagParser(pEdid, EDB_AUDIO_TAG,0);
	if (pIndex) {
		length = EDD_LENGTH(pIndex[0]) + 1;
		memcpy(&pAudEdid->b[index], pIndex, length);
		/* edidTagPrint( pIndex ); */
	}
	return length;
}

uint8_t sEdidSpeakerDataGet(uint8_t *pEdid, SiiEdidAudioData_t *pAudEdid,
		uint8_t index)
{
	uint8_t *pIndex;
	uint8_t length = 0;

	pIndex = sEdidTagParser(pEdid, EDB_SPEAKER_TAG,0);
	if (pIndex) {
		length = EDD_LENGTH(pIndex[0]) + 1; /* +1 for tag header */
		memcpy(&pAudEdid->b[index], pIndex, length);
		/* edidTagPrint( pIndex ); */
	}
	return length;
}

uint8_t sEdidParseExtendedAudioBlocks(uint8_t *pEdid,
		SiiEdidAudioData_t *pAudEdid,uint8_t index)
{
	uint8_t length = 0;
	uint8_t count = 0;
	uint8_t tagCode,tagLength;
	uint8_t extendedTagCode;
	/*The 2nd byte in EDID second block contains the offset of
	 * beginning of DTD's(i.e end of CEA Data Blocks)*/
	uint8_t dataBlockEnd = pEdid[SII_EDID_BLOCK_SIZE+2];
	/*Offset for beginning of CEA Data Blocks*/
	uint8_t offset = SII_EDID_BLOCK_SIZE+4;
	 /*Length of CEA Data Blocks*/
	uint8_t dataBlockLength = dataBlockEnd-4;

	while(length < dataBlockLength){
		tagCode = EDD_TAG(pEdid[offset]);
		tagLength = EDD_LENGTH(pEdid[offset]);

		if (tagCode == EDB_USE_EXTENDED_TAG){
			extendedTagCode = pEdid[offset+1];

			if((extendedTagCode == EDB_VS_AUDIO_ETAG)
					||(extendedTagCode == EDB_ROOM_CONF_ETAG)
					||(extendedTagCode == EDB_SPK_LOCATION_ETAG))
			{
				/*Copy the desired blocks into Audio EDID*/
				memcpy(&pAudEdid->b[index], pEdid+offset, tagLength+1);
				index += tagLength+1;
				count += tagLength+1;
			}
		}
		length += tagLength + 1;
		offset += tagLength + 1;
	}

	return count;
}

static void sCheckSum(uint8_t *pEdid)
{
	uint16_t i;
	uint8_t cs = 0;

	for (i = SII_EDID_BLOCK_SIZE; i < SII_EDID_LENGTH-1; i++)
		cs += pEdid[i];
	cs = 0x100 - cs;
	pEdid[SII_EDID_LENGTH-1] = cs & 0xFF;
}

uint8_t *sEidAudParse(SiiEdidAudioData_t *pAudEdid, SII_EDID_TAG tagId,
		  SII_EDID_EX_TAG extendedTagId)
{
	uint16_t index = 0;
	uint8_t tag;
	uint16_t length;
	uint8_t extendedTagCode;

	while (index < SII_EDID_AUD_SIZE) {
		tag = EDD_TAG(pAudEdid->b[index]);
		length = EDD_LENGTH(pAudEdid->b[index]);

		if (tag == tagId){
			if(tag != EDB_USE_EXTENDED_TAG )
				return &pAudEdid->b[index];

			extendedTagCode = pAudEdid->b[index+1];
			/*Return only when extended Tag is matched else
			 * continue to next extended block*/
			if(extendedTagId == extendedTagCode){
				return &pAudEdid->b[index];
			}
		}
		index += length + 1;
	}
	return NULL;
}

bool_t sEdidDataSet(uint8_t *pEdid, SiiEdidAudioData_t *pAudEdid,
		SII_EDID_TAG tag,SII_EDID_EX_TAG extendedTag)
{
	uint8_t *pEdidIndex;
	uint8_t *pAudIndex;
	uint16_t audLength;
	uint16_t editLength;

	pEdidIndex = sEdidTagParser(pEdid, tag, extendedTag);
	if (pEdidIndex) {

		/* total length left over start from audio
		 * data index to checksum */
		uint16_t length = &pEdid[SII_EDID_LENGTH] - pEdidIndex;
		memset(edid, 0, SII_EDID_AUD_SIZE);

		/* audio length of the EDID buffer */
		editLength =  EDD_LENGTH(pEdidIndex[0]) + 1;
		/* audio length of the audio data */
		pAudIndex = sEidAudParse(pAudEdid, tag, extendedTag);

		if (pAudIndex == NULL) {
			SII_LOG3A("", NULL, (" %s is not present in Caps DS\n",
					(tag == EDB_AUDIO_TAG) ?
					"Audio Block" : "Speaker Block"));
			return TRUE;
		}

		audLength = EDD_LENGTH(pAudIndex[0]) + 1;
		if (audLength <= editLength) {
			uint16_t remain;
			/* copy the audio data to temporary EDID buffer */
			if (audLength > (length - 1) ) {
				SII_LOG1A("", NULL,
					(" Error: No Space in EDID\n"));
				return TRUE;
			}

			memcpy(&edid[0], pAudIndex, audLength);
			remain = length - editLength - 1; /* don't copy
							check sum */
			memcpy(&edid[audLength], pEdidIndex+editLength, remain);
			memcpy(pEdidIndex, &edid[0], length);
			/* Update DTD offset if DTDs are present*/
			if (pEdid[SII_EDID_BLOCK_SIZE + 2])
				pEdid[SII_EDID_BLOCK_SIZE + 2] -=
						(editLength - audLength);
		} else {
			uint16_t remain;
			if (audLength > (length - 1) ) {
				SII_LOG1A("", NULL,
					(" Error: No Space in EDID\n"));
				return TRUE;
			}

			memcpy(&edid[0], pAudIndex, audLength);
			remain = length - audLength;
			memcpy(&edid[audLength], pEdidIndex+editLength, remain);
			memcpy(pEdidIndex, &edid[0], length);
			/* Update DTD offset if DTDs are present*/
			if (pEdid[SII_EDID_BLOCK_SIZE + 2])
				pEdid[SII_EDID_BLOCK_SIZE + 2] +=
						(audLength - editLength);
		}
		sCheckSum(pEdid);
	} else {
		SII_LOG3A("", NULL, (" Edid block %d not present\n", tag));
		pAudIndex = sEidAudParse(pAudEdid, tag, extendedTag);
		if(!pAudIndex){
			SII_LOG3A("",NULL,("Block %d not present in Audio EDID\n",
					extendedTag));
			return TRUE;
		}
		else{
			uint8_t dtdCount = (pEdid[SII_EDID_BLOCK_SIZE+3] & 0xF)-2;
			uint8_t dtdLength = dtdCount * 18;
			uint8_t dtdOffset = pEdid[SII_EDID_BLOCK_SIZE+2];
			uint8_t edidEmptyBytes = SII_EDID_BLOCK_SIZE-dtdOffset-dtdLength-1;
			uint8_t dtdStart = SII_EDID_BLOCK_SIZE + dtdOffset;
			audLength = EDD_LENGTH(pAudIndex[0]) + 1;

			if(audLength > edidEmptyBytes){
				SII_LOG1A("",NULL,("No space in EDID to fit the"" block %d\n",
						tag));
			}
			else{
				memset(edid, 0, SII_EDID_AUD_SIZE);
				memcpy(&edid[0],&pEdid[dtdStart],dtdLength);
				memcpy(&pEdid[dtdStart],&pAudIndex[0],audLength);
				pEdid[SII_EDID_BLOCK_SIZE + 2] += audLength;
				memcpy(&pEdid[dtdStart+audLength],&edid[0],dtdLength);
				sCheckSum(pEdid);
			}
		}
	 }
	return TRUE;
}

uint8_t sEdidSpeakerDataSet(uint8_t *pEdid, SiiEdidAudioData_t *pAudEdid)
{
	uint8_t *pIndex;
	uint8_t length = 0;

	pIndex = sEdidTagParser(pEdid, EDB_SPEAKER_TAG,0);
	if (pIndex) {
		length += EDD_LENGTH(pIndex[0]) + 1; /* +1 for tag header */
		/* edidTagPrint( pIndex ); */
	}
	return length;
}

/**************************** Public Functions *******************************/
uint8_t SiiLibEdidAudioDataGet(uint8_t *pEdid, SiiEdidAudioData_t *pAudEdid)
{
	uint8_t length = 0;
	length += sEdidAudioDataGet(pEdid, pAudEdid, length);
	length += sEdidSpeakerDataGet(pEdid, pAudEdid, length);
	length += sEdidParseExtendedAudioBlocks(pEdid,pAudEdid,length);
	/* printEdidData(); */
	return length;
}

bool_t SiiLibEdidAudioDataSet(uint8_t *pEdid, SiiEdidAudioData_t *pAudEdid)
{
	bool_t results = FALSE;
	/*set audio data */
	if (sEdidDataSet(pEdid, pAudEdid, EDB_AUDIO_TAG,0)) {
		/* set speaker data */
		if (sEdidDataSet(pEdid, pAudEdid, EDB_SPEAKER_TAG,0))
			results = TRUE;
	}
	sEdidDataSet(pEdid,pAudEdid,EDB_USE_EXTENDED_TAG,EDB_VS_AUDIO_ETAG);
	sEdidDataSet(pEdid,pAudEdid,EDB_USE_EXTENDED_TAG,EDB_ROOM_CONF_ETAG);
	sEdidDataSet(pEdid,pAudEdid,EDB_USE_EXTENDED_TAG,EDB_SPK_LOCATION_ETAG);

	if (results)
		printEdidData();
	else
		SII_LOG1A("", NULL, (" Wrong EDID data\n"));

	return results;
}
/***** end of file ***********************************************************/
