#include "Autionmation.h"
#include<fstream>
#include <map>
#include <queue>
#include <stack>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <codecvt>
#include "EditorConfigTab.h"
#include "node.h"


//���캯������������     �ڹ����ʱ�򴴽�һ���յĸ��ڵ㡣
Automation::Automation()
{
    root = new Node;
}

Automation::~Automation()
{
    delete root;
}
//�ַ����ָ��       ������Ҫ�������ģ�������Ҫ�������ַ����ָ�ķ�����           ���ַ����͹ؼ��ʷָ���
void Automation::splitWord(const string& word, int& wordLength, vector <string>& characters)
{
    int wordSize = word.size();

    int i = 0;
    while (i < wordSize) {
        int characterSize = 1;

        if (word[i] & 0x80) {
            char character = word[i];
            character <<= 1;
            do {
                character <<= 1;
                ++characterSize;
            } while (character & 0x80);
        }

        string subWord;
        subWord = word.substr(i, characterSize);
        characters.push_back(subWord);

        i += characterSize;
        ++wordLength;
    }
}

//�ӵ�ǰ�ڵ������ȡ��һ�ڵ�    
/*
*ÿ���ڵ���ӽڵ㶼�洢��next�����У�
*next��һ��map<string, Node*>������
*֮������map���洢������[]�������洢�� 
*�����������ֽ϶࣬ʹ��[]����1�ǳ�ʼ�������㣬2�ǱȶԲ������ܽϲ
*/
Node* Automation::getNext(Node* current, string& character)
{
    map<string, Node*>::iterator item = current->next.find(character);

    if (item != current->next.end()) {
        return item->second;
    }

    if (current == root) {
        return root;
    }

    return 0;
}



//�����ֵ���    �����ؼ��ʣ�Ȼ���root���ڵ���룬��������ַ�֪���ӽڵ㣬���ƶ����ӽڵ������һ��ѭ����
//���û�У��򴴽��ڵ㡣����ַ�������β�������ҵ���������һ���ڵ��matched����Ϊtrue��
//1. ����word�ǳ�������������wordֻ���ڴ���ǰ�޸ģ���Ҫ���ǵ��ؼ���β��������\r|\n�Ļ��з����¹ؼ���
//������Ԥ�����������һ���ַ������� 
//2. �����Ĵ�����ĳһ��·���Ͽ��ܻ��ж���ڵ�ᱻ���Ϊ�����Ĵ��顣��������β����һ��������������.
void Automation::add(const string& word)
{
    int wordLength = 0;
    vector<string> characters;
    splitWord(word, wordLength, characters);

    Node* temp = root;

    int i = 1;
    string pathWord = "";
    // �����ַ���
    for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character) {
        pathWord.append(*character);
        map<string, Node*>::iterator item = temp->next.find(*character);

        if (item != temp->next.end()) {   // �м�ڵ㣬ָ���Ƶ���һ�ڵ�
            temp = item->second;
        }
        else {                          // β�ڵ㣬����µĽڵ�
            Node* n = new Node;
            n->word = pathWord;
            n->wordLength = i;
            temp->next.insert(make_pair(*character, n));
            temp = n;
        }

        if (character + 1 == characters.end()) {
            temp->matched = true;
            // cout<< "��������:" << temp->word << endl;
        }

        ++i;
    }
  
}

