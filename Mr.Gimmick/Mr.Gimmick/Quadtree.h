#pragma once
#include "GameObject.h"
#include "Rect.h"
#include "Game.h"
#include "Nakiri.h"

#define MAX_LEVEL                25
#define MAX_OBJECT_IN_REGION     4

class Quadtree
{
private:
    int m_level;
    Rect* m_region;
    vector<LPGAMEOBJECT>* m_objects_list;
    Quadtree** m_nodes;

    bool IsContain(LPGAMEOBJECT entity);
    void Split();
public:
    Quadtree() { m_level = 0; m_region = new Rect(); m_objects_list = NULL; m_nodes = NULL; };
    Quadtree(int level, Rect* region);
    ~Quadtree() {};

    void Clear();
    void Insert(LPGAMEOBJECT entity);
    void Retrieve(vector<LPGAMEOBJECT>* return_objects_list, LPGAMEOBJECT entity);
};
