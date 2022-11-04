git.cmd diff $* -- `git.cmd ls-files | grep -vE "^project|pic32.*X|headers|nichelite"`
