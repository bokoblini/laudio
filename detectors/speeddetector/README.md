# Objective

The objective is to detect "fast-changing samples" 

# Definition of "fast-samples"

Fast-changing is defined as matching the following conditions:
1. for at least a given interval in time, the value moves upwards with each step
2. in each step growing at least a given amount 

# Fast-change-detector

## Input

1. Series of frames (format: unnumbered lists of each frame split by an empty
   row)
2. N: number of frames after which a growing sample is considered fast-changing
3. g: minimum frame to frame growth of a sample to be considered fast-changing

## Output

A list of locations, which at any time matched the conditions of "fast-growing" (see
defintion above)

## Logic

The Algorithm recieves a small number of Frames, within a given Timespan dt
before the passing moment.

A sample is considered either fast changing or not for the entire dt. If within
dt, there was a continuous series of frame to frame growth of a given sample for
at least N frames, then the sample is considered fast-changing for the entire
dt.