// class LedPosInfo;
void initLedPosInfoLoops(AddressableLight &it, int *loopLedStartNumbers, int *loopDiameter);
static int loopLedStartNumbers[]={14,27,50,78,113,145,175 -1};
static int loopDiameter[]={50,100,150,200,230,255,255};

void initLedPosInfo(AddressableLight &it) {
	initLedPosInfoLoops(it, loopLedStartNumbers, loopDiameter);
}
