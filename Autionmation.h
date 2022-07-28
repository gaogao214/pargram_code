#pragma once

#ifndef AC_AUTOMATION_AUTOMATION_H
#define AC_AUTOMATION_AUTOMATION_H
#include <vector>
#include <string>
#include <map>
using namespace std;
class Node;
class Automation
{
public:
    /**
     * 根节点
     */
    Node* root;

    /**
     * 用于存储所所有尾节点
     */
    vector<Node*> instances;
    vector<pair<size_t, size_t>> indexArr;
    int bufLength;
    string word;

public:
    Automation();
    ~Automation();

    /**
     * 将字符串拆分成数组
     *
     * @param current
     * @param wordLength
     * @param characters
     */
    void splitWord(const string& word, int& wordLength, vector<string>& characters);

    /**
     * 获取当前节点的下一节点
     *
     * @param current    当前节点
     * @param character  键值
     * @return
     */
    Node* getNext(Node* current, string& character);

    /**
     * 添加字典
     *
     * @param word
     */
    void add(const string& word);

    /**
     * 创建字典
     */
    void build();

    /**
     * 匹配 词典中任何一个词汇匹配成功都会返回
     *
     * @param buf
     * @return
     */
    bool match(const string& buf);
    /* 编写函数：
    * string& replace_all (string& src, const string& old_value, const string& new_value);
    * 参数：源字符串src    被替换的子串old_value    替换的子串new_value
    *
    * 功能：将 源串src 中 子串old_value 全部被替换为 new_value
    */
    string& replace_all(string& src, /*const*/ string& old_value,/* const*/ string& new_value);

    //string printFile(const string& path/*, const string& outPath*/);

    /**
     * 返回所有命中的节点信息
     *
     * @param buf
     */
    string search(/*const*/ string& buf, map<string, Node*>& nodes);
    void readTab(const string& file);

    /**
     * 打印字典
     */
    void print();
};

#endif //AC_AUTOMATION_AUTOMATION_H
