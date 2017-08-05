/*
	360�� == IDENOM * 4
*/

#include "all.h"

/*
	Sine90.txt
		0�� �` 90�� �� (IDENOM + 1) ���� -> sin(0 �` IDENOM) * IMAX
*/
static autoList<int> *GetSine90(void)
{
	static autoList<int> *sine90;

	if(!sine90)
	{
		autoList<uchar> *fileData = GetEtcRes()->GetHandle(ETC_SINE90TXT);
		int rIndex = 0;

		sine90 = new autoList<int>();

		for(int index = 0; index <= IDENOM; index++)
		{
			sine90->AddElement(atoi_x(readLine(fileData, rIndex)));
		}
		delete fileData->Ecdysis(); // ��ɂ���B
	}
	return sine90;
}
int iSin(fPInt_t angle, int retScale)
{
	errorCase(angle < -IMAX || IMAX < angle);
	errorCase(retScale < 0 || IMAX < retScale);

	angle %= IDENOM * 4;

	if(angle < 0)
		angle += IDENOM * 4;

	errorCase(!m_isRange(angle, 0, IDENOM * 4 - 1)); // 2bs
	int retSign = IDENOM * 2 < angle ? -1 : 1;

	angle %= IDENOM * 2;
	angle = IDENOM - angle;
	angle = abs(angle);
	angle = IDENOM - angle;

	__int64 retval = GetSine90()->GetElement(angle) * retSign;
	retval *= retScale;
	retval /= IMAX;
	return (int)retval;
}
int iCos(fPInt_t angle, int retScale)
{
	return iSin(angle + IDENOM, retScale);
}
