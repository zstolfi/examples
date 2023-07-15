class Solution {
private:
    //                      ┌─ begin ─┐  ┌── end ──┐
    using Group = std::pair<std::size_t, std::size_t>;
    std::list<Group> groups {};
    std::size_t size;

    bool hasGroupOfSize(int m) {
        for (auto [i0, i1] : groups)
            if (i1-i0 + 1 == m) { return true; }
        return false;
    }

    void addBitToGroups(std::size_t i) {
        auto it = groups.begin();
        while (it!=groups.end() && it->second < i) { ++it; } 
        auto prev = std::prev(it), next = it;
        bool mergeLeft  = (it != groups.begin() && prev->second == i-1);
        bool mergeRight = (it != groups.end  () && next->first  == i+1);

        /**/ if (!mergeLeft && !mergeRight) { groups.insert(it, {i,i}); }
        else if ( mergeLeft && !mergeRight) { prev->second++; }
        else if (!mergeLeft &&  mergeRight) { next->first--; }
        else if ( mergeLeft &&  mergeRight) {
            prev->second = next->second;
            groups.erase(next);
        }
    }

public:
    int findLatestStep(std::vector<int>& arr, int m) {
        size = arr.size();
        int result = -1, step = 1;
        for (int i : arr) {
            addBitToGroups(i);
            if (hasGroupOfSize(m))
                result = step;
            step++;
        }
        return result;
    }
};