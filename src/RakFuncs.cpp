#include "main.h"

HuffmanEncodingTreeNode* root;

struct stSAMPDialog sampDialog;

unsigned DecodeArray(BitStream* input, unsigned sizeInBits, unsigned maxCharsToWrite, unsigned char* output)
{
	HuffmanEncodingTreeNode* currentNode;

	unsigned outputWriteIndex;
	outputWriteIndex = 0;
	currentNode = root;

	// For each bit, go left if it is a 0 and right if it is a 1.  When we reach a leaf, that gives us the desired value and we restart from the root

	for (unsigned counter = 0; counter < sizeInBits; counter++)
	{
		if (input->ReadBit() == false)   // left!
			currentNode = currentNode->left;
		else
			currentNode = currentNode->right;

		if (currentNode->left == 0 && currentNode->right == 0)   // Leaf
		{

			if (outputWriteIndex < maxCharsToWrite)
				output[outputWriteIndex] = currentNode->value;

			outputWriteIndex++;

			currentNode = root;
		}
	}

	return outputWriteIndex;
}

// Pass an array of encoded bytes to array and a preallocated BitStream to receive the output
void DecodeArray(unsigned char* input, unsigned sizeInBits, BitStream* output)
{
	HuffmanEncodingTreeNode* currentNode;

	if (sizeInBits <= 0)
		return;

	BitStream bitStream(input, BITS_TO_BYTES(sizeInBits), false);

	currentNode = root;

	// For each bit, go left if it is a 0 and right if it is a 1.  When we reach a leaf, that gives us the desired value and we restart from the root
	for (unsigned counter = 0; counter < sizeInBits; counter++)
	{
		if (bitStream.ReadBit() == false)   // left!
			currentNode = currentNode->left;
		else
			currentNode = currentNode->right;

		if (currentNode->left == 0 && currentNode->right == 0)   // Leaf
		{
			output->WriteBits(&(currentNode->value), sizeof(char) * 8, true); // Use WriteBits instead of Write(char) because we want to avoid TYPE_CHECKING
			currentNode = root;
		}
	}
}

bool DecodeString(char* output, int maxCharsToWrite, BitStream* input)
{
	unsigned short stringBitLength;
	int bytesInStream;

	output[0] = 0;

	if (input->ReadCompressed(stringBitLength) == false)
		return false;

	if (input->GetNumberOfUnreadBits() < stringBitLength)
		return false;

	bytesInStream = DecodeArray(input, stringBitLength, maxCharsToWrite, (unsigned char*)output);

	if (bytesInStream < maxCharsToWrite)
		output[bytesInStream] = 0;
	else
		output[maxCharsToWrite - 1] = 0;

	return true;
}