//
// Created by Kidimos on 2023/11/27.
//

#ifndef MYSKIPLIST_SKIPLIST_H
#define MYSKIPLIST_SKIPLIST_H

#endif //MYSKIPLIST_SKIPLIST_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <mutex>
#include <cstring>
#include <fstream>

#define STORE_FILE "store/dumpFile"

std::mutex mtx;
std::string delimiter = ":";

//Class template to implement node
template<typename K, typename V>
class Node{
public:
    Node(){}

    Node(K k,V v,int level);

    ~Node();

    K get_key() const;

    V get_value() const;

    void set_value(V v);

    Node<K,V> **forward;

    int node_level;
private:
    K key;
    V value;
};

template<typename K,typename V>
Node<K,V>::Node(const K k, const V v, int level) :key(k),value(v),node_level(level){
    this->forward = new Node<K,V>*[level+1];
    memset(this->forward,0, sizeof(Node<K,V>*)*(level+1));
}

template<typename K,typename V>
Node<K,V>::~Node() {
    delete[] forward;
}

template<typename K,typename V>
K Node<K,V>::get_key() const {
    return key;
}

template<typename K,typename V>
V Node<K,V>::get_value() const {
    return value;
}

template<typename K,typename V>
void Node<K,V>::set_value(V v) {
    this->value = value;
}

