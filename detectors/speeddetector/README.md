#Objective
The objective is to detect "fast-changing sites" 
#Definition of "fast-changing"
Fast-changing is defined as matching the following conditions:
1. for at least a given interval, the value moves upwards with each step
2. in each step growing at least a given amount 
#Fast-change-detector
##Input
1. Series of frames (format: unnumbered lists of each frame split by an empty row)
2. interval (in number of samples)
3. growth
##Output
A list of sites, which at any time matched the conditions of "fast-growing" (see defintion above)