# external_sort
It was a test task for some company. Works with some limitations.

First, it can be made faster by using multiple threads for first stage of sorting, or loading records directly into buffer and sorting in-place.

Second, there is no check for indices in the sorted chunks, so then data in the input file distributed very unevenly output will be composed incorrectly.
