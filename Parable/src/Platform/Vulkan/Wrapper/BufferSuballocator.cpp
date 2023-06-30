#include "BufferSuballocator.h"


namespace Parable::Vulkan
{


// TODO: could be doing this wayy more efficiently but this works for now (probs premature optimize anyway)
//      look at the use of insert_free_range
//      could be improved by taking a hint (e.g. insert_free_range_after / _before) (BufferSuballocator::free)
//      and by another which allows us to move an existing one when the size changes (BufferSuballocator::allocate)
void BufferSuballocator::insert_free_range(BufferFreeRange& free_range)
{
    for (auto it = m_free_ranges.begin(); it != m_free_ranges.end(); ++it)
    {
        if (it->size >= free_range.size)
        {
            m_free_ranges.insert(it, free_range);
            return;
        }
    }

    m_free_ranges.push_back(free_range);
}

BufferSlice BufferSuballocator::allocate(vk::DeviceSize allocation_size)
{
    // traverse the free range list (kept in increasing size order)
    // to find the smallest free range that fits this allocation
    std::vector<BufferFreeRange>::iterator smallest_adequate_free_range = m_free_ranges.end();
    for (auto it = m_free_ranges.begin(); it != m_free_ranges.end(); ++it)
    {
        if (it->size >= allocation_size)
        {
            smallest_adequate_free_range = it;
            break;
        }
    }

    if (smallest_adequate_free_range == m_free_ranges.end()) 
    {
        return BufferSlice{0,0};
    }

    BufferSlice allocation {smallest_adequate_free_range->start, allocation_size};

    if (allocation_size == smallest_adequate_free_range->size)
    {
        m_free_ranges.erase(smallest_adequate_free_range);
    } 
    else
    {   
        BufferFreeRange new_free_range {smallest_adequate_free_range->start + allocation_size, smallest_adequate_free_range->size - allocation_size};
        m_free_ranges.erase(smallest_adequate_free_range);
        insert_free_range(new_free_range);
    }

    return allocation;
}

void BufferSuballocator::free(BufferSlice slice)
{
    // convert the slice to a free range
    BufferFreeRange new_free_range {slice.start,slice.size};

    // see if we can coalesce with some other free range(s)

    for (auto it = m_free_ranges.begin(); it != m_free_ranges.end(); ++it)
    {
        if (it->get_end() == new_free_range.start)
        {
            // 'it' points to a free range directly before new_free_range
            new_free_range.start = it->start;
            new_free_range.size += it->size;
            m_free_ranges.erase(it);
            break;
        }
    }
    
    for (auto it = m_free_ranges.begin(); it != m_free_ranges.end(); ++it)
    {
        if (new_free_range.get_end() == it->start)
        {
            // 'it' points to a free range directly after new_free_range
            new_free_range.size += it->size;
            m_free_ranges.erase(it);
            break;
        }
    }
    
    insert_free_range(new_free_range);
}


}
