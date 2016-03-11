# Cache-Miss-Estimator
Integrated Cache Miss Estimator for a multi level cache. Predicts whether a given block is present in the cache or not with a notion of confidence.
* L1 is a 4KB cache with (C, B, S) as (12, 6, 3)
* L2 is a 32KB cache with (C, B, S) as (15, 6, 5)
* L3 is a 256KB cache with (C, B, S) as (18, 6, 5)


Usage
=====
Run the experiments using Python script 
This opens a GUI which allows for a better user experience. Enter all the arguments and hit the 'Experiments' button.
```
$ python script.py 
```
or
```
$ ./script.py
```


Project updates
===============
v1.2 [2015/03/05] First commit with poor results for L3.

v2.0 [2015/04/21] Final miss estimator with separate cpp files to generate prediction.

v3.0 [2015/08/09] Added Tkinter GUI for better user experience. Also added Report and PPT.