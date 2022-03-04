/*
 * =====================================================================================
 *
 *       Filename:  ImageResize.cpp
 *
 *    Description:  图片缩放功能
 *
 *        Version:  1.0
 *        Created:  2020年02月19日 17时32分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include <Magick++.h>
#include <math.h>

using namespace Magick;
int32_t ImageResize(const CMyString &strFile, const CMyString &strThumb, const CMyString &strGeometry)
{
    try
    {
        InitializeMagick("./");

        Image img1;
        LogDebug("准备装载图片 [%s]", strFile.c_str());
        img1.read(strFile.c_str());
        int32_t w = img1.columns();
        int32_t h = img1.rows();
        LogDebug("图片的原始宽及高: w[%d], h[%d]", w, h); 

        char szGeometry[64] = "";
        int32_t tb_w(0), tb_h(0);
        int32_t tb_val(100);
        char szType[64] = "";
        CMyString strLog;
        if (sscanf(strGeometry.c_str(), "%d%[xX*]%d", &tb_w, szType, &tb_h)==3)
        {
            strLog.Format("=> 指定图片宽及高 ");
            sprintf(szGeometry, "%dx%d", tb_w, tb_h);
        }
        else if (sscanf(strGeometry.c_str(), "%d%[wWhH%]", &tb_val, szType)==2)
        {
            char cType = szType[0];
            if (cType == 'w' || cType=='W')
            {
                strLog.Format("=> 指定图片宽度 ");
                tb_w = tb_val;
                tb_h = (int32_t)ceil(tb_w * h * 1.0 / w);
                sprintf(szGeometry, "%dx%d", tb_w, tb_h);
            }
            else if (cType=='h' || cType=='H')
            {
                strLog.Format("=> 指定图片高度 ");
                tb_h = tb_val;
                tb_w = (int32_t)ceil(tb_h * w * 1.0 / h);
                sprintf(szGeometry, "%dx%d", tb_w, tb_h);
            }
            else if (cType=='%')
            {
                strLog.Format("=> 指定图片比例 ");
                tb_w = (int32_t)ceil(w*tb_val/100.0);
                tb_h = (int32_t)ceil(h*tb_val/100.0);
                sprintf(szGeometry, "%d%%", tb_val);
            }
            else
            {
                LogErr("geomerty string [%s] invalid.", strGeometry.c_str());
                return -5;
            }
        }
        else
        {
            LogErr("geomerty string [%s] invalid.", strGeometry.c_str());
            return -7;
        }
        strLog.Append(" w[%d] h[%d]", tb_w, tb_h);
        LogDebug(strLog.c_str());

        img1.resize(szGeometry);

        img1.write(strThumb.c_str());
    }
    catch(Exception &e)
    {
        LogErr("image resize error: %s", e.what());
        return -1;
    }
    catch(...)
    {
        LogErr("image resize error.");
        return -3;
    }
    return 0;
}


