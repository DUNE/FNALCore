FNAL fhicl-cpp C++ Library Binding
==================================

LBNE's github "mirror" of the [FNAL fhicl-cpp Library](https://cdcvs.fnal.gov/redmine/projects/fhicl-cpp/).
This repo is not a true mirror as we keep the option open to add
branches for fixes to propose upstream to FNAL. A dedicated branch
`cdcvs/master` is used to track and retain a pristine version of
the authoratative FNAL master branch. This should not be committed
other than to merge in changes from upstream.

The primary purpose of this repo is as a holding pen for code
to be imported via subtree to the LBNE/FNALCore project. Because
patches are needed, these can be pushed from the subtree back here
and then submitted upstream easily.

Git Workflow for Following FNAL
-------------------------------
Because this github repo shares history with the FNAL upstream,
there are several ways to keep things in sync (from FNAL to
github). This example uses the github repo as a starting point

```sh
$ git clone git@github.com:LBNE/fnal-fhicl-cpp.git
...
$ git remote add -f cdcvs https://cdcvs.fnal.gov/projects/fhicl-cpp
...
$ git branch -a
* master
 remotes/cdcvs/feature/TopLevelPSinReg
 remotes/cdcvs/master
 remotes/cdcvs/v2_19_01_branch
 remotes/cdcvs/v2_19_04_branch
 remotes/origin/HEAD -> origin/master
 remotes/origin/cdcvs/master
 remotes/origin/master
```

Now we create a local branch (here we just name it `localcdcvs` for clarity)
to track the FNAL master branch, then check it out and pull changes.

```sh
$ git branch --track localcdcvs cdcvs/master
...
$ git checkout localcdcvs
$ git pull
$ git push origin localcdcvs:cdcvs/master
```

The final command here pushes the changes to the `cdcvs/master` branch on
the github remote that is used to store the pristine sources

The github master branch itself is used to patch in changes that are needed
to fix portability issues. However, it should in the first instance be
merged with upstream changes so that these are ported across. To do this,
after you have pulled in upstream changes, do:

```sh
$ git checkout master
$ git merge localcdcvs
... fix any conflicts, commit...
$ git push origin master
```

