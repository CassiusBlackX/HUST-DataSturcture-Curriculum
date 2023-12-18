#include "printfile.h"
status PrintFile(ifstream & fp){
    int indentnum = 0,line = 1;
    char c;
    string filename = "../output/C_print_file.txt";
    ofstream print_fp(filename);
    while(!printList.empty()){
        indentnum = printList.front().indent;
        printList.pop();
        while(!printList.empty() && line==printList.front().linenum) printList.pop();
        while(!printList.empty() && line<printList.front().linenum){
            for(int i=0;i<indentnum;i++){
                print_fp<<"\t";
            }
            while(fp.get(c) && c!='\n'){
                print_fp<<c;
            }
            print_fp<<c;
            line++;
        }
    }
    while(fp.get(c) && c!=EOF){
        print_fp<<c;
    }
    print_fp.close();
    return OK;
}