//����ʧ��ָ��
/*
����ʧ��ָ���������

�����пսڵ��µĵ�һ���ӽڵ��ʧ��ָ�붼����Ϊroot, ������������
�������У�Ϊ��ǰ�ڵ��next�ڵ�����ʧ��ָ�룬����next�ӵ��н�����м���ѭ����������Ľڵ㵯�����С�*/
void Automation::build()
{
    queue<Node*> Q;

    // ����һ���ڵ������ʧ��ָ�붼����Ϊroot �������е�һ���ڵ�������
    for (map<string, Node*>::iterator item = root->next.begin(); item != root->next.end(); ++item) {
        item->second->fail = root;
        Q.push(item->second);
    }

    instances.clear();
    //instances.push_back(root);

    // ��������
    while (!Q.empty()) {
        Node* temp = Q.front();
        Q.pop();

        // ������ǰ�ڵ����һ�ڵ�
        for (map<string, Node*>::iterator item = temp->next.begin(); item != temp->next.end(); ++item) {
            // ��ֵ
            string character = item->first;

            // ����ǰ�ڵ�������ѭ��
            Q.push(item->second);

            // ����ʧ��ָ�� ����һ�ڵ��ʧ��ָ�����
            Node* parent = temp->fail;
            while (!getNext(parent, character)) parent = parent->fail;
            item->second->fail = getNext(parent, character);
            if (!getNext(parent, character)) throw 1;
        }

        // �洢β�ڵ�
        if (temp->matched) {
            instances.push_back(temp);
        }
    }
}
//ƥ�䷽��      ����Ҫƥ����ַ��������ֵ��root�ڵ����������ֱȶԣ�һ����ƥ�䣬��ڵ�ָ���ƶ���ʧ��ָ�룬 ƥ�䵽�ؼ������˳�ѭ�����ٽ��к�����ַ�ƥ�䡣
bool Automation::match(const string& buf)
{
    int bufLength = 0;
    vector<string> characters;
    splitWord(buf, bufLength, characters);

    Node* temp = root;
    // �����ַ���
    for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character) {
        while (!getNext(temp, *character)) {
            temp = temp->fail;
       
        }
        temp = getNext(temp, *character);
       


        if (temp->matched) {
            return true;
        }
    }

    return false;
}
/* ��д������
 * string& replace_all (string& src, const string& old_value, const string& new_value);
 * ������Դ�ַ���src    ���滻���Ӵ�old_value    �滻���Ӵ�new_value
 *
 * ���ܣ��� Դ��src �� �Ӵ�old_value ȫ�����滻Ϊ new_value
 */
string& Automation::replace_all(string& src, /*const*/ string& old_value, /*const*/ string& new_value) {
    // ÿ�����¶�λ��ʼλ�ã���ֹ�����滻����ַ����γ��µ�old_value
    for (string::size_type pos(0); pos != string::npos; pos += new_value.length()) {
        if ((pos = src.find(old_value, pos)) != string::npos) {
            src.replace(pos, old_value.length(), new_value);
        }
        else break;
    }

    
    return src;
}

/*
* �����ĵ�����   �ı�ƥ��ؼ��� 
* ƥ����ɾ��ÿ�е�<w>
* ֮�󱣴�
*/
void Automation::readTab(const string& file)
{
    sandbox::EditorConfigTab editorconfigtab;
    map<string, Node*> nodes;
    editorconfigtab.Load(file);
    for (int row = 0; row < editorconfigtab.m_all_content.size(); row++)   //��
    {
        auto& line = editorconfigtab.m_all_content[row];          //ÿ�е�����
        string buf(line[3].c_str());
        line[3] = search(buf, nodes);
        string buf_w(line[3].c_str());     //��ƥ�������<w>֮���һ�е��ַ�������buf_w
        if (!buf_w.empty() && buf_w.size()>3)   //�����һ�в�Ϊ�� �� �������� 3 ��  
        {
            word = buf_w.substr(buf_w.size() - 3, buf_w.size() - 1);//����������ַ�����word
           // cout << word << endl;
            if (word == "<w>")     //�����������ַ�==<w>  ��ɾ��
            {
                line[3] = buf_w.erase(buf_w.size() - 3, buf_w.size() - 1);   //ɾ������������ַ���λ��

            }

        }
       
        nodes.clear();
    }
    editorconfigtab.Save();
    editorconfigtab.m_all_content.clear();
}

