docker run -t --rm -P -v `pwd`:/home/dealii/deal2lkit:rw dealii/dealii:master-focal /bin/sh -c "cd deal2lkit; $@"
