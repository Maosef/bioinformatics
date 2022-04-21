
g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib make_input.cpp -o make_input -lsdsl -ldivsufsort -ldivsufsort64
# g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib buildsa.cpp -o buildsa -lsdsl -ldivsufsort -ldivsufsort64
# g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib querysa.cpp -o querysa -lsdsl -ldivsufsort -ldivsufsort64

# ./make_input data/celegans.fa

# vary query length
length=1000000
for query_length in 11 21 31
do
    ./make_input data/celegans.fa ${query_length}


    for length in 1000 10000 100000 1000000 10000000
    do

        # ./buildsa ${length}.fa ${length}.fa.sa
        ./buildsa ${length}.fa ${length}.fa.sa
        # ./buildsa --preftab ${k} ${length}.fa ${length}.fa.sa
        ./querysa ${length}.fa.sa ${length}.queries.fa naive ${length}.out

    done

done


# vary k for preftable
for k in 0 4 8 10
do

    for length in 1000 10000 100000 1000000 10000000
    do
        # ./buildsa ${length}.fa ${length}.fa.sa
        # ./buildsa ${length}.fa ${length}.fa.sa
        ./buildsa --preftab ${k} ${length}.fa ${length}.fa.sa
        ./querysa ${length}.fa.sa ${length}.queries.fa naive ${length}.out

    done

done


# for length in 1000 10000 100000 1000000 10000000
# do
#     # ./buildsa ${length}.fa ${length}.fa.sa
#     ./buildsa --preftab 6 ${length}.fa ${length}.fa.sa

#     ./querysa ${length}.fa.sa ${length}.queries.fa naive ${length}.out

# done

# for length in 1000 10000 100000 1000000 10000000
# do
#     # ./buildsa --preftab 8 ${length}.fa ${length}.fa.sa
#     ./querysa ${length}.fa.sa ${length}.queries.fa simpaccel ${length}.out

#     # ./querysa ${length}.fa.sa ${length}.queries.fa naïve ${length}.out

# done