text CC = "g++" ;

text VMDEP = "D:/active/home/bin/CCore-VMakeDep.exe" ;

text OBJ_PATH = ".obj" ;

text TARGET = "./main.exe" ;

text DEP = OBJ_PATH+'/deps.vm.ddl' ;

Target obj = { 'obj' , OBJ_PATH+'/empty' } ;

Rule robj = { {} , {&obj} , {&intobj1,&intobj2} } ;

IntCmd intobj1 = { 'MKDIR' , &mkdir1 } ;

Mkdir mkdir1 = { { OBJ_PATH } } ;

IntCmd intobj2 = { 'ECHO' , &echo1 } ;

Echo echo1 = { { 'empty' } , OBJ_PATH+'/empty' } ;

Target clean = { 'clean' } ;

Rule rclean = { {} , {&clean} , {&intclean} } ;

IntCmd intclean = { 'CLEAN' , &rm1 } ;

Rm rm1 = { { TARGET , OBJ_PATH+'/*' } } ;

Target clean_dep = { 'clean dep' } ;

Rule rclean_dep = { {} , {&clean_dep} , {&intclean_dep} } ;

IntCmd intclean_dep = { 'CLEAN DEP' , &rm2 } ;

Rm rm2 = { { DEP , OBJ_PATH+'/*.dep' } } ;

Target clean_vdep = { 'clean vdep' } ;

Rule rclean_vdep = { {} , {&clean_vdep} , {&intclean_vdep} } ;

IntCmd intclean_vdep = { 'CLEAN VDEP' , &rm3 } ;

Rm rm3 = { { DEP , OBJ_PATH+'/*.vm.dep' } } ;

Target cpp1 = { "main.cpp" , "./main.cpp" } ;
Target dcpp1 = { "main.dep" , OBJ_PATH+"/main.dep" } ;
Rule rdcpp1 = { {&cpp1,&obj} , {&dcpp1} , {&execpp1} } ;
Exe execpp1 = { "CC-DEP main.cpp" , CC , {
  "-I../../Target/WIN32"
 ,"-I../../HCore"
 ,"-I../../Simple"
 ,"-I../../Fundamental"
 ,"-I../../Applied"
 ,"-I../../Desktop/Core"
 ,"-I../../Desktop/Draw"
 ,"-I../../Desktop/Font"
 ,"-I../../Desktop/Lib"
 ,"-I../../Desktop/Tools"
 ,"-I../../Desktop/App"
 ,"-I."
 ,"-MM"
 ,"-MT"
 ,OBJ_PATH+"/main.o"
 ,"./main.cpp"
 ,"-MF"
 ,OBJ_PATH+"/main.dep"
} } ;

Target vdcpp1 = { "main.vm.dep" , OBJ_PATH+"/main.vm.dep" } ;
Rule rvdcpp1 = { {&dcpp1} , {&vdcpp1} , {&exedcpp1} } ;
Exe exedcpp1 = { "CC-VM-DEP main.cpp" , VMDEP , { OBJ_PATH+"/main.dep" , OBJ_PATH+"/main.vm.dep" , '1' } } ;

Target make_dep = { 'make_dep' , DEP } ;

Rule rmkdep = { {&obj
,&vdcpp1} , {&make_dep} , {&intmkdep} } ; 

IntCmd intmkdep = { 'CAT' , &cat1 } ;

Cat cat1 = { { 
 vdcpp1.file } , DEP } ;

