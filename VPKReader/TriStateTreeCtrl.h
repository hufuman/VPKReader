#pragma once


// CTriStateTreeCtrl

enum TriStateTreeState
{
    TSS_None            = 0,
    TSS_UnChecked       = 1,
    TSS_Checked         = 2,
    TSS_Indeterminate   = 3,
};

class CTriStateTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTriStateTreeCtrl)

public:
	CTriStateTreeCtrl();
	virtual ~CTriStateTreeCtrl();

    TriStateTreeState GetTriCheck(HTREEITEM hItem);

private:
    void CheckSubItem(HTREEITEM hItem, BOOL bChecked);
    TriStateTreeState GetSubItemState(HTREEITEM hItem);
    void TriUpdateItemState(HTREEITEM hTreeItem, TriStateTreeState state);

protected:
    CImageList  m_hImageList;
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
protected:
    virtual void PreSubclassWindow();
public:
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

};


