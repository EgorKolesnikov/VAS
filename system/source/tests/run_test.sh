# echo 'SYS: Compiling wav_file.cpp'
# g++ -std=c++11 -c ../wav_file.cpp -o ../object_files/wav_file.o

# echo 'SYS: Compiling wav_python_work.cpp'
# g++ -std=c++11 -c ../wav_python_work.cpp -o ../object_files/wav_python_work.o

# echo 'SYS: Compiling test_auth_kernel.cpp'
# g++ -std=c++11 -c test_auth_kernel.cpp -o ../object_files/test_auth_kernel.o

# echo 'SYS: Compiling test_mfcc.cpp'
# g++ -std=c++11 -c test_mfcc.cpp -o ../object_files/test_mfcc.o

# echo 'SYS: Linking first test...'
# g++ -std=c++11 ../object_files/wav_file.o ../object_files/wav_python_work.o ../object_files/test_auth_kernel.o -lpython2.7 -lm -lboost_regex -lboost_filesystem -lboost_system -o a.out

# echo 'SYS: Done! Running script.'
# ./a.out

# echo 'SYS: Linking second test...'
# g++ -std=c++11 ../object_files/wav_file.o ../object_files/wav_python_work.o ../object_files/test_mfcc.o -lpython2.7 -lm -lboost_regex -lboost_filesystem -lboost_system -o a.out

# echo 'SYS: Done! Running script.'
# ./a.out

echo 'SYS: Running first test...'
g++ -std=c++11 test_auth_kernel.cpp -lpython2.7 -lm -lboost_regex -lboost_filesystem -lboost_system -o a.out
./a.out

echo 'SYS: Running second test...'
g++ -std=c++11 test_mfcc.cpp -lpython2.7 -lm -lboost_regex -lboost_filesystem -lboost_system -o a.out
./a.out
