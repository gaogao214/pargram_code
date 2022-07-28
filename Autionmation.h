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
     * ���ڵ�
     */
    Node* root;

    /**
     * ���ڴ洢������β�ڵ�
     */
    vector<Node*> instances;
    vector<pair<size_t, size_t>> indexArr;
    int bufLength;
    string word;

public:
    Automation();
    ~Automation();

    /**
     * ���ַ�����ֳ�����
     *
     * @param current
     * @param wordLength
     * @param characters
     */
    void splitWord(const string& word, int& wordLength, vector<string>& characters);

    /**
     * ��ȡ��ǰ�ڵ����һ�ڵ�
     *
     * @param current    ��ǰ�ڵ�
     * @param character  ��ֵ
     * @return
     */
    Node* getNext(Node* current, string& character);

    /**
     * ����ֵ�
     *
     * @param word
     */
    void add(const string& word);

    /**
     * �����ֵ�
     */
    void build();

    /**
     * ƥ�� �ʵ����κ�һ���ʻ�ƥ��ɹ����᷵��
     *
     * @param buf
     * @return
     */
    bool match(const string& buf);
    /* ��д������
    * string& replace_all (string& src, const string& old_value, const string& new_value);
    * ������Դ�ַ���src    ���滻���Ӵ�old_value    �滻���Ӵ�new_value
    *
    * ���ܣ��� Դ��src �� �Ӵ�old_value ȫ�����滻Ϊ new_value
    */
    string& replace_all(string& src, /*const*/ string& old_value,/* const*/ string& new_value);

    //string printFile(const string& path/*, const string& outPath*/);

    /**
     * �����������еĽڵ���Ϣ
     *
     * @param buf
     */
    string search(/*const*/ string& buf, map<string, Node*>& nodes);
    void readTab(const string& file);

    /**
     * ��ӡ�ֵ�
     */
    void print();
};

#endif //AC_AUTOMATION_AUTOMATION_H
