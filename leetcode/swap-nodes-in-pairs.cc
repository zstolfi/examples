class Solution1 {
public:
	ListNode *swapPairs(ListNode *head) {
		ListNode *cursor   = head
		,        *prev     = nullptr
		,        *prevprev = nullptr
		,        *result   = nullptr;

		enum : bool { Even, Odd } state = Even;
		auto nextState = [&state] { state = (state==Even)? Odd : Even; };

		while (cursor != nullptr) {
			nextState();
			if (state == Odd) {
				if (cursor->next == nullptr) break;
				prevprev = prev;
				prev = cursor;
				cursor = cursor->next;
			}
			else if (state == Even) {
				if (result == nullptr) result = cursor;
				if (prevprev != nullptr) prevprev->next = cursor;
				prev->next = cursor->next;
				cursor->next = prev;
				cursor = cursor->next->next;
			}
		}

		return result ? result : head;
	}
};



class Solution2 {
public:
	ListNode *swapPairs(ListNode *head) {
		std::vector<ListNode *> ptrs;
		for(ListNode *n = head; n != nullptr; n = n->next) {
			ptrs.push_back(n);
		}
		
		// value of ptr[i]->next changes as a function of i and N
		// .
		// . 0
		// 2 0 .
		// 3 0 . 2
		// 3 0 4 2 .
		// 3 0 5 2 . 4
		// 3 0 5 2 6 4 .
		// 3 0 5 2 7 4 . 6
		//     etc ...
		const std::size_t N = ptrs.size();
		for (std::size_t i=0; i<N; i++) {
			ptrs[i]->next = (i % 2 == 1) ? ptrs[i-1]
			:               (N - i <= 2) ? nullptr
			:               (N - i == 3) ? ptrs[N-1]
			:              /*i % 2 == 0*/  ptrs[i+3];
		}
		return (ptrs.size() == 0) ? nullptr
		:      (ptrs.size() == 1) ? ptrs[0]
		:     /*ptrs.size() >= 2*/  ptrs[1];
	}
};