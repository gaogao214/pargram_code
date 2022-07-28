#pragma once
#ifndef AC_AUTOMATION_NODE_H
#define AC_AUTOMATION_NODE_H

#include <map>
#include <string>
#include <iostream>

using namespace std;

class Node
{
public:
    /**
     * ��
     */
    string word;

    /**
     * ��һ�ڵ�
     */
    map<string, Node*> next;

    /**
     * ʧ��ָ��
     */
    Node* fail;

    /**
     * �Ƿ�ƥ�䣬���Ƿ�Ϊ���ڵ�
     */
    bool matched;

    /**
     * ���еĴ�Ƶ
     */
    int repeats;

    /**
     * �ֳ�
     * ע��: ���ַ���
     */
    int wordLength;

    /**
     * ��һ�γ���λ��
     */
    int index;

public:
    Node() : word(""), fail(0), matched(false), repeats(0), wordLength(0), index(-1) {};
};

#endif //AC_AUTOMATION_NODE_H