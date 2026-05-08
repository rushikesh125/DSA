# Product of Array Except Self

- Platform: LeetCode
- URL: https://leetcode.com/problems/product-of-array-except-self/submissions/1998142169/
- Difficulty: Medium
- Language: C++
- Status: Accepted
- Runtime: 0 ms
- Memory: 25.64
MB
- Solved At: 2026-05-08T13:14:57.005Z

## Code
```cpp
prefix = 1
        i=0
        prefixarr = []
        while i <len(nums):
            prefixarr.append(prefix)
            prefix*=nums[i]
            i+=1
        
        suffix = 1
        for j in range(len(nums)-1,-1,-1):
            prefixarr[j]*=suffix
            suffix*= nums[j] 
        return prefixarr
```