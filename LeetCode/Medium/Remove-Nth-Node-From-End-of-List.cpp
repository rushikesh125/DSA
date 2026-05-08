# Remove Nth Node From End of List

- Platform: LeetCode
- URL: https://leetcode.com/problems/remove-nth-node-from-end-of-list/
- Difficulty: Medium
- Language: C++
- Status: Accepted
- Runtime: 0 ms
- Memory: N/A
- Solved At: 2026-05-08T17:46:16.355Z

## Code
```cpp
slow = head
        prev = None
        
        while fast is not None :
            prev = slow
            fast = fast.next
            slow = slow.next
        if prev is None:
            return head.next
            counter+=1
            fast = fast.next
        while  counter <n:
        counter = 0
        fast = head
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
class Solution:
#         self.next = next
#         self.val = val
```