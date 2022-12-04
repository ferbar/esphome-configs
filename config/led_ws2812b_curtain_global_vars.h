class LedPosInfo;
void initLedPosInfoMatrix(AddressableLight &it, int width, int height);

void initLedPosInfo(AddressableLight &it) {
	initLedPosInfoMatrix(it, 20, 20);
}
