text OBJ_PATH = ".obj" ;

text CC = "g++" ;

text AS = "as" ;

text LD = "g++" ;

text AR = "ar" ;

text TARGET = "./main.exe" ;

Target cpp1 = { "main.cpp" , "./main.cpp" } ;
Target ocpp1 = { "main.o" , OBJ_PATH+"/main.o" } ;
Rule rcpp1 = { {&cpp1} , {&ocpp1} , {&intdep1,&execpp1} } ;
Exe execpp1 = { "CC main.cpp" , CC , {
  "-c"
 ,"-std=c++20"
 ,"-fcoroutines"
 ,"-fwrapv"
 ,"-fconcepts-diagnostics-depth=3"
 ,"-O3"
 ,"-march=ivybridge"
 ,"-mmmx"
 ,"-msse"
 ,"-msse2"
 ,"-Wall"
 ,"-Wextra"
 ,"-Wno-non-virtual-dtor"
 ,"-Wno-switch"
 ,"-Wno-type-limits"
 ,"-Wno-enum-compare"
 ,"-Wno-missing-field-initializers"
 ,"-Wno-delete-non-virtual-dtor"
 ,"-Wno-misleading-indentation"
 ,"-I../../Target/WIN32"
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
 ,"./main.cpp"
 ,"-o"
 ,OBJ_PATH+"/main.o"
} } ;

IntCmd intdep1 = { 'RM DEP' , &rmdep1 } ;

Rm rmdep1 = { { OBJ_PATH+"/main.dep" } } ;

text ARGS = '@'+OBJ_PATH+'/target.args' ;

IntCmd intargs = { 'ARGS' , &echoargs } ;

Echo echoargs = { { 
 '"'+ocpp1.file+"\"\n" } , OBJ_PATH+'/target.args' } ;

Target main = { 'main' , TARGET } ;

Rule rmain = { { core_ptr 
,&ocpp1 } , {&main} , {&intargs,&exemain} } ;

Exe exemain = { 'LD '+TARGET , LD , {
  ARGS
 ,"-Wl,-s"
 ,"../../Target/WIN32/CCore.a"
 ,"-lws2_32"
 ,"-lgmp"
 ,"-lgdi32"
 ,"-lfreetype"
 ,"-o"
 ,TARGET
} } ;

Target core = { 'CCore' , "../../Target/WIN32/CCore.a" } ;

Target *core_ptr = &core ;

include <.obj/deps.vm.ddl>