//Class template for Skip list
template<typename K,typename V>
class SkipList{
public:
    SkipList(int);
    ~SkipList();
    int get_random_level();
    Node<K,V>* create_node(K,V,int);
    int insert_element(K,V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    int get_size();

private:
    void get_key_value_from_string(const std::string &str,std::string *key,std::string *value);
    bool is_valid_string(const std::string &str);

private:
    // Maximum level of the skip list
    int _max_level;

    // current level of skip list
    int _skip_list_level;

    // pointer to header node
    Node<K,V> *_header;

    // file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    // skiplist current element count
    int _element_count;
};

//create new node
template<typename K,typename V>
Node<K,V>* SkipList<K,V>::create_node(const K k, const V v, int level) {
    auto *n = new Node<K,V>(k,v,level);
    return n;
}

// Insert given key and value in skip list
// return 1 means element exists
// return 0 means insert successfully
template<typename K,typename V>
int SkipList<K,V>::insert_element(const K key, const V value) {
    mtx.lock();
    Node<K,V> *current = this->_header;

    //创建并初始化update数组
    Node<K,V> *update[_max_level+1];
    memset(update,0, sizeof(Node<K,V>*) * (_max_level + 1));

    //从跳表的最高层开始向下搜索
    for(int i = _skip_list_level;i >= 0;i--){
        while(current->forward[i] != NULL && current->forward[i]->get_key() < key){
            current = current->forward[i];
        }
        update[i] = current;
    }
    //到达第0层并将指针指向右侧的结点，即插入位置
    current = current->forward[0];

    //如果当前结点的键等于搜索的值，则该键已存在
    if(current != nullptr && current->get_key() == key){
        std::cout << "key: " << key << ", exists" << std::endl;
        mtx.unlock();
        return 1;
    }

    //如果当前为nullptr或者当前的键不等于搜索的键，则需要在update[0]和当前结点之间插入新结点
    if(current == nullptr || current->get_key() != key){
        //生成新节点的随机层级
        int random_level = get_random_level();
        //如果随机层级大于跳表的当前层级，则将update数组的未初始化层级设置为头节点。
        if(random_level > _skip_list_level){
            for(int i = _skip_list_level+1;i < random_level + 1;i++){
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }
        //创建带有随机层级的新节点
        Node<K,V> *inserted_node = create_node(key,value,random_level);
        //插入节点
        for(int i = 0;i <= random_level;i++){
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        _element_count++;
    }
    mtx.unlock();
    return 0;
}

// 展示跳表
template<typename K,typename V>
void SkipList<K,V>::display_list() {
    std::cout << "\n*****Skip List*****" << "\n";
    for(int i = 0;i <= _skip_list_level;i++){
        Node<K,V> *node = this->_header->forward[i];
        std::cout << "Level " << i <<": ";
        while(node != nullptr){
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

// 将内存中的数据转储到文件
template<typename K,typename V>
void SkipList<K,V>::dump_file() {
    std::cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    Node<K,V> *node = this->_header->forward[0];

    while(node != nullptr){
        _file_writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }
    _file_writer.flush();
    _file_writer.close();
}

// 从磁盘加载数据
template<typename K,typename V>
void SkipList<K,V>::load_file() {
    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    auto *key = new std::string();
    auto *value = new std::string();
    while(getline(_file_reader,line)){
        get_key_value_from_string(line,key,value);
        if(key->empty() || value->empty()){
            continue;
        }
        // 定义键为int型
        insert_element(stoi(*key),*value);
        std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    delete key;
    delete value;
    _file_reader.close();
}

// 获得当前的skiplist大小
template<typename K,typename V>
int SkipList<K,V>::get_size() {
    return _element_count;
}

template<typename K,typename V>
void SkipList<K,V>::get_key_value_from_string(const std::string &str, std::string *key, std::string *value) {
    if(!is_valid_string(str)) return;
    *key = str.substr(0,str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1,str.length());
}

template<typename K,typename V>
bool SkipList<K,V>::is_valid_string(const std::string &str) {
    if (str.empty()){
        return false;
    }
    if (str.find(delimiter) == std::string::npos){
        return false;
    }
    return true;
}

// 从跳表中删除元素
template<typename K,typename V>
void SkipList<K,V>::delete_element(K key) {

    mtx.lock();
    Node<K,V> *current = this->_header;
    Node<K,V> *update[_max_level + 1];
    memset(update,0, sizeof(Node<K,V>*) * (_max_level+1));

    //从高到低开始进行搜索
    for(int i = _skip_list_level;i >= 0;i--){
        //下一个节点不为空且下一个节点的键小于要找的键则前进一位
        while(current->forward[i] != nullptr && current->forward[i]->get_key() < key){
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];

    if(current != nullptr && current->get_key() == key){
        for(int i = 0;i <= _skip_list_level;i++){
            if(update[i]->forward[i] != current)
                break;
            update[i]->forward[i] = current->forward[i];
        }

        while(_skip_list_level > 0 && _header->forward[_skip_list_level] == 0){
            _skip_list_level--;
        }
        std::cout << "Successfully deleted key "<< key << std::endl;
        delete current;
        _element_count--;
    }
    mtx.unlock();
}

// 从跳表中搜索元素
template<typename K,typename V>
bool SkipList<K,V>::search_element(K key) {
    std::cout << "search_element-----------------" << std::endl;
    Node<K,V> *current = _header;

    for(int i =_skip_list_level;i >= 0;i--){
        while(current->forward[i] != nullptr && current->forward[i]->get_key() < key){
            current = current->forward[i];
        }
    }

    current = current->forward[0];

    if(current != nullptr && current->get_key() == key){
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }
    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

// 构建跳表
template<typename K,typename V>
SkipList<K,V>::SkipList(int max_level):_max_level(max_level),_skip_list_level(0),_element_count(0) {
    // create header node and initialize key and value to null
    K k;
    V v;
    this->_header = new Node<K,V>(k,v,_max_level);
}

template<typename K,typename V>
SkipList<K,V>::~SkipList(){
    if(_file_writer.is_open()){
        _file_writer.close();
    }
    if(_file_reader.is_open()){
        _file_reader.close();
    }
    delete _header;
}

template<typename K,typename V>
int SkipList<K,V>::get_random_level() {

    //代码随想录中，将k的初值设为1，但是我觉得应该可以设为0？
    int k = 0;
    while(rand() % 2){
        k++;
    }
    k = (k < _max_level) ? k : _max_level;
    return k;
}