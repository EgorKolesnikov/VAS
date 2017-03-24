name1=13_26_2.0_1.0
echo Running experiment $name1
./admin.sh -r --reparse-wav --norm --nb-mfcc=13 --nb-fbank=26 --frame-window=2.0 --frame-step=1.0 --mode=train
mkdir /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name1}
mv -v /home/kolegor/Code/VAS/data/wav_files_features/* /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name1}/

name2=20_20_2.0_1.0
echo Running experiment $name2
./admin.sh -r --reparse-wav --norm --nb-mfcc=20 --nb-fbank=20 --frame-window=2.0 --frame-step=1.0 --mode=train
mkdir /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name2}
mv -v /home/kolegor/Code/VAS/data/wav_files_features/* /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name2}/

name3=35_25_2.0_1.0
echo Running experiment $name3
./admin.sh -r --reparse-wav --norm --nb-mfcc=35 --nb-fbank=25 --frame-window=2.0 --frame-step=1.0 --mode=train
mkdir /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name3}
mv -v /home/kolegor/Code/VAS/data/wav_files_features/* /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name3}/

name4=13_26_3.5_0.5
echo Running experiment $name4
./admin.sh -r --reparse-wav --norm --nb-mfcc=13 --nb-fbank=26 --frame-window=3.5 --frame-step=0.5 --mode=train
mkdir /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name4}
mv -v /home/kolegor/Code/VAS/data/wav_files_features/* /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name4}/

name5=20_20_3.5_0.5
echo Running experiment $name5
./admin.sh -r --reparse-wav --norm --nb-mfcc=20 --nb-fbank=20 --frame-window=3.5 --frame-step=0.5 --mode=train
mkdir /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name5}
mv -v /home/kolegor/Code/VAS/data/wav_files_features/* /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name5}/

name6=35_25_3.5_0.5
echo Running experiment $name6
./admin.sh -r --reparse-wav --norm --nb-mfcc=35 --nb-fbank=25 --frame-window=3.5 --frame-step=0.5 --mode=train
mkdir /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name6}
mv -v /home/kolegor/Code/VAS/data/wav_files_features/* /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name6}/

name7=30_25_1.0_1.0
echo Running experiment $name7
./admin.sh -r --reparse-wav --norm --nb-mfcc=30 --nb-fbank=25 --frame-window=1.0 --frame-step=1.0 --mode=train
mkdir /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name7}
mv -v /home/kolegor/Code/VAS/data/wav_files_features/* /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name7}/

name8=30_25_3.0_3.0
echo Running experiment $name8
./admin.sh -r --reparse-wav --norm --nb-mfcc=30 --nb-fbank=25 --frame-window=3.0 --frame-step=3.0 --mode=train
mkdir /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name8}
mv -v /home/kolegor/Code/VAS/data/wav_files_features/* /home/kolegor/Code/VAS/tests/parsed_data_for_test/${name8}/