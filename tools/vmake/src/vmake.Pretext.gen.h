" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
" \n"
"\n"
"struct Target\n"
" {\n"
"  text desc;\n"
"  text file = null ;\n"
" };\n"
"\n"
"struct Env\n"
" {\n"
"  text name;\n"
"  text value;\n"
" };\n"
"\n"
"struct Exe\n"
" {\n"
"  text echo;\n"
"  text exe;\n"
"  text[] args;\n"
"  text wdir;\n"
"  Env[] env;\n"
" };\n"
"\n"
"struct Cmd\n"
" {\n"
"  text echo;\n"
"  text cmdline;\n"
"  text wdir;\n"
"  Env[] env;\n"
" };\n"
"\n"
"struct VMake\n"
" {\n"
"  text echo;\n"
"  text file;\n"
"  text target = 'main' ;\n"
"  text wdir;\n"
" };\n"
"\n"
"struct Echo\n"
" {\n"
"  text[] strs;\n"
"  text outfile;\n"
" };\n"
"\n"
"struct Cat\n"
" {\n"
"  text[] files;\n"
"  text outfile;\n"
" };\n"
"\n"
"struct Rm\n"
" {\n"
"  text[] files;\n"
" };\n"
"\n"
"struct Mkdir\n"
" {\n"
"  text[] paths;\n"
" };\n"
"\n"
"struct IntCmd\n"
" {\n"
"  text echo;\n"
"  {Echo,Cat,Rm,Mkdir} *cmd;\n"
"  text wdir;\n"
" };\n"
"\n"
"struct Rule\n"
" {\n"
"  Target * [] src;\n"
"  Target * [] dst;\n"
"  {Exe,Cmd,VMake,IntCmd} * [] cmd;\n"
" };\n"
"\n"
"struct Dep\n"
" {\n"
"  Target * [] src;\n"
"  Target * [] dst;\n"
" };\n"
"\n"
"struct MenuItem\n"
" {\n"
"  Target *dst;\n"
"  text desc;\n"
"\n"
"  MenuItem * [] sub = {} ;\n"
" };\n"
"\n"
"struct Menu\n"
" {\n"
"  MenuItem[] list;\n"
" };\n"
"\n"
