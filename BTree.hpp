//
// Created by Yggdrasils on 2019/5/27.
//

#include "utility.hpp"
#include <cstring>
#include <functional>
#include <fstream>
#include <cstddef>
#include <iostream>
#include "exception.hpp"
#define M 500
#define L 200
#define MAXN 4096
#define FileLength 8

namespace sjtu {

    template <class Key, class Value, class Compare = std::less<Key> >

    class BTree {
        typedef off_t offset_t;
    public:
        struct dataNodeLeaf                 //叶子结点数据
        {
            Key k;
            Value value;                    //数据
        };

        struct leafNode                     //叶子结点
        {
            short dataSize;                 //数据数量
            /*bool isdelete;                  //块是否从文件中删除*/
            offset_t nodeOffset;
            offset_t next;
            offset_t pre;
            offset_t parent;
            dataNodeLeaf data[L];        //数据

            leafNode():dataSize(0),nodeOffset(0),next(0),pre(0),parent(0){}
        };

        struct dataNodeInter                //内部结点数据
        {
            Key k;
            offset_t dataPos;               //文件偏移量
        };

        struct interNode                    //内部结点
        {
            short dataSize;
            /*bool isdelete;                  //块是否从文件中删除*/
            offset_t offset;
            offset_t parent;
            bool isLeaf;                    //孩子是否为叶子结点
            dataNodeInter data[500];       //pair数

            interNode():offset(0),parent(0),isLeaf(0){}
        };

    private:
        std::fstream pFile;
        interNode *root;            //根结点
        offset_t rootOffSet;        //根结点的位置
        offset_t headLeaf;
        offset_t tailLeaf;
        offset_t End;
        char FileName[FileLength];
        bool isExist;
        char rwBuffer[MAXN];

        // Your private members go here

        void openFile() {
            isExist = 1;
            if (!pFile.is_open()) {
                pFile.open(FileName,std::ios::in);
                if (!pFile) {
                    isExist = 0;
                    pFile.open(FileName,std::ios::in | std::ios::out);
                }
                else readLeaf(root,rootOffSet);
            }
        }

        void close_file() {
            if (pFile.is_open()) {
                pFile.close();
            }
        }

        /**readLeaf():
         *      从文件中读入叶子结点
         *      给定偏移量offset
         *      返回是否成功读入
         */
        bool readLeaf(leafNode *p,offset_t offset) {
            if (!pFile.is_open()) pFile.open(FileName,std::ios::in | std::ios::out);
            pFile.seekg(offset,std::ios::cur);
            if (!pFile.fail()) {
                pFile.close();
                return false;
            }
            pFile.read(rwBuffer,MAXN);
            memcpy(p,rwBuffer, sizeof(leafNode));
            return true;
        }

        /**readInter():
         *      从文件中读入内部结点
         *      给定偏移量offset
         *      返回是否成功读入
         */
        bool readInter(interNode *p, offset_t offset) {
            if (!pFile.is_open()) pFile.open(FileName,std::ios::in | std::ios::out);
            pFile.seekg(offset,std::ios::cur);
            if (!pFile.fail()) {
                pFile.close();
                return false;
            }
            pFile.read(rwBuffer,MAXN);
            memcpy(p,rwBuffer, sizeof(interNode));
            return true;
        }

        /**writeLeaf():
         *      将叶子结点写入文件末尾
         *      返回是否成功写入
         */
        bool writeleaf(leafNode *p,offset_t offset = 0) {
            if (!pFile.is_open()) pFile.open(FileName,std::ios::in | std::ios::out);
            if (offset == 0) {
                pFile.seekg(0, std::ios_base::end);
                offset = pFile.tellg();
            }
            p->nodeOffset = offset;
            if (pFile.fail()) {
                pFile.close();
                return false;
            }
            pFile.seekp(offset);
            memcpy(rwBuffer,p, sizeof(leafNode));
            pFile.write(rwBuffer, sizeof(char)*MAXN);
            pFile.flush();
            return true;
        }

