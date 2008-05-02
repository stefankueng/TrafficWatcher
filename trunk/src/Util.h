#ifndef __CUtil_h__
#define __CUtil_h__

/**
 * CUtil.
 * a class with only static methods. contains independent utility and helper methods
 * not covered by system api's.
 */

class CUtil
{
public:

	static float int64_to_float(DWORD64 value)
	{
		float temp;

		temp = ((float)((signed __int64)value >> 32) * 4294967295) + ((float)((signed __int64)value & 0xFFFFFFFF));
		return temp;
	}
	static CString	GetNumberString(float value)
	{
		if (IsIEC())
			return GetNumberStringIEC(value);
		return GetNumberStringSI(value);
	}
	static CString 	GetNumberString(double value) {return GetNumberString( (float)value );};
	static CString	GetNumberString(DWORD64 value) {return GetNumberString( int64_to_float(value) );};
	static CString	GetNumberString(DWORD value) {return GetNumberString( (float)value );};
	static CString	GetNumberString(SHORT value) {return GetNumberString( (float)value );};
	static CString	GetNumberString(int value) {return GetNumberString( (float)value );};
	/*!
	 * converts a number to a readable String. big numbers are shortened with
	 * the SI prefix. the SI tells that "k" is 1000, "M" is 1'000'000 and so on.
	 * if you want the readable string to be based on binary use the IEC-counterparts
	 * of this method.
	 * example: a number of 149670 will be converted to "149.67 k".
	 * its now easy to add "bytes/s" or "Bytes" -> "149.67 kbytes/s" or "149.67 kBytes"
	 * @param value : the number to convert into a readable string
	 */
	static CString	GetNumberStringSI(float value)
	{
		CString temp;
		CString b;

		if (IsBits())
		{
			b = "Bits";
			value *= 8;
		}
		else
			b = "Bytes";

		if (value < 1000)
		{
			temp.Format("%.2f ", value);
			return temp+b;
		}
		value /= 1000;
		if (value < 1000)
		{
			temp.Format("%.2f k", value);
			return temp+b;
		}
		value /= 1000;
		if (value < 1000)
		{
			temp.Format("%.2f M", value);
			return temp+b;
		}
		value /= 1000;
		if (value < 1000)
		{
			temp.Format("%.2f G", value);
			return temp+b;
		}
		value /= 1000;
		if (value < 1000)
		{
			temp.Format("%.2f T", value);
			return temp+b;
		}
		value /= 1000;
		if (value < 1000)
		{
			temp.Format("%.2f P", value);
			return temp+b;
		}
		value /= 1000;
		temp.Format("%.2f E", value);
		return temp+b;
	}
	static CString 	GetNumberStringSI(double value) {return GetNumberStringSI( (float)value );};
	static CString	GetNumberStringSI(DWORD64 value) {return GetNumberStringSI( int64_to_float(value) );};
	static CString	GetNumberStringSI(DWORD value) {return GetNumberStringSI( (float)value );};
	static CString	GetNumberStringSI(SHORT value) {return GetNumberStringSI( (float)value );};
	static CString	GetNumberStringSI(int value) {return GetNumberStringSI( (float)value );};

	/*!
	 * converts a number to a readable String. big numbers are shortened with
	 * the IEC prefix. the IEC tells that "k" is 1024, "M" is 1'048'576 and so on.
	 * if you want the readable string to be based on decimal use the SI-counterparts
	 * of this method.
	 * example: a number of 149670 will be converted to "146.16 ki".
	 * its now is easy to add "bytes/s" or "Bytes" -> "1146.16 kibytes/s" or "146.16 kiBytes"
	 * @param value : the number to convert into a readable string
	 */
	static CString	GetNumberStringIEC(float value)
	{
		CString temp;
		CString b;
		if (IsBits())
		{
			b = "Bits";
			value *= 8;
		}
		else
			b = "Bytes";

		if (value < 1024)
		{
			temp.Format("%.2f ", value);
			return temp+b;
		}
		value /= 1024;
		if (value < 1024)
		{
			temp.Format("%.2f Ki", value);
			return temp+b;
		}
		value /= 1024;
		if (value < 1024)
		{
			temp.Format("%.2f Me", value);
			return temp+b;
		}
		value /= 1024;
		if (value < 1024)
		{
			temp.Format("%.2f Gi", value);
			return temp+b;
		}
		value /= 1024;
		if (value < 1024)
		{
			temp.Format("%.2f Ti", value);
			return temp+b;
		}
		value /= 1024;
		if (value < 1024)
		{
			temp.Format("%.2f Pi", value);
			return temp+b;
		}
		value /= 1024;
		temp.Format("%.2f Ei", value);
		return temp+b;
	}
	static CString 	GetNumberStringIEC(double value) {return GetNumberStringIEC( (float)value );};
	static CString	GetNumberStringIEC(DWORD64 value) {return GetNumberStringIEC( CUtil::int64_to_float(value) );};
	static CString	GetNumberStringIEC(DWORD value) {return GetNumberStringIEC( (float)value );};
	static CString	GetNumberStringIEC(SHORT value) {return GetNumberStringIEC( (float)value );};
	static CString	GetNumberStringIEC(int value) {return GetNumberStringIEC( (float)value );};



