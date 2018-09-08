

#ifndef RBTREE_H
#define RBTREE_H


#ifdef __cplusplus
extern "C" {
#endif

#define NIL 0

#define RB_PARENT(node)    (node->pstParent)
#define RB_COLOR(node)     (node->enColor)
#define RB_IS_RED(node)    (COLOR_RED == node->enColor)
#define RB_IS_BLACK(node)  (COLOR_BLACK == node->enColor)
#define RB_SET_RED(node)   do{node->enColor = COLOR_RED;}while(0)
#define RB_SET_BLACK(node) do{node->enColor = COLOR_BLACK;}while(0)
#define RB_SET_PARENT(node, parent) do{node->pstParent = parent;}while(0)
#define RB_SET_COLOR(node, color)   do{node->enColor = color;}while(0)

// pointer address, so it can be cast any struct.
typedef unsigned long KEY;

enum NODE_COLOR
{
    COLOR_BLACK = 0,
    COLOR_RED,
    COLOR_BUTT  
};

typedef struct tagRBTREE_NODE_S
{
    struct tagRBTREE_NODE_S *pstParent;
    struct tagRBTREE_NODE_S *pstLeft;
    struct tagRBTREE_NODE_S *pstRight;

    enum NODE_COLOR enColor;
} RBTREE_NODE_S, *pstRBTREE_NODE_S;

typedef struct tagRBTREE_ROOT_S
{
    struct tagRBTREE_NODE_S *pstNode;
} RBTREE_ROOT_S;

#ifdef __cplusplus
}
#endif

#endif /* RBTREE_H */




