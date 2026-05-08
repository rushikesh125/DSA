# Linked List Cycle

- Platform: LeetCode
- URL: https://leetcode.com/problems/linked-list-cycle/description/
- Difficulty: Easy
- Language: C++
- Status: Accepted
- Runtime: N/A
- Memory: N/A
- Solved At: 2026-05-08T13:09:24.326Z

## Code
```cpp
#         self.next = None
class Solution:
    def hasCycle(self, head: Optional[ListNode]) -> bool:
        slow = head
        fast = head
        while fast is not None and fast.next is not None:
            fast = fast.next.next
            slow = slow.next
            if fast == slow:
                return True
        return False
```