        /**writeInter():
        *      将内部结点写入文件末尾
        *      返回是否成功写入
        */
        bool writeInter(interNode *p,offset_t offset = 0) {
            if (!pFile.is_open()) pFile.open(FileName,std::ios::in | std::ios::out);
            if (offset == 0) {
                pFile.seekg(0, std::ios_base::end);
                offset = pFile.tellg();
            }
            p->nodeOffset = offset;
            if (pFile.fail()) {
                pFile.close();
                return true;
            }
            pFile.seekp(offset);
            memcpy(rwBuffer,p, sizeof(interNode));
            pFile.write(rwBuffer, sizeof(char)*MAXN);
            pFile.flush();
            return false;
        }
//===========================================file operation======================================//

    public:

        typedef pair<const Key, Value> value_type;


        class const_iterator;

        class iterator {

        private:

            bool isLeaf;
            offset_t offset;


        public:

            bool modify(const Value& value_t){

            }

            iterator() {
                isLeaf = false;
                offset = 0;
            }

            iterator(const iterator& other) {
                isLeaf = other.isLeaf;
                offset = other.offset;
            }

            // Return a new iterator which points to the n-next elements

            iterator operator++(int) {
                // Todo iterator++
            }

            iterator& operator++() {
                // Todo ++iterator

            }

            iterator operator--(int) {
                // Todo iterator--
            }

            iterator& operator--() {
                // Todo --iterator
            }

            // Overloaded of operator '==' and '!='

            // Check whether the iterators are same

            bool operator==(const iterator& rhs) const {
                // Todo operator ==

            }

            bool operator==(const const_iterator& rhs) const {
                // Todo operator ==

            }

            bool operator!=(const iterator& rhs) const {
                // Todo operator !=

            }

            bool operator!=(const const_iterator& rhs) const {
                // Todo operator !=

            }

        };

        class const_iterator {

            // it should has similar member method as iterator.

            //  and it should be able to construct from an iterator.

        private:

        public:

            const_iterator() {

            }

            const_iterator(const const_iterator& other) {

            }

            const_iterator(const iterator& other) {

            }

            const_iterator &operator++(int){

            }
            // And other methods in iterator, please fill by yourself.

        };

        // Default Constructor and Copy Constructor

        BTree() {
            pFile = nullptr;
            headLeaf = tailLeaf = End = rootOffSet = 0;
            strcpy(FileName,"BPT.txt");
            openFile();
            if (!isExist) BuildTree();
        }



        BTree(const BTree& other) {
            pFile = other.pFile;
            root = other.root;
            rootOffSet = other.rootOffSet;
            headLeaf = other.headLeaf;
            tailLeaf = other.tailLeaf;
            End = other.End;
            strcpy(FileName,other.FileName);
            isExist = other.isExist;
        }

        BTree& operator=(const BTree& other) {
            if (other == *this ) {
                return *this;
            }
            pFile = other.pFile;
            root = other.root;
            rootOffSet = other.rootOffSet;
            headLeaf = other.headLeaf;
            tailLeaf = other.tailLeaf;
            End = other.End;
            strcpy(FileName,other.FileName);
            isExist = other.isExist;
            return *this;
        }

        ~BTree() {
            close_file();

        }


        pair<iterator,OperationResult> insert_leafNode(const Key &key, const Value &value, leafNode &leaf) {
            int l = 0, r = leaf.dataSize, mid = (l + r)/2;
            while(l < r) {
                if (key == leaf.data[mid]) return pair<iterator,OperationResult> (iterator(nullptr), Fail);
                if (key < leaf.data[mid]) r = mid;
                else l = mid + 1;
                mid = (l + r) / 2;
            }
            int cnt = mid;

            for (int i = leaf.dataSize-1; i > cnt; --i) {
                leaf.data[i+1].k = leaf.data[i].k;
                leaf.data[i+1].value = leaf.data[i].value;
            }
            leaf.data[cnt+1].k = key; leaf.data[cnt+1].value = value;
            leaf.dataSize++;
            if (leaf.dataSize > L) split_node(leaf, key);
            else writeleaf(&leaf, leaf.nodeOffset);
            return pair<iterator,OperationResult> (iterator(nullptr),Success);
        }

