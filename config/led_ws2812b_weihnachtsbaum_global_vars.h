// class LedPosInfo;
void initLedPosInfoLoops(AddressableLight &it, int *loopLedStartNumbers, int *loopDiameter);
// wenn der letzte kreis nicht volständig ist dann ~ letztes kreis ende angeben
// !! die erste Led ist natürlich beim doserl !!
//static int loopLedStartNumbers[]={14,27,50,78,113,145,175 -1};
static int loopLedStartNumbers[]={40,65,90,105,120,135,145,160, -1};
// von aussen nach innen:
static int loopDiameter[]={255,255,150,150,100,100,100};

void initLedPosInfo(AddressableLight &it) {
	initLedPosInfoLoops(it, loopLedStartNumbers, loopDiameter);
}
