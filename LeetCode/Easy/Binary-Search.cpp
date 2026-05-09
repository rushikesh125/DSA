# Binary Search

- Platform: LeetCode
- URL: https://leetcode.com/problems/binary-search/submissions/1998722683/
- Difficulty: Easy
- Language: C++
- Status: Accepted
- Runtime: 0 ms
- Memory: 20.46
MB
- Solved At: 2026-05-09T10:03:16.451Z

## Code
```cpp
class Solution:
    def binSearch(self,nums,start,end,target)
:
        if start>end:
            return -1
        mid  = (start+end)//2
        if(nums[mid]==target):
            return mid
        elif nums[mid]<target:
            return self.binSearch(nums,mid+1,
end,target)
        else:
            return self.binSearch(nums,start,
mid-1,target)
    def search(self, nums: List[int],
```