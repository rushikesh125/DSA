# Reorder List

- Platform: LeetCode
- URL: https://leetcode.com/problems/reorder-list/
- Difficulty: Medium
- Language: C++
- Status: Accepted
- Runtime: 0 ms
- Memory: 27.89
MB
- Solved At: 2026-05-08T18:50:00.078Z

## Code
```cpp
second = prev
        first = head
        while second:
            temp1 = first.next
            temp2 = second.next
            first.next = second
            second.next = temp1
            first = temp1 
            second = temp2 
            
        
            second = after 
            prev =second
            second.next = prev
```