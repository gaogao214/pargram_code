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
//ע�����ͷ�ļ�
#include <io.h>
#include <numeric>
using namespace std;

void string_replace(string& strBig, const string& strsrc, const string& strdst);   //�ַ����滻
void getFileNames(string path, vector<string>& files);      //�ҵ�·���µ��ļ�

int main(int argc, char* argv[]) {
    //system("chcp 65001");
    std::cout << "\n\n" << endl;

    clock_t starts, ends;
    Automation* automation = new Automation;
    string str;//Ҫƥ��Ĵʵ�
    string filepath;//Ҫƥ���·��
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

                if (word.find("/") != string::npos)   //����ҵ�"/"    �ͽ�ȡ/ ǰ���ַ���    ��/����ַ���
                {
                    auto pos = word.find_first_of("/");   //�ҵ�"/"
                    if (pos == string::npos)
                        return 0;
                    auto str1 = word.substr(0, pos);      //��ȡ/ǰ���ַ���
                    automation->add(str1);                //��ӵ�   automation��
                    auto str2 = word.substr(pos + 1);     //     /����ַ���
                    automation->add(str2);                //��ӵ�  automation��
                }

                automation->add(word);
                ++counts;
            }
           // cout << "count >:" << counts<<endl;
        }
        else {
            std::cout << "�ֵ��ȡʧ�ܣ�" << endl;
        }
    }
    automation->build();
    ends = clock();
    std::cout << "�����ֵ�ɹ��� �ֵ���Ŀ��: " << counts << " ��ʱ:" << ends - starts << "ms\n" << endl;
    cout << "argv[argc - 1]  >: " << argv[argc - 1] << endl;

    vector<string> fileNames;
   /* cout << "������Ҫƥ����ļ� -> :";
    cin >> filepath;
    cout << endl;*/
    string path(/*"E:\\Parsing_the_text\\Parsing_the_text\\Parsing_the_text\\lang"*/argv[argc-1]); 	//�Լ�ѡ��Ŀ¼����

    getFileNames(path, fileNames);

    for (auto& ph : fileNames) {  //����Ҫƥ���Ŀ¼

   /* string buf("lang_items_th.txt");
    automation->readTab(buf);*/

    automation->readTab(ph);
    cout << "ph==" << ph.c_str() << endl;

    }
    delete automation;
    return 0;
}




//�ַ����滻
void string_replace(string& strBig, const string& strsrc, const string& strdst)  //word   \n  " "
{
    string::size_type pos = 0;
    string::size_type srclen = strsrc.size();
    string::size_type dstlen = strdst.size();

    while ((pos = strBig.find(strsrc, pos)) != string::npos)
    {
        strBig.replace(pos, srclen, strdst);////�ӵ�һ��posλ�ÿ�ʼ��srclrn���ַ��滻��strdst
        pos += dstlen;
    }
    pos = 0;
}



//���Ĵ���        //�ҵ�·���µ��ļ�
/*
����·���µ��ļ����Լ��ļ�
*/
void getFileNames(string path, vector<string>& files)
{
    //�ļ����
    //ע�⣺�ҷ�����Щ���´���˴���long���ͣ�ʵ�������лᱨ������쳣
    intptr_t hFile = 0;
    //�ļ���Ϣ
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //�����Ŀ¼,�ݹ����
            //�������,���ļ�����·������vector��
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