        void split_node(leafNode &leaf, const Key &key) { // Todo define if need to change the parent
            leafNode tmpLeaf;
            tmpLeaf.dataSize = leaf.dataSize - leaf.dataSize/2;
            int pos = leaf.dataSize / 2;
            for (int i = 0; i < tmpLeaf.dataSize; ++i) {
                tmpLeaf.data[i] = leaf.data[i + pos];
            }
            tmpLeaf.nodeOffset = End;
            tmpLeaf.pre = leaf.nodeOffset; tmpLeaf.next = leaf.next;
            if (leaf.next == 0) tailLeaf = tmpLeaf.nodeOffset;          //如果原来的叶子是最后一个叶子结点
            else{
                leafNode endLeaf;           //分裂前的next叶子
                readLeaf(&endLeaf,tmpLeaf.next);
                endLeaf.pre = tmpLeaf.nodeOffset;
                writeleaf(&endLeaf,endLeaf.nodeOffset);
            }
            End = tmpLeaf.nodeOffset + sizeof(leafNode);

            writeleaf(&tmpLeaf,tmpLeaf.nodeOffset);
            writeleaf(leaf, leaf.nodeOffset);
            insert_interNode(key,leaf.parent,leaf.nodeOffset);
        }

        pair<iterator, OperationResult > insert_interNode(const Key &key, offset_t interoffset,offset_t leafoffset) {
            interNode inter;
            readInter(&inter,interoffset);
            int l = 0, r = inter.dataSize-1, mid = (l + r)/2;
            while(l < r) {
                if (key < inter.data[mid]) r = mid;
                else l = mid + 1;
                mid = (l + r) / 2;
            }

            int pos = mid;
            for (int i = inter.dataSize-1; i > pos;i--) {
                inter.data[i+1] = inter.data[i];
            }
            inter.data[pos+1].k = key;
            inter.data[pos+1].dataPos = leafoffset;
            inter.dataSize++;
            if (inter.dataSize > M) split_index(&inter,interoffset);
            else writeInter(&inter,inter.offset);
        }

        void split_index(interNode &index, offset_t offset) {
            interNode tmpNode;
            tmpNode.isLeaf = index.isLeaf;
            tmpNode.dataSize= index.dataSize - index.dataSize/2;
            offset_t pos = index.dataSize / 2;
            for (int i = 0; i < tmpNode.dataSize; ++i) {
                tmpNode.data[i] = index.data[i+pos];
            }
            index.dataSize = index.dataSize /2;

            if (tmpNode.isLeaf) {
                leafNode leaf;
                for (int i=0; i < tmpNode.dataSize; ++i){
                    readLeaf(&leaf,tmpNode.data[i].dataPos);
                    leaf.parent = tmpNode.offset;
                    writeleaf(&leaf,leaf.nodeOffset);
                }
            }
            else {
                interNode inter;
                for (int i = 0; i < tmpNode.dataSize; ++i) {
                    readInter(&inter,tmpNode.data[i].dataPos);
                    inter.parent = tmpNode.offset;
                    writeInter(&inter,inter.offset);
                }
            }
            //更新根节点
            if (tmpNode.offset == rootOffSet) {
                interNode newroot;
                newroot.offset = End;
                End += sizeof(interNode);
                newroot.dataSize = 2;
                newroot.data[0] = index.data[0];
                newroot.data[1] = tmpNode.data[0];
                index.parent = tmpNode.parent = newroot.offset;
                root = newroot;
                rootOffSet = newroot.offset;
                writeInter(&newroot,newroot.offset);
                writeInter(&tmpNode,tmpNode.offset);
                writeInter(&index,index.offset);
            }
            else {
                writeInter(&tmpNode,tmpNode.offset);
                writeInter(&index,index.offset);
                insert_interNode(tmpNode.data[0].k,tmpNode.parent,tmpNode.offset);
            }

        }

