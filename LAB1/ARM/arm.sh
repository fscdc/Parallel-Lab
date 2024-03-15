echo 'N,common_algo,optimized_algo'
for i in {1..90}
do 
    g++ -g -DN=$[16*i] ./1-arm.cpp -o ./1
    ./1
done