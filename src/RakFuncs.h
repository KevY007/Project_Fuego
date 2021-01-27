#pragma once

struct HuffmanEncodingTreeNode
{
	unsigned char value;
	unsigned weight;
	HuffmanEncodingTreeNode* left;
	HuffmanEncodingTreeNode* right;
	HuffmanEncodingTreeNode* parent;
};

void DecodeArray(unsigned char* input, unsigned sizeInBits, BitStream* output);
unsigned DecodeArray(BitStream* input, unsigned sizeInBits, unsigned maxCharsToWrite, unsigned char* output);
bool DecodeString(char* output, int maxCharsToWrite, BitStream* input);

struct stSAMPDialog
{
	int iIsActive;
	BYTE bDialogStyle;
	WORD wDialogID;
	BYTE bTitleLength;
	char szTitle[257];
	BYTE bButton1Len;
	char szButton1[257];
	BYTE bButton2Len;
	char szButton2[257];
	char szInfo[257];
};
extern struct stSAMPDialog sampDialog;