    /*!
     * checks if the app should show speed and data as bits or bytes. read from the registry.
     *
     * @return static BOOL  : TRUE for bits, FALSE for bytes
     */
	static BOOL IsBits()
	{
		HKEY hKey;
		LONG lnRes;
		CString key;
		BOOL bits = FALSE;
		key = "SOFTWARE\\";
		key += LPCTSTR( M_APPNAME );
		lnRes = RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey);
		if (lnRes == ERROR_SUCCESS)
		{
			DWORD valuesize;
			ULONG type;
			valuesize = sizeof(BOOL);
			RegQueryValueEx(hKey, "bits", 0, &type, (unsigned char *)&bits, &valuesize);
		}
		RegCloseKey(hKey);
		return bits;
	}

    /*!
     * checks if the app should show speed and data in SI or IEC format. read from the registry.
     *
     * @return static BOOL  : TRUE for IEC format, FALSE for SI format.
     */
	static BOOL IsIEC()
	{
		HKEY hKey;
		LONG lnRes;
		CString key;
		BOOL iec = FALSE;
		key = "SOFTWARE\\";
		key += LPCTSTR( M_APPNAME );
		lnRes = RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey);
		if (lnRes == ERROR_SUCCESS)
		{
			DWORD valuesize;
			ULONG type;
			valuesize = sizeof(BOOL);
			RegQueryValueEx(hKey, "iec", 0, &type, (unsigned char *)&iec, &valuesize);
		}
		RegCloseKey(hKey);
		return iec;
	}
	

    /*!
     * sets if the app should show speed and data as bits or bytes. written to the registry.
     *
     * @param b : TRUE for bits, FALSE for bytes
     */
	static void SetBits(BOOL b)
	{
		HKEY hKey;
		LONG lnRes;
		CString key;
		DWORD value;
		key = "SOFTWARE\\";
		key += LPCTSTR( M_APPNAME );
		lnRes = RegCreateKeyEx(HKEY_CURRENT_USER, key, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &value);
		if (lnRes == ERROR_SUCCESS)
		{
			DWORD valuesize;
			valuesize = sizeof(BOOL);
			RegSetValueEx(hKey, "bits", 0, REG_DWORD, (unsigned char *)&b, valuesize);
		}
		RegCloseKey(hKey);
	}


    /*!
     * sets if the app should show speed and data in IEC or SI format. written to the registry.
     *
     * @param b : TRUE for IEC format, FALSE for SI format.
     */
	static void SetIEC(BOOL b)
	{
		HKEY hKey;
		LONG lnRes;
		CString key;
		DWORD value;
		key = "SOFTWARE\\";
		key += LPCTSTR( M_APPNAME );
		lnRes = RegCreateKeyEx(HKEY_CURRENT_USER, key, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &value);
		if (lnRes == ERROR_SUCCESS)
		{
			DWORD valuesize;
			valuesize = sizeof(BOOL);
			RegSetValueEx(hKey, "iec", 0, REG_DWORD, (unsigned char *)&b, valuesize);
		}
		RegCloseKey(hKey);
	}

