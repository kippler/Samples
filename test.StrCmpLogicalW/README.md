# The bug of StrCmpLogicalW


StrCmpLogicalW is used to compare file names in a folder.


I found this bug a few weeks ago.


Here is the output of this test program and you can find the problem.


```
55.jpg > 3.jpg
ƼƼ.jpg > 3.jpg
6.jpg < 55.jpg
6.jpg > ƼƼ.jpg   <- The probme.
55.jpg > 10.jpg
ƼƼ.jpg > 10.jpg
```


"ƼƼ" (not 55) treated as number 55 is some case but its not treated as 55 in some cases.
