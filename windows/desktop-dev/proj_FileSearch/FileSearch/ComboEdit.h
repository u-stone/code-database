#pragma once


// CComboEdit

class CComboEdit : public CEdit
{
	DECLARE_DYNAMIC(CComboEdit)

public:
	CComboEdit();
	virtual ~CComboEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};