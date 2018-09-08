
#include "rbtree.h"

#ifdef __cplusplus
extern "C" {
#endif



static void rbtreeMidVisitHelper(pstRBTREE_NODE_S v_pstNode);
RBTREE_NODE_S *rbtreeNodeSearch(pstRBTREE_NODE_S v_pstRoot, KEY v_uiLba);
static void rbtreeNodePrint(pstRBTREE_NODE_S v_pstNode);
static void rbtreeDeleteFixup(pstRBTREE_NODE_S v_pstNode,
                                          pstRBTREE_NODE_S v_pstParent,
                                          RBTREE_ROOT_S   *v_pstRoot);

/*
external function declarations
how to compare key depend on caller.
*/
extern int rbtreeKeyCompare(pstRBTREE_NODE_S v_pstTreeNode, KEY v_uiKey);


/*****************************************************************************/
void rbtreeNodeInit(pstRBTREE_NODE_S v_pstNode)
{
    v_pstNode->enColor     = COLOR_BUTT;
    v_pstNode->pstLeft   = NIL;
    v_pstNode->pstRight  = NIL;
    v_pstNode->pstParent = NIL;

    return;
}

/*-----------------------------------------------------------------------------
|  A              B
| / \    ==>     / \
|a   B          A   y
|    / \       / \
|    b y       a b
-----------------------------------------------------------------------------*/
static void rbtreeLeftRotate(pstRBTREE_NODE_S v_pstNode,
                                         RBTREE_ROOT_S   *v_pstRoot)
{
    pstRBTREE_NODE_S pstRightNode  = v_pstNode->pstRight;
    pstRBTREE_NODE_S pstParentNode = RB_PARENT(v_pstNode);

    if ((v_pstNode->pstRight = pstRightNode->pstLeft))
    {
        RB_SET_PARENT(pstRightNode->pstLeft, v_pstNode);
    }

    pstRightNode->pstLeft = v_pstNode;

    RB_SET_PARENT(pstRightNode, pstParentNode);

    if (pstParentNode)
    {
        if (v_pstNode == pstParentNode->pstLeft)
        {
            pstParentNode->pstLeft = pstRightNode;
        }
        else
        {
            pstParentNode->pstRight = pstRightNode;
        }
    }
    else
    {
        v_pstRoot->pstNode = pstRightNode;
    }

    RB_SET_PARENT(v_pstNode, pstRightNode);

    return;
}

/*-----------------------------------------------------------------------------
|    A              B
|   / \            / \
|  B   y   ==>    a   A
| / \                / \
|a   b              b   y
-----------------------------------------------------------------------------*/
static void rbtreeRightRotate(pstRBTREE_NODE_S v_pstNode,
                                          RBTREE_ROOT_S   *v_pstRoot)
{
    pstRBTREE_NODE_S pstLeftNode   = v_pstNode->pstLeft;
    pstRBTREE_NODE_S pstParentNode = RB_PARENT(v_pstNode);

    if ((v_pstNode->pstLeft = pstLeftNode->pstRight))
    {
        RB_SET_PARENT(pstLeftNode->pstRight, v_pstNode);
    }

    pstLeftNode->pstRight = v_pstNode;

    RB_SET_PARENT(pstLeftNode, pstParentNode);

    if (pstParentNode)
    {
        if (v_pstNode == pstParentNode->pstRight)
        {
            pstParentNode->pstRight = pstLeftNode;
        }
        else
        {
            pstParentNode->pstLeft = pstLeftNode;
        }
    }
    else
    {
        v_pstRoot->pstNode = pstLeftNode;
    }

    RB_SET_PARENT(v_pstNode, pstLeftNode);

    return;
}

static void rbtreeInsertFixup(pstRBTREE_NODE_S v_pstNode,
                                          RBTREE_ROOT_S   *v_pstRoot)
{
    pstRBTREE_NODE_S pstParentNode;
    pstRBTREE_NODE_S pstGparentNode;

    while ((pstParentNode = RB_PARENT(v_pstNode)) && RB_IS_RED(pstParentNode))
    {
        pstGparentNode = RB_PARENT(pstParentNode);

        if (pstParentNode == pstGparentNode->pstLeft)
        {
            {
                register pstRBTREE_NODE_S pstUncle = pstGparentNode->pstRight;
                if (pstUncle && RB_IS_RED(pstUncle))
                {
                    RB_SET_BLACK(pstUncle);
                    RB_SET_BLACK(pstParentNode);
                    RB_SET_RED(pstGparentNode);
                    v_pstNode = pstGparentNode;

                    continue;
                }
            }

            if (pstParentNode->pstRight == v_pstNode)
            {
                register pstRBTREE_NODE_S pstTmp;
                rbtreeLeftRotate(pstParentNode, v_pstRoot);
                pstTmp = pstParentNode;
                pstParentNode = v_pstNode;
                v_pstNode = pstTmp;
            }

            RB_SET_BLACK(pstParentNode);
            RB_SET_RED(pstGparentNode);
            rbtreeRightRotate(pstGparentNode, v_pstRoot);
        }
        else
        {
            {
                register pstRBTREE_NODE_S pstUncle = pstGparentNode->pstLeft;
                if (pstUncle && RB_IS_RED(pstUncle))
                {
                    RB_SET_BLACK(pstUncle);
                    RB_SET_BLACK(pstParentNode);
                    RB_SET_RED(pstGparentNode);
                    v_pstNode = pstGparentNode;

                    continue;
                }
            }

            if (pstParentNode->pstLeft == v_pstNode)
            {
                register pstRBTREE_NODE_S pstTmp;
                rbtreeRightRotate(pstParentNode, v_pstRoot);
                pstTmp = pstParentNode;
                pstParentNode = v_pstNode;
                v_pstNode = pstTmp;
            }

            RB_SET_BLACK(pstParentNode);
            RB_SET_RED(pstGparentNode);
            rbtreeLeftRotate(pstGparentNode, v_pstRoot);
        }
    }

    RB_SET_BLACK(v_pstRoot->pstNode);
    return;
}

static void rbtreeLinkNode(pstRBTREE_NODE_S v_pstNode,
                                       pstRBTREE_NODE_S v_pstParent,
                                       RBTREE_NODE_S  **ppstLink)
{
    v_pstNode->pstParent = v_pstParent;
    v_pstNode->pstLeft  = NIL;
    v_pstNode->pstRight = NIL;
    v_pstNode->enColor = COLOR_RED; /* XXX. */

    *ppstLink = v_pstNode;

    return;
}

void rbtreeNodeInsert(RBTREE_ROOT_S   *v_pstRoot,
                              pstRBTREE_NODE_S v_pstNode)
{
    int iRet;

    RBTREE_NODE_S *pstParent = NULL;
    RBTREE_NODE_S **ppstLink = &v_pstRoot->pstNode;

    while (*ppstLink)
    {
        pstParent = *ppstLink;

        iRet = rbtreeKeyCompare(pstParent, RBTREE_NODE_TO_KEY(v_pstNode));

        if (iRet < 0)
        {
            ppstLink = &(*ppstLink)->pstLeft;
        }
        else if (iRet > 0)
        {
            ppstLink = &(*ppstLink)->pstRight;
        }
        /* panic.*/
        else
        {
            return;
        }
    }

    rbtreeLinkNode(v_pstNode, pstParent, ppstLink);
    rbtreeInsertFixup(v_pstNode, v_pstRoot);

    return;
}

void rbtreeNodeDelete(pstRBTREE_NODE_S v_pstNode,
                              RBTREE_ROOT_S   *v_pstRoot)
{
    pstRBTREE_NODE_S pstChildNode;
    pstRBTREE_NODE_S pstParentNode;
    OSP_U32 uiColor;

    if (!v_pstNode->pstLeft)
    {
        pstChildNode = v_pstNode->pstRight;
    }
    else if (!v_pstNode->pstRight)
    {
        pstChildNode = v_pstNode->pstLeft;
    }
    else
    {
        pstRBTREE_NODE_S pstOldNode = v_pstNode;
        pstRBTREE_NODE_S pstLeftNode;

        v_pstNode = v_pstNode->pstRight;

        while ((pstLeftNode = v_pstNode->pstLeft) != NIL)
        {
            v_pstNode = pstLeftNode;
        }

        pstChildNode  = v_pstNode->pstRight;
        pstParentNode = RB_PARENT(v_pstNode);
        uiColor = RB_COLOR(v_pstNode);

        if (pstChildNode)
        {
            RB_SET_PARENT(pstChildNode, pstParentNode);
        }

        if (pstParentNode == pstOldNode)
        {
            pstParentNode->pstRight = pstChildNode;
            pstParentNode = v_pstNode;
        }
        else
        {
            pstParentNode->pstLeft = pstChildNode;
        }

        /* XXX. */
        v_pstNode->pstParent = pstOldNode->pstParent;
        v_pstNode->enColor = pstOldNode->enColor;
        v_pstNode->pstRight = pstOldNode->pstRight;
        v_pstNode->pstLeft = pstOldNode->pstLeft;

        if (RB_PARENT(pstOldNode))
        {
            if (RB_PARENT(pstOldNode)->pstLeft == pstOldNode)
            {
                RB_PARENT(pstOldNode)->pstLeft = v_pstNode;
            }
            else
            {
                RB_PARENT(pstOldNode)->pstRight = v_pstNode;
            }
        }
        else
        {
            v_pstRoot->pstNode = v_pstNode;
        }

        RB_SET_PARENT(pstOldNode->pstLeft, v_pstNode);

        if (pstOldNode->pstRight)
        {
            RB_SET_PARENT(pstOldNode->pstRight, v_pstNode);
        }

        goto color;
    }

    pstParentNode = RB_PARENT(v_pstNode);
    uiColor = RB_COLOR(v_pstNode);

    if (pstChildNode)
    {
        RB_SET_PARENT(pstChildNode, pstParentNode);
    }

    if (pstParentNode)
    {
        if (pstParentNode->pstLeft == v_pstNode)
        {
            pstParentNode->pstLeft = pstChildNode;
        }
        else
        {
            pstParentNode->pstRight = pstChildNode;
        }
    }
    else
    {
        v_pstRoot->pstNode = pstChildNode;
    }

color:
    if (uiColor == COLOR_BLACK)
    {
        rbtreeDeleteFixup(pstChildNode, pstParentNode, v_pstRoot);
    }

    return;
}

RBTREE_NODE_S *rbtreeNodeSearch(pstRBTREE_NODE_S v_pstRoot, KEY v_uiLba)
{
    int iRet;
    pstRBTREE_NODE_S pstNode;

    pstNode = v_pstRoot;
    for (;;)
    {
        iRet = rbtreeKeyCompare(pstNode, v_uiLba);

        if (0 == iRet)
        {
            return pstNode;
        }

        if (iRet < 0)
        {
            if (NIL != pstNode->pstLeft)
            {
                pstNode = pstNode->pstLeft;
            }
            else
            {
                return NIL;
            }
        }
        else
        {
            if (NIL != pstNode->pstRight)
            {
                pstNode = pstNode->pstRight;
            }
            else
            {
                return NIL;
            }
        }
    }
}

RBTREE_NODE_S *rbtreeGetFirstNode(pstRBTREE_NODE_S v_pstRoot)
{
    pstRBTREE_NODE_S pstNext = v_pstRoot;

    if (NIL == v_pstRoot)
    {
        return NIL;
    }

    while (pstNext->pstLeft)
    {
        pstNext = pstNext->pstLeft;
    }

    return pstNext;
}

RBTREE_NODE_S *rbtreeGetNextNode(pstRBTREE_NODE_S v_pstNode)
{
    pstRBTREE_NODE_S pstParent;

    if (NIL == v_pstNode)
    {
        return NIL;
    }

    if (v_pstNode == v_pstNode->pstParent)
    {
        return NIL;
    }

    if (v_pstNode->pstRight)
    {
        v_pstNode = v_pstNode->pstRight;

        while (v_pstNode->pstLeft)
        {
            v_pstNode = v_pstNode->pstLeft;
        }

        return v_pstNode;
    }

    while ((NIL != (pstParent = v_pstNode->pstParent)) && (v_pstNode == pstParent->pstRight))
    {
        v_pstNode = pstParent;
    }

    return pstParent;
}

void rbtreePrint(RBTREE_ROOT_S *v_pstRoot)
{
    pstRBTREE_NODE_S pstNode = rbtreeGetFirstNode(v_pstRoot->pstNode);

    if (NIL == v_pstRoot->pstNode)
    {
        return;
    }

    rbtreeNodePrint(v_pstRoot->pstNode);

    for (; pstNode; pstNode = rbtreeGetNextNode(pstNode))
    {
        rbtreeNodePrint(pstNode);
    }

    return;
}

static void rbtreeNodePrint(pstRBTREE_NODE_S v_pstNode)
{
    return;
}


static void rbtreeDeleteFixup(pstRBTREE_NODE_S v_pstNode,
                                          pstRBTREE_NODE_S v_pstParent,
                                          RBTREE_ROOT_S   *v_pstRoot)
{
    pstRBTREE_NODE_S pstOther;

    while ((!v_pstNode || RB_IS_BLACK(v_pstNode)) && (v_pstNode != v_pstRoot->pstNode))
    {
        if (v_pstParent->pstLeft == v_pstNode)
        {
            pstOther = v_pstParent->pstRight;

            if (RB_IS_RED(pstOther))
            {
                RB_SET_BLACK(pstOther);
                RB_SET_RED(v_pstParent);
                rbtreeLeftRotate(v_pstParent, v_pstRoot);
                pstOther = v_pstParent->pstRight;
            }

            if ((!pstOther->pstLeft || RB_IS_BLACK(pstOther->pstLeft))
               && (!pstOther->pstRight || RB_IS_BLACK(pstOther->pstRight)))
            {
                RB_SET_RED(pstOther);
                v_pstNode   = v_pstParent;
                v_pstParent = RB_PARENT(v_pstNode);
            }
            else
            {
                if (!pstOther->pstRight || RB_IS_BLACK(pstOther->pstRight))
                {
                    pstRBTREE_NODE_S pstOtherLeft;

                    if ((pstOtherLeft = pstOther->pstLeft))
                    {
                        RB_SET_BLACK(pstOtherLeft);
                    }

                    RB_SET_RED(pstOther);
                    rbtreeRightRotate(pstOther, v_pstRoot);
                    pstOther = v_pstParent->pstRight;
                }

                RB_SET_COLOR(pstOther, RB_COLOR(v_pstParent));
                RB_SET_BLACK(v_pstParent);

                if (pstOther->pstRight)
                {
                    RB_SET_BLACK(pstOther->pstRight);
                }

                rbtreeLeftRotate(v_pstParent, v_pstRoot);
                v_pstNode = v_pstRoot->pstNode;

                break;
            }
        }
        else
        {
            pstOther = v_pstParent->pstLeft;

            if (RB_IS_RED(pstOther))
            {
                RB_SET_BLACK(pstOther);
                RB_SET_RED(v_pstParent);
                rbtreeRightRotate(v_pstParent, v_pstRoot);
                pstOther = v_pstParent->pstLeft;
            }

            if ((!pstOther->pstLeft || RB_IS_BLACK(pstOther->pstLeft))
               && (!pstOther->pstRight || RB_IS_BLACK(pstOther->pstRight)))
            {
                RB_SET_RED(pstOther);
                v_pstNode   = v_pstParent;
                v_pstParent = RB_PARENT(v_pstNode);
            }
            else
            {
                if (!pstOther->pstLeft || RB_IS_BLACK(pstOther->pstLeft))
                {
                    register pstRBTREE_NODE_S pstOtherRight;

                    if ((pstOtherRight = pstOther->pstRight))
                    {
                        RB_SET_BLACK(pstOtherRight);
                    }

                    RB_SET_RED(pstOther);
                    rbtreeLeftRotate(pstOther, v_pstRoot);
                    pstOther = v_pstParent->pstLeft;
                }

                RB_SET_COLOR(pstOther, RB_COLOR(v_pstParent));
                RB_SET_BLACK(v_pstParent);

                if (pstOther->pstLeft)
                {
                    RB_SET_BLACK(pstOther->pstLeft);
                }

                rbtreeRightRotate(v_pstParent, v_pstRoot);
                v_pstNode = v_pstRoot->pstNode;

                break;
            }
        }
    }

    if (v_pstNode)
    {
        RB_SET_BLACK(v_pstNode);
    }

    return;
}

static void rbtreeNodeGroupPrint(pstRBTREE_NODE_S v_pstNode)
{
    if (NIL != v_pstNode->pstParent)
    {
        rbtreeNodePrint(v_pstNode->pstParent);
    }

    if (NIL != v_pstNode->pstLeft)
    {
        rbtreeNodePrint(v_pstNode->pstLeft);
    }

    if (NIL != v_pstNode->pstRight)
    {
        rbtreeNodePrint(v_pstNode->pstRight);
    }

    return;
}

void rbtreeMidVisit(pstRBTREE_NODE_S v_pstNode)
{
    rbtreeMidVisitHelper(v_pstNode);

    return;
}

static void rbtreeMidVisitHelper(pstRBTREE_NODE_S v_pstNode)
{
    if (NIL != v_pstNode)
    {
        rbtreeNodeGroupPrint(v_pstNode);

        if (NIL != v_pstNode->pstLeft)
        {
            rbtreeMidVisitHelper(v_pstNode->pstLeft);
        }

        if (NIL != v_pstNode->pstRight)
        {
            rbtreeMidVisitHelper(v_pstNode->pstRight);
        }
    }

    return;
}

RBTREE_NODE_S *rbtreeGetLastNode(pstRBTREE_NODE_S v_pstRoot)
{
    pstRBTREE_NODE_S pstNode = v_pstRoot;

    if (NIL == pstNode)
    {
        return NIL;
    }

    while (pstNode->pstRight)
    {
        pstNode = pstNode->pstRight;
    }

    return pstNode;
}

RBTREE_NODE_S *rbtreeGetPrevNode(pstRBTREE_NODE_S v_pstNode)
{
    pstRBTREE_NODE_S pstParent;
    pstRBTREE_NODE_S pstNode = v_pstNode;

    if (NIL == v_pstNode)
    {
        return NIL;
    }

    if (v_pstNode == v_pstNode->pstParent)
    {
        return NIL;
    }

    /* If we have a left-hand child, go down and then right as far as we can. */
    if (pstNode->pstLeft)
    {
        pstNode = pstNode->pstLeft;

        while (pstNode->pstRight)
        {
            pstNode = pstNode->pstRight;
        }

        return pstNode;
    }

    /*
     * No left-hand children. Go up untill we find an ancestor
     * which is a right-hand child of its parent.
     */
    while ((NIL != (pstParent = pstNode->pstParent)) && (pstNode == pstParent->pstLeft))
    {
        pstNode = pstParent;
    }

    return pstParent;
}


#ifdef __cplusplus
}
#endif


