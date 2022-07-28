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


//构造函数和析构函数     在构造的时候创建一个空的根节点。
Automation::Automation()
{
    root = new Node;
}

Automation::~Automation()
{
    delete root;
}
//字符串分割方法       由于需要兼容中文，所以需要独立的字符串分割的方法。           给字符串和关键词分隔开
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

//从当前节点出发获取下一节点    
/*
*每个节点的子节点都存储在next向量中，
*next是一个map<string, Node*>向量，
*之所以用map来存储而不用[]数组来存储， 
*是由于中文字较多，使用[]数组1是初始化不方便，2是比对查找性能较差。
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



//创建字典树    遍历关键词，然后从root根节点进入，如果根据字符知道子节点，就移动到子节点进行下一次循环，
//如果没有，则创建节点。如果字符遍历到尾部，则将找到活创建的最后一个节点的matched设置为true。
//1. 由于word是常量参数，所以word只能在传参前修改，需要考虑到关键词尾部可能有\r|\n的换行符导致关键词
//不会如预期那样到最后一个字符结束。 
//2. 这样的创建在某一条路线上可能会有多个节点会被标记为完整的词组。并不是最尾部的一个才是完整词组.
void Automation::add(const string& word)
{
    int wordLength = 0;
    vector<string> characters;
    splitWord(word, wordLength, characters);

    Node* temp = root;

    int i = 1;
    string pathWord = "";
    // 遍历字符串
    for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character) {
        pathWord.append(*character);
        map<string, Node*>::iterator item = temp->next.find(*character);

        if (item != temp->next.end()) {   // 中间节点，指针移到下一节点
            temp = item->second;
        }
        else {                          // 尾节点，添加新的节点
            Node* n = new Node;
            n->word = pathWord;
            n->wordLength = i;
            temp->next.insert(make_pair(*character, n));
            temp = n;
        }

        if (character + 1 == characters.end()) {
            temp->matched = true;
            // cout<< "完整词组:" << temp->word << endl;
        }

        ++i;
    }
  
}

//设置失配指针
/*
设置失配指针分两步：

将所有空节点下的第一级子节点的失配指针都设置为root, 并将其加入队列
遍历队列，为当前节点的next节点设置失配指针，所有next接地男介入队列继续循环。处理过的节点弹出队列。*/
void Automation::build()
{
    queue<Node*> Q;

    // 将第一级节点的所有失配指针都设置为root 并将所有第一级节点加入队列
    for (map<string, Node*>::iterator item = root->next.begin(); item != root->next.end(); ++item) {
        item->second->fail = root;
        Q.push(item->second);
    }

    instances.clear();
    //instances.push_back(root);

    // 遍历队列
    while (!Q.empty()) {
        Node* temp = Q.front();
        Q.pop();

        // 遍历当前节点的下一节点
        for (map<string, Node*>::iterator item = temp->next.begin(); item != temp->next.end(); ++item) {
            // 键值
            string character = item->first;

            // 将当前节点加入队列循环
            Q.push(item->second);

            // 设置失配指针 从上一节点的失配指针查起
            Node* parent = temp->fail;
            while (!getNext(parent, character)) parent = parent->fail;
            item->second->fail = getNext(parent, character);
            if (!getNext(parent, character)) throw 1;
        }

        // 存储尾节点
        if (temp->matched) {
            instances.push_back(temp);
        }
    }
}
//匹配方法      遍历要匹配的字符串，从字典的root节点出发，逐个字比对，一旦不匹配，则节点指针移动到失配指针， 匹配到关键词则退出循环不再进行后面的字符匹配。
bool Automation::match(const string& buf)
{
    int bufLength = 0;
    vector<string> characters;
    splitWord(buf, bufLength, characters);

    Node* temp = root;
    // 遍历字符串
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
/* 编写函数：
 * string& replace_all (string& src, const string& old_value, const string& new_value);
 * 参数：源字符串src    被替换的子串old_value    替换的子串new_value
 *
 * 功能：将 源串src 中 子串old_value 全部被替换为 new_value
 */
string& Automation::replace_all(string& src, /*const*/ string& old_value, /*const*/ string& new_value) {
    // 每次重新定位起始位置，防止上轮替换后的字符串形成新的old_value
    for (string::size_type pos(0); pos != string::npos; pos += new_value.length()) {
        if ((pos = src.find(old_value, pos)) != string::npos) {
            src.replace(pos, old_value.length(), new_value);
        }
        else break;
    }

    
    return src;
}

/*
* 给表格的第四列   文本匹配关键词 
* 匹配完删除每行的<w>
* 之后保存
*/
void Automation::readTab(const string& file)
{
    sandbox::EditorConfigTab editorconfigtab;
    map<string, Node*> nodes;
    editorconfigtab.Load(file);
    for (int row = 0; row < editorconfigtab.m_all_content.size(); row++)   //行
    {
        auto& line = editorconfigtab.m_all_content[row];          //每行的内容
        string buf(line[3].c_str());
        line[3] = search(buf, nodes);
        string buf_w(line[3].c_str());     //把匹配完加上<w>之后的一行的字符串赋给buf_w
        if (!buf_w.empty() && buf_w.size()>3)   //如果这一行不为空 且 数量大于 3 个  
        {
            word = buf_w.substr(buf_w.size() - 3, buf_w.size() - 1);//把最后三个字符赋给word
           // cout << word << endl;
            if (word == "<w>")     //如果最后三个字符==<w>  就删除
            {
                line[3] = buf_w.erase(buf_w.size() - 3, buf_w.size() - 1);   //删除掉最后三个字符的位置

            }

        }
       
        nodes.clear();
    }
    editorconfigtab.Save();
    editorconfigtab.m_all_content.clear();
}

//搜索方法     搜索方法和匹配方法原理几乎一致，不同的是匹配方法在匹配到关键词的时候继续匹配，会使用一个map结构来存储匹配的节点。
string Automation::search(/*const*/ string& buf, map<string, Node*>& nodes)
{
    int bufLength = 0;
    vector<string> characters;
    splitWord(buf, bufLength, characters);

    int index = 0;
   
    stack<pair<size_t, size_t>> stack_;

    //a  第一个关键词 字符第一个的位置 和关键词最后的一个字符的位置    b是第二个关键词 字符第一个的位置 和关键词最后的一个字符的位置
    auto lambda = [](const pair<size_t, size_t>& a, const pair<size_t, size_t>& b) {return a.first < b.first; };//给匹配到字符串的下标排序

    //key  是 当前 由那几个 value 合成的最长 串。。可以 后面 还有 比这个key 更长的 。。也用到了key中的字符串 。。这个时候 key 就 不匹配了
    //key 里存的是最长的那个字符串的开头和结尾    value 里存的是由谁组成的  football 0,7 是由0,3和4,7   0,7是key 0,3和 4,7在value里面存着
    //vector 里存着 key 里的下标 。。由哪些下标组成             key 之间比较
    map<pair<size_t, size_t>, vector<pair<size_t, size_t>>, decltype(lambda)> mmmm(lambda);//散

    vector<size_t> insertIndex;

    Node* temp = root;
    // 遍历字符串
    for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character) {

        while (!getNext(temp, *character)) {
            temp = temp->fail;
        }

        temp = getNext(temp, *character);

        if (temp->matched) { //如果匹配
            map<string, Node*>::iterator nodeFind = nodes.find(temp->word);
            if (nodeFind == nodes.end()) {
                temp->repeats = 1;                       //词频
                temp->index = index + 1 - temp->wordLength ; //从起始位置开始  -  字长  是这个关键词 字符第一个的位置  

                nodes.insert(make_pair(temp->word, temp));   
                //print();
            }
            else {
                nodeFind->second->repeats += 1;
            }
            //indexArr.emplace_back(index + 1 - temp->wordLength, index);//是这个关键词 字符第一个的位置     和关键词最后的一个字符的位置

         pair<size_t, size_t>key_word_location(index + 1 - temp->wordLength, index);   //关键字的第一个位置   和关键词最后一个位置
        if (stack_.empty()) stack_.push(key_word_location);
        else
        {
            vector<pair<size_t, size_t>> star; // 聚
            bool isc = true;
            while (!stack_.empty())
            {
                auto stack_top = stack_.top();
                if (stack_top.first >= key_word_location.first) //全部插入
                {

                    star.emplace_back(stack_top);//聚时一朵朵
                    stack_.pop();

                }
                else if (key_word_location.first > stack_top.first && key_word_location.first <= stack_top.second && key_word_location.second - key_word_location.first + 1 > stack_top.second - stack_top.first + 1) //插入一半
                {
                    stack_.pop();
                    if (mmmm.find(stack_top) != mmmm.end())
                    {
                        for (auto v = mmmm[stack_top].rbegin(); v != mmmm[stack_top].rend(); ++v)
                            stack_.push(*v);//散时满天星
                        mmmm.erase(mmmm.find(stack_top));
                    }

                }
                else if (key_word_location.first > stack_top.second) //插不进去
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

   
    //输入 utf-8
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
            if (temp == "<w>")            //如果关键词位置有一个<w>了，就跳过   
            {
                character += 3;
            }

            str2 += "<w>";                  //给关键词添加<w>
          
        }
    }

   
    return str2;
}


//匹配到的关键词 在word.txt中开头下标和末尾下标   例如  football(0,7) foot(0,3) 如果 football.first=foot.first &&football.second>foot.second  foot.pop
//string Automation::printFile(const string& path/*, const string& outPath*/)//
//{
//    stack<pair<size_t, size_t>> stack_;
//
//    //a  第一个关键词 字符第一个的位置 和关键词最后的一个字符的位置    b是第二个关键词 字符第一个的位置 和关键词最后的一个字符的位置
//    auto lambda = [](const pair<size_t, size_t>& a, const pair<size_t, size_t>& b) {return a.first < b.first; };//给匹配到字符串的下标排序
//
//    //key  是 当前 由那几个 value 合成的最长 串。。可以 后面 还有 比这个key 更长的 。。也用到了key中的字符串 。。这个时候 key 就 不匹配了
//    //key 里存的是最长的那个字符串的开头和结尾    value 里存的是由谁组成的  football 0,7 是由0,3和4,7   0,7是key 0,3和 4,7在value里面存着
//    //vector 里存着 key 里的下标 。。由哪些下标组成             key 之间比较
//    map<pair<size_t, size_t>, vector<pair<size_t, size_t>>, decltype(lambda)> mmmm(lambda);//散
//
//    vector<size_t> insertIndex;
//
//    std::sort(indexArr.begin(), indexArr.end(), [](const auto& a, const auto& b) {return a.second < b.second; });//插前准备 升序 (由小到大排序)
//    //indexArr 里面存的 是这个关键词 字符第一个的位置     和关键词最后的一个字符的位置
//    for (const auto& key_word_location : indexArr)
//    {
//        if (stack_.empty()) stack_.push(key_word_location);
//        else
//        {
//            vector<pair<size_t, size_t>> star; // 聚
//            bool isc = true;
//            while (!stack_.empty())
//            {
//                auto stack_top = stack_.top();
//                if (stack_top.first >= key_word_location.first) //全部插入
//                {
//                    
//                    star.emplace_back(stack_top);//聚时一朵朵
//                    stack_.pop();
//                   
//                }
//                else if (key_word_location.first > stack_top.first && key_word_location.first <= stack_top.second && key_word_location.second - key_word_location.first + 1 > stack_top.second - stack_top.first + 1) //插入一半
//                {
//                    stack_.pop();
//                    if (mmmm.find(stack_top) != mmmm.end())
//                    {
//                        for (auto v = mmmm[stack_top].rbegin(); v != mmmm[stack_top].rend(); ++v)
//                            stack_.push(*v);//散时满天星
//                        mmmm.erase(mmmm.find(stack_top));
//                    }
//
//                }
//                else if (key_word_location.first > stack_top.second) //插不进去
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
//    //输入 utf-8
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
//    //输出 utf-8
//  /*  std::ofstream wof(outPath, ios::binary);
//    wof << str2;
//    wof.close();*/
//    return str2;
//}
//字典打印方法
void Automation::print()
{
    int n = 0;
    for (vector<Node*>::iterator node = instances.begin(); node != instances.end(); ++node) {
        ++n;
        cout << "--------------------------------" << endl;
        cout << "编号 : " << n << endl;
        cout << "分词 ：" << (*node)->word << endl;
        cout << "长度 : " << (*node)->wordLength << endl;
        cout << "尾点 : " << ((*node)->matched ? "是" : "否") << endl;
    }
    cout << "--------------------------------" << endl;
}
















