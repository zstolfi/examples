/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode* next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode* next) : val(x), next(next) {}
 * };
 */
class Solution {
private:
	struct List {
		ListNode *begin, *end;
		List() : begin(nullptr), end(nullptr) {}
		List(ListNode* l) : begin(l), end(l) { if (end) while(end->next) end = end->next; }
		void push(int val) {
			if (!begin) { end = begin     = new ListNode(val); }
			else        { end = end->next = new ListNode(val); }
		}
		int pop() {
		if (!begin) { return {}; }
		int result = begin->val;
		begin = begin->next;
		return result;
		}
	};

public:
	ListNode* mergeKLists(std::vector<ListNode*>& lists) {
		List result;
		// ensure there are no null elements before we start
		for (auto it=lists.begin(); it!=lists.end(); /**/) {
			if (*it == nullptr) { it = lists.erase(it); }
			else { ++it; }
		}

		while (!lists.empty()) {
			auto it = std::min_element(lists.begin(), lists.end(), [](auto* a, auto* b) {
				if (!a) { return false; } // ignore 'a' if it's null
				if (!b) { return true ; } // ignore 'b' if it's null
				return (*a).val < (*b).val;
			});
			result.push((*it)->val);
			(*it) = (*it)->next;
			// remove null elements so we don't have to loop over them later
			if (*it == nullptr) { lists.erase(it); }
		}
		return result.begin;
	}
};