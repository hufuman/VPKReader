// TriStateTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VPKReader.h"
#include "TriStateTreeCtrl.h"

#include "resource.h"

// CTriStateTreeCtrl

IMPLEMENT_DYNAMIC(CTriStateTreeCtrl, CTreeCtrl)

CTriStateTreeCtrl::CTriStateTreeCtrl()
{
    m_hImageList.Create(15, 14, ILC_COLOR24, 1, 1);
    CBitmap bmp;
    VERIFY(bmp.LoadBitmap(IDB_BITMAP_TREE));
    m_hImageList.Add(&bmp, (CBitmap*)NULL);
}

CTriStateTreeCtrl::~CTriStateTreeCtrl()
{
    m_hImageList.DeleteImageList();
}


BEGIN_MESSAGE_MAP(CTriStateTreeCtrl, CTreeCtrl)
    ON_NOTIFY_REFLECT(NM_CLICK, &CTriStateTreeCtrl::OnNMClick)
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CTriStateTreeCtrl message handlers



void CTriStateTreeCtrl::OnNMClick(NMHDR *, LRESULT *pResult)
{
    DWORD dwPos = ::GetMessagePos();

    CPoint Pt;
    Pt.x = LOWORD(dwPos);
    Pt.y = HIWORD(dwPos);

    ::ScreenToClient(m_hWnd, &Pt);

    UINT uFlags = 0;
    HTREEITEM hItem = HitTest(Pt, &uFlags);
    if(hItem != NULL && (uFlags & TVHT_ONITEMSTATEICON))
    {
        TriStateTreeState nState = (TriStateTreeState)(GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12);
        if(nState == TSS_UnChecked)
        {
            nState = TSS_Checked;
        }
        else if(nState == TSS_Checked)
        {
            nState = TSS_UnChecked;
        }
        else if(nState == TSS_Indeterminate)
        {
            nState = TSS_UnChecked;
        }
        else
        {
            nState = TSS_UnChecked;
        }
        SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
        TriUpdateItemState(hItem, nState);
        *pResult = 1;
        return;
    }

    *pResult = 0;
}

void CTriStateTreeCtrl::TriUpdateItemState(HTREEITEM hTreeItem, TriStateTreeState state)
{
    BOOL bChecked = (state != TSS_UnChecked);
    CheckSubItem(hTreeItem, bChecked);

    HTREEITEM hParent = hTreeItem;

    while((hParent = GetParentItem(hParent)) != NULL)
    {
        TriStateTreeState parentState = GetSubItemState(hParent);
        SetItemState(hParent, INDEXTOSTATEIMAGEMASK(parentState), TVIS_STATEIMAGEMASK);
    }
}

TriStateTreeState CTriStateTreeCtrl::GetTriCheck(HTREEITEM hItem)
{
    if(hItem == NULL)
        return TSS_UnChecked;
    TriStateTreeState nState = (TriStateTreeState)(GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12);
    return nState;
}

void CTriStateTreeCtrl::CheckSubItem(HTREEITEM hItem, BOOL bChecked)
{
    SetItemState(hItem, INDEXTOSTATEIMAGEMASK(bChecked ? TSS_Checked : TSS_UnChecked), TVIS_STATEIMAGEMASK);

    HTREEITEM hChild = GetNextItem(hItem, TVGN_CHILD);
    while(hChild != NULL)
    {
        CheckSubItem(hChild, bChecked);
        hChild = GetNextSiblingItem(hChild);
    }
}

TriStateTreeState CTriStateTreeCtrl::GetSubItemState(HTREEITEM hItem)
{
    int nCheckCount = 0;
    int nUnCheckCount = 0;

    HTREEITEM hChild = GetNextItem(hItem, TVGN_CHILD);
    while(hChild != NULL)
    {
        UINT nState = GetItemState(hChild, TVIS_STATEIMAGEMASK) >> 12;

        switch(nState)
        {
        case TSS_Checked:
        case TSS_Indeterminate:
            ++ nCheckCount;
            break;
        case TSS_UnChecked:
            ++ nUnCheckCount;
            break;
        }

        hChild = GetNextSiblingItem(hChild);
    }

    if(nCheckCount == 0)
        return TSS_UnChecked;
    else if(nUnCheckCount == 0)
        return TSS_Checked;
    else
        return TSS_Indeterminate;
}

void CTriStateTreeCtrl::PreSubclassWindow()
{
    // TODO: Add your specialized code here and/or call the base class

    CTreeCtrl::PreSubclassWindow();

    SetImageList(&m_hImageList, TVSIL_STATE);
}

void CTriStateTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if(nChar != VK_SPACE)
        CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void CTriStateTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Add your message handler code here and/or call default

    if(nChar == VK_SPACE)
    {
        HTREEITEM hItem = this->GetSelectedItem();
        CString strText = this->GetItemText(hItem);
        if(hItem != NULL)
        {
            TriStateTreeState nState = (TriStateTreeState)(GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12);
            if(nState == TSS_UnChecked)
            {
                nState = TSS_Checked;
            }
            else if(nState == TSS_Checked)
            {
                nState = TSS_UnChecked;
            }
            else if(nState == TSS_Indeterminate)
            {
                nState = TSS_UnChecked;
            }
            else
            {
                nState = TSS_UnChecked;
            }
            SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
            TriUpdateItemState(hItem, nState);
            return;
        }
    }

    CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
