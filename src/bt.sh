g++ -O3  -Wall -Wextra -Wconversion -Werror test_kdtree.c && time ./a.out <in.txt | diff /dev/stdin out.txt
g++ -O3  -Wall -Wextra -Wconversion -Werror test_triangular.c && time ./a.out
