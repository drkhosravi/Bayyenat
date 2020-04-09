#include "StdAfx.h"
#include "BUtils.h"
#include "Settings.h"

using namespace cv;
cv::Mat CreateGradient(int H, int W, ScalarF c1, ScalarF c2)
{
	cv::Mat im = cv::Mat(H, W, CV_8UC3);
	cv::Scalar m = (c2 - c1) / (W - 1);
	for (int y = 0; y < H; y++)
	{
		uchar* p = im.ptr(y);
		for (int x = 0; x < W; x++)
		{
			p[x * 3]	 = (uchar)(m[0] * x + c1[0]);
			p[x * 3 + 1] = (uchar)(m[1] * x + c1[1]);
			p[x * 3 + 2] = (uchar)(m[2] * x + c1[2]);
		}
	}
	return im;
}

int Mat2CImage(Mat *mat, CImage &img)
{
	if (!mat || mat->empty())
		return -1;
	int nBPP = mat->channels() * 8;
	if(img.IsNull() || img.GetWidth() != mat->cols || img.GetHeight() != mat->rows || img.GetBPP() != nBPP)
		img.Create(mat->cols, mat->rows, nBPP);
	if (nBPP == 8)
	{
		static RGBQUAD pRGB[256];
		for (int i = 0; i < 256; i++)
			pRGB[i].rgbBlue = pRGB[i].rgbGreen = pRGB[i].rgbRed = i;
		img.SetColorTable(0, 256, pRGB);
	}
	uchar* psrc = mat->data;
	uchar* pdst = (uchar*)img.GetBits();
	int imgPitch = img.GetPitch();
	for (int y = 0; y < mat->rows; y++)
	{
		memcpy(pdst, psrc, mat->cols*mat->channels());//mat->step is incorrect for those images created by roi (sub-images!)
		psrc += mat->step;
		pdst += imgPitch;
	}
	//memcpy(pdst, psrc, mat->rows*mat->cols*mat->channels());
	return 0;
}


void CImage2Mat(CImage& img, Mat& mat)
{
	// CImage to Mat
	if (img.IsNull())
	{
		return;
		//MessageBox(_T("Not loaded successfully"));
	}
	if (1 == img.GetBPP() / 8)
	{
		mat.create(img.GetHeight(), img.GetWidth(), CV_8UC1);
	}
	else if (3 == img.GetBPP() / 8)
	{
		mat.create(img.GetHeight(), img.GetWidth(), CV_8UC3);
	}

	uchar* psrc = (uchar*)img.GetBits();
	uchar* pdst = mat.data;
	int imgPitch = img.GetPitch();
	for (int y = 0; y < mat.rows; y++)
	{
		memcpy(pdst, psrc, mat.cols * mat.channels());
		psrc += imgPitch; 
		pdst += mat.step;
	}
}

cv::Mat ReadImage(const wchar_t* filename)
{
	//OpenCV can not read from unicode path!

	FILE* fp = _wfopen(filename, L"rb");
	if (!fp)
	{
		std::wstring msg = L"File\n" + std::wstring(filename) + L" could not be openned";
		AfxMessageBox(msg.c_str());
		return cv::Mat::zeros(100, 100, CV_8U);
	}
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	char* buf = new char[sz];
	fseek(fp, 0, SEEK_SET);
	long n = fread(buf, 1, sz, fp);
	_InputArray arr(buf, sz);
	Mat img = imdecode(arr, cv::IMREAD_COLOR);
	delete[] buf;
	fclose(fp);
	return img;
}

int WriteImage(cv::Mat &img, const wchar_t* filename)
{
	//imwrite accepts ascii strings, so we produce an ascii path, then use MoveFile to move it to the right path
	std::string fnA((const char*)str_local_pathA);
	char tempA[10] = { 'A' + rand() % 26, 'A' + rand() % 26, 'A' + rand() % 26, 'A' + rand() % 26, '.','j','p','g' };
	fnA += tempA;
	wchar_t tempW[MAX_PATH] = { 0 };
	bool result = imwrite(fnA, img);
	if (!result)
	{
		if (!DirExists(str_local_pathW))
			CreateDirectory(str_local_pathW, 0);
		//Try again
		if (!imwrite(fnA, img))
		{
			std::wstring msg = L"File\n" + std::wstring(filename) + L" could not be written";
			AfxMessageBox(msg.c_str());
			return -1;
		}
	}
	mbstowcs(tempW, fnA.c_str(), MAX_PATH);
	BOOL res = MoveFile(tempW, filename);
	if (res == 0)
	{
		DWORD error_code = GetLastError();
		if (error_code == ERROR_ALREADY_EXISTS)
		{
			res = DeleteFile(filename);
			error_code = GetLastError();
			assert(res);
			res = MoveFile(tempW, filename);
			error_code = GetLastError();
			assert(res);
			if (!res)
				MessageBox(AfxGetMainWnd()->m_hWnd, L"File Can not be written in workspace directory.", (wchar_t*)filename, MB_ICONERROR);
		}
	}
	//Delete temporary file
	DeleteFile(tempW);
	return 0;
}

bool CopyToClipboard(HWND owner, const std::wstring& w)
{
	if (OpenClipboard(owner))
	{
		HGLOBAL hgClipBuffer = nullptr;
		std::size_t sizeInWords = w.size() + 1;
		std::size_t sizeInBytes = sizeInWords * sizeof(wchar_t);
		hgClipBuffer = GlobalAlloc(GHND | GMEM_SHARE, sizeInBytes);
		if (!hgClipBuffer)
		{
			CloseClipboard();
			return false;
		}
		wchar_t* wgClipBoardBuffer = static_cast<wchar_t*>(GlobalLock(hgClipBuffer));
		wcscpy_s(wgClipBoardBuffer, sizeInWords, w.c_str());
		GlobalUnlock(hgClipBuffer);
		EmptyClipboard();
		SetClipboardData(CF_UNICODETEXT, hgClipBuffer);
		CloseClipboard();
		return true;
	}
	return false;
}


bool CopyImageToClipboard(CImage& img)
{
	bool result = false;
	Gdiplus::Bitmap* gdibmp = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)img, 0);

	if (gdibmp)
	{
		HBITMAP hbitmap;
		gdibmp->GetHBITMAP(0, &hbitmap);
		if (OpenClipboard(NULL))
		{
			EmptyClipboard();
			DIBSECTION ds;
			if (GetObject(hbitmap, sizeof(DIBSECTION), &ds))
			{
				HDC hdc = GetDC(HWND_DESKTOP);
				//create compatible bitmap (get DDB from DIB)
				HBITMAP hbitmap_ddb = CreateDIBitmap(hdc, &ds.dsBmih, CBM_INIT,
					ds.dsBm.bmBits, (BITMAPINFO*)&ds.dsBmih, DIB_RGB_COLORS);
				ReleaseDC(HWND_DESKTOP, hdc);
				SetClipboardData(CF_BITMAP, hbitmap_ddb);
				DeleteObject(hbitmap_ddb);
				result = true;
			}
			CloseClipboard();
		}

		//cleanup:
		DeleteObject(hbitmap);
		delete gdibmp;
	}
	return result;
}