#pragma once

#include <vector>

#include "debug.hpp"


template <typename T>
class SolidVector
{
    using ID = uint32_t;
protected:
    void assertId(ID id) const
    {
        DEV_ASSERT(id < m_forwardMap.size());
        DEV_ASSERT(m_forwardMap[id].occupied);
    }

public:

    struct ForwardIndex
    {
        uint32_t index;
        bool occupied;
    };

    bool occupied(ID id) const { DEV_ASSERT(id < m_forwardMap.size()); return m_forwardMap[id].occupied; }

    uint32_t size() const { return uint32_t(m_data.size()); }

    const T* data() const { return m_data.data(); }
    T* data() { return m_data.data(); }

    const T& at(uint32_t index) const { DEV_ASSERT(index < m_data.size());  return m_data[index]; }
    T& at(uint32_t index) { DEV_ASSERT(index < m_data.size());  return m_data[index]; }

    const T& operator[](ID id) const { assertId(id); return m_data[m_forwardMap[id].index]; }
    T& operator[](ID id) { assertId(id); return m_data[m_forwardMap[id].index]; }

    ID insert(const T& value)
    {
        ID id = m_nextUnused;
        DEV_ASSERT(id <= m_forwardMap.size());

        if (id == m_forwardMap.size())
            m_forwardMap.push_back({ static_cast<uint32_t>(m_forwardMap.size()) + 1, false});

        ForwardIndex& forwardIndex = m_forwardMap[id];
        DEV_ASSERT(!forwardIndex.occupied);

        m_nextUnused = forwardIndex.index;
        forwardIndex = { static_cast<uint32_t>(m_data.size()), true };

        m_data.emplace_back(value);
        m_backwardMap.emplace_back(id);

        return id;
    }

    void erase(ID id)
    {
        DEV_ASSERT(id < m_forwardMap.size());

        ForwardIndex& forwardIndex = m_forwardMap[id];
        DEV_ASSERT(forwardIndex.occupied);

        m_data[forwardIndex.index] = std::move(m_data.back());
        m_data.pop_back();

        ID backwardIndex = m_backwardMap.back();

        m_backwardMap[forwardIndex.index] = backwardIndex;
        m_backwardMap.pop_back();

        m_forwardMap[backwardIndex].index = forwardIndex.index;

        forwardIndex = { m_nextUnused, false };
        m_nextUnused = id;
    }

    void clear()
    {
        m_forwardMap.clear();
        m_backwardMap.clear();
        m_data.clear();
        m_nextUnused = 0;
    }

    std::vector<T> m_data;
    std::vector<ForwardIndex> m_forwardMap;
    std::vector<ID> m_backwardMap;

    ID m_nextUnused = 0;
};