//��������     ����������ƥ�䷽��ԭ����һ�£���ͬ����ƥ�䷽����ƥ�䵽�ؼ��ʵ�ʱ�����ƥ�䣬��ʹ��һ��map�ṹ���洢ƥ��Ľڵ㡣
string Automation::search(/*const*/ string& buf, map<string, Node*>& nodes)
{
    int bufLength = 0;
    vector<string> characters;
    splitWord(buf, bufLength, characters);

    int index = 0;
   
    stack<pair<size_t, size_t>> stack_;

    //a  ��һ���ؼ��� �ַ���һ����λ�� �͹ؼ�������һ���ַ���λ��    b�ǵڶ����ؼ��� �ַ���һ����λ�� �͹ؼ�������һ���ַ���λ��
    auto lambda = [](const pair<size_t, size_t>& a, const pair<size_t, size_t>& b) {return a.first < b.first; };//��ƥ�䵽�ַ������±�����

    //key  �� ��ǰ ���Ǽ��� value �ϳɵ�� ���������� ���� ���� �����key ������ ����Ҳ�õ���key�е��ַ��� �������ʱ�� key �� ��ƥ����
    //key ����������Ǹ��ַ����Ŀ�ͷ�ͽ�β    value ��������˭��ɵ�  football 0,7 ����0,3��4,7   0,7��key 0,3�� 4,7��value�������
    //vector ����� key ����±� ��������Щ�±����             key ֮��Ƚ�
    map<pair<size_t, size_t>, vector<pair<size_t, size_t>>, decltype(lambda)> mmmm(lambda);//ɢ

    vector<size_t> insertIndex;

    Node* temp = root;
    // �����ַ���
    for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character) {

        while (!getNext(temp, *character)) {
            temp = temp->fail;
        }

        temp = getNext(temp, *character);

        if (temp->matched) { //���ƥ��
            map<string, Node*>::iterator nodeFind = nodes.find(temp->word);
            if (nodeFind == nodes.end()) {
                temp->repeats = 1;                       //��Ƶ
                temp->index = index + 1 - temp->wordLength ; //����ʼλ�ÿ�ʼ  -  �ֳ�  ������ؼ��� �ַ���һ����λ��  

                nodes.insert(make_pair(temp->word, temp));   
                //print();
            }
            else {
                nodeFind->second->repeats += 1;
            }
            //indexArr.emplace_back(index + 1 - temp->wordLength, index);//������ؼ��� �ַ���һ����λ��     �͹ؼ�������һ���ַ���λ��

         pair<size_t, size_t>key_word_location(index + 1 - temp->wordLength, index);   //�ؼ��ֵĵ�һ��λ��   �͹ؼ������һ��λ��
        if (stack_.empty()) stack_.push(key_word_location);
        else
        {
            vector<pair<size_t, size_t>> star; // ��
            bool isc = true;
            while (!stack_.empty())
            {
                auto stack_top = stack_.top();
                if (stack_top.first >= key_word_location.first) //ȫ������
                {

                    star.emplace_back(stack_top);//��ʱһ���
                    stack_.pop();

                }
                else if (key_word_location.first > stack_top.first && key_word_location.first <= stack_top.second && key_word_location.second - key_word_location.first + 1 > stack_top.second - stack_top.first + 1) //����һ��
                {
                    stack_.pop();
                    if (mmmm.find(stack_top) != mmmm.end())
                    {
                        for (auto v = mmmm[stack_top].rbegin(); v != mmmm[stack_top].rend(); ++v)
                            stack_.push(*v);//ɢʱ������
                        mmmm.erase(mmmm.find(stack_top));
                    }

                }
                else if (key_word_location.first > stack_top.second) //�岻��ȥ
                    break;
                else
                {
                    isc = false;
                    break;
                }
            }
            stack_.push(key_word_location);
            if (!star.empty() && mmmm.find(key_word_location) == mmmm.end())   mmmm[key_word_location] = star;
        }
        }

        ++index;
    }

   
    //���� utf-8
    string str1, str2;
    str2.clear();

    /*ifstream buf(path);
    string str((istreambuf_iterator<char>(buf)), istreambuf_iterator<char>());
    vector<string> characters;
    splitWord(str, bufLength, characters);*/


    while (!stack_.empty())
    {
        insertIndex.emplace_back(stack_.top().second);
        stack_.pop();
    }

    reverse(insertIndex.begin(), insertIndex.end());

    int j = 0;
    for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character)
    {
        str2 += *character;
        if (j < insertIndex.size() && insertIndex[j] == distance(characters.begin(), character))
        {
            string temp;
            if (characters.end() - character > 3)
            {
                temp = *(character + 1) + *(character + 2) + *(character + 3);

            }
            ++j;
            if (temp == "<w>")            //����ؼ���λ����һ��<w>�ˣ�������   
            {
                character += 3;
            }

            str2 += "<w>";                  //���ؼ������<w>
          
        }
    }

   
    return str2;
}


