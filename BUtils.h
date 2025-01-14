#pragma once
#include <opencv.hpp>

typedef cv::Scalar_<float> ScalarF;

using namespace Gdiplus;

inline Color CLR2Color(COLORREF clr, BYTE alfa=255) //255 means no transparency
{
	return Color(alfa, GetRValue(clr), GetGValue(clr), GetBValue(clr));
}
cv::Mat CreateGradient(int H, int W, ScalarF c1, ScalarF c2);

int Mat2CImage(cv::Mat *mat, CImage &img);
void CImage2Mat(CImage& img, cv::Mat& mat);

cv::Mat ReadImage(const wchar_t* filename);

int WriteImage(cv::Mat &img, const wchar_t* filename);

inline bool DirExists(const std::wstring& dirName_in)
{
	DWORD ftyp = GetFileAttributes(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

bool CopyToClipboard(HWND owner, const std::wstring& w);

bool CopyImageToClipboard(CImage& img);
/*
Color Codes
:: Gradient Colors for BAYYENAT software
:: Set gradient colors in HEX
:: if 2 color codes appear in a line, background will be a gradient
:: if 1 color code appear in a line, background will be solid color
:: Each code must have 6 digits (2 for RED, 2 for GREEN and 2 for BLUE channeld)
000428, 004E92
253DB5
42275A, 734B6D
4B2C47
141E30, 243B55
0000D4
2C3E50, 4CA1AF
975700
3A7BD5, 3A6073
6E6A2F
4CA1AF, C4E0E5
2960D3
000000, 434343
34B7A2
4B79A1, 283E51
EEEEEE
2980B9, 2C3E50
3F33A3
4DA0B0, D39D38
286BD2
1E3C72, 2A5298
323498
457FCA, 5691C8
9A8A79
C02425, F0CB35
089DF3
FFB75E, ED8F03
997D40
76B852, 8DC26F
37AF79
8E0E00, 1F1C18
000000
2C3E50, 3498DB
98B8D5
BA8B02, 181818
AAAAAA
6A9113, 141517
333333
00BF8F, 001510
517FA4, 243949
FFB347, FFCC33
43CEA2, 185A9D
360033, 0B8793
485563, 29323C
52C234, 061700
000000, E74C3C
FF4E50, F9D423
000000, 53346D
870000, 190A05
666600, 999966
E9D362, 333333
4B6CB7, 182848
414D0B, 727A17
24C6DC, 514A9D
283048, 859398
3D7EAA, FFE47A
232526, 414345
134E5E, 71B280
085078, 85D8CE
614385, 516395
1F1C2C, 928DAB
16222A, 3A6073
FF8008, FFC837
EB3349, F45C43
FF512F, F09819
1A2980, 26D0CE
F09819, EDDE5D
3CA55C, B5AC49
16A085, F4D03F
E52D27, B31217
41295A, 2F0743
3A6186, 89253E
67B26F, 4CA2CD
56AB2F, A8E063
000000, 777777
00537E, 3AA17E
*/