#define COLOR_WHITE RGB(255,255,255)
#define COLOR_BLACK RGB(0,0,0)
#define NCOLORSHADES 128		// this many shades in gradient



    /*!
     * Draws a "good looking" data bar. the bar has a light effect.
     *
     * @param pDC : the painting context
     * @param rect : the rectangle for the bar.
     * @param colorFrom : the actual color of the bar
     * @param colorTo : the color to use for the light effect. default is white
     * @param IsHori : TRUE to draw the bar horizontally, FALSE for vertically
	 * \image html "file" "caption"
     */
	static void DrawColorCube(CDC* pDC, CRect& rect, COLORREF colorFrom, COLORREF colorTo = RGB(255,255,255), BOOL IsHori = true)
	{
		CRect       newRect;
		CPen*       oldPen;
		CBrush*     oldBr;    

		/*
		 *  If value as 0, draw a line here;
		 */
		if(rect.Width()<=0 )
			rect.right = rect.left +2;
		else if(rect.Height() <=0)
			rect.top-=2;

		BOOL backHori = IsHori;
		//IsHori = true;

		/* 
		 * draw first 1/3 part;
		 */
		if(IsHori)
		{
			newRect.SetRect(rect.left, rect.top,(rect.right - rect.left)/3+rect.left, rect.bottom);
			PaintGradiantRect(pDC,newRect,colorFrom, colorTo);
		}
		else
		{
			newRect.SetRect(rect.left, rect.bottom - 2*(rect.bottom-rect.top)/3, rect.right,rect.bottom);
			PaintGradiantRect(pDC,newRect,colorFrom, colorTo, IsHori, false);
		}

		/*
		 * then left 2/3 part;
		 */
		if(IsHori)
		{
			newRect.SetRect((rect.right - rect.left)/3+rect.left, rect.top,rect.right, rect.bottom);
			PaintGradiantRect(pDC,newRect,colorFrom, colorTo, IsHori, false);
		}
		else
		{
			newRect.SetRect(rect.left, rect.top, rect.right, rect.bottom - 2*(rect.bottom-rect.top)/3);
			PaintGradiantRect(pDC,newRect,colorFrom, colorTo, IsHori, true);
		}

		/*
		 *   Frame the rect with colorFrom, will look better;
		 */
		CPen pen(PS_SOLID,2,colorFrom);
		oldPen  = (CPen*)pDC->SelectObject(&pen);
		oldBr   = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(&rect);
		pDC->SelectObject(oldPen);
		pDC->SelectObject(oldBr);
		pen.DeleteObject();


	}



private:
	static void PaintRect(CDC* pDC, int x, int y, int w, int h, COLORREF color)
	{
		CBrush brush(color);
		CBrush* pOldBrush = pDC->SelectObject(&brush);
		pDC->PatBlt(x, y, w, h, PATCOPY);
		pDC->SelectObject(pOldBrush);
		brush.DeleteObject();
	}

	static void PaintGradiantRect(CDC *pDC, const RECT &rect,COLORREF clrFrom, COLORREF clrTo =RGB(255,255,255), 
						   BOOL hori = true, BOOL ascend = true)
	{
		int cxCap = rect.right - rect.left;
		int cyCap = rect.bottom - rect.top;

		// Get the intensity values for the ending color
		int r1 = GetRValue(clrTo); // red
		int g1 = GetGValue(clrTo); // green
		int b1 = GetBValue(clrTo); // blue
		
		// Get the intensity values for the begining color
		int r2 = GetRValue(clrFrom); // red
		int g2 = GetGValue(clrFrom); // green
		int b2 = GetBValue(clrFrom); // blue

		int r, g, b;

		if(hori) //paint horizontal rect;
		{
			int x = cxCap;	
			int w = x;							// width of area to shade
			int xDelta= max(w/NCOLORSHADES,1);	// width of one shade band


			while (x >= xDelta) {
				x -= xDelta;
				if (r1 > r2)
					r = r1 - (r1-r2)*(w-x)/w;
				else
					r = r1 + (r2-r1)*(w-x)/w;

				if (g1 > g2)
					g = g1 - (g1-g2)*(w-x)/w;
				else
					g = g1 + (g2-g1)*(w-x)/w;

				if (b1 > b2)
					b = b1 - (b1-b2)*(w-x)/w;
				else
					b = b1 + (b2-b1)*(w-x)/w;

				if(ascend) // Paint from  left to right;
					PaintRect(pDC, rect.left+x, rect.top, xDelta, cyCap, RGB(r, g, b));
				else               // Paint from  right to left;
					PaintRect(pDC, rect.right-x-xDelta, rect.top, xDelta, cyCap, RGB(r, g, b));
			}
		}
		else    //paint vertical rect;
		{
			int y = cyCap;	
			int w = y;							// height of area to shade
			int yDelta= max(w/NCOLORSHADES,1);	// height of one shade band


			//while (y >= yDelta) {
			while (y > 0) {
				y -= yDelta;
				if (r1 > r2)
					r = r1 - (r1-r2)*(w-y)/w;
				else
					r = r1 + (r2-r1)*(w-y)/w;

				if (g1 > g2)
					g = g1 - (g1-g2)*(w-y)/w;
				else
					g = g1 + (g2-g1)*(w-y)/w;

				if (b1 > b2)
					b = b1 - (b1-b2)*(w-y)/w;
				else
					b = b1 + (b2-b1)*(w-y)/w;
				
				if(ascend) // Paint from  top to bottom;
					PaintRect(pDC, rect.left, rect.top+y, cxCap, yDelta, RGB(r, g, b));
				else       // Paint from  bottom to top;
					PaintRect(pDC, rect.left, rect.bottom-y-yDelta, cxCap, yDelta, RGB(r, g, b));
			}
		}

	}

};

#endif
