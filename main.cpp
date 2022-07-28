#include <iostream>
#include<fstream>
#include <time.h>
#include "Node.h"
#include "Autionmation.h"
#include "EditorConfigTab.h"
#include <string>
#include <vector>
#include <map>
#include <set>
//注意这个头文件
#include <io.h>
#include <numeric>
using namespace std;

void string_replace(string& strBig, const string& strsrc, const string& strdst);   //字符串替换
void getFileNames(string path, vector<string>& files);      //找到路径下的文件

int main(int argc, char* argv[]) {
    //system("chcp 65001");
    std::cout << "\n\n" << endl;

    clock_t starts, ends;
    Automation* automation = new Automation;
    string str;//要匹配的词典
    string filepath;//要匹配的路径
    int num=0;
  
    std::map<std::string, std::set<std::string>> string_id_table;
    std::map<std::string, int> id_linenum_talbe;
    multiset<std::string> exit_id;
  
 
    int counts = 0;
   // cout << "argc >:" << argc << endl;
    for (int i = 0; i < argc - 2; i++)
    {
        ifstream fin(argv[i + 1]);
        string word;
        cout << "argv >: " << argv[i + 1] << endl;
        starts = clock();
        if (fin) {
            while (getline(fin, word)) {
                string_replace(word, "\n", "");
                string_replace(word, "\r", "");

                if (word.find("/") != string::npos)   //如果找到"/"    就截取/ 前得字符串    和/后得字符串
                {
                    auto pos = word.find_first_of("/");   //找到"/"
                    if (pos == string::npos)
                        return 0;
                    auto str1 = word.substr(0, pos);      //截取/前的字符串
                    automation->add(str1);                //添加到   automation里
                    auto str2 = word.substr(pos + 1);     //     /后的字符串
                    automation->add(str2);                //添加到  automation里
                }

                automation->add(word);
                ++counts;
            }
           // cout << "count >:" << counts<<endl;
        }
        else {
            std::cout << "字典读取失败！" << endl;
        }
    }
    automation->build();
    ends = clock();
    std::cout << "创建字典成功！ 字典条目数: " << counts << " 耗时:" << ends - starts << "ms\n" << endl;
    cout << "argv[argc - 1]  >: " << argv[argc - 1] << endl;

    vector<string> fileNames;
   /* cout << "请输入要匹配的文件 -> :";
    cin >> filepath;
    cout << endl;*/
    string path(/*"E:\\Parsing_the_text\\Parsing_the_text\\Parsing_the_text\\lang"*/argv[argc-1]); 	//自己选择目录测试

    getFileNames(path, fileNames);

    for (auto& ph : fileNames) {  //遍历要匹配的目录

   /* string buf("lang_items_th.txt");
    automation->readTab(buf);*/

    automation->readTab(ph);
    cout << "ph==" << ph.c_str() << endl;

    }
    delete automation;
    return 0;
}




//字符串替换
void string_replace(string& strBig, const string& strsrc, const string& strdst)  //word   \n  " "
{
    string::size_type pos = 0;
    string::size_type srclen = strsrc.size();
    string::size_type dstlen = strdst.size();

    while ((pos = strBig.find(strsrc, pos)) != string::npos)
    {
        strBig.replace(pos, srclen, strdst);////从第一个pos位置开始的srclrn个字符替换成strdst
        pos += dstlen;
    }
    pos = 0;
}



//核心代码        //找到路径下的文件
/*
遍历路径下的文件夹以及文件
*/
void getFileNames(string path, vector<string>& files)
{
    //文件句柄
    //注意：我发现有些文章代码此处是long类型，实测运行中会报错访问异常
    intptr_t hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //如果是目录,递归查找
            //如果不是,把文件绝对路径存入vector中
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    getFileNames(p.assign(path).append("\\").append(fileinfo.name), files);
            }
            else
            {
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

