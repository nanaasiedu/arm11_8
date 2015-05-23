# arm11_8
C Project for Imperial Computing Summer Term - Year 1

###TODO

### Git for groups stuff

> git diff --cached <file name>
    shows differences in a file and the version on the repository

#### After a git fetch, to check if an automatic merge will be valid

> git log -p origin/master
    will give you a log of changes with all diffs that occur

> git log --stats
    gives statistics on the changes on files

> git blame --date=short <file name>
    you can see who did what at what time
    Useful if you need to understand a bit of code about someone

> git cherry-pick <commit name>
    only merge certain commit to your file

> git add -p
    interactively add chunks to the index
    i.e. add only certain chunks of code in a merge

#### Branches

In general, people commit on their own branches, then merge with master
Branching can also be used for - bug fixing
                               - new experimental features

> git checkout -b <new branch name>
    creates new branch

> git branch -a
    shows all branches

> git checkout <branch name>
    changes branch currently operating on

This is helpful if you just want to reset to a last checkpoint so to speak

> git reset --hard HEAD
    takes your branch back to the last commit
