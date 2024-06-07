#include <stdint.h>

#ifndef BIT
#define BIT(x) (uint32_t)(1 << x)
#endif // !BIT

//#define MASK_BITSLIST
#define DISABLE_BIT_LOGGER 0

typedef struct {
#if DISABLE_BIT_LOGGER==0
	uint32_t cntr32bit[32]; //4x32 = 128bytes
	uint32_t _tempCntr[32]; //128bytes  //for 32bit logging signals we waste the 260bytes. mb there are solvings on another way.
#endif
	uint32_t Q32bit;
	//Timerwp_t BugScannerTimer; //StuckScannerTimer  sounds better//?
	//uint32_t filterCntr4bit[4]; //? //4bit cntr for 32bit-channel = 8bytes
	//uint32_t _tempFilterCntr4bit[4]; //8bytes
#ifdef MASK_BITSLIST
	uint32_t mask;
	uint32_t maskedQ32bit;
#endif // MASK_BITSLIST
}BitLoggerList_t;

void InitBitLoggerList(BitLoggerList_t* BitLogger);
uint32_t BitLoggerList(BitLoggerList_t* BitLogger);
void SetBitToLoggerList(uint32_t inputBITx, BitLoggerList_t* BitLogger);
void ResetSpecBitOnLoggerList(uint32_t inputBITx, BitLoggerList_t* BitLogger);
#ifdef MASK_BITSLIST
void SetMaskToBitLogger(BitLoggerList_t* BitLogger, uint32_t maskVal);
#endif // MASK_BITSLIST
