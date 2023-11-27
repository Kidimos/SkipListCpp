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
    std::iostream _file_reader;

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
    memset(update, sizeof(Node<K,V>*) * (_max_level + 1));

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
    for(int i = 0;i < _skip_list_level;i++){
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

}