        offset_t Findleaf(const Key &key, offset_t offset) {
            interNode rt;
            readInter(&rt,rootOffSet);
            int low = 0, high = rt.dataSize-1,mid = (low + high)/2;
            while (low < high) {
                if (key < rt.data[mid]) low = mid;
                else high = mid - 1;
                mid = (low + high) / 2;
            }
            if (rt.isLeaf) {
                /*leafNode leaf;
                readLeaf(&leaf,rt.data[mid].dataPos);
                low = 0; high = leaf.nodeOffset - 1; mid = (low + high) / 2;
                while (low < high) {
                    if (key < leaf.data[mid]) low = mid;
                    else high = mid -1;
                    mid = (low + high) /2;
                }*/
                return rt.data[mid].dataPos;
            }
            else {
                Findleaf(key,rt.data[mid].dataPos);
            }
        }
        /** Insert: Insert certain Key-Value into the database
          *Return a pair, the first of the pair is the iterator point to the new
          *element, the second of the pair is Success if it is successfully inserted
          */
        pair<iterator, OperationResult> insert(const Key& key, const Value& value) {
            if (End == 0) {
                root->data[0].k = key;
                root->data[0].dataPos = 0;
                root->dataSize++;
                leafNode leaf;
                readLeaf(&leaf,root->data[0].dataPos);
                insert_leafNode(key,value,&leaf);
                return pair<iterator,OperationResult >(iterator(nullptr),Success);
            }

            interNode inter;
            leafNode leaf;
            offset_t offset = Findleaf(key,rootOffSet);
            readLeaf(leaf,offset);
            return insert_leafNode(key,value,leaf);

        }

        // Erase: Erase the Key-Value

        // Return Success if it is successfully erased

        // Return Fail if the key doesn't exist in the database

        OperationResult erase(const Key& key) {

            // TODO erase function

            return Fail;  // If you can't finish erase part, just remaining here.

        }

        // Return a iterator to the beginning

        iterator begin() {}

        const_iterator cbegin() const {}

        // Return a iterator to the end(the next element after the last)

        iterator end() {}

        const_iterator cend() const {}

        // Check whether this BTree is empty

        bool empty() const {}

        // Return the number of <K,V> pairs

        size_t size() const {}

        // Clear the BTree

        void clear() {}

        /**

         * Returns the number of elements with key

         *   that compares equivalent to the specified argument,

         * The default method of check the equivalence is !(a < b || b > a)

         */

        size_t count(const Key& key) const {}

        /**

         * Finds an element with key equivalent to key.

         * key value of the element to search for.

         * Iterator to an element with key equivalent to key.

         *   If no such element is found, past-the-end (see end()) iterator is

         * returned.

         */

        Value at(Key key) {
            offset_t offset = Findleaf(key,rootOffSet);
            leafNode leaf;
            readLeaf(&leaf,offset);
            for (int i = 0; i < leaf.dataSize; ++i) {
                if (key == leaf.data[i]) return leaf.data[i].value;
            }
        }

        iterator find(const Key& key) {}

        const_iterator find(const Key& key) const {}

        void BuildTree() {
            End = 0;
            root->offset = End;
            rootOffSet = 0;
            End += sizeof(interNode);
            headLeaf = tailLeaf = End;
            //BTree
            root->parent = 0;
            root->dataSize = 1;
            root->isLeaf = 1;

            leafNode leaf;
            leaf.nodeOffset = End;
            End += sizeof(leafNode);
            root->data[0].dataPos = leaf.nodeOffset;
            leaf.next = leaf.pre = 0;
            leaf.dataSize = 0;
            writeInter(root,rootOffSet);
            writeleaf(leaf,leaf.nodeOffset);

        }

    };

}  // namespace sjtu

