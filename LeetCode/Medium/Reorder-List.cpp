# Reorder List

- Platform: LeetCode
- URL: https://leetcode.com/problems/reorder-list/
- Difficulty: Medium
- Language: C++
- Status: Accepted
- Runtime: 0 ms
- Memory: N/A
- Solved At: 2026-05-08T18:43:06.727Z

## Code
```cpp
after = second.next
            second.next = prev
            prev = second
            second = after
        second = prev
        first = head
        while second:
            temp1 = first.next
            temp2 = second.next
            first.next = second
            second.next = temp1
            first = temp1
            second = temp2
```