//ƥ�䵽�Ĺؼ��� ��word.txt�п�ͷ�±��ĩβ�±�   ����  football(0,7) foot(0,3) ��� football.first=foot.first &&football.second>foot.second  foot.pop
//string Automation::printFile(const string& path/*, const string& outPath*/)//
//{
//    stack<pair<size_t, size_t>> stack_;
//
//    //a  ��һ���ؼ��� �ַ���һ����λ�� �͹ؼ�������һ���ַ���λ��    b�ǵڶ����ؼ��� �ַ���һ����λ�� �͹ؼ�������һ���ַ���λ��
//    auto lambda = [](const pair<size_t, size_t>& a, const pair<size_t, size_t>& b) {return a.first < b.first; };//��ƥ�䵽�ַ������±�����
//
//    //key  �� ��ǰ ���Ǽ��� value �ϳɵ�� ���������� ���� ���� �����key ������ ����Ҳ�õ���key�е��ַ��� �������ʱ�� key �� ��ƥ����
//    //key ����������Ǹ��ַ����Ŀ�ͷ�ͽ�β    value ��������˭��ɵ�  football 0,7 ����0,3��4,7   0,7��key 0,3�� 4,7��value�������
//    //vector ����� key ����±� ��������Щ�±����             key ֮��Ƚ�
//    map<pair<size_t, size_t>, vector<pair<size_t, size_t>>, decltype(lambda)> mmmm(lambda);//ɢ
//
//    vector<size_t> insertIndex;
//
//    std::sort(indexArr.begin(), indexArr.end(), [](const auto& a, const auto& b) {return a.second < b.second; });//��ǰ׼�� ���� (��С��������)
//    //indexArr ������ ������ؼ��� �ַ���һ����λ��     �͹ؼ�������һ���ַ���λ��
//    for (const auto& key_word_location : indexArr)
//    {
//        if (stack_.empty()) stack_.push(key_word_location);
//        else
//        {
//            vector<pair<size_t, size_t>> star; // ��
//            bool isc = true;
//            while (!stack_.empty())
//            {
//                auto stack_top = stack_.top();
//                if (stack_top.first >= key_word_location.first) //ȫ������
//                {
//                    
//                    star.emplace_back(stack_top);//��ʱһ���
//                    stack_.pop();
//                   
//                }
//                else if (key_word_location.first > stack_top.first && key_word_location.first <= stack_top.second && key_word_location.second - key_word_location.first + 1 > stack_top.second - stack_top.first + 1) //����һ��
//                {
//                    stack_.pop();
//                    if (mmmm.find(stack_top) != mmmm.end())
//                    {
//                        for (auto v = mmmm[stack_top].rbegin(); v != mmmm[stack_top].rend(); ++v)
//                            stack_.push(*v);//ɢʱ������
//                        mmmm.erase(mmmm.find(stack_top));
//                    }
//
//                }
//                else if (key_word_location.first > stack_top.second) //�岻��ȥ
//                    break;
//                else
//                {
//                    isc = false;
//                    break;
//                }
//            }
//            stack_.push(key_word_location);
//            if (!star.empty() && mmmm.find(key_word_location) == mmmm.end())   mmmm[key_word_location] = star;
//        }
//    }
//    indexArr.clear();
//    //���� utf-8
//    string str1, str2;
//    str2.clear();
//
//    ifstream buf(path);
//    string str((istreambuf_iterator<char>(buf)), istreambuf_iterator<char>());
//    vector<string> characters;
//    splitWord(str, bufLength, characters);
//
//
//    while (!stack_.empty())
//    {
//        insertIndex.emplace_back(stack_.top().second);
//        stack_.pop();
//    }
//
//    reverse(insertIndex.begin(), insertIndex.end());
//
//    int j = 0;
//    for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character)
//    {
//        str2 += *character;
//        if (j < insertIndex.size() && insertIndex[j] == distance(characters.begin(), character))
//        {
//            string temp;
//            if (characters.end() - character >= 3)
//            {
//                temp = *(character + 1) + *(character + 2) + *(character + 3);
//
//            }
//            ++j;
//            if (temp == "<w>")
//            {
//                character += 3;
//            }
//            
//                str2 += "<w>";
//
//                   
//        }
//    }
//
//    //��� utf-8
//  /*  std::ofstream wof(outPath, ios::binary);
//    wof << str2;
//    wof.close();*/
//    return str2;
//}
//�ֵ��ӡ����
void Automation::print()
{
    int n = 0;
    for (vector<Node*>::iterator node = instances.begin(); node != instances.end(); ++node) {
        ++n;
        cout << "--------------------------------" << endl;
        cout << "��� : " << n << endl;
        cout << "�ִ� ��" << (*node)->word << endl;
        cout << "���� : " << (*node)->wordLength << endl;
        cout << "β�� : " << ((*node)->matched ? "��" : "��") << endl;
    }
    cout << "--------------------------------" << endl;
}
















