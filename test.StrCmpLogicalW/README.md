# The bug of StrCmpLogicalW


StrCmpLogicalW is used to compare file names in a folder.


I found this bug a few weeks ago.


Here is the output of this test program and you can find the problem.


```
55.jpg > 3.jpg
ƼƼ.jpg > 3.jpg
6.jpg < 55.jpg
6.jpg > ƼƼ.jpg   <- The problem
55.jpg > 10.jpg
ƼƼ.jpg > 10.jpg
```


In some cases, "ƼƼ" is treated as the number 55, but in others, it is not treated as "55"

