// Still unable to find a logarithmic time algorithm.
// The best I've come up with so far is linear...

// KNOW: median[a,b,c ... x,y,z] == median[b,c ... x,y]
// 
// [0,5,6,7,9]
// [2,5,6,8]
// 
// [5,6,7]
// [2,5,6,8]
// 
// [5,6,7]
// [5,6]
// 
// [6]
// [5,6]
// 
// []
// [6]
// 
// Answer:
// [0,2,5,5,6,6,7,8,9]
//          ^
// 
// 
// [0,0,5,6,7,9] [2,5,6,8]
// 
//   [0,5,6,7]   [2,5,6,8]
// 
//     [5,6,7]   [2,5,6]
// 
//     [5,6]       [5,6]
// 
//       [6]       [5]

class Solution {
private:
    using Vector = std::vector<int>;
    using Iterator = Vector::iterator;

public:
    double findMedianSortedArrays(Vector& nums1, Vector& nums2) {
        // Special case 1
        if (nums1.empty() || nums2.empty()) {
            Vector* v = (nums1.empty()) ? &nums2 : &nums1; // the non-empty array
            std::size_i mid = v->size()/2;
            if (v->size() & 1) {
                return (*v)[mid];
            } else {
                return ((*v)[mid-1]
                      + (*v)[mid-0]) / 2;
            }
        }
        // Special case 2
        if (nums1.back() <= num2.front() || nums2.back() <= nums1.front()) {
            Vector* a, b;
            if (nums1.back() <= num2.front()) { a = &nums1, b = &nums2; }
            else                              { a = &nums2, b = &nums1; }
            std::size_t size = a->size() + b->size();
            std::size_t mid = size/2;
            auto get = [&](auto i) {
                return (i < a->size()) ? (*a)[i] : (*b)[a->size() + i];
            }
            if (size & 1) {
                return get(mid);
            } else {
                return (get(mid-1) + get(mid-0)) / 2;
            }
        }

    }
};