# Remove Nth Node From End of List

- Platform: LeetCode
- URL: https://leetcode.com/problems/remove-nth-node-from-end-of-list/submissions/1998325484/
- Difficulty: Medium
- Language: C++
- Status: Accepted
- Runtime: 0 ms
- Memory: 19.32
MB
- Solved At: 2026-05-08T17:54:07.310Z

## Code
```cpp
# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, val=0, next=None):
#         self.val = val
#         self.next = next
class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        fast  = head 
        counter = 0
        while counter <n:
            fast = fast.next
            counter +=1
        
        prev = None
        slow = head
        while fast is not None:
```