/*
 * Copyright (c) 2005-2012, Claybird
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:

 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Claybird nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include "stdafx.h"
#include "LogListDialog.h"
#include "../Utilities/OSUtil.h"

//ログ情報をセット
void CLogListDialog::SetLogArray(const std::vector<ARCLOG> &rLog)
{
	m_LogArray=rLog;

	//すべてOKかどうかをチェック
	UINT size=m_LogArray.size();
	m_bAllOK=true;
	for(UINT i=0;i<size;i++){
		if(m_LogArray[i].Result!=EXTRACT_OK&&m_LogArray[i].Result!=TEST_OK){
			m_bAllOK=false;
			//break;
		}
		//文字の置き換え
		m_LogArray[i].strMsg.Replace(_T("\n"),_T("\r\n"));
		m_LogArray[i].strMsg.Replace(_T("\r\r\n"),_T("\r\n"));
	}
}

LRESULT CLogListDialog::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	if(m_LogArray.empty()){
		m_bAllOK=true;
	}

	CenterWindow();

	//---ダイアログの初期化
	// 大きいアイコン設定
	HICON hIcon = AtlLoadIconImage(IDI_APP, LR_DEFAULTCOLOR,::GetSystemMetrics(SM_CXICON),::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	// 小さいアイコン設定
	HICON hIconSmall = AtlLoadIconImage(IDI_APP, LR_DEFAULTCOLOR,::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	//ダイアログキャプション設定
	SetWindowText(m_strCaption+(m_bAllOK?_T("[All Clear]"):_T("[NG]")));

	//処理モード文字列設定
	CStatic StaticInfo=GetDlgItem(IDC_STATIC_LOGINFO);
	StaticInfo.SetWindowText(CString(MAKEINTRESOURCE(IDS_LOGINFO_OPERATION_TESTARCHIVE)));

	//------------------------
	//---エディットの初期化
	m_MsgEdit=GetDlgItem(IDC_EDIT_LOGINFO_MSG);
	m_PathEdit=GetDlgItem(IDC_EDIT_LOGINFO_FILE);


	//---リストビューの初期化
	m_ItemListView=GetDlgItem(IDC_LIST_LOGINFO_ITEMS);
	//スタイル設定
	m_ItemListView.SetExtendedListViewStyle(/*LVS_EX_INFOTIP | */LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	//---リストビューにカラム追加
	//ファイル名
	m_ItemListView.InsertColumn(0, CString(MAKEINTRESOURCE(IDS_FILELIST_COLUMN_FILENAME)), LVCFMT_LEFT, 160,-1);
	//検査結果(OK/NG)
	CRect rc;
	m_ItemListView.GetClientRect(rc);
	m_ItemListView.InsertColumn(1, CString(MAKEINTRESOURCE(IDS_LOGINFO_RESULT)), LVCFMT_LEFT, rc.Width()-160-15,-1);

	//レポートモードに設定
	DWORD Style=m_ItemListView.GetWindowLong(GWL_STYLE);
	Style&=~(LVS_ICON|LVS_REPORT|LVS_SMALLICON|LVS_LIST);
	m_ItemListView.SetWindowLong(GWL_STYLE,Style|LVS_REPORT);


	//アイテム数設定
	m_ItemListView.SetItemCount(m_LogArray.size());
	if(!m_LogArray.empty()){
		m_ItemListView.SetItemState(0,LVNI_SELECTED,LVNI_SELECTED);
	}

	//---リストビューにフォーカスを持たせる
	m_ItemListView.SetFocus();

	//---ダイアログリサイズ初期化
	DlgResize_Init(true, true, WS_THICKFRAME | WS_CLIPCHILDREN);
	//---ウィンドウをアクティブにする
	UtilSetAbsoluteForegroundWindow(hWnd);

	return TRUE;
}



//仮想リストビューのアイテム取得に反応
LRESULT CLogListDialog::OnGetDispInfo(LPNMHDR pnmh)
{
	LV_DISPINFO* pstLVDInfo=(LV_DISPINFO*)pnmh;

	//添え字チェック
	if(pstLVDInfo->item.iItem<0||(unsigned int)pstLVDInfo->item.iItem>=m_LogArray.size())return 0;

	ARCLOG &LogInfo=m_LogArray[pstLVDInfo->item.iItem];

	switch(pstLVDInfo->item.iSubItem){
	case 0:	//ファイル名
		if(pstLVDInfo->item.mask & LVIF_TEXT){
			_tcsncpy_s(pstLVDInfo->item.pszText,pstLVDInfo->item.cchTextMax, LogInfo.strFile,pstLVDInfo->item.cchTextMax);
			PathStripPath(pstLVDInfo->item.pszText);
		}
		break;
	case 1:	//検査結果(OK/NG)
		if(pstLVDInfo->item.mask & LVIF_TEXT){
			WORD wCaption=-1;
			switch(LogInfo.Result){
			case EXTRACT_OK:
				wCaption=IDS_LOGINFO_RESULT_OK;
				break;
			case EXTRACT_CANCELED:
			case EXTRACT_NG:
				wCaption=IDS_LOGINFO_RESULT_NG;
				break;
			case TEST_OK:
				wCaption=IDS_LOGINFO_RESULT_OK;
				break;
			case TEST_NG:
				wCaption=IDS_LOGINFO_RESULT_NG;
				break;
			case TEST_NOTIMPL:
				wCaption=IDS_LOGINFO_RESULT_NOTIMPL;
				break;
			case EXTRACT_NOTARCHIVE:	//FALLTHROUGH
			case TEST_NOTARCHIVE:
				wCaption=IDS_LOGINFO_RESULT_NOTARCHIVE;
				break;
			case TEST_ERROR:
				wCaption=IDS_LOGINFO_RESULT_ERROR;
				break;
			case TEST_INFECTED:
			case EXTRACT_INFECTED:
				wCaption=IDS_LOGINFO_RESULT_INFECTED;
				break;
			default:
				ASSERT(!"This code cannot be run");
				return 0;
			}
			_tcsncpy_s(pstLVDInfo->item.pszText,pstLVDInfo->item.cchTextMax, CString(MAKEINTRESOURCE(wCaption)),pstLVDInfo->item.cchTextMax);
		}
		break;
	}
	return 0;
}


