// startpos für neue loops (start bei 0)
static int loopLedNumbers[]={3,9,15,22,29,37,47,60, -1};
static int loopDiameter[]={50,80,110,140,170,200,230,255};
void initLedPosInfo(AddressableLight &it) {
	initLedPosInfoLoops(it, loopLedStartNumbers, loopDiameter);
}
