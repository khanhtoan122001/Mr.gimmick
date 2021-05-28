#include "Quadtree.h"
void Quadtree::Clear()
{
    // Clear all nodes
    if (m_nodes)
    {
        for (int i = 0; i < 4; i++)
        {
            m_nodes[i]->Clear();
            delete m_nodes[i];
        }
        delete[] m_nodes;
    }

    // Clear current Quadtree
    m_objects_list->clear();

    delete m_objects_list;
    delete m_region;
}

bool Quadtree::IsContain(LPGAMEOBJECT entity)
{
    Rect* bound = new Rect(entity->GetBoundingBox());

    return !(bound->tf.x + bound->width() < m_region->tf.x ||
        bound->tf.y + bound->height() < m_region->tf.y ||
        bound->tf.x > m_region->tf.x + m_region->width() ||
        bound->tf.y > m_region->tf.y + m_region->height());
}

void Quadtree::Split()
{
    m_nodes = new Quadtree * [4];

    m_nodes[0] = new Quadtree(m_level + 1,
        new Rect(m_region->tf, m_region->width() / 2, m_region->height() / 2));
    m_nodes[1] = new Quadtree(m_level + 1,
        new Rect(Point(m_region->tf.x + m_region->width() / 2, m_region->tf.y), m_region->width() / 2, m_region->height() / 2));
    m_nodes[2] = new Quadtree(m_level + 1,
        new Rect(Point(m_region->tf.x, m_region->tf.y + m_region->height() / 2), m_region->width() / 2, m_region->height() / 2));
    m_nodes[3] = new Quadtree(m_level + 1,
        new Rect(Point(m_region->tf.x + m_region->width() / 2,
            m_region->tf.y + m_region->height() / 2), m_region->width() / 2, m_region->height() / 2));
}

Quadtree::Quadtree(int level, Rect* region)
{
    Quadtree::Quadtree();
    m_level = level;
    m_region = region;
    m_objects_list = new vector<LPGAMEOBJECT>();
}

void Quadtree::Insert(LPGAMEOBJECT entity)
{
    // Insert entity into corresponding nodes
    if (m_nodes)
    {
        for (int i = 0; i < MAX_OBJECT_IN_REGION; i++) {
            if (m_nodes[i]->IsContain(entity))
                m_nodes[i]->Insert(entity);
        }

        return; // Return here to ignore rest.
    }

    // Insert entity into current quadtree
    if (this->IsContain(entity))
        m_objects_list->push_back(entity);

    // Split and move all objects in list into it’s corresponding nodes
    if (m_objects_list->size() > MAX_OBJECT_IN_REGION && m_level < MAX_LEVEL)
    {
        Split();

        while (!m_objects_list->empty())
        {
            for(int i = 0; i < MAX_OBJECT_IN_REGION; i++)
                if (m_nodes[i]->IsContain(m_objects_list->back()))
                    m_nodes[i]->Insert(m_objects_list->back());

            m_objects_list->pop_back();
        }
    }
}

void Quadtree::Retrieve(vector<LPGAMEOBJECT>* return_objects_list, LPGAMEOBJECT entity)
{
    if (m_nodes)
    {
        for (int i = 0; i < MAX_OBJECT_IN_REGION; i++)
        if (m_nodes[i]->IsContain(entity))
            m_nodes[i]->Retrieve(return_objects_list, entity);

        return; // Return here to ignore rest.
    }

    // Add all entities in current region into return_objects_list
    if (this->IsContain(entity))
    {
        for (auto i = m_objects_list->begin(); i != m_objects_list->end(); i++)
        {
            if (entity != *i)
                return_objects_list->push_back(*i);
        }
    }
}

