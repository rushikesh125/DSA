# Merge Two Sorted Lists

- Platform: LeetCode
- URL: https://leetcode.com/problems/merge-two-sorted-lists/submissions/1998137242/
- Difficulty: Unknown
- Language: C++
- Status: Accepted
- Runtime: 0
ms
- Memory: 19.28
MB
- Solved At: 2026-05-08T13:06:51.083Z

## Code
```cpp
# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, val=0, next=None):
#         self.val = val
#         self.next = next
class Solution:
    def mergeTwoLists(self, list1: Optional[ListNode], list2: Optional[ListNode]) -> Optional[ListNode]:
        first = list1
        second = list2
        dummy = ListNode()
        tail = dummy
        while first is not None and second is not None:
            if first.val <= second.val:
                tail.next = first
                first =first.next
```