//カスタムドロー
DWORD CLogListDialog::OnPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd)
{
	if(lpnmcd->hdr.idFrom==IDC_LIST_LOGINFO_ITEMS)return CDRF_NOTIFYITEMDRAW;
	return CDRF_DODEFAULT;
}

DWORD CLogListDialog::OnItemPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd)
{
	if(lpnmcd->hdr.idFrom==IDC_LIST_LOGINFO_ITEMS){
		//検査結果によって色を変える
		LPNMLVCUSTOMDRAW lpnmlv = (LPNMLVCUSTOMDRAW)lpnmcd;
		switch(m_LogArray[lpnmcd->dwItemSpec].Result){
		case EXTRACT_OK:	//FALLTHROUGH
		case TEST_OK:
			return CDRF_DODEFAULT;
		case EXTRACT_CANCELED:	//FALLTHROUGH
		case EXTRACT_NG:	//FALLTHROUGH
		case TEST_NG:
			lpnmlv->clrText = RGB(0, 0, 0);
			lpnmlv->clrTextBk = RGB(255, 153, 153);
			break;
		case TEST_NOTIMPL:
			lpnmlv->clrText = RGB(0, 0, 0);
			lpnmlv->clrTextBk = RGB(204, 204, 255);
			break;
		case EXTRACT_NOTARCHIVE:	//FALLTHROUGH
		case TEST_NOTARCHIVE:
			lpnmlv->clrText = RGB(0, 0, 0);
			lpnmlv->clrTextBk = RGB(240, 240, 0);
			break;
		case EXTRACT_INFECTED:	//FALLTHROUGH
		case TEST_INFECTED:
		case TEST_ERROR:
			lpnmlv->clrText = RGB(0, 0, 0);
			lpnmlv->clrTextBk = RGB(255, 51, 51);
			break;
		default:
			ASSERT(!"This code cannot be run");
		}
		return CDRF_NOTIFYITEMDRAW;
	}
	return CDRF_DODEFAULT;
}

LRESULT CLogListDialog::OnItemChanged(LPNMHDR pnmh)
{
	if(pnmh->hwndFrom!=m_ItemListView){
		return FALSE;
	}

	int iItem=m_ItemListView.GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
	//iItem==-1 if not found
	if(iItem<0||(unsigned)iItem>=m_LogArray.size())return FALSE;
	m_MsgEdit.SetWindowText(m_LogArray[iItem].strMsg);
	m_PathEdit.SetWindowText(m_LogArray[iItem].strFile);
	return TRUE;
}

//コントロールの色:読み取り専用エディット
HBRUSH CLogListDialog::OnCtrlColorEdit(HDC,HWND hWnd)
{
	if(hWnd==m_MsgEdit||hWnd==m_PathEdit){
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
	}
	else return (HBRUSH)CDialogImpl<CLogListDialog>::DefWindowProc();
}


//比較関数オブジェクト
struct COMP{
	int nCol;	//比較に使う列
	COMP(int n):nCol(n){}
	bool operator()(const ARCLOG& x, const ARCLOG& y)const{
		switch(nCol){
		case 0:	//ファイル名
			return (_tcsicmp(x.strFile, y.strFile)<0);
		case 1:	//ステータス
			return (x.Result< y.Result);
		default:
			ASSERT(!"This code cannot be run");
			return false;
		}
	}
};


//ソート
LRESULT CLogListDialog::OnSortItem(LPNMHDR pnmh)
{
	//リストビューにしか反応しない
	if(pnmh->hwndFrom!=m_ItemListView){
		return 0;
	}

	//件数が1個以下ならソートは必要ない
	if(m_LogArray.size()<=1){
		return 0;
	}

	LPNMLISTVIEW lpNMLV=(LPNMLISTVIEW)pnmh;
	int nCol=lpNMLV->iSubItem;
	if(nCol==m_nSortColumn){
		//逆順ソート
		m_bSortDescending = !m_bSortDescending;
		std::reverse(m_LogArray.begin(),m_LogArray.end());	//逆順に
		m_ItemListView.Invalidate();
		return 0;
	}

	switch(nCol){
	case 0:	//ファイル名
	case 1:	//ステータス
		std::sort(m_LogArray.begin(),m_LogArray.end(),COMP(nCol));
		break;
	default:
		ASSERT(!"This code cannot be run");
		return 0;
	}

	m_nSortColumn=nCol;
	m_ItemListView.Invalidate();
	